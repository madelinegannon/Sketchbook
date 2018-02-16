#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

	ofBackground(30, 30, 130);

	// send an initial ping to the OSC Server
	sender.setup("localhost", senderPort);
	ofxOscMessage m;
	m.setAddress("/ping");
	sender.sendMessage(m);

	ofEnableDepthTest();
	cam.setDistance(100);

	setupViewports();

}

//--------------------------------------------------------------
void ofApp::update(){

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
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

		if (m.getAddress() == "/pos") {
			mouseX = m.getArgAsInt32(0);
			mouseY = m.getArgAsInt32(1);
		}
        
		// check for mouse moved message
		if(m.getAddress() == "/mouse/position"){
			// both the arguments are int32's
			mouseX = m.getArgAsInt32(0);
			mouseY = m.getArgAsInt32(1);
		}
		// check for mouse button message
		else if(m.getAddress() == "/mouse/button"){
			// the single argument is a string
			mouseButtonState = m.getArgAsString(0);
		}
        // check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through ) 
        else if(m.getAddress() == "/image" ){
            ofBuffer buffer = m.getArgAsBlob(0);
            receivedImage.load(buffer);
        }
		else{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

	}

	if (viewport_showAll)
		findActiveViewportID();

}

//--------------------------------------------------------------
void ofApp::findActiveViewportID() {


	if (viewport_top.inside(ofGetMouseX(), ofGetMouseY()))
		viewport_activeID = 0;
	else if (viewport_left.inside(ofGetMouseX(), ofGetMouseY()))
		viewport_activeID = 1;
	else if (viewport_front.inside(ofGetMouseX(), ofGetMouseY()))
		viewport_activeID = 2;
	else 
		viewport_activeID = 3;
		
	/*
	if (viewportSim.inside(ofGetMouseX(), ofGetMouseY()))
	{
		activeCam = 1;
		if (!cams[1]->getMouseInputEnabled()) {
			cams[1]->enableMouseInput();
		}
		if (cams[0]->getMouseInputEnabled()) {
			cams[0]->disableMouseInput();
		}
		if (gizmo.isInteracting() && cams[1]->getMouseInputEnabled()) {
			cams[1]->disableMouseInput();
		}
	}*/

	for (int i = 0; i < cams.size(); i++) {
		if (i == viewport_activeID && !cams[i]->getMouseInputEnabled())
			cams[i]->enableMouseInput();
		else if (i != viewport_activeID)
			cams[i]->disableMouseInput();
	}

}

