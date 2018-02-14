#pragma once

#include "ofMain.h"
#include "ofxOpenVR.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxOpenVR\libs\glm\include\glm\mat4x4.hpp"

#define HOST "10.2.169.143"
#define PORT 7777

//--------------------------------------------------------------
class ofApp : public ofBaseApp {

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

	void render(vr::Hmd_Eye nEye);

	void controllerEvent(ofxOpenVRControllerEventArgs& args);

	ofMatrix4x4 toOf(const glm::mat4 &mat) {
		return reinterpret_cast<const ofMatrix4x4&>(mat);
	}


	//----------------------------------------
	// OSC Server:

	bool debugOsc = true;

	ofxOscReceiver serverReceiver; // OSC receiver
	int serverRecvPort = 9000;				// port we're listening on: must match port from sender!
	int serverSendPort = serverRecvPort+1;	// port we're SENDING on


						// Message display variables
	vector<string>serverMessages; //vector containing the received messages for display
	unsigned int maxServerMessages; //nr of messages fitting on the screen

	void checkForNewClients();
	vector<string>knownClients; //collected IP's of chat participants
	ofxOscSender serverSender;

	//Distribute a received message among the known hosts
	void broadcastToClients();


	//----------------------------------------
	// GUI:
	// send Tracking info
	ofParameter<bool> sendTracking = true;
	ofParameter<bool> sendLeftController = true;
	ofParameter<bool> sendRightController = true;

private:

	ofxOpenVR openVR;

	ofImage _texture;
	ofBoxPrimitive _box;
	ofMatrix4x4 _translateMatrix;
	ofShader _shader;

	ofBoxPrimitive _controllerBox;
	ofShader _controllersShader;
};
