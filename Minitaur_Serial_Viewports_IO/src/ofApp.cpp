#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
		
	ofBackground(ofColor::darkGray);

	// setup serial communication
	cout << "Is Serial Setup? " << serial.setup(COM_PORT, baud) << endl;

	// listen on the given port
	receiver.setup(PORT);
	cout << "listening for osc messages on port " << PORT << "\n";

	// send an initial ping to the OSC Server
	sender.setup("localhost", senderPort);
	ofxOscMessage m;
	m.setAddress("/ping");
	sender.sendMessage(m);
	
	// setup the Minitaur
	robot.setup();
	
	// setup any agents
	agent.setup();

	// set up GUI
	setupViewports();
	setupGUI();
	if (viewport_showAll)
		findActiveViewportID();
	else {
		viewport_showAll = true;
		viewport_activeID = 3;
		handleViewportPresets(' ');
	}

	// load world models
	loadModels();

	ofMatrix4x4 mat;
	mat.setTranslation(1500, 1500, 150);
	if (robot.behaviorMode == Robot::PHYSICS)
		mat.setTranslation(0, 0, 75);
	gizmo_lookAt.setMatrix(mat);
	gizmo_lookAt.setDisplayScale(1.5);
	gizmo_lookAt.enableMouseInput();
	gizmo_lookAt.setViewDimensions(ofGetWidth(), ofGetHeight());

	// setup Bullet world
	setupPhysics();
	
}

//--------------------------------------------------------------
void ofApp::update() {

	//checkForMessages(); // should be within its own COM thread .... update holds pointer to incoming message in thread
	//

	//if (robot.behaviorMode == Robot::BEHAVIOR::TRACKER_FOLLOW || robot.behaviorMode == Robot::BEHAVIOR::TRACKER_LOOKAT) {
	//	checkForMessages_Vive();
	//}


	// update viewports
	if (viewport_showAll) {
		robot.gizmo.disableMouseInput();
		gizmo_lookAt.disableMouseInput();
		findActiveViewportID();
	}
	// override with gizmo interaction when in single viewport mode
	else {
		if (robot.showGizmo) {
			robot.gizmo.enableMouseInput();
			gizmo_lookAt.enableMouseInput();
		}
		else {
			robot.gizmo.disableMouseInput();
			gizmo_lookAt.disableMouseInput();
		}
	}

	if (robot.behaviorMode == Robot::BEHAVIOR::POSTURE) {

		// make the gizmos larger for the poster demo
		if (robot.gizmo.getDisplayScale() != 2) {
			robot.gizmo.setDisplayScale(2);
		}
	
		// update the robot (based on gizmo)
		robot.update();

	}

	else if (robot.behaviorMode == Robot::BEHAVIOR::LOOKAT_AGENT) {
		agent.arrive(gizmo_lookAt.getMatrix().getTranslation());
		agent.update();

		robot.setLookAtPt(agent.pos);
		robot.update();
	}

	else if (robot.behaviorMode == Robot::BEHAVIOR::PHYSICS) {

		// make the gizmos smaller for the physics demo, since we need to be zoomed in
		if (robot.gizmo.getDisplayScale() != .5) {
			robot.gizmo.setDisplayScale(.5);
			gizmo_lookAt.setDisplayScale(.75);
		}

		// update the physics world
		// NOTE: picking only works when zoomed waaay in

		// control the sphere with the world's lookAt gizmo
		btTransform mat;
		btVector3 pos = toBT(gizmo_lookAt.getMatrix().getTranslation());
		btQuaternion orient = toBT(gizmo_lookAt.getMatrix().getRotate());
		mat.setOrigin(pos);
		mat.setRotation(orient);
		// set sphere to lookAt gizmo
		anchors[0]->getRigidBody()->setWorldTransform(mat);
		
		world.update();

		// update the robot's gizmo
		robot.gizmo.setMatrix(box->getTransformationMatrix());

		// update the robot (based on gizmo)
		robot.update();

	}

	
	if (robot.behaviorMode == Robot::BEHAVIOR::LOOKAT_TRACKER) {

		// when we are using the vive tracker
		if (trackers.size() > 0 && trackers[3].isConnected) {

			// update the gizmo_lookAt to be the tracker[3]'s transform matirx

		}

		// look at secondary gizmo for now
		robot.setLookAtPt(gizmo_lookAt.getMatrix().getTranslation());
		robot.update();
	}

	// if we are streaming data to the robot, send the message
	if (streamPRZ || streamAngleExt || sendGains || sendFlag) {
		sendMessage("");
	}



	// enable/disable the camera's input based on the gizmos
	if ((robot.showGizmo && robot.gizmo.isInteracting()) || gizmo_lookAt.isInteracting()) {
		cams[viewport_activeID]->disableMouseInput();
	}
	else if (!cams[viewport_activeID]->getMouseInputEnabled()) {
		cams[viewport_activeID]->enableMouseInput();
	}


}

