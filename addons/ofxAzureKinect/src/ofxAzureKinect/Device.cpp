#include "Device.h"

#include "ofLog.h"

const int32_t TIMEOUT_IN_MS = 1000;

namespace ofxAzureKinect
{
	DeviceSettings::DeviceSettings()
		: depthMode(K4A_DEPTH_MODE_WFOV_2X2BINNED)
		, colorResolution(K4A_COLOR_RESOLUTION_2160P)
		, colorFormat(K4A_IMAGE_FORMAT_COLOR_BGRA32)
		, cameraFps(K4A_FRAMES_PER_SECOND_30)
		, wiredSyncMode(K4A_WIRED_SYNC_MODE_STANDALONE)
		, depthDelayUsec(0)
		, subordinateDelayUsec(0)
		, updateColor(true)
		, updateIr(true)
		, updateWorld(true)
		, updateVbo(true)
		, syncImages(true)
	{}

	int Device::getInstalledCount()
	{
		return k4a_device_get_installed_count();
	}

	Device::Device()
		: Stream()
		, index(-1)
	{}

	Device::~Device()
	{
		this->close();
	}

	bool Device::open(uint32_t idx)
	{
		if (this->bOpen)
		{
			ofLogWarning(__FUNCTION__) << "Device " << this->index << " / " << this->serialNumber << " already open!";
			return false;
		}

		// Load the device at the requested index.
		try
		{
			// Open connection to the device.
			this->device = k4a::device::open(idx);

			// Get the device index and serial number.
			this->index = idx;
			this->serialNumber = this->device.get_serialnum();
		}
		catch (const k4a::error& e)
		{
			ofLogError(__FUNCTION__) << e.what();

			this->device.close();

			return false;
		}

		ofLogNotice(__FUNCTION__) << "Successfully opened device " << this->index << " / " << this->serialNumber << ".";
		this->bOpen = true;

		return true;
	}

	bool Device::open(const std::string& serialNumber)
	{
		if (this->bOpen)
		{
			ofLogWarning(__FUNCTION__) << "Device " << this->index << " / " << this->serialNumber << " already open!";
			return false;
		}

		// Loop through devices and find the one with the requested serial.
		bool deviceFound = false;
		int numConnected = Device::getInstalledCount();
		for (int i = 0; i < numConnected; ++i)
		{
			try
			{
				// Open connection to the device.
				this->device = k4a::device::open(static_cast<uint32_t>(i));

				// Get the device serial number and check it.
				this->serialNumber = this->device.get_serialnum();
				if (this->serialNumber == serialNumber)
				{
					deviceFound = true;
					this->index = i;
					break;
				}
				else
				{
					this->device.close();
				}
			}
			catch (const k4a::error& e)
			{
				// Don't worry about it; we just might be trying to access an already open device.
				continue;
			}
		}

		if (!deviceFound)
		{
			ofLogError(__FUNCTION__) << "No device found with serial number " << serialNumber;
			return false;
		}

		ofLogNotice(__FUNCTION__) << "Successfully opened device " << this->index << " / " << this->serialNumber << ".";
		this->bOpen = true;

		return true;
	}

	bool Device::close()
	{
		if (!this->bOpen) return false;

		this->stopRecording();
		this->stopCameras();

		this->device.close();

		this->index = -1;
		this->serialNumber = "";
		this->bOpen = false;

		return true;
	}

