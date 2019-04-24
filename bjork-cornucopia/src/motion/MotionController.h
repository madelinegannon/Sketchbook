#pragma once

#include "ofMain.h"
#include "ofxGizmo.h"
#include "ofxOsc.h"
#include "ofxEasing.h"
#include "ofxGui.h"

class MotionController
{
public:
	MotionController();
	~MotionController();

	void setup(int num_agents);
	void update();
	void draw(ofEasyCam * cam);
    void keyPressed(int key);

	// targets
    void update_gizmos(int view_dim_x, int view_dim_y);

    vector<ofMatrix4x4*> get_targets() { return targets; }
    
    void attach_to_node_base_geomety(ofNode node);
    void attach_to_node_targets(ofNode node);
    void set_sensor_target(ofVec3f tgt);
   

    enum MOTION_MODE {
        PAUSE=0,
        RUN,
        HOME,
        TIMELINE
    };
    int get_motion_mode() { return mode; }
//    void set_mode(int _mode) { mode = _mode; }

    bool has_gizmos_isInteracting();

    //ofParameterGroup get_gui() { return params; }
    void draw_gui(int x, int y);
    void reset_all();
    void reset(int i);

    enum MOTION_MODIFIER {
        NONE,
        NOISE,
        SINE,
        CURVE
    };
  
    ofParameterGroup params;
    ofParameterGroup params_modifiers;
    ofParameterGroup params_positions;
    vector<ofParameter<ofVec3f>> positions;
    ofParameter<ofVec3f> halo_pos;

private:
    int NUM_AGENTS;

    vector<ofMatrix4x4*> targets;
    vector<ofxGizmo*> gizmos;
    
    // ----------- BASE GEOM FOR CONTROLLER -----------
    void setup_base_geometry();
    void draw_halo();
    ofPolyline halo;
    
    ofParameter<float> halo_radius;
    ofParameter<float> halo_spin;
//    ofParameter<ofVec3f> halo_pos;
    void listener_halo_radius(float &val);
    void listener_halo_pos(ofVec3f &val);
    
    ofVec3f sensor_target;


	// whether we are streaming RobotState info to robots
	bool isStreaming = true;

    void update_mode();
	int mode = 0;
	int modifier = 0;

	void setup_gizmos();
	void draw_gizmos(ofEasyCam * cam);

	ofxOscReceiver receiver;
	void setup_osc();
	void update_osc();

	vector<ofVec3f> motion_targets;
	void setup_motion_targets();
	void update_motion_targets();
	void draw_motion_targets();

	void add_motion_modifiers(); // <-- add_behaviors()?
	ofVec3f add_noise(int i, float _speed, float _scalar);
	ofVec3f add_sine(int i, float _speed, float _amplitude, float _frequency, float _scalar);

	void setup_gui();
	ofxPanel panel;
	
	ofParameter<bool> show_gizmos;
	ofParameter<string> mode_string;
	ofParameter<bool> use_sensors;
    ofParameter<bool> use_geometry;
	ofParameter<bool> _reset_all;
    
    // ... I don't think I use these anymore
//    ofParameter<ofVec3f> home_target;

	
	ofParameter<bool> use_noise;
	ofParameter<float> noise_scalar, noise_speed;
	ofParameter<bool> use_sine;
	ofParameter<bool> sine_preset_idle;
	ofParameter<float> sine_amplitude, sine_frequency, sine_speed, sine_scalar;

//    ofParameterGroup params_timers;
//    ofParameter<float> timer_reset_timeout;

    void listener_use_sensors(bool &val);
	void listener_use_noise(bool &val);
	void listener_use_sine(bool &val);
	void listener_sine_preset_idle(bool &val);
	void listener_reset_all(bool &val);

	
	void update_mode_string();

//    ofParameter<int> easing_type;

    //--------------------------------------------------------------
	// for some reason, pointers to ofxGizmo objects aren't initializing properly
	// ... we have to manually add these to the ofxGizmo vectors
	ofxGizmo gizmo_0, gizmo_1, gizmo_2, gizmo_3, gizmo_4, gizmo_5, gizmo_6, gizmo_7, gizmo_8, gizmo_9, gizmo_10, gizmo_11;
	
};

