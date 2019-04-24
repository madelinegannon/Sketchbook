#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxJSON.h"
#include "Skeleton.h" 

class SensorController
{
public:
	SensorController();
	~SensorController();

	void setup(bool _use_live);
	void update();
	void draw();
    
    ofNode get_attachment_node();
    ofNode get_target_node();

    ofParameterGroup params;
    ofParameter<bool> attach_geom;
    ofParameter<bool> attach_target;
    
    vector<string> filenames = { "example_kinectron_skeleton_wave_walk.json" };

private:
    
    bool use_live = false;

    void setup_gui();
    
    bool convert_mm = true;
    bool convert_world_coords = true;
    
    ofxJSONElement result;
    void setup_canned_data(string filename);
    void update_canned_data();
    
    vector<Skeleton*> canned;
    
    int counter = 0;
    int start_frame, end_frame;
    ofVec3f world_offset;
    
    void listener_attach_geom(bool &val);
    void listener_attach_target(bool &val);

};