	bool Device::startCameras(DeviceSettings deviceSettings)
	{
		if (!this->bOpen)
		{
			ofLogError(__FUNCTION__) << "Open device before starting cameras!";
			return false;
		}

		// Generate device config.
		this->config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
		this->config.depth_mode = deviceSettings.depthMode;
		this->config.color_format = deviceSettings.colorFormat;
		this->config.color_resolution = deviceSettings.colorResolution;
		this->config.camera_fps = deviceSettings.cameraFps;
		this->config.synchronized_images_only = deviceSettings.syncImages;

		this->config.wired_sync_mode = deviceSettings.wiredSyncMode;
		this->config.depth_delay_off_color_usec = deviceSettings.depthDelayUsec;
		this->config.subordinate_delay_off_master_usec = deviceSettings.subordinateDelayUsec;

		// Set update flags.
		this->bUpdateColor = deviceSettings.updateColor;
		this->bUpdateIr = deviceSettings.updateIr;
		this->bUpdateWorld = deviceSettings.updateWorld;
		this->bUpdateVbo = deviceSettings.updateWorld && deviceSettings.updateVbo;

		// Get calibration.
		try
		{
			this->calibration = this->device.get_calibration(this->config.depth_mode, this->config.color_resolution);
		}
		catch (const k4a::error& e)
		{
			ofLogError(__FUNCTION__) << e.what();
			return false;
		}

		if (this->bUpdateColor)
		{
			// Create transformation.
			this->transformation = k4a::transformation(this->calibration);
		}

		if (this->bUpdateWorld)
		{
			// Load depth to world LUT.
			this->setupDepthToWorldTable();

			if (this->bUpdateColor)
			{
				// Load color to world LUT.
				this->setupColorToWorldTable();
			}
		}

		// Check compatible sync mode and connection.
		if (this->config.wired_sync_mode == K4A_WIRED_SYNC_MODE_MASTER && !this->isSyncOutConnected())
		{
			ofLogWarning(__FUNCTION__) << "Wired sync mode set to Master but Sync Out not connected! Reverting to Standalone.";
			this->config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
		}
		else if (this->config.wired_sync_mode == K4A_WIRED_SYNC_MODE_SUBORDINATE && !this->isSyncInConnected())
		{
			ofLogWarning(__FUNCTION__) << "Wired sync mode set to Subordinate but Sync In not connected! Reverting to Standalone.";
			this->config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
		}

		if (this->config.wired_sync_mode != K4A_WIRED_SYNC_MODE_SUBORDINATE)
		{
			this->config.subordinate_delay_off_master_usec = 0;
		}

		// Start cameras.
		try
		{
			this->device.start_cameras(&this->config);
		}
		catch (const k4a::error& e)
		{
			ofLogError(__FUNCTION__) << e.what();
			return false;
		}

		return this->startStreaming();
	}

	bool Device::stopCameras()
	{
		if (!this->bStreaming) return false;

		this->stopStreaming();

		this->depthToWorldImg.reset();
		this->transformation.destroy();

		this->device.stop_cameras();

		return true;
	}

	bool Device::startRecording(std::string filepath)
	{
		if (!this->bOpen) return false;

		if (this->isRecording())
		{
			this->stopRecording();
		}

		if (filepath.empty())
		{
			filepath = "k4a_" + ofGetTimestampString("%Y%m%d_%H%M%S") + ".mkv";
		}

		if (this->recorder.open(this->device, this->config, filepath))
		{
			this->bRecording = true;
		}

		return this->bRecording;
	}

	bool Device::stopRecording()
	{
		if (!this->isRecording()) return false;

		this->recorder.close();
		this->bRecording = false;

		return this->bRecording;
	}

	bool Device::isSyncInConnected() const
	{
		return this->device.is_sync_in_connected();
	}

	bool Device::isSyncOutConnected() const
	{
		return this->device.is_sync_out_connected();
	}

	bool Device::updateCapture()
	{
		try
		{
			if (this->device.get_capture(&this->capture, std::chrono::milliseconds(TIMEOUT_IN_MS)))
			{
				return true;
			}
			else
			{
				ofLogWarning(__FUNCTION__) << "Timed out waiting for a capture for device " << this->index << "::" << this->serialNumber << ".";
				return false;
			}
		}
		catch (const k4a::error& e)
		{
			ofLogError(__FUNCTION__) << e.what();
			return false;
		}
	}

	void Device::updatePixels()
	{
		Stream::updatePixels();

		if (this->bRecording)
		{
			this->recorder.writeCapture(this->capture);
		}
	}

	bool Device::isRecording() const
	{
		return this->recorder.isOpen() && this->bRecording;
	}

	DepthMode Device::getDepthMode() const
	{
		return this->config.depth_mode;
	}

	ImageFormat Device::getColorFormat() const
	{
		return this->config.color_format;
	}

	ColorResolution Device::getColorResolution() const
	{
		return this->config.color_resolution;
	}

	FramesPerSecond Device::getCameraFps() const
	{
		return this->config.camera_fps;
	}

	WiredSyncMode Device::getWiredSyncMode() const
	{
		return this->config.wired_sync_mode;
	}

	uint32_t Device::getDepthDelayUsec() const
	{
		return this->config.depth_delay_off_color_usec;
	}

	uint32_t Device::getSubordinateDelayUsec() const
	{
		return this->config.subordinate_delay_off_master_usec;
	}

	bool Device::getSyncImages() const
	{
		return this->config.synchronized_images_only;
	}

	const Recorder& Device::getRecorder() const
	{
		return this->recorder;
	}

	Recorder& Device::getRecorder()
	{
		return this->recorder;
	}
}