// --------------------------------------------------------------
void ofApp::checkForMessages() {

	// check for incoming messages from Minitaur
	incomingMsg = "";
	stringstream ss;
	char ch;
	if (serial.isInitialized()){// && serial.available()) {
		while ((ch = serial.readByte()) > 0 && ch != '\n')
			ss << ch;

		incomingMsg = ss.str();

		if (incomingMsg != "") {
			cout << "	Incoming Message from Minitaur: [ " << incomingMsg << " ]" << endl << endl;
		}

	}

}

// --------------------------------------------------------------
void ofApp::checkForMessages_Vive() {
	
	// check for incoming messages from Vive
	for (auto &tracker : trackers) {
		tracker.second.isConnected = false;
	}

	// check for waiting messages
	while (receiver.hasWaitingMessages()) {

		if (!bIsConnected) {
			bIsConnected = true;

			msg_status_osc.set(hostname);
			msg_listening_osc.set(ofToString(PORT));
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

				if (id == 3) {
					//gizmo.setNode(trackers[id]);
					if (lookAtTracker) {
						
					}
					else {
						//body.setParent(trackers[id]);
					}
				}
				
				string msg_string;

				msg_string = "tracker type";
				msg_string += ": ";
				msg_string += m.getAddress();
				msg_string += " | ID: ";
				msg_string += ofToString(id);

				cout << msg_string << endl;
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

			if (lookAtTracker) {
				rot.makeRotate(ofVec3f(0, 1, 0), ofVec3f(0, 0, 1));
			}
			else {
				rot.makeRotate(ofVec3f(0, 1, 0), ofVec3f(0, 0, -1));
			}
			mat.rotate(rot);


			// assign tracker values
			trackers[id].pos.set(tempPos.x, -tempPos.z, tempPos.y); // why doesn't mat.getTranslation() work?
			trackers[id].orient.set(mat.getRotate());

			// update the internal ofNode matrix
			trackers[id].setTransformMatrix(mat);
			trackers[id].setPosition(trackers[id].pos);
			trackers[id].setOrientation(trackers[id].orient);

			// update the tracker trail
			trackers[id].update();


			// debug
			/*
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
			*/

		}


	}

}

