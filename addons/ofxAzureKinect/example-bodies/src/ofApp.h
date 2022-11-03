#pragma once

#include "ofMain.h"

#include "ofxAzureKinect.h"
#include "ofxNetwork.h"

#define HOST "localhost"
#define PORT 12345

class ofApp 
	: public ofBaseApp 
{
public:
	void setup();
	void exit();

	void update();
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

private:
	ofxAzureKinect::Device kinectDevice;

	ofEasyCam camera;

	ofVbo pointsVbo;
	ofShader shader;

	ofVboMesh skeletonMesh;

	// Communication Parameters
	ofxUDPManager udpConnection;
	void setup_comms();
	void send_message(k4abt_skeleton_t body, int id);

	ofJson data;

	int get_closest_body(vector<k4abt_skeleton_t> bodies);

	string k4abt_joint_names[K4ABT_JOINT_COUNT] = {
		"K4ABT_JOINT_PELVIS",
		"K4ABT_JOINT_SPINE_NAVEL",
		"K4ABT_JOINT_SPINE_CHEST",
		"K4ABT_JOINT_NECK",
		"K4ABT_JOINT_CLAVICLE_LEFT",
		"K4ABT_JOINT_SHOULDER_LEFT",
		"K4ABT_JOINT_ELBOW_LEFT",
		"K4ABT_JOINT_WRIST_LEFT",
		"K4ABT_JOINT_HAND_LEFT",
		"K4ABT_JOINT_HANDTIP_LEFT",
		"K4ABT_JOINT_THUMB_LEFT",
		"K4ABT_JOINT_CLAVICLE_RIGHT",
		"K4ABT_JOINT_SHOULDER_RIGHT",
		"K4ABT_JOINT_ELBOW_RIGHT",
		"K4ABT_JOINT_WRIST_RIGHT",
		"K4ABT_JOINT_HAND_RIGHT",
		"K4ABT_JOINT_HANDTIP_RIGHT",
		"K4ABT_JOINT_THUMB_RIGHT",
		"K4ABT_JOINT_HIP_LEFT",
		"K4ABT_JOINT_KNEE_LEFT",
		"K4ABT_JOINT_ANKLE_LEFT",
		"K4ABT_JOINT_FOOT_LEFT",
		"K4ABT_JOINT_HIP_RIGHT",
		"K4ABT_JOINT_KNEE_RIGHT",
		"K4ABT_JOINT_ANKLE_RIGHT",
		"K4ABT_JOINT_FOOT_RIGHT",
		"K4ABT_JOINT_HEAD",
		"K4ABT_JOINT_NOSE",
		"K4ABT_JOINT_EYE_LEFT",
		"K4ABT_JOINT_EAR_LEFT",
		"K4ABT_JOINT_EYE_RIGHT",
		"K4ABT_JOINT_EAR_RIGHT"
	};


};
