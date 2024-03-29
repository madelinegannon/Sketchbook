#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "Robot.h"

// listen on port 9001
#define PORT 9001
#define MAD_PC 10.2.164.236
#define NUM_MSG_STRINGS 20

class ofApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();
		void exit();

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
		string hostname = "10.2.169.178";
		int senderPort = PORT - 1;
		bool bIsConnected = false;
		void tryReconnect();

		ofTrueTypeFont font;
		ofxOscReceiver receiver;
		void checkForMessages();

		Robot robot;
		

		// Connecting to OpenVR
		class Tracker : public ofNode{
		public:
			Tracker() {};

			int id;
			ofVec3f pos;
			ofQuaternion orient;
			ofVec3f  scale;

			ofMesh mesh;
			ofPolyline trail;

			bool isConnected = false;


			void update() {

				isConnected = true;

				trail.addVertex(pos);
				if (trail.getVertices().size() > 75)
					trail.getVertices().erase(trail.getVertices().begin());

			};

			void draw() {
				ofPushStyle();
				
				ofPushMatrix();
				ofMultMatrix(this->getGlobalTransformMatrix());
				
				ofFill();
				ofSetColor(ofColor::aquamarine,50);
				//ofDrawBox(50);
				ofNoFill();
				ofSetColor(ofColor::antiqueWhite, 200);
				
				//ofDrawBox(50);
				mesh.drawWireframe();
				ofPopMatrix();

				
				ofSetLineWidth(5);
				ofSetColor(ofColor::paleVioletRed, 100);
				trail.draw();

				ofPopStyle();
			};
		};

		map<int, Tracker> trackers;
		void printmap(map<int, Tracker> & m);

		void drawTracker(int id);

		void loadModels();
		ofxAssimpModelLoader loader;
		ofMesh trackerMesh;
		ofMesh controllerMesh;
		ofMesh hmdMesh;
		
		/////////////////////////////////////////
		// GUI

		void setupViewports();
		bool bLoadFromFile = true;
		void setupGUI();

		bool viewport_showAll = true;
		int viewport_activeID;
		vector<ofRectangle*> viewports;
		vector<string> viewport_labels;

		bool showGUI = true;
		ofxPanel panel;

		ofxPanel panel_osc;
		ofParameterGroup params_osc;
		ofParameter<string> msg_status;
		ofParameter<string> msg_listening;
		ofxButton reconnect;
		void reconnectBtnPressed();

		ofParameterGroup params_tracking;
		map<int, ofParameter<bool> > trackingIDs;
		

		void drawViewports();
		void findActiveViewportID();



		/////////////////////////////////////////
		// Navigation

		// 3D Navigation
		vector<ofEasyCam*> cams;

		// checking for double click
		unsigned long lastTap;
		bool bDoubleClick = false;

		
		
};
