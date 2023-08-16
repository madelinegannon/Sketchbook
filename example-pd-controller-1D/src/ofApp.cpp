#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//plot = TimeSeriesPlot(2);
	plot.name = "RAW VALUE (BLUE), SMOOTHED VALUE (CYAN)";
	plot.min = 50 * -1;
	plot.max = 50 * 1;

	curr_pos.set(ofGetWidth() / 2, ofGetHeight() / 2, 0);
	desired_pos.set(ofGetWidth() / 2, ofGetHeight() / 2, 0);

	setup_gui();
}

//--------------------------------------------------------------
void ofApp::update(){

	// use the mouseY as the raw setpoint for the pd controller
	pd_controller.update(ofGetMouseY());

	// visualize the raw and filtered data as a 2D point
	desired_pos.y = pd_controller.get_setpoint();
	curr_pos.y = pd_controller.get_smoothed_val();

	// visualize the raw and filtered data as a timeseries plot
	// update the timeseries plot with desired data
	vector<float> data;
	data.push_back(ofMap(ofGetMouseY(), 0, ofGetHeight(), plot.min, plot.max));
	data.push_back(ofMap(pd_controller.get_smoothed_val(), 0, ofGetHeight(), plot.min, plot.max));

	plot.update(data);
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofPushStyle();

	// draw the current_to_desired position
	ofSetColor(80, 200);
	ofDrawLine(desired_pos, curr_pos);

	// draw the current position
	ofSetColor(ofColor::orangeRed);
	ofDrawEllipse(curr_pos, 35, 35);
	ofSetColor(ofColor::white);
	ofNoFill();
	ofSetLineWidth(1);
	ofDrawEllipse(curr_pos, 36, 36);


	ofPushMatrix();
	ofTranslate(ofGetWidth() - 550, 60);
	plot.draw();
	ofPopMatrix();
	
	ofPopStyle();
	
	panel.draw();
}

//--------------------------------------------------------------
void ofApp::setup_gui() {
	panel.setup("PD_Controller_Example");
	panel.setPosition(10, 10);
	panel.setWidthElements(250);
	panel.add(pd_controller.params);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}