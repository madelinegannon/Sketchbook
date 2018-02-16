#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	ofBackground(30, 30, 130);

	// send an initial ping to the OSC Server
	sender.setup("localhost", senderPort);
	ofxOscMessage m;
	m.setAddress("/ping");
	sender.sendMessage(m);

	// set up GUI
	setupViewports();
	setupGUI();
}

//--------------------------------------------------------------
void ofApp::setupGUI() {

	params_osc.setName("OSC Parameters");
	params_osc.add(msg_status.set("Status","Not Connected..."));
	params_osc.add(msg_listening.set("Connection", ""));

	params_tracking.setName("Tracking Options");
	params_tracking.add(trackedObjects[0].set("Tracker " + ofToString(0), false));
	params_tracking.add(trackedObjects[1].set("Tracker " + ofToString(1), false));
	params_tracking.add(trackedObjects[2].set("Tracker " + ofToString(2), false));

	panel.setup(params_osc);
	panel.add(reconnect.setup("Reconnect"));
	panel.add(params_tracking);

	panel.setPosition(ofGetWidth() - 200 - 10, 10);

	if (bLoadFromFile)
		panel.loadFromFile("settings.xml");

	reconnect.addListener(this, &ofApp::reconnectBtnPressed);
}

//--------------------------------------------------------------
void ofApp::reconnectBtnPressed() {
	tryReconnect();
}

//--------------------------------------------------------------
void ofApp::tryReconnect() {
	ofxOscMessage m;
	m.setAddress("/ping");
	sender.sendMessage(m);

	cout << "Sent ping to " << hostname << endl;
}

//--------------------------------------------------------------
void ofApp::update(){

	// check for incoming OSC messages
	checkForMessages();

	if (viewport_showAll)
		findActiveViewportID();

}

//--------------------------------------------------------------
void ofApp::checkForMessages() {

	// check for waiting messages
	while(receiver.hasWaitingMessages()){

		if (!bIsConnected){
			bIsConnected = true;

			msg_status.set("CONNECTED to " + hostname);
			msg_listening.set("Listening on PORT " + ofToSting(PORT));
		}

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		if (m.getAddress() == "/tracker") {
			int id = m.getArgAsInt(0);

			if (trackers.find(id) == trackers.end()) {
				Tracker t;
				t.id = id;
				trackers[id] = t;
			}

			trackers[id].pos.x = m.getArgAsFloat(1);
			trackers[id].pos.y = m.getArgAsFloat(2);
			trackers[id].pos.z = m.getArgAsFloat(3);

		}

	}
}
//--------------------------------------------------------------
void ofApp::findActiveViewportID() {

	// find the active viewport
	for (int i = 0; i < viewports.size(); i++) {
		if (viewports[i]->inside(ofGetMouseX(), ofGetMouseY())) {
			viewport_activeID = i;
		}

	}

		
	// disable cam interaction with inactive viewports
	for (int i = 0; i < cams.size(); i++) {
		if (i == viewport_activeID && !cams[i]->getMouseInputEnabled())
			cams[i]->enableMouseInput();
		else if (i != viewport_activeID)
			cams[i]->disableMouseInput();
	}

}

//--------------------------------------------------------------
void ofApp::drawViewports() {

	for (int i = 0; i < viewports.size(); i++) {

		// show all 4 viewports
		if (viewport_showAll) {
			cams[i]->begin(*viewports[i]);
			ofEnableDepthTest();

			ofDrawAxis(100);
			ofNoFill();
			ofDrawBox(50);

			ofDisableDepthTest();
			cams[i]->end();

			// Draw 2D Elements
			ofPushStyle();
			ofNoFill();
			ofDrawRectangle(*viewports[i]);
			ofDrawBitmapString(viewport_labels[i], viewports[i]->x + 10, viewports[i]->y + 20);
			ofPopStyle();

			// Highlight active viewport
			if (viewport_activeID == i) {
				ofPushStyle();
				ofFill();
				ofSetColor(ofColor::whiteSmoke, 10);
				ofDrawRectangle(*viewports[i]);
				ofPopStyle();
			}
		}

		// show active viewport at full screen
		else {
			if (viewport_activeID == i) {
				cams[i]->begin(*viewports[i]);
				ofEnableDepthTest();

				ofDrawAxis(100);
				ofNoFill();
				ofDrawBox(50);

				ofDisableDepthTest();
				cams[i]->end();

				// Draw 2D Elements
				ofPushStyle();
				ofNoFill();
				ofDrawRectangle(*viewports[i]);
				ofDrawBitmapString(viewport_labels[i], viewports[i]->x + 10, viewports[i]->y + 20);
				ofPopStyle();

			}
		}


	}


}

		


