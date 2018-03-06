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

		// Wekinator connection
		ofxOscSender wek_sender;
		ofxOscReceiver wek_receiver;
		int wek_senderPort = 6448;
		int wek_receiverPort = 12000;
		void wek_checkForMessages();

		Robot robot;

		ofNode node;
		
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

			int triggerBtnState = -1;
			int gripBtnState = -1;
			enum ButtonState { PRESS, TOUCH, UNPRESS, UNTOUCH };

			bool triggerPressed = false;
			bool gripPressed = false;


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
				//ofSetColor(ofColor::aquamarine,50);
				//ofDrawBox(50);
				ofNoFill();
				if (id == 1) {

					if (triggerPressed)
						ofSetColor(ofColor::purple);
					else if (gripPressed)
						ofSetColor(ofColor::orange);
					else
						ofSetColor(ofColor::green, 200);

				}
				else {
					if (triggerPressed)
						ofSetColor(ofColor::purple);
					else if (gripPressed)
						ofSetColor(ofColor::orange);
					else
						ofSetColor(ofColor::red, 200);
				}
					
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

		class Gripper : public ofNode {
		public:

			ofVec3f centroid;
			ofCylinderPrimitive body;
			ofBoxPrimitive finger1, finger2;

			float timer; // 1 = open, 0 = close

			bool isOpen = true;
			bool isClosed = false;

			ofVec3f openFinger1Pos, openFinger2Pos, closePos;

			void setup() {

				float scalar = 2.5;

				
				body.set(25 * scalar, 25 * scalar, true);
				body.setResolution(25, 1);
				body.rotate(90, 0, 1, 0);
				body.setPosition(0, -body.getRadius() / 2, 0);

				finger1.set(10 * scalar);
				finger1.setHeight(25 * scalar);
				finger1.rotate(90, 0, 1, 0);

				finger2.set(10 * scalar);
				finger2.setHeight(25 * scalar);
				finger2.rotate(90, 0, 1, 0);



				finger1.setPosition(-body.getRadius(),  finger1.getHeight() / 3, 0);
				finger2.setPosition( body.getRadius(),  finger1.getHeight() / 3, 0);

				openFinger1Pos.set(finger1.getPosition());
				openFinger2Pos.set(finger2.getPosition());
				closePos.set(openFinger1Pos.getInterpolated(openFinger2Pos, .5));




			};

			void open() {
			
				timer = ofClamp(timer + .1, 0, 1);

				if (timer == 1)
					isOpen = true;
				else
					isOpen = false;
			};

			void close() {

				timer -= .25;// = ofClamp(timer - .1, 1, 0);
				timer = ofClamp(timer, 0, 1);

				cout << "gripper closed %: " << timer << endl;

				if (timer == 1)
					isClosed = true;
				else
					isClosed = false;
			};

			void update() {

				finger1.setPosition(closePos.getInterpolated(openFinger1Pos, timer));
				finger2.setPosition(closePos.getInterpolated(openFinger2Pos, timer));
			};

			void draw() {
				ofPushStyle();

				ofPushMatrix();
				ofMultMatrix(this->getGlobalTransformMatrix());
				
				
				ofSetColor(ofColor::dimGray,120);
				body.draw();
				ofSetColor(ofColor::black,200);
				body.drawWireframe();
				finger1.draw();
				finger2.draw();

				ofPopMatrix();


				ofPopStyle();
			};

		};

		Gripper gripperL;
		Gripper gripperR;

		ofBoxPrimitive table;
		
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

		ofxPanel panel_interaction;
		ofParameterGroup params_interaction;
		ofParameter<bool> mirrorMode;
		ofParameter<bool> offsetMode;
		ofParameter<float> offset;
		ofParameter<bool> dynamicOffsetMode;
		ofParameter<bool> conjMode;
		

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
