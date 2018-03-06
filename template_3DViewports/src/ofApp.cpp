#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// listen on the given port
	
	ofBackground(ofColor::darkGray);

	

	// set up GUI
	setupViewports();
	setupGUI();

	// load world models
	//loadModels();

	// set up world geometry
	box.setWidth(500);
	box.setHeight(250);
	box.setDepth(100);

	// set up gizmo and attach to geometry
	gizmo.setDisplayScale(2.);
	gizmo.enableMouseInput();
	gizmo.setNode(box);

}

//--------------------------------------------------------------
void ofApp::update() {

	
	
	// update viewports
	if (viewport_showAll) {
		gizmo.disableMouseInput();
		findActiveViewportID();
	}
	// override with gizmo interaction when in single viewport mode
	else {
		
		gizmo.enableMouseInput();

		if (gizmo.isInteracting())
			cams[viewport_activeID]->disableMouseInput();
		else if (!cams[viewport_activeID]->getMouseInputEnabled())
			cams[viewport_activeID]->enableMouseInput();
	}


	// update geometry attached to gizmo
	box.setTransformMatrix(gizmo.getMatrix());

}

// --------------------------------------------------------------
void ofApp::draw() {

	// draw 3D
	drawViewports();

	// draw GUI
	//if (showGUI)
	panel.draw();

}

// -------------------------------------------------------------------
void ofApp::exit() {

	panel.saveToFile("settings.xml");

}

//--------------------------------------------------------------
void ofApp::drawViewports() {

	for (int i = 0; i < viewports.size(); i++) {

		// show all 4 viewports
		if (viewport_showAll) {
			cams[i]->begin(*viewports[i]);
			ofEnableDepthTest();

			// draw the background grids and axes
			ofDrawGrid(100, 10, false, false, false, true);
			ofPushStyle();
			ofSetLineWidth(5);
			ofDrawAxis(1000);
			ofPopStyle();
			ofNoFill();

			// draw here ...

			box.drawWireframe();

			if (gizmo_isVisible) {
				gizmo.draw(*cams[i]);
			}


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

				ofDrawGrid(100, 10, false, false, false, true);

				ofPushStyle();
				ofSetLineWidth(5);
				ofDrawAxis(1000);
				ofPopStyle();
				ofNoFill();
			
				// draw here ...
				
				box.drawWireframe();

				if (gizmo_isVisible) {
					gizmo.draw(*cams[i]);
				}
				
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
void ofApp::keyPressed(int key){

	if (key == 'f'){
		ofToggleFullscreen();
		panel.setPosition(ofGetWidth() - 200 - 10, 10);
		
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


	// gizmo controls
	if (key == 'e' || key == 'E') {
		gizmo.setType(ofxGizmo::OFX_GIZMO_ROTATE);
	}
	if (key == 'w' || key == 'W') {
		gizmo.setType(ofxGizmo::OFX_GIZMO_MOVE);
	}
	if (key == 'c')
		gizmo.setNode(ofNode());



	float dist = 1500;
	if (key == '1') {
		cams[viewport_activeID]->reset();
		cams[viewport_activeID]->setPosition(0, 0, dist);
		cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));
		cams[viewport_activeID]->setScale(5);
	}
	// LEFT VIEW ... use ortho cam?
	else if (key == '2') {
		cams[viewport_activeID]->reset();
		cams[viewport_activeID]->setPosition(dist, 0, 0);
		cams[viewport_activeID]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
	}
	// FRONT VIEW ... use ortho cam?
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
void ofApp::setupGUI() {

	// show COM feedback to use
	params_osc.setName("OSC Parameters");
	params_osc.add(msg_status.set("IP Addr", "Not Connected..."));
	params_osc.add(msg_listening.set("PORT", ""));

	params_tracking.setName("Tracking Options");
	params_tracking.add(trackingIDs[0].set("HMD", false));
	params_tracking.add(trackingIDs[1].set("Left Controller", false));
	params_tracking.add(trackingIDs[2].set("Right Controller", false));
	params_tracking.add(trackingIDs[3].set("Tracker " + ofToString(1), false));
	params_tracking.add(trackingIDs[4].set("Tracker " + ofToString(2), false));

	// 3D Nav specific
	params_gui.setName("Navigation");
	params_gui.add(gizmo_isVisible.set("Show Gizmo", true));

	panel.setup(params_osc);
	panel.add(params_tracking);
	panel.add(params_gui);
	panel.add(save.setup("Save"));

	panel.setPosition(ofGetWidth() - 200 - 10, 10);

	if (bLoadFromFile)
		panel.loadFromFile("settings.xml");

	save.addListener(this, &ofApp::saveBtnPressed);
}



//--------------------------------------------------------------
void ofApp::setupViewports() {

	viewport_activeID = 0;
	float dist = 1500;

	for (int i = 0; i < 4; i++) {
		cams.push_back(new ofEasyCam());
		

		cams[i]->setFarClip(99999999);
		cams[i]->setNearClip(0);
		cams[i]->setDistance(dist);

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
	// TOP
	cams[0]->enableOrtho();
	cams[0]->setPosition(0, 0, dist);
	cams[0]->lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));
	cams[viewport_activeID]->setScale(5); // <-- can't zoom in ortho cam, but can scale
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
void ofApp::saveBtnPressed() {

	// do save

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



// ------------------------------------------------------------------

void ofApp::printmap(map<int, string> & m) {
	for (const auto& elem : m) {
		cout << " [id, pos], [" << elem.first << ", " << ofToString(elem.second) << "]" << endl;
	}
	cout << endl << "size() == " << m.size() << endl << endl;
}

// ------------------------------------------------------------------

void ofApp::loadModels() {
	
	// load 3D model of Vive Tracker
	if (loader.loadModel(ofToDataPath("models/your_filename.stl"), false)) {
		cout << ofToString(loader.getNumMeshes()) << endl;
 
		// adjust to proper orientation
		//for (auto &vert : trackerMesh.getVertices()) {
		//	// go from Z-UP to Y-UP
		//	vert.rotate(180, ofVec3f(1, 0, 0));
		//	// align with Z axis
		//	vert.rotate(30, ofVec3f(0, 0, 1));
		//	// scale from meters to millimeters
		//	vert *= 1000;
		//}
	}

		

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

