#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(false);

	// We need to pass the method we want ofxOpenVR to call when rending the scene
	openVR.setup(std::bind(&ofApp::render, this, std::placeholders::_1));
	openVR.setDrawControllers(true);

	ofAddListener(openVR.ofxOpenVRControllerEvent, this, &ofApp::controllerEvent);

	_texture.load("of.png");
	_texture.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
	
	_box.set(1);
	_box.enableColors();
	_box.mapTexCoordsFromTexture(_texture.getTexture());
	
	// Create a translation matrix to place the box in the space
	_translateMatrix.translate(ofVec3f(0.0, .0, -2.0));

	// Vertex shader source
	string vertex;

	vertex = "#version 150\n";
	vertex += STRINGIFY(
						uniform mat4 matrix;

						in vec4  position;
						in vec2  texcoord;

						out vec2 texCoordVarying;

						void main()
						{
							texCoordVarying = texcoord;
							gl_Position = matrix * position;

						}
					);

	// Fragment shader source
	string fragment = "#version 150\n";
	fragment += STRINGIFY(
						uniform sampler2DRect baseTex;
						
						in vec2 texCoordVarying;

						out vec4 fragColor;

						vec2 texcoord0 = texCoordVarying;

						void main() {
							vec2 tx = texcoord0;
							tx.y = 256.0 - tx.y;
							vec4 image = texture(baseTex, tx);
							fragColor = image;
						}
					);

	// Shader
	_shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
	_shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
	_shader.bindDefaults();
	_shader.linkProgram();

	// Controllers
	_controllerBox.set(.1);
	_controllerBox.enableColors();

	// Vertex shader source
	vertex = "#version 150\n";
	vertex += STRINGIFY(
						uniform mat4 matrix;

						in vec4 position;
						in vec3 v3ColorIn;

						out vec4 v4Color;

						void main() {
							v4Color.xyz = v3ColorIn; v4Color.a = 1.0;
							gl_Position = matrix * position;
						}
					);

	// Fragment shader source
	fragment = "#version 150\n";
	fragment += STRINGIFY(
						in vec4 v4Color;
						out vec4 outputColor;
						void main() {
							outputColor = v4Color;
						}
					);

	// Shader
	_controllersShader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
	_controllersShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
	_controllersShader.bindDefaults();
	_controllersShader.linkProgram();

	//Server
	//listen for incoming messages on a port; setup OSC receiver with usage:
	serverReceiver.setup(serverRecvPort);
}


//--------------------------------------------------------------
void ofApp::exit() {
	ofRemoveListener(openVR.ofxOpenVRControllerEvent, this, &ofApp::controllerEvent);

	openVR.exit();
}

//--------------------------------------------------------------
void ofApp::update(){
	openVR.update();


	// update Server
	checkForNewClients();

	if (knownClients.size() > 0) {
		//broadcastToClients();

		

		handleTrackers();
		handleControllers();
		handleHMD();
	}


}

//--------------------------------------------------------------
void ofApp::draw(){
	openVR.render();

	openVR.renderDistortion();

	openVR.drawDebugInfo();

	if (debugOsc) {

		stringstream ss;

		ss << "Broadcasting on PORT " << serverSendPort << " to the following IP addresses:" << endl;
		for (int i = 0; i < knownClients.size(); i++) {
			ss << "\t" << knownClients[i] << endl;
		}
		ofDrawBitmapStringHighlight(ss.str(), 10, 250);

	}
}

//--------------------------------------------------------------
void ofApp::checkForNewClients() {
	// check for waiting messages
	while (serverReceiver.hasWaitingMessages()) {
		// get the next message
		ofxOscMessage m;
		serverReceiver.getNextMessage(&m);
		//Log received message for easier debugging of participants' messages:
		ofLogVerbose("Server recvd msg from " + m.getRemoteIp());

		// check the address of the incoming message
		if (m.getAddress() == "/ping") {
			//Identify host of incoming msg
			string incomingHost = m.getRemoteIp();
			//See if incoming host is a new one:
			if (std::find(knownClients.begin(), knownClients.end(), incomingHost)
				== knownClients.end()) {
				knownClients.push_back(incomingHost); //add new host to list
				ofLogVerbose("YAY! Someone new is listening!");
			}

		}
		// handle getting random OSC messages here
		else {
			ofLogWarning("Server got weird message: " + m.getAddress());
		}
	}
}

