
#include "ofMain.h"
#include "ofApp.h"
#include <k4a/k4atypes.h>
#include <k4arecord/types.h>


void do_record_imu(k4a_device_t *device, k4a_record_t *recording)
{

	k4a_wait_result_t result;

	// Loop to get the queued IMU samples after every capture.
	// We kick out of the loop when result returns K4A_WAIT_RESULT_TIMEOUT
	do
	{
		k4a_imu_sample_t sample;
		result = k4a_device_get_imu_sample(*device, &sample, 0);
		if (result == K4A_WAIT_RESULT_SUCCEEDED)
		{
			// Write the IMU data to file
			k4a_result_t write_result = k4a_record_write_imu_sample(*recording, sample);
			if (K4A_FAILED(write_result))
			{
				std::cerr << "Runtime error: k4a_record_write_imu_sample() returned " << write_result << std::endl;
				break;
			}
		}
		else if (result == K4A_WAIT_RESULT_TIMEOUT)
		{
			// Indicates that there are no queued samples and none have arrived in the timeout specified.
			break;
		}
		else
		{
			std::cerr << "Runtime error: k4a_device_get_imu_sample() returned " << result << std::endl;
			break;
		}

		// printf(" | Accelerometer temperature:%.2f x:%.4f y:%.4f z: %.4f\n",
		// 	   sample.temperature,
		// 	   sample.acc_sample.xyz.x,
		// 	   sample.acc_sample.xyz.y,
		// 	   sample.acc_sample.xyz.z);

	} while (result != K4A_WAIT_RESULT_FAILED);
}

void do_record_capture(k4a_device_t *device, k4a_record_t *recording, bool record_imu)
{
	k4a_capture_t capture;
	k4a_wait_result_t result = k4a_device_get_capture(*device, &capture, K4A_WAIT_INFINITE);
	if (result == K4A_WAIT_RESULT_SUCCEEDED)
	{
		// Write the capture to the built-in tracks
		k4a_record_write_capture(*recording, capture);

		// Write the capture for any other custom tracks (not imu; do that after releasing the capture)
		// ...

		k4a_capture_release(capture);
	}
	else if (result == K4A_WAIT_RESULT_TIMEOUT)
	{
		// TIMEOUT should never be returned when K4A_WAIT_INFINITE is set.
		printf("k4a_device_get_capture() timed out!\n");
		return;
	}
	else
	{
		printf("k4a_device_get_capture() returned error: %d\n", result);
		return;
	}

	// Handle the IMU data

	// Code from k4arecorder (Line 213)
	// https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/master/tools/k4arecorder/recorder.cpp
	// and https://docs.microsoft.com/en-us/azure/kinect-dk/retrieve-imu-samples

	if (record_imu)
	{
		do_record_imu(device, recording);
	}

	// Indicate that we are recording
	cout << ".";
	cout.flush();
}


//========================================================================
int main()
{
	ofSetupOpenGL(1024, 768, OF_WINDOW); // <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

/*
	// Testing Headless Recording code
	
	// Set up and start the kinect

	char *recording_filename = "output.mkv";

	k4a_device_configuration_t device_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	device_config.depth_mode = K4A_DEPTH_MODE_NFOV_2X2BINNED;
	device_config.synchronized_images_only = true;
	device_config.color_resolution = K4A_COLOR_RESOLUTION_1080P;
	device_config.synchronized_images_only = true;
	device_config.camera_fps = K4A_FRAMES_PER_SECOND_30;

	k4a_device_t device;
	k4a_device_open(0, &device);
	k4a_device_start_cameras(device, &device_config);

	bool record_imu = true;
	if (record_imu)
	{
		k4a_device_start_imu(device);
	}

	printf("Device started\n");

	// Set up the default recording tracks

	k4a_record_t recording;
	if (K4A_FAILED(k4a_record_create(recording_filename, device, device_config, &recording)))
	{
		printf("Unable to create recording file: %s\n", recording_filename);
		return 1;
	}

	// Add any tracks and metadata you want to record
	if (record_imu)
	{
		k4a_record_add_imu_track(recording);
	}

	cout << "Recording Started ";
	cout.flush();

	// Write the recording header after all the track metadata is set up.
	k4a_record_write_header(recording);

	int recording_time = 10 * 30; // n seconds * 30FPS
	for (int i = 0; i < recording_time; i++)
	{
		do_record_capture(&device, &recording, record_imu);
	}

	cout << endl
		 << "Saving recording to: " << recording_filename << endl;
	k4a_record_flush(recording);
	k4a_record_close(recording);
	// Stop the cameras and write out the recording
	if (record_imu)
	{
		k4a_device_stop_imu(device);
	}
	k4a_device_stop_cameras(device);

	// Shutdown the device and exit
	printf("Recording Done\n");
	k4a_device_close(device);

	return 0;
*/
}
