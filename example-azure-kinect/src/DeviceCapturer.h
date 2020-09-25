#pragma once

#include "ofMain.h"
#include "k4a.h"

constexpr int64_t TIMEOUT_IN_MS = 1000;

class DeviceCapturer : ofThread
{

public:
    DeviceCapturer();

    void setup();
    void stop();

    ofTexture *get_color_texture() { return &color_tex; }
    ofTexture *get_depth_texture() { return &depth_tex; }
    ofTexture *get_ir_texture() { return &ir_tex; }

private:
    void open();
    void start();
    void update(ofEventArgs &args);
    void threadedFunction();

    void get_capture();

    void update_pixels_color(k4a_image_t image);
    void update_pixels_depth(k4a_image_t image);
    void update_pixels_ir(k4a_image_t image);

    void update_textures(ofPixels *pix_color, ofShortPixels *pix_depth, ofShortPixels *pix_ir);

    ofParameter<bool> retrieve_color;
    ofParameter<bool> retrieve_depth;
    ofParameter<bool> retrieve_ir;

    ofParameter<bool> get_raw;

    k4a_device_t device = NULL;
    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;

    ofPixels raw_color_pixels;
    ofShortPixels raw_depth_pixels;
    ofShortPixels raw_ir_pixels;

    ofShortPixels depth_pixels;
    ofShortPixels ir_pixels;

    ofTexture color_tex;
    ofTexture depth_tex;
    ofTexture ir_tex;

    std::condition_variable condition;
    int frame_num_pix = 0;
    int frame_num_tex = 0;
};