//--------------------------------------------------------------
void ofApp::handleTrackers() {



	map<int, glm::mat4x4> trackers = openVR.getTrackers();
	for (auto &tracker : trackers) {


		if (openVR.isTrackerConnected(vr::TrackedDeviceClass_GenericTracker, tracker.first)) {

			ofMatrix4x4 mat = toOf(tracker.second);
			ofVec3f P;
			ofQuaternion Q;
			ofVec3f  S;
			ofQuaternion SO;
			mat.decompose(P, Q, S, SO);

			ofxOscMessage m;
			m.setAddress("/tracker");
			m.addInt32Arg(tracker.first + 2); // tracker ID's begin after controllers

			m.addFloatArg(P.x);
			m.addFloatArg(P.y);
			m.addFloatArg(P.z);

			m.addFloatArg(S.x);
			m.addFloatArg(S.y);
			m.addFloatArg(S.z);

			m.addFloatArg(Q.x());
			m.addFloatArg(Q.y());
			m.addFloatArg(Q.z());
			m.addFloatArg(Q.w());

			m.addFloatArg(SO.x());
			m.addFloatArg(SO.y());
			m.addFloatArg(SO.z());
			m.addFloatArg(SO.w());

			broadcastToClients(m);
		}



	}
}

//--------------------------------------------------------------
void ofApp::handleHMD()
{
	//ofMatrix4x4 currentViewProjectionMatrix = &openVR.getCurrentViewProjectionMatrix(nEye)[0][0];
	//ofMatrix4x4 hdmPoseMat = _translateMatrix * currentViewProjectionMatrix;

	ofMatrix4x4 mat = hmdMat;
	ofVec3f P;
	ofQuaternion Q;
	ofVec3f  S;
	ofQuaternion SO;
	mat.decompose(P, Q, S, SO);

	ofxOscMessage m;
	m.setAddress("/hmd");
	m.addInt32Arg(0); // tracker ID's begin after controllers

	m.addFloatArg(P.x);
	m.addFloatArg(P.y);
	m.addFloatArg(P.z);

	m.addFloatArg(S.x);
	m.addFloatArg(S.y);
	m.addFloatArg(S.z);

	m.addFloatArg(Q.x());
	m.addFloatArg(Q.y());
	m.addFloatArg(Q.z());
	m.addFloatArg(Q.w());

	m.addFloatArg(SO.x());
	m.addFloatArg(SO.y());
	m.addFloatArg(SO.z());
	m.addFloatArg(SO.w());

	broadcastToClients(m);
}

//--------------------------------------------------------------
void ofApp::handleControllers() {


	if (openVR.isControllerConnected(vr::TrackedControllerRole_LeftHand)) {

		ofMatrix4x4 mat = toOf(openVR.getControllerPose(vr::TrackedControllerRole_LeftHand));
		ofVec3f P;
		ofQuaternion Q;
		ofVec3f  S;
		ofQuaternion SO;
		mat.decompose(P, Q, S, SO);

		if (sendLeftController) {

			ofxOscMessage m;
			m.setAddress("/controller/left");
			m.addInt32Arg(vr::TrackedControllerRole_LeftHand);

			m.addFloatArg(P.x);
			m.addFloatArg(P.y);
			m.addFloatArg(P.z);

			m.addFloatArg(S.x);
			m.addFloatArg(S.y);
			m.addFloatArg(S.z);

			m.addFloatArg(Q.x());
			m.addFloatArg(Q.y());
			m.addFloatArg(Q.z());
			m.addFloatArg(Q.w());

			m.addFloatArg(SO.x());
			m.addFloatArg(SO.y());
			m.addFloatArg(SO.z());
			m.addFloatArg(SO.w());

			broadcastToClients(m);
		}
	}

	// Right controller
	if (openVR.isControllerConnected(vr::TrackedControllerRole_RightHand)) {

		ofMatrix4x4 mat = toOf(openVR.getControllerPose(vr::TrackedControllerRole_RightHand));
		ofVec3f P;
		ofQuaternion Q;
		ofVec3f  S;
		ofQuaternion SO;
		mat.decompose(P, Q, S, SO);

		if (sendLeftController) {

			ofxOscMessage m;
			m.setAddress("/controller/right");
			m.addInt32Arg(vr::TrackedControllerRole_RightHand);

			m.addFloatArg(P.x);
			m.addFloatArg(P.y);
			m.addFloatArg(P.z);

			m.addFloatArg(S.x);
			m.addFloatArg(S.y);
			m.addFloatArg(S.z);

			m.addFloatArg(Q.x());
			m.addFloatArg(Q.y());
			m.addFloatArg(Q.z());
			m.addFloatArg(Q.w());

			m.addFloatArg(SO.x());
			m.addFloatArg(SO.y());
			m.addFloatArg(SO.z());
			m.addFloatArg(SO.w());

			broadcastToClients(m);
		}
	}
}

