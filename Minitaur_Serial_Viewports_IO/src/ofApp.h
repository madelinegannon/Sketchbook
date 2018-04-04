#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGizmo.h"
#include "ofxRay.h"
#include "ofxBullet.h"
#include "Robot.h"
#include "Tracker.h"
#include "Agent.h"
//#include "SerialThread.h"


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
		
		// ... not working yet :(
		//SerialThread serialThread;
		//string *msgOut;
		//string sendMsg = "";
		//string *msgIn;

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
		// Bullet
		void setupPhysics();
		void drawPhysics();

		ofxBulletWorldRigid	world;
		ofxBulletBox ground;
		ofxBulletCone* fulcrum;
		btTransform tr_fulcrum;
		vector<ofxBulletRigidBody*> anchors;
		vector<ofxBulletJoint*>	joints;
		float jointLength = .3;
		vector <ofxBulletBox*> bounds;
		ofxBulletCustomShape* boundsShape;
		float boundsWidth;

		btBoxShape*	boxShape;
		ofxBulletBox* box;
		
		void onCollision(ofxBulletCollisionData& cdata);
		void mousePickEvent(ofxBulletMousePickEvent &e);
		int	mousePickIndex;
		ofVec3f	mousePickPos;

		// helpers
		btVector3 toBT(ofVec3f pos);
		btQuaternion toBT(ofQuaternion orient);
		/////////////////////////////////////////
		

		
		/////////////////////////////////////////
		// GUI

		void setupViewports();
		bool bLoadFromFile = true;
		void setupGUI();

		bool viewport_showAll = false;
		int viewport_activeID = 3 ;
		vector<ofRectangle*> viewports;
		vector<string> viewport_labels;

		bool showGUI = true;
		ofxPanel panel;
		ofxPanel panel_robot;
		ofxPanel panel_agent;

		ofParameterGroup params_com;
		ofParameter<string> msg_status;
		ofParameter<string> msg_listening;
		ofParameter<bool> streamPRZ; 		// send the roll, pitch, height of the robot		
		ofParameter<bool> streamAngleExt;	// send the angles & extensions of each limb			
		ofxButton sendGains;	// send the PD gains for the limbs
		ofxButton sendFlag;		// send a flag or state to the robot
		

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
