#pragma once

#include "ofMain.h"

#include "ofxAzureKinect.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void exit();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxAzureKinect::Device device;
	void setup_device();

	bool streaming = false;

	void pin_skeleton(int index, glm::vec3 pt);


private:
	// Playback Params
	string filename;
	// bool play = true;
	float play_head = 0;

	// 3D Visualization
	ofEasyCam cam;
	// ofxGizmo gizmo;

	void setup_world();
	void draw_world();

	void setup_gui();
	void draw_gui();

	void setup_camera();
	void keypressed_camera(int key);

	ofxPanel panel;
	ofParameterGroup params;
	ofParameter<bool> show_gui, show_top, show_front, show_side, show_perspective;
	void listener_show_top(bool &val);
	void listener_show_front(bool &val);
	void listener_show_side(bool &val);
	void listener_show_perspective(bool &val);

	void setup_gui_playback();
	ofxPanel panel_playback;
	ofParameterGroup params_playback;
	ofParameter<bool> play;
	ofParameter<float> seek;

	void listener_play(bool &val);
	void listener_seek(float &val);

	void setup_gui_body_tracking();
	ofxPanel panel_body_tracking;
	ofParameterGroup params_body_tracking;
	ofParameter<bool> show_bodies;
	ofParameter<bool> show_skeltons;
	ofParameter<bool> show_body_map;
	ofParameter<float> joint_smoothing;

	void listener_joint_smoothing(float &val);

	ofColor background_inner = ofColor(238);
	ofColor background_outer = ofColor(118);
};
