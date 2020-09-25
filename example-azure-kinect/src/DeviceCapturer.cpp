#include "DeviceCapturer.h"

DeviceCapturer::DeviceCapturer(){};

void DeviceCapturer::setup()
{

    // check for connected devices
    uint32_t device_count = k4a_device_get_installed_count();
    if (device_count == 0)
    {
        ofLogError() << "No Devices Connected" << endl;
        return;
    }
    else
    {
        printf("Found %d connected devices:\n", device_count);
    }

    retrieve_color.set("Color", true);
    retrieve_depth.set("Depth", true);
    retrieve_ir.set("IR", true);
    get_raw.set("Get_Raw", true);

    open();
}

void DeviceCapturer::update(ofEventArgs &args)
{
    // get_capture(); // move to its own thread?

    if (frame_num_pix != frame_num_tex)
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        if (get_raw)
        {
            update_textures(&raw_color_pixels, &raw_depth_pixels, &raw_ir_pixels);
        }
        else
        {
            update_textures(&raw_color_pixels, &depth_pixels, &ir_pixels);
        }
        this->condition.notify_all();
    }
}

void DeviceCapturer::threadedFunction()
{
    while (this->isThreadRunning())
    {
        std::unique_lock<std::mutex> lock(this->mutex);
        while (this->isThreadRunning() && frame_num_pix != frame_num_tex)
        {
            this->condition.wait(lock);
        }
        get_capture();
    }
}

void DeviceCapturer::stop()
{
    ofRemoveListener(ofEvents().update, this, &DeviceCapturer::update);

    std::unique_lock<std::mutex> lock(this->mutex);
    this->stopThread();
    this->condition.notify_all();

    k4a_device_stop_cameras(device);
}

void DeviceCapturer::open()
{
    // try opening the default device
    if (K4A_RESULT_SUCCEEDED != k4a_device_open(K4A_DEVICE_DEFAULT, &device))
    {
        printf("Failed to open device\n");
        return;
    }

    start();
}

void DeviceCapturer::start()
{
    // setup a default configuration
    config.camera_fps = K4A_FRAMES_PER_SECOND_30;
    config.depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution = K4A_COLOR_RESOLUTION_720P;

    // try starting the device
    if (K4A_RESULT_SUCCEEDED != k4a_device_start_cameras(device, &config))
    {
        printf("Failed to start device\n");
        return;
    }
    else
    {
        cout << "Started..." << endl;
    }

    ofAddListener(ofEvents().update, this, &DeviceCapturer::update);
    this->startThread();
}

void DeviceCapturer::get_capture()
{
    // get a capture
    k4a_capture_t capture;
    switch (k4a_device_get_capture(device, &capture, TIMEOUT_IN_MS))
    {
    case K4A_WAIT_RESULT_SUCCEEDED:
        break;
    case K4A_WAIT_RESULT_TIMEOUT:
        printf("Timed out waiting for a capture\n");
        k4a_device_stop_cameras(device);
        // k4a_device_close(device);
        exit(0);
    case K4A_WAIT_RESULT_FAILED:
        printf("Failed to read a capture\n");
        k4a_device_stop_cameras(device);
        // k4a_device_close(device);
        exit(0);
        // return;
    }

    // retrieve images from capture

    if (retrieve_color)
    {
        // Access the color image
        k4a_image_t image = k4a_capture_get_color_image(capture);
        if (image)
        {
            // printf(" | Color res:%4dx%4d stride:%5d ",
            //        k4a_image_get_height_pixels(image),
            //        k4a_image_get_width_pixels(image),
            //        k4a_image_get_stride_bytes(image));

            update_pixels_color(image);
            k4a_image_release(image);
            frame_num_pix = ofGetFrameNum();
        }
        else
        {
            printf(" | Color None                       ");
        }
    }

    if (retrieve_depth)
    {
        // Access the depth16 image
        k4a_image_t image = k4a_capture_get_depth_image(capture);
        if (image != NULL)
        {
            // printf(" | Depth16 res:%4dx%4d stride:%5d\n",
            //        k4a_image_get_height_pixels(image),
            //        k4a_image_get_width_pixels(image),
            //        k4a_image_get_stride_bytes(image));

            update_pixels_depth(image);
            k4a_image_release(image);
            frame_num_pix = ofGetFrameNum();
        }
        else
        {
            printf(" | Depth None                       ");
        }
    }

    if (retrieve_ir)
    {
        // Access the ir image
        k4a_image_t image = k4a_capture_get_ir_image(capture);
        if (image != NULL)
        {
            // printf(" | IR res:%4dx%4d stride:%5d\n",
            //        k4a_image_get_height_pixels(image),
            //        k4a_image_get_width_pixels(image),
            //        k4a_image_get_stride_bytes(image));

            update_pixels_ir(image);
            k4a_image_release(image);
            frame_num_pix = ofGetFrameNum();
        }
        else
        {
            printf(" | IR None                       ");
        }
    }

    // release the capture
    k4a_capture_release(capture);
}

