#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "PD_Controller.h"
#include "TimeSeriesPlot.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mousePressed(int x, int y, int button);

		PD_Controller pd_controller;
		TimeSeriesPlot plot = TimeSeriesPlot(2);

		ofVec3f desired_pos, curr_pos;
		
		// GUI
		void setup_gui();
		ofxPanel panel;


};