//--------------------------------------------------------------
void ofApp::draw(){

	// draw 3D
	drawViewports();


	// draw GUI
	//if (showGUI)
	panel.draw();

	//if (trackers.size() > 0) {
	//	cam.begin();
	//	ofBackground(0);
	//	ofDrawAxis(100);
	//	
	//	ofPushStyle();
	//	ofSetColor(255, 0, 255);
	//	ofFill();
	//	ofTranslate(trackers.begin()->second.pos.scale(100));
	//	ofDrawBox(10);
	//	ofPopStyle();
	//	
	//	cam.end();
	//}

}

// -------------------------------------------------------------------
void ofApp::exit() {
	
	panel.saveToFile("settings.xml");
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'f'){
		ofToggleFullscreen();
		
		if (viewport_showAll) {

			viewports[0]->setPosition(0, 0); 
			viewports[1]->setPosition(0, ofGetWindowHeight() / 2);
			viewports[2]->setPosition(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewports[3]->setPosition(ofGetWindowWidth() / 2, 0);

			for (int i = 0; i < viewports.size(); i++) {
				viewports[i]->setSize(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			}

		}

		else {
			for (int i = 0; i < viewports.size(); i++) {
				if (viewport_activeID == i)
					viewports[i]->set(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			}
		}
	}

	if (key == ' ') {
		viewport_showAll = !viewport_showAll;
		
		if (viewport_showAll) {
			viewports[0]->setPosition(0, 0);
			viewports[1]->setPosition(0, ofGetWindowHeight() / 2);
			viewports[2]->setPosition(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewports[3]->setPosition(ofGetWindowWidth() / 2, 0);

			for (int i = 0; i < viewports.size(); i++) {
				viewports[i]->setSize(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			}
		}

		else {
			for (int i = 0; i < viewports.size(); i++) {
				if (viewport_activeID == i)
					viewports[i]->set(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			}
		}

	}

	float dist = 200;
	if (key == '1') {
		cams[viewport_activeID]->reset();
		cams[viewport_activeID]->setPosition(0, 0, dist);
		cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	}
	// LEFT VIEW ... use ortho cam
	else if (key == '2') {
		cams[viewport_activeID]->reset();
		cams[viewport_activeID]->setPosition(dist, 0, 0);
		cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	}
	// FRONT VIEW ... use ortho cam
	else if (key == '3') {
		cams[viewport_activeID]->reset();
		cams[viewport_activeID]->setPosition(0, dist, 0);
		cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	}
	// PERSPECTIVE VIEW 
	else if (key == '4') {
		cams[viewport_activeID]->reset();
		cams[viewport_activeID]->setPosition(dist, dist, 2*dist);
		cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

	

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
	// checking for double click
	unsigned long doubleclickTime = 200;
	unsigned long curTap = ofGetElapsedTimeMillis();
	if (lastTap != 0 && curTap - lastTap < doubleclickTime) {
		//cout << "DOUBLE CLICK!" << endl;
		bDoubleClick = true;
		return;
	}
	else if (bDoubleClick)
		bDoubleClick = false;

	lastTap = curTap;

}

//--------------------------------------------------------------
void ofApp::setupViewports() {

	viewport_activeID = 0;

	for (int i = 0; i < 4; i++) {
		cams.push_back(new ofEasyCam());

		string viewport_label;
		if (i == 0) {
			viewports.push_back(new ofRectangle(0, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
			viewport_label = "TOP VIEW";
		}
		else if (i == 1) {
			viewports.push_back(new ofRectangle(0, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
			viewport_label = "LEFT VIEW";
		}
		else if (i == 2) {
			viewports.push_back(new ofRectangle(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
			viewport_label = "FRONT VIEW";
		}
		else {
			viewports.push_back(new ofRectangle(ofGetWindowWidth() / 2, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
			viewport_label = "PERSPECTIVE VIEW";
		}
		viewport_labels.push_back(viewport_label);
	}
		
	float dist = 200;
	// TOP
	cams[0]->enableOrtho();
	cams[0]->setPosition(0, 0, dist);
	cams[0]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	// LEFT
	//cams[1]->enableOrtho(); // ortho not working properly for this camera
	cams[1]->setPosition(dist, 0, 0);
	cams[1]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	// FRONT
	cams[2]->setPosition(0, dist, 0);
	cams[2]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	//cams[2]->enableOrtho(); // ortho not working properly for this camera
	// PERSPECTIVE
	cams[3]->setPosition(dist, dist, 5*dist / 4);
	cams[3]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));

}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