void DeviceCapturer::update_pixels_color(k4a_image_t image)
{
    auto dims = glm::ivec2(k4a_image_get_width_pixels(image), k4a_image_get_height_pixels(image));
    int num_channels = 4;

    // Update the pixel array
    if (config.color_format == K4A_IMAGE_FORMAT_COLOR_BGRA32)
    {
        if (!raw_color_pixels.isAllocated())
        {
            raw_color_pixels.allocate(dims.x, dims.y, OF_PIXELS_BGRA);
        }
        raw_color_pixels.setFromPixels(reinterpret_cast<uint8_t *>(k4a_image_get_buffer(image)), dims.x, dims.y, num_channels);
    }
}

void DeviceCapturer::update_pixels_depth(k4a_image_t image)
{
    auto dims = glm::ivec2(k4a_image_get_width_pixels(image), k4a_image_get_height_pixels(image));
    int num_channels = 1;

    ofShortPixels *pix;
    if (get_raw)
    {
        pix = &raw_depth_pixels;
    }
    else
    {
        pix = &depth_pixels;
        num_channels = 4;
    }
    // Update the pixel array
    if (!pix->isAllocated() || pix->getNumChannels() != num_channels)
    {
        pix->clear();
        pix->allocate(dims.x, dims.y, num_channels);
    }
    pix->setFromPixels(reinterpret_cast<uint16_t *>(k4a_image_get_buffer(image)), dims.x, dims.y, num_channels);
}

void DeviceCapturer::update_pixels_ir(k4a_image_t image)
{
    auto dims = glm::ivec2(k4a_image_get_width_pixels(image), k4a_image_get_height_pixels(image));
    int num_channels = 1;

    ofShortPixels *pix;
    if (get_raw)
    {
        pix = &raw_ir_pixels;
    }
    else
    {
        pix = &ir_pixels;
    }
    // Update the pixel array
    if (!pix->isAllocated())
    {
        pix->allocate(dims.x, dims.y, num_channels);
    }
    pix->setFromPixels(reinterpret_cast<uint16_t *>(k4a_image_get_buffer(image)), dims.x, dims.y, num_channels);
}

void DeviceCapturer::update_textures(ofPixels *pix_color, ofShortPixels *pix_depth, ofShortPixels *pix_ir)
{
    if (retrieve_color)
    {
        if (!color_tex.isAllocated() && pix_color->isAllocated())
        {
            color_tex.allocate(*pix_color);
            color_tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

            if (config.color_format == K4A_IMAGE_FORMAT_COLOR_BGRA32)
            {
                color_tex.bind();
                {
                    glTexParameteri(color_tex.texData.textureTarget, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
                    glTexParameteri(color_tex.texData.textureTarget, GL_TEXTURE_SWIZZLE_B, GL_RED);
                }
                color_tex.unbind();
            }
        }
        color_tex.loadData(*pix_color);
    }
    if (retrieve_depth)
    {
        ofShortPixels *pix;
        if (get_raw)
        {
            pix = &raw_depth_pixels;
        }
        else
        {
            pix = &depth_pixels;
        }

        if (!depth_tex.isAllocated() && pix->isAllocated())
        {
            depth_tex.allocate(*pix);
            depth_tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

            if (config.color_format == K4A_IMAGE_FORMAT_COLOR_BGRA32)
            {
                depth_tex.bind();
                {
                    glTexParameteri(depth_tex.texData.textureTarget, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
                    glTexParameteri(depth_tex.texData.textureTarget, GL_TEXTURE_SWIZZLE_B, GL_RED);
                }
                depth_tex.unbind();
            }
        }
        depth_tex.loadData(*pix);
    }
    if (retrieve_ir)
    {
        ofShortPixels *pix;
        if (get_raw)
        {
            pix = &raw_ir_pixels;
        }
        else
        {
            pix = &ir_pixels;
        }
        if (!ir_tex.isAllocated() && pix->isAllocated())
        {
            ir_tex.allocate(*pix);
            ir_tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

            if (config.color_format == K4A_IMAGE_FORMAT_COLOR_BGRA32)
            {
                ir_tex.bind();
                {
                    glTexParameteri(ir_tex.texData.textureTarget, GL_TEXTURE_SWIZZLE_R, GL_BLUE);
                    glTexParameteri(ir_tex.texData.textureTarget, GL_TEXTURE_SWIZZLE_B, GL_RED);
                }
                ir_tex.unbind();
            }
        }
        ir_tex.loadData(*pix);
    }
    // update the pixel frame number
    frame_num_tex = frame_num_pix;
}
