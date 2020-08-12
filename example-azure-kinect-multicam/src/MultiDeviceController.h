#pragma once

#include "ofMain.h"

#include <k4a/k4a.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "transformation.h"
#include "MultiDeviceCapturer.h"

class MultiDeviceController
{
public:
    MultiDeviceController(){};
    MultiDeviceController(vector<string> serial_numbers);

    void setup();
    void update();
    void draw();
    bool calibrate();

private:
    vector<string> serial_numbers;

    MultiDeviceCapturer capturer;
    k4a_device_configuration_t main_config;
    k4a_device_configuration_t secondary_config;

    k4a::calibration main_calibration;
    k4a::transformation main_depth_to_main_color;
    k4a::calibration secondary_calibration;
    k4a::transformation secondary_depth_to_main_color;

    cv::Mat background_image;
    cv::Mat output_image;

    vector<ofPixels> capture_color_pixels;
    vector<ofTexture> capture_color_textures;
    vector<ofShortPixels> capture_depth_pixels;
    vector<ofTexture> capture_depth_textures;
    vector<ofShortPixels> capture_ir_pixels;
    vector<ofTexture> capture_ir_textures;
    vector<ofVbo> capture_point_clouds;

    vector<ofPixels> colorized_depth_pixels;
    vector<ofTexture> colorized_depth_textures;
    vector<ofPixels> colorized_ir_pixels;
    vector<ofTexture> colorized_ir_textures;

    void k4a_color_image_to_OF(k4a::image &img, ofPixels *pix, ofTexture *tex);
    void k4a_depth_image_to_OF(k4a::image &img, ofShortPixels *pix, ofTexture *tex);
    void k4a_colorized_depth_image_to_OF(k4a::image &img, ofPixels *pix, ofTexture *tex, bool grayscale = false);
    ofColor colorize_grayscale(uint16_t &value, uint16_t &min, uint16_t &max);
    ofColor colorize_red2blue(uint16_t &value, uint16_t &min, uint16_t &max);

    float chessboard_square_length = 25.; // must be included in the input params
    int32_t color_exposure_usec = 8000;   // somewhat reasonable default exposure time
    int32_t powerline_freq = 2;           // default to a 60 Hz powerline
    cv::Size chessboard_pattern;          // height, width. Both need to be set.
    uint16_t depth_threshold = 2000;      // default to 1 meter
    size_t num_devices = 0;
    double calibration_timeout = 60.0;                                // default to timing out after 60s of trying to get calibrated
    double greenscreen_duration = std::numeric_limits<double>::max(); // run forever

    vector<uint32_t> device_indices{0}; // Set up a MultiDeviceCapturer to handle getting many synchronous captures
                                        // Note that the order of indices in device_indices is not necessarily
                                        // preserved because MultiDeviceCapturer tries to find the master device based
                                        // on which one has sync out plugged in. Start with just { 0 }, and add
                                        // another if needed
};

// Allowing at least 160 microseconds between depth cameras should ensure they do not interfere with one another.
constexpr uint32_t MIN_TIME_BETWEEN_DEPTH_CAMERA_PICTURES_USEC = 160;

// ideally, we could generalize this to many OpenCV types
static cv::Mat color_to_opencv(const k4a::image &im);
static cv::Mat depth_to_opencv(const k4a::image &im);
static cv::Matx33f calibration_to_color_camera_matrix(const k4a::calibration &cal);
static Transformation get_depth_to_color_transformation_from_calibration(const k4a::calibration &cal);
static k4a::calibration construct_device_to_device_calibration(const k4a::calibration &main_cal,
                                                               const k4a::calibration &secondary_cal,
                                                               const Transformation &secondary_to_main);
static vector<float> calibration_to_color_camera_dist_coeffs(const k4a::calibration &cal);
static bool find_chessboard_corners_helper(const cv::Mat &main_color_image,
                                           const cv::Mat &secondary_color_image,
                                           const cv::Size &chessboard_pattern,
                                           vector<cv::Point2f> &main_chessboard_corners,
                                           vector<cv::Point2f> &secondary_chessboard_corners);
static Transformation stereo_calibration(const k4a::calibration &main_calib,
                                         const k4a::calibration &secondary_calib,
                                         const vector<vector<cv::Point2f>> &main_chessboard_corners_list,
                                         const vector<vector<cv::Point2f>> &secondary_chessboard_corners_list,
                                         const cv::Size &image_size,
                                         const cv::Size &chessboard_pattern,
                                         float chessboard_square_length);
static k4a_device_configuration_t get_master_config();
static k4a_device_configuration_t get_subordinate_config();
static Transformation calibrate_devices(MultiDeviceCapturer &capturer,
                                        const k4a_device_configuration_t &main_config,
                                        const k4a_device_configuration_t &secondary_config,
                                        const cv::Size &chessboard_pattern,
                                        float chessboard_square_length,
                                        double calibration_timeout);
static k4a::image create_depth_image_like(const k4a::image &im);