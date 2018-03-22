#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGizmo.h"
#include "ofxRay.h"
#include "Robot.h"
#include "Tracker.h"
#include "Agent.h"


// listen on port 9001
#define PORT 9001
#define MAD_PC 10.2.163.78


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

		
		
		
		/////////////////////////////////////////
		// COMS

		// communicating with Minitaur over Serial
		ofSerial serial;
		string COM_PORT = "COM9";
		int baud = 115200;

		void checkForMessages();
		void sendMessage(string msg);
		string incomingMsg;
		float sendDelay = 0;
		float tLastMsg = 0;

		// communicating with Vive over OSC
		ofxOscSender sender;
		string hostname = "10.2.163.78";
		int senderPort = PORT - 1;
		bool bIsConnected = false;
		void tryReconnect();

		ofxOscReceiver receiver;
		void checkForMessages_Vive();

		
		/////////////////////////////////////////
		// Minitaur

		Robot robot;
		bool lookAtTracker = false;
		
		/////////////////////////////////////////
		// Vive Trackers

		map<int, Tracker> trackers;
		ofxGizmo gizmo_lookAt;
		ofMesh trackerMesh;

		/////////////////////////////////////////

		/////////////////////////////////////////
		// Agents

		Agent agent;

		/////////////////////////////////////////
		

		
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
		ofxPanel panel_robot;
		ofxPanel panel_agent;

		ofParameterGroup params_com;
		ofParameter<string> msg_status;
		ofParameter<string> msg_listening;

		ofParameterGroup params_osc;
		ofParameter<string> msg_status_osc;
		ofParameter<string> msg_listening_osc;
		ofxButton reconnect;
		void reconnectBtnPressed();
		
		//ofParameterGroup params_gui;
		//ofxButton save;
		//void saveBtnPressed();

		ofParameterGroup params_tracking;
		map<int, ofParameter<bool> > trackingIDs;
		

		void drawViewport(int id);
		void drawViewports();
		void findActiveViewportID();
		void handleViewportPresets(int key);
		void handleCameraPresets(int key);


		/////////////////////////////////////////
		// Navigation

		// 3D Navigation
		vector<ofEasyCam*> cams;

		//ofxGizmo gizmo;
		//ofQuaternion prevGizmoRotation;
		void handleGizmoPresets(int key);

		// checking for double click
		unsigned long lastTap;
		bool bDoubleClick = false;

		
		
		/////////////////////////////////////////
		// UTILS
		void loadModels();
		ofxAssimpModelLoader loader;

		void printmap(map<int, string> & m);

};
