#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	ofBackground(ofColor::darkGray);

	// send an initial ping to the OSC Server
	sender.setup("localhost", senderPort);
	ofxOscMessage m;
	m.setAddress("/ping");
	sender.sendMessage(m);

	// set up GUI
	setupViewports();
	setupGUI();

	// load world models
	loadModels();

	robot.setup(Robot::MINITAUR);
}

//--------------------------------------------------------------
void ofApp::update() {

	// check for incoming OSC messages
	checkForMessages();

	if (viewport_showAll)
		findActiveViewportID();

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
void ofApp::checkForMessages() {

	for (auto &tracker : trackers) {
		tracker.second.isConnected = false;
	}

	// check for waiting messages
	while (receiver.hasWaitingMessages()) {

		if (!bIsConnected) {
			bIsConnected = true;

			msg_status.set(hostname);
			msg_listening.set(ofToString(PORT));
		}
		else {

			// get the next message
			ofxOscMessage m;
			receiver.getNextMessage(m);

			// debug
			//stringstream ss;
			//ss << m.getAddress() << ", " << m.getArgAsInt32(0);
			////cout << ss.str() << endl;


			int id = m.getArgAsInt(0);

			// add to map if new tracker
			if (trackers.find(id) == trackers.end()) {
				Tracker t;
				t.id = id;
				trackers[id] = t;

				if (m.getAddress() == "/tracker")
					trackers[id].mesh = trackerMesh;
				else if (m.getAddress() == "/hmd")
					trackers[id].mesh = hmdMesh;
				else
					trackers[id].mesh = controllerMesh;

				string msg_string;

				msg_string = "tracker type";
				msg_string += ": ";
				msg_string += m.getAddress();
				msg_string += " | ID: ";
				msg_string += ofToString(id);

				cout << msg_status << endl;
			}

			// get the VR tracker's pos and orientation
			ofVec3f tempPos;
			ofQuaternion tempOrient;
			ofVec3f tempScale;

			tempPos.x = m.getArgAsFloat(1) * 1000;
			tempPos.y = m.getArgAsFloat(2) * 1000;
			tempPos.z = m.getArgAsFloat(3) * 1000;

			// not using scale (osc message args 4,5,6)

			tempOrient.set(m.getArgAsFloat(7), m.getArgAsFloat(8), m.getArgAsFloat(9), m.getArgAsFloat(10));

			// make transformation matrix
			ofMatrix4x4 mat, matT, matO;
			mat.makeIdentityMatrix();
			matT.makeTranslationMatrix(tempPos);
			matO.makeRotationMatrix(tempOrient);
			mat *= matT * matO;



			// convert from right-handed to left-handed coordinate system
			ofQuaternion rot;
			rot.makeRotate(ofVec3f(0, 1, 0), ofVec3f(0, 0, 1));
			mat.rotate(rot);

			// assign tracker values
			//if (id == 0) { // HMD not working yet ... don't know if it's sender or receiver error (likely sender)
			//	trackers[id].pos.set(-tempPos.x, tempPos.z, tempPos.y); // why doesn't mat.getTranslation() work?
			//	trackers[id].orient.set(mat.getRotate());

			//	// update the internal ofNode matrix
			//	trackers[id].setTransformMatrix(ofMatrix4x4(trackers[id].orient));
			//	trackers[id].setPosition(trackers[id].pos);
			//	trackers[id].setOrientation(trackers[id].orient);
			//}
			//else {
				trackers[id].pos.set(tempPos.x, -tempPos.z, tempPos.y); // why doesn't mat.getTranslation() work?
				trackers[id].orient.set(mat.getRotate());
				
				// update the internal ofNode matrix
				trackers[id].setTransformMatrix(mat);
				trackers[id].setPosition(trackers[id].pos);
				trackers[id].setOrientation(trackers[id].orient);
			//}




			// update the tracker trail
			trackers[id].update();


			// debug
			/*if (id == 0) {

				string msg_string;

				msg_string = "tracker";
				msg_string += ": ";
				msg_string += m.getAddress();
				msg_string += " | ID: ";
				msg_string += ofToString(id);
				msg_string += " | coords: ";
				msg_string += ofToString(trackers[id].pos);
				msg_string += ", ";
				msg_string += ofToString(trackers[id].orient);

				cout << msg_string << endl;
			}*/

		}


	}
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


			
			// draw only the trackers that are currently being tracked
			for (auto &tracker : trackers) {
				if (trackingIDs[tracker.second.id] && tracker.second.isConnected && tracker.second.id != 0) { // don't show HMD yet ... not working
					// temp for now
					ofPushMatrix();
					ofMultMatrix(tracker.second.getGlobalTransformMatrix());
					robot.draw();
					ofPopMatrix();

					tracker.second.draw();
				}
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
			
				// draw only the trackers that are currently being tracked
				for (auto &tracker : trackers) {
					if (trackingIDs[tracker.second.id] && tracker.second.isConnected && tracker.second.id != 0) { // don't show HMD yet ... not working
						// temp for now
						ofPushMatrix();
						ofMultMatrix(tracker.second.getGlobalTransformMatrix());
						robot.draw();
						ofPopMatrix();
						
						tracker.second.draw();
					}
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

	params_osc.setName("OSC Parameters");
	params_osc.add(msg_status.set("IP Addr", "Not Connected..."));
	params_osc.add(msg_listening.set("PORT", ""));

	params_tracking.setName("Tracking Options");
	params_tracking.add(trackingIDs[0].set("HMD", false));
	params_tracking.add(trackingIDs[1].set("Left Controller", false));
	params_tracking.add(trackingIDs[2].set("Right Controller", false));
	params_tracking.add(trackingIDs[3].set("Tracker " + ofToString(1), false));
	params_tracking.add(trackingIDs[4].set("Tracker " + ofToString(2), false));

	panel.setup(params_osc);
	panel.add(reconnect.setup("Reconnect"));
	panel.add(params_tracking);

	panel.setPosition(ofGetWidth() - 200 - 10, 10);

	if (bLoadFromFile)
		panel.loadFromFile("settings.xml");

	reconnect.addListener(this, &ofApp::reconnectBtnPressed);
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

// ------------------------------------------------------------------

void ofApp::printmap(map<int, Tracker> & m) {
	for (const auto& elem : m) {
		cout << " [id, pos], [" << elem.first << ", " << ofToString(elem.second.pos) << "]" << endl;
	}
	cout << endl << "size() == " << m.size() << endl << endl;
}

// ------------------------------------------------------------------

void ofApp::loadModels() {
	
	// load 3D model of Vive Tracker
	if (loader.loadModel(ofToDataPath("models/TRACKER-3D.stl"), false)) {
		cout << ofToString(loader.getNumMeshes()) << endl;
		trackerMesh = loader.getMesh(0);
		
		// adapt tracker mesh configuration to real world coordinates
		for (auto &vert : trackerMesh.getVertices()) {
			// go from Z-UP to Y-UP
			vert.rotate(180, ofVec3f(1, 0, 0));
			// align with Z axis
			vert.rotate(30, ofVec3f(0, 0, 1));
			// scale from meters to millimeters
			vert *= 1000;
		}
	}

	// load 3D model of Vive Controller
	if (loader.loadModel(ofToDataPath("models/vr_controller_vive_1_5.obj"), false)) {
		cout << ofToString(loader.getNumMeshes()) << endl;
		controllerMesh = loader.getMesh(0);

		// adapt tracker mesh configuration to real world coordinates
		for (auto &vert : controllerMesh.getVertices()) {
			// scale from meters to millimeters
			vert *= 1000;
		}
	}

	// load 3D model of Generic HMD
	if (loader.loadModel(ofToDataPath("models/generic_hmd.obj"), false)) {
		cout << ofToString(loader.getNumMeshes()) << endl;
		hmdMesh = loader.getMesh(0);

		// adapt tracker mesh configuration to real world coordinates
		for (auto &vert : hmdMesh.getVertices()) {
			// flip around the Z-axes
			vert.rotate(180, ofVec3f(0, 0, 1));
			// scale from meters to millimeters
			vert *= 1000;
		}
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

