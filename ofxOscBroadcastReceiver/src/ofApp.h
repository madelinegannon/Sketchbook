#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

// listen on port 9001
#define PORT 9001
#define MAD_PC 10.2.169.178
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp {
	public:

		void setup();
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

		// Connect to OSC Server
		ofxOscSender sender;
		int senderPort = PORT - 1;

		ofTrueTypeFont font;
		ofxOscReceiver receiver;

		int current_msg_string;
		string msg_strings[NUM_MSG_STRINGS];
		float timers[NUM_MSG_STRINGS];

		int mouseX, mouseY;
		string mouseButtonState;
    
        ofImage receivedImage;

		// Connecting to OpenVR
		class Tracker {
		public:
			Tracker() {};

			int id;
			ofVec3f pos;
			ofQuaternion orient;
			ofMatrix4x4 mat;

			ofMesh mesh;
			ofBoxPrimitive bb;
		};

		map<int, Tracker> trackers;

		ofEasyCam cam;
		
};
