#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class Trajectory {
private:
    float time_last = 0;
    float time_diff = 1 / 60.;
    float circumference = 314.0;
    float max_vel_dist = 10.0;     // max distance per time step (based on max velocity)
    float rpm = 0.0;

    ofVec3f accel;
    ofVec3f heading;
    ofVec3f vel_projected; // curr_vel projected onto the heading vector

    void update_desired_vel();
    void update_current_vel();
    void compute_rpm(ofVec3f prev_pos, ofVec3f curr_pos);
    void update_position();

    void setup_gui();
    float get_max_dist_from_vel(float max_vel);
public:
    
    Trajectory();

    ofVec3f desired_pos, desired_vel, curr_pos, curr_vel;
    ofPolyline path;
    
    void update();
    void update(glm::vec3 pos);
    void draw(bool show_debug = true);
    void reset(glm::vec3 start=glm::vec3());
    void add_target(glm::vec3 target);
    
    glm::vec3 get_curr_pos() { return glm::vec3(curr_pos); }
    glm::vec3 get_heading() { return glm::vec3(heading.getNormalized()); }
    glm::vec3 get_last_target();
    int get_num_targets() { return path.getVertices().size(); }
    float get_rpm() { return rpm; }

    ofxPanel panel;
    ofParameterGroup params;
    ofParameterGroup params_pid;
    ofParameterGroup params_rpm;
    ofParameterGroup params_info;
    ofParameter<float> kp, kd;
    ofParameter<float> steering_scalar;
    ofParameter<float> wheel_diameter;
    ofParameter<float> look_ahead_radius;
    ofParameter<float> max_vel;
    ofParameter<string> info_max_vel_dist;
    ofParameter<string> info_rpm;

    void on_wheel_diameter_changed(float& val);
    void on_max_vel_changed(float& val);
    
};