//--------------------------------------------------------------
void ofApp::drawViewports() {

	if (viewport_showAll) {

		// TOP
		cams[0]->begin(viewport_top);
		ofEnableDepthTest();

		ofDrawAxis(100);

		ofDisableDepthTest();
		cams[0]->end();
		ofPushStyle();
		ofNoFill();
		ofDrawRectangle(viewport_top);
		ofPopStyle();

		// LEFT
		cams[1]->begin(viewport_left);
		ofEnableDepthTest();

		ofDrawAxis(100);

		ofDisableDepthTest();
		cams[1]->end();
		ofPushStyle();
		ofNoFill();
		ofDrawRectangle(viewport_left);
		ofPopStyle();

		// FRONT
		cams[2]->begin(viewport_front);
		ofEnableDepthTest();

		ofDrawAxis(100);

		ofDisableDepthTest();
		cams[2]->end();
		ofPushStyle();
		ofNoFill();
		ofDrawRectangle(viewport_front);
		ofPopStyle();
		
		// PERSPECTIVE
		cams[3]->begin(viewport_persp);
		ofEnableDepthTest();

		ofDrawAxis(100);

		ofDisableDepthTest();
		cams[3]->end();
		ofPushStyle();
		ofNoFill();
		ofDrawRectangle(viewport_persp);
		ofPopStyle();

	}

	// show active viewport at full screen
	else {

		if (viewport_activeID == 0) {
			// TOP
			cams[0]->begin(viewport_top);
			ofEnableDepthTest();

			ofDrawAxis(100);

			ofDisableDepthTest();
			cams[0]->end();
			ofPushStyle();
			ofNoFill();
			ofDrawRectangle(viewport_top);
			ofPopStyle();
		}
		else if (viewport_activeID == 1) {
			// LEFT
			cams[1]->begin(viewport_left);
			ofEnableDepthTest();

			ofDrawAxis(100);

			ofDisableDepthTest();
			cams[1]->end();
			ofPushStyle();
			ofNoFill();
			ofDrawRectangle(viewport_left);
			ofPopStyle();
		}
		else if (viewport_activeID == 2) {
			// FRONT
			cams[2]->begin(viewport_front);
			ofEnableDepthTest();

			ofDrawAxis(100);

			ofDisableDepthTest();
			cams[2]->end();
			ofPushStyle();
			ofNoFill();
			ofDrawRectangle(viewport_front);
			ofPopStyle();
		}
		else {
			// PERSPECTIVE
			cams[3]->begin(viewport_persp);
			ofEnableDepthTest();

			ofDrawAxis(100);

			ofDisableDepthTest();
			cams[3]->end();
			ofPushStyle();
			ofNoFill();
			ofDrawRectangle(viewport_persp);
			ofPopStyle();
		}
		



	}

	// highlight active viewport
	ofPushStyle();
	ofSetColor(ofColor::whiteSmoke, 10);
	if (viewport_activeID == 0) 
		ofDrawRectangle(viewport_top);
	else if (viewport_activeID == 1)
		ofDrawRectangle(viewport_left);
	else if (viewport_activeID == 2)
		ofDrawRectangle(viewport_front);
	else
		ofDrawRectangle(viewport_persp);
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::draw(){

	string buf;
	buf = "listening for osc messages on port" + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);

	// draw 3D
	drawViewports();


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

	//else {
	//	if (receivedImage.getWidth() > 0) {
	//		ofDrawBitmapString("Image:", 10, 160);
	//		receivedImage.draw(10, 180);
	//	}

	//	// draw mouse state
	//	buf = "mouse: " + ofToString(mouseX, 4) + " " + ofToString(mouseY, 4);
	//	ofDrawBitmapString(buf, 430, 20);
	//	ofDrawBitmapString(mouseButtonState, 580, 20);

	//	for (int i = 0; i < NUM_MSG_STRINGS; i++) {
	//		ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
	//	}
	//}
    



}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'f'){
		ofToggleFullscreen();
		
		if (viewport_showAll) {
			viewport_top.set(0, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewport_left.set(0, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewport_front.set(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewport_persp.set(ofGetWindowWidth() / 2, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
		}
	}

	if (key == ' ') {
		viewport_showAll = !viewport_showAll;

		if (viewport_showAll) {
			viewport_top.set(0, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewport_left.set(0, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewport_front.set(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
			viewport_persp.set(ofGetWindowWidth() / 2, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
		}

		else {
			if (viewport_activeID == 0) {
				viewport_top.set(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			}
			else if (viewport_activeID == 1) {
				viewport_left.set(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			}
			else if (viewport_activeID == 2) {
				viewport_front.set(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			}
			else {
				viewport_persp.set(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			}
		}
		
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

//--------------------------------------------------------------
void ofApp::setupViewports() {

	viewport_top	= ofRectangle(0, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
	viewport_left = ofRectangle(0, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
	viewport_front  = ofRectangle(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
	viewport_persp	= ofRectangle(ofGetWindowWidth() / 2, 0, ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);

	viewport_activeID = 0;


	for (int i = 0; i < 4; i++) {
		cams.push_back(new ofEasyCam());
		savedCamMats.push_back(ofMatrix4x4());
		viewportLabels.push_back("");
	}

	//cams[0]->begin(viewport_top);
	//cams[0]->end();
	//cams[0]->enableMouseInput();

	//cams[1]->begin(viewport_left);
	//cams[1]->end();
	//cams[1]->enableMouseInput();

	//cams[2]->begin(viewport_front);
	//cams[2]->end();
	//cams[2]->enableMouseInput();

	//cams[3]->begin(viewport_persp);
	//cams[3]->end();
	//cams[3]->enableMouseInput();

	//for (int i = 0; i < cams.size(); i++) {
	//	cams[i]->enableMouseInput();
	//}
}

//--------------------------------------------------------------
void ofApp::handleViewportPresets(int key) {

	float dist = 2000;
	float zOffset = 450;

	if (viewport_activeID != -1) {
		// TOP VIEW  ... use ortho cam
		if (key == '1') {
			cams[viewport_activeID]->reset();
			cams[viewport_activeID]->setPosition(0, 0, dist);
			cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
			viewportLabels[viewport_activeID] = "TOP VIEW";
		}
		// LEFT VIEW ... use ortho cam
		else if (key == '2') {
			cams[viewport_activeID]->reset();
			cams[viewport_activeID]->setPosition(dist, 0, 0);
			cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
			viewportLabels[viewport_activeID] = "LEFT VIEW";
		}
		// FRONT VIEW ... use ortho cam
		else if (key == '3') {
			cams[viewport_activeID]->reset();
			cams[viewport_activeID]->setPosition(0, dist, 0);
			cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
			viewportLabels[viewport_activeID] = "FRONT VIEW";
		}
		// PERSPECTIVE VIEW 
		else if (key == '4') {
			cams[viewport_activeID]->reset();
			cams[viewport_activeID]->setPosition(-dist, -dist, dist / 4);
			cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
			viewportLabels[viewport_activeID] = "PERSPECTIVE VIEW";
		}
	}
}