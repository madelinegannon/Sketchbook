#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGizmo.h"



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
		
		ofParameterGroup params_gui;
		ofParameter<bool> gizmo_isVisible;
		ofxButton save;

		void saveBtnPressed();

		ofParameterGroup params_tracking;
		map<int, ofParameter<bool> > trackingIDs;
		

		void drawViewports();
		void findActiveViewportID();



		/////////////////////////////////////////
		// Navigation

		// 3D Navigation
		vector<ofEasyCam*> cams;

		ofxGizmo gizmo;
		ofBoxPrimitive box;

		// checking for double click
		unsigned long lastTap;
		bool bDoubleClick = false;

		
		
		/////////////////////////////////////////
		// UTILS
		void loadModels();
		ofxAssimpModelLoader loader;

		void printmap(map<int, string> & m);

};