// --------------------------------------------------------------
void ofApp::sendMessage(string msg) {

	stringstream ss;

	if (streamPRZ) {
		ss << "PRZ/";
		ss << ofToString(robot.PRZ.get().x) << "," <<
			ofToString(robot.PRZ.get().y) << "," <<
			ofToString(robot.PRZ.get().z) << endl;
	}
	else if (streamAngleExt) {
		ss << "AE/";
		ss << ofToString(robot.limbs[0].angle) << "," <<
			ofToString(robot.limbs[0].extension) << ";" <<
			ofToString(robot.limbs[1].angle) << "," <<
			ofToString(robot.limbs[1].extension) << ";" <<
			ofToString(robot.limbs[2].angle) << "," <<
			ofToString(robot.limbs[2].extension) << ";" <<
			ofToString(robot.limbs[3].angle) << "," <<
			ofToString(robot.limbs[3].extension) << endl;		
	}
	// interrupt streaming to send gains or flags
	if (sendGains) {
		// use the gains set to the first limb
		ss << "gain/";
		ss << ofToString(robot.limbs[0].gain_angle_pk) << "," <<
			ofToString(robot.limbs[0].gain_angle_pd) << "," <<
			ofToString(robot.limbs[0].gain_ext_pk) << "," <<
			ofToString(robot.limbs[0].gain_ext_pd) << endl;
	}
	if (sendFlag) {
		ss << "flag/";
		ss << msg << endl;
		//sendFlag = false;
	}

	// send the message over serial if we are intialized and enough time has passed
	if (serial.isInitialized() && ofGetElapsedTimeMillis() - tLastMsg > sendDelay){// && serial.available()) {

		string message = ss.str();
	
		unsigned char* chars = (unsigned char*)message.c_str();
		serial.writeBytes(chars, message.length());
	
		//cout << "Sending Message [ " << message << " ] to Minitaur" << endl;
		
		tLastMsg = ofGetElapsedTimeMillis();
	}
	else {
		//cout << "Serial NOT Initialized" << endl;
	}

}

// --------------------------------------------------------------
void ofApp::draw() {

	// draw 3D
	drawViewports();

	// draw GUI
	if (showGUI) {
		panel.draw();
		panel_robot.draw();
		panel_agent.draw();
	}

	
}

// -------------------------------------------------------------------
void ofApp::exit() {

	panel.saveToFile("settings.xml");
	robot.exit();
	//for (auto &m : limbs) {
	//	m.exit();
	//}


}


//--------------------------------------------------------------
void ofApp::drawViewports() {

	// show all 4 viewports
	if (viewport_showAll) {
		for (int i = 0; i < viewports.size(); i++) {

			// Draw 3D Elements
			drawViewport(i);

			// Draw 2D Elements
			ofPushStyle();
			ofNoFill();
			ofDrawRectangle(*viewports[i]);
			ofDrawBitmapString(viewport_labels[i], viewports[i]->x + 10, viewports[i]->y + 20);
			string behaviorLabel = "Robot Behavior Mode: " + robot.behaviorNames[robot.behaviorMode];
			ofSetColor(ofColor::white, 100);
			ofDrawBitmapString(behaviorLabel, viewports[i]->x + 10, viewports[i]->y + 35);
			
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
	}
	// show active viewport at full screen
	else {
		
		// Draw 3D Elements
		drawViewport(viewport_activeID);
		
		// Draw 2D Elements
		ofPushStyle();
		ofNoFill();
		ofDrawRectangle(*viewports[viewport_activeID]);
		ofDrawBitmapString(viewport_labels[viewport_activeID], viewports[viewport_activeID]->x + 10, viewports[viewport_activeID]->y + 20);
		string behaviorLabel = "Robot Behavior Mode: " + robot.behaviorNames[robot.behaviorMode];
		ofSetColor(ofColor::white, 100);
		ofDrawBitmapString(behaviorLabel, viewports[viewport_activeID]->x + 10, viewports[viewport_activeID]->y + 35);
		ofPopStyle();

	}
}

//--------------------------------------------------------------
void ofApp::drawViewport(int i) {

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



	robot.draw();
	if (robot.behaviorMode == Robot::BEHAVIOR::LOOKAT_TRACKER || robot.behaviorMode == Robot::BEHAVIOR::LOOKAT_AGENT) {
		gizmo_lookAt.draw(*cams[i]);
		
		if (robot.behaviorMode == Robot::BEHAVIOR::LOOKAT_AGENT)
			agent.draw();

	}
	else if (robot.behaviorMode == Robot::BEHAVIOR::PHYSICS) {
		
		ofPushStyle();
		world.drawDebug();

		ofFill();
		ofSetColor(100);
		ground.draw();

		ofNoFill();
		for (int i = 0; i < bounds.size() - 1; i++) {
			bounds[i]->draw();
		}

		ofFill();
		ofSetColor(200);
		box->draw();

		fulcrum->draw();

		ofSetColor(255, 0, 255);
		for (int i = 0; i < anchors.size(); i++) {
			anchors[i]->draw();
		}
		ofSetColor(220, 220, 220);
		for (int i = 0; i < joints.size(); i++) {
			joints[i]->draw();
		}
		
		ofPopStyle();

		gizmo_lookAt.draw(*cams[i]);
	}

	// draw only the trackers that are currently being tracked
	//for (auto &tracker : trackers) {
	//	if (trackingIDs[tracker.second.id] && tracker.second.isConnected && tracker.second.id != 0) { // don't show HMD yet ... not working
	//		tracker.second.draw();
	//	}
	//}
	

	if (robot.showGizmo) {
		robot.gizmo.draw(*cams[i]);
	}

	ofDisableDepthTest();
	cams[i]->end();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	// send key inputs to Minitaur over serial
	if (key == OF_KEY_UP) {

	}
	else if (key == OF_KEY_DOWN) {
		//sendMessage("pitch/-");
	}
	else if (key == OF_KEY_RIGHT) {
		
	}
	else if (key == 'g') {
		showGUI = !showGUI;
	}
	else if (key == 'd') {
		robot.showDebug = !robot.showDebug;
	}
	

	
	// keys mapped: 'f', ' '
	handleViewportPresets(key);
	// keys mapped: 'w', 'e', 'c'
	handleGizmoPresets(key);
	// keys mapped: '1', '2', '3', '4'
	handleCameraPresets(key);

}

//--------------------------------------------------------------
void ofApp::handleViewportPresets(int key) {

	if (key == 'f') {
		ofToggleFullscreen();
		panel.setPosition(ofGetWidth() - 200 - 10, 10);
		panel_robot.setPosition(panel.getPosition().x - 210, 10);
		panel_agent.setPosition(panel_robot.getPosition().x - 210, 10);
		robot.gizmo.setViewDimensions(ofGetWidth(), ofGetHeight());
		gizmo_lookAt.setViewDimensions(ofGetWidth(), ofGetHeight());

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

			world.setCamera(cams[viewport_activeID]);
		}

	}

}