//--------------------------------------------------------------
//void ofApp::broadcastToClients() {
//
//	ofxOscMessage m;
//
//
//	// Left controller
//	if (openVR.isControllerConnected(vr::TrackedControllerRole_LeftHand) && sendLeftController) {
//
//		glm::vec4 controllerPoseGlm = openVR.getControllerPose(vr::TrackedControllerRole_LeftHand)[3];
//		ofVec3f controllerPose = ofVec3f(controllerPoseGlm.x, controllerPoseGlm.y, controllerPoseGlm.z);
//
//		m.setAddress("/left/pos");
//		m.addFloatArg(controllerPose.x);
//		m.addFloatArg(controllerPose.y);
//		m.addFloatArg(controllerPose.z);
//
//	}
//
//	// Right controller
//	if (openVR.isControllerConnected(vr::TrackedControllerRole_RightHand) ) {
//
//		auto pose = openVR.getControllerPose(vr::TrackedControllerRole_RightHand);
//
//
//		
//	}
//
//	//vr::TrackedDeviceClass_GenericTracker
//
//	
//
//
//	////create a new OSC message
//	//ofxOscMessage m;
//	//m.setAddress("/pos");
//	//m.addInt32Arg(ofGetMouseX());
//	//m.addInt32Arg(ofGetMouseY());
//
//	cout << "knownClients.size(): " << knownClients.size() << endl;
//	//Send message to all known hosts
//	// use another port to avoid a localhost loop
//	for (int i = 0; i < knownClients.size(); i++) {
//		serverSender.setup(knownClients[i], serverSendPort);
//		m.setRemoteEndpoint(knownClients[i], serverSendPort);
//		serverSender.sendMessage(m);// , false);
//		//ofLogVerbose("Server broadcasting message {" + ofToString(m.getArgAsInt32(0)) + "," + ofToString(m.getArgAsInt32(1)) + "} to " + m.getRemoteIp()
//		//	+ ":" + ofToString(m.getRemotePort()));
//		//cout << "Server broadcasting message {" << ofToString(m.getArgAsInt32(0)) + "," << ofToString(m.getArgAsInt32(1)) + "} to " + m.getRemoteIp()
//		//	<< ":" << ofToString(m.getRemotePort()) << endl;
//
//	}
//}


//--------------------------------------------------------------
void ofApp::broadcastToClients(ofxOscMessage m) {

	//Send message to all known hosts
	// use another port to avoid a localhost loop
	for (int i = 0; i < knownClients.size(); i++) {
		serverSender.setup(knownClients[i], serverSendPort);
		m.setRemoteEndpoint(knownClients[i], serverSendPort);
		serverSender.sendMessage(m);

	}

}



//--------------------------------------------------------------
void  ofApp::render(vr::Hmd_Eye nEye)
{

	// OF textured cube
	//glm::mat4x4 currentViewProjectionMatrix = openVR.getCurrentViewProjectionMatrix(nEye);
	//glm::mat4x4 hdmPoseMat = _translateMatrix * currentViewProjectionMatrix;

	ofMatrix4x4 currentViewProjectionMatrix = &openVR.getCurrentViewProjectionMatrix(nEye)[0][0];
	ofMatrix4x4 hdmPoseMat = _translateMatrix * currentViewProjectionMatrix;
	hmdMat = hdmPoseMat;

	_shader.begin();
	_shader.setUniformMatrix4f("matrix", hdmPoseMat, 1);
	_shader.setUniformTexture("baseTex", _texture, 0);
	_box.draw();
	_shader.end();

	// Left controller
	if (openVR.isControllerConnected(vr::TrackedControllerRole_LeftHand)) {
		
		glm::mat4x4 leftControllerPoseMat = openVR.getCurrentViewProjectionMatrix(nEye) * openVR.getControllerPose(vr::TrackedControllerRole_LeftHand);
		_controllersShader.begin();
		_controllersShader.setUniformMatrix4f("matrix", toOf(leftControllerPoseMat), 1);
		_controllerBox.drawWireframe();
		_controllersShader.end();
	}

	// Right controller
	if (openVR.isControllerConnected(vr::TrackedControllerRole_RightHand)) {
		glm::mat4x4 rightControllerPoseMat = openVR.getCurrentViewProjectionMatrix(nEye) * openVR.getControllerPose(vr::TrackedControllerRole_RightHand);

		_controllersShader.begin();
		_controllersShader.setUniformMatrix4f("matrix", toOf(rightControllerPoseMat), 1);
		_controllerBox.drawWireframe();
		_controllersShader.end();
	}

	// Trackers
	map<int, glm::mat4x4> tracker = openVR.getTrackers();
	for (auto &ent1 : tracker) {

		glm::mat4x4 trackerPoseMat = openVR.getCurrentViewProjectionMatrix(nEye) * ent1.second;
		
		_controllersShader.begin();
		_controllersShader.setUniformMatrix4f("matrix", toOf(trackerPoseMat), 1);
		_controllerBox.drawWireframe();
		_controllersShader.end();
	}
}

//--------------------------------------------------------------
void ofApp::controllerEvent(ofxOpenVRControllerEventArgs& args)
{
	cout << "ofApp::controllerEvent > role: " << (int)args.controllerRole << " - event type: " << (int)args.eventType << " - button type: " << (int)args.buttonType << " - x: " << args.analogInput_xAxis << " - y: " << args.analogInput_yAxis << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	openVR.toggleGrid();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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