//--------------------------------------------------------------
void ofApp::handleGizmoPresets(int key) {

	// gizmo controls
	if (key == 'e' || key == 'E') {
		robot.gizmo.setType(ofxGizmo::OFX_GIZMO_ROTATE);
	}
	if (key == 'w' || key == 'W') {
		robot.gizmo.setType(ofxGizmo::OFX_GIZMO_MOVE);
	}
	if (key == 'c') {
		robot.reset();		
	}

}

//--------------------------------------------------------------
void ofApp::handleCameraPresets(int key) {
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
		cams[viewport_activeID]->setPosition(dist, dist, 2 * dist);
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
	params_com.setName("COM Parameters");
	params_com.add(msg_status.set("COM PORT", COM_PORT));
	params_com.add(msg_listening.set("baud", ofToString(baud)));
	params_com.add(streamPRZ.set("Stream RPZ", false));
	params_com.add(streamAngleExt.set("Stream Angle/Ext", false));
	//params_com.add(sendGains.set("Send Gains", false));
	//params_com.add(sendFlag.set("Send Flag", false));

	params_osc.setName("OSC Parameters");
	params_osc.add(msg_status_osc.set("IP Addr", "Not Connected..."));
	params_osc.add(msg_listening_osc.set("PORT", ""));
	
	params_tracking.setName("Tracking Options");
	params_tracking.add(trackingIDs[0].set("HMD", false));
	params_tracking.add(trackingIDs[1].set("Left Controller", false));
	params_tracking.add(trackingIDs[2].set("Right Controller", false));
	params_tracking.add(trackingIDs[3].set("Tracker " + ofToString(1), false));
	params_tracking.add(trackingIDs[4].set("Tracker " + ofToString(2), false));

	// 3D Nav specific
	//params_gui.setName("Navigation");
	//.add(gizmo_isVisible.set("Show Gizmo", true));

	panel.setup(params_com);
	panel.add(sendGains.setup("Send Gains"));
	panel.add(sendFlag.setup("Send Flag"));
	panel.add(params_osc);
	panel.add(reconnect.setup("Reconnect"));
	panel.add(params_tracking);
	//panel.add(params_gui);
	//panel.add(save.setup("Save"));

	panel_robot.setup(robot.params);
	panel_agent.setup(agent.params);

	panel.setPosition(ofGetWidth() - 200 - 10, 10);
	panel_robot.setPosition(panel.getPosition().x - 210, 10);
	panel_agent.setPosition(panel_robot.getPosition().x - 210, 10);

	if (bLoadFromFile)
		panel.loadFromFile("settings.xml");

	reconnect.addListener(this, &ofApp::reconnectBtnPressed);
	//save.addListener(this, &ofApp::saveBtnPressed);
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
//void ofApp::saveBtnPressed() {
//
//	// do save
//
//}

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

void ofApp::loadModels() {

	// load 3D model of Vive Tracker
	if (loader.loadModel(ofToDataPath("models/TRACKER-3D.stl"), false)) {
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
	cout << "Number of Meshes Loaded: " << ofToString(loader.getNumMeshes()) << endl;

}



// ------------------------------------------------------------------

void ofApp::printmap(map<int, string> & m) {
	for (const auto& elem : m) {
		cout << " [id, pos], [" << elem.first << ", " << ofToString(elem.second) << "]" << endl;
	}
	cout << endl << "size() == " << m.size() << endl << endl;
}


//--------------------------------------------------------------
void ofApp::setupPhysics() {

	// setup the world
	world.setup();
	world.enableGrabbing();
	world.setCamera(cams[3]);
	world.setGravity(ofVec3f(0, 0, -1));
	//world.world->stepSimulation(btScalar(1.) / btScalar(60.), btScalar(1.) / btScalar(60.)); // don't really know what this is doing

	// enable mouse pick events 
	ofAddListener(world.MOUSE_PICK_EVENT, this, &ofApp::mousePickEvent);

	// enable collision  events 
	//world.enableCollisionEvents();
	//ofAddListener(world.COLLISION_EVENT, this, &ofApp::onCollision);

	// create a bounding area
	ground.create(world.world, ofVec3f(0., 0., 0), 0., 3000.f, 3000.f, 1.f);
	ground.setProperties(.25, .95);
	ground.add();


	// try a bounding box for picking
	ofVec3f startLoc;
	ofPoint dimens;
	boundsWidth = 3000.;
	float hwidth = boundsWidth * .5;
	float depth = 2.;
	float hdepth = depth * .5;
	boundsShape = new ofxBulletCustomShape();
	boundsShape->create(world.world, ofVec3f(0, 0, 0), 10.);

	for (int i = 0; i < 6; i++) {
		bounds.push_back(new ofxBulletBox());
		if (i == 0) { // ground //
			startLoc.set(0., hwidth + hdepth, 0.);
			dimens.set(boundsWidth, depth, boundsWidth);
		}
		else if (i == 1) { // back wall //
			startLoc.set(0, 0, hwidth + hdepth);
			dimens.set(boundsWidth, boundsWidth, depth);
		}
		else if (i == 2) { // right wall //
			startLoc.set(hwidth + hdepth, 0, 0.);
			dimens.set(depth, boundsWidth, boundsWidth);
		}
		else if (i == 3) { // left wall //
			startLoc.set(-hwidth - hdepth, 0, 0.);
			dimens.set(depth, boundsWidth, boundsWidth);
		}
		else if (i == 4) { // ceiling //
			startLoc.set(0, -hwidth - hdepth, 0.);
			dimens.set(boundsWidth, depth, boundsWidth);
		}
		else if (i == 5) { // front wall //
			startLoc.set(0, 0, -hwidth - hdepth);
			dimens.set(boundsWidth, boundsWidth, depth);
		}
		btBoxShape* boxShape = ofBtGetBoxCollisionShape(dimens.x, dimens.y, dimens.z);
		boundsShape->addShape(boxShape, startLoc);

		bounds[i]->create(world.world, startLoc, 0., dimens.x, dimens.y, dimens.z);
		bounds[i]->setProperties(.25, .95);
		bounds[i]->add();
	}


	// setup the fulcrum to balance the body
	fulcrum = new ofxBulletCone();
	// reorient the cone so that Z is up	
	tr_fulcrum.setIdentity();
	btQuaternion quat;
	quat.setEuler(0, ofDegToRad(90), 0);
	tr_fulcrum.setRotation(quat);
	tr_fulcrum.setOrigin(btVector3(0, 0, 75));
	fulcrum->create(world.world, tr_fulcrum, 10, 50, 140);
	// make the fulcrum a static rigid body
	fulcrum->getRigidBody()->setLinearFactor(btVector3(0, 0, 0));
	fulcrum->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));
	fulcrum->getRigidBody()->setGravity(btVector3(0, 0, 0));
	// add to physics world
	fulcrum->add();

	// setup robot body
	boxShape = ofBtGetBoxCollisionShape(240, 410, 100);
	box = new ofxBulletBox();
	box->init(boxShape);
	box->create(world.world, ofVec3f(0, 0, 140), 100);
	box->setActivationState(DISABLE_DEACTIVATION);
	box->setRestitution(1);
	box->add();

	// setup joints to attached body to ground
	anchors.push_back(new ofxBulletSphere());
	((ofxBulletSphere*)anchors[anchors.size() - 1])->create(world.world, ofVec3f(0, box->getHeight() / 2, box->getPosition().z), 10, 50);
	anchors[0]->getRigidBody()->setLinearFactor(btVector3(0, 0, 0));
	anchors[0]->getRigidBody()->setAngularFactor(btVector3(0, 0, 0));
	anchors[0]->getRigidBody()->setGravity(btVector3(0, 0, 0));
	
	anchors[anchors.size() - 1]->add();


	// made in same ordering as minitaur legs
	joints.push_back(new ofxBulletJoint());
	joints[joints.size() - 1]->create(world.world, box, ofVec3f(-box->getWidth() / 2, box->getHeight() / 2, 0));
	joints[joints.size() - 1]->add();

	joints.push_back(new ofxBulletJoint());
	joints[joints.size() - 1]->create(world.world, box, ofVec3f(-box->getWidth() / 2, -box->getHeight() / 2, 0));
	joints[joints.size() - 1]->add();

	joints.push_back(new ofxBulletJoint());
	joints[joints.size() - 1]->create(world.world, box, ofVec3f(box->getWidth() / 2, box->getHeight()/2, 0));
	joints[joints.size() - 1]->add();

	joints.push_back(new ofxBulletJoint());
	joints[joints.size() - 1]->create(world.world, box, ofVec3f(box->getWidth() / 2, -box->getHeight() / 2, 0));
	joints[joints.size() - 1]->add();

	for (auto &joint : joints) {

		// restrict motion in the XZ Plane
		joint->getRigidBodyA()->setLinearFactor(btVector3(0, 1, 1));
		joint->getRigidBodyB()->setLinearFactor(btVector3(0, 1, 1));
		
	}
	
}

//--------------------------------------------------------------
void ofApp::mousePickEvent(ofxBulletMousePickEvent &e) {

	cams[viewport_activeID]->disableMouseInput();

		cout << "touch!" << endl;
}

btVector3 ofApp::toBT(ofVec3f pos)
{
	return btVector3(pos.x, pos.y, pos.z);
}

btQuaternion ofApp::toBT(ofQuaternion orient)
{
	return btQuaternion(orient.x(), orient.y(), orient.z(), orient.w());
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (!cams[viewport_activeID]->getMouseInputEnabled())
		cams[viewport_activeID]->enableMouseInput();

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

