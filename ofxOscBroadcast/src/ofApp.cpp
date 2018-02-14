#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60); // run at 60 fps
	ofSetVerticalSync(true);

	// set bg to a nice grey!
	ofBackground(150);

	//Server
	//listen for incoming messages on a port; setup OSC receiver with usage:
	serverRecvPort = 9000;
	serverReceiver.setup(serverRecvPort);
	// set sending port
	serverSendPort = serverRecvPort + 1;

}

//--------------------------------------------------------------
void ofApp::update(){

	checkForNewClients();

	if (knownClients.size() > 0) {
		broadcastToClients();
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
void ofApp::draw(){
	
	ofBackground(150);

	if (debugOsc) {

		stringstream ss;

		ss << "Broadcasting on PORT " << serverSendPort << " to the following IP addresses:" << endl;
		for (int i = 0; i < knownClients.size(); i++) {
			ss << "\t" << knownClients[i] << endl;
		}
		ofDrawBitmapStringHighlight(ss.str(), 15, 15);

	}
}

//--------------------------------------------------------------
void ofApp::broadcastToClients() {

	//create a new OSC message
	ofxOscMessage m;
	m.setAddress("/pos");
	m.addInt32Arg(ofGetMouseX());
	m.addInt32Arg(ofGetMouseY());

	cout << "knownClients.size(): " << knownClients.size() << endl;
	//Send message to all known hosts
	// use another port to avoid a localhost loop
	for (int i = 0; i < knownClients.size(); i++) {
		serverSender.setup(knownClients[i], serverSendPort);
		m.setRemoteEndpoint(knownClients[i], serverSendPort);
		serverSender.sendMessage(m);// , false);
		ofLogVerbose("Server broadcasting message {" + ofToString(m.getArgAsInt32(0)) + "," + ofToString(m.getArgAsInt32(1)) + "} to " + m.getRemoteIp()
			+ ":" + ofToString(m.getRemotePort()));
		cout << "Server broadcasting message {" << ofToString(m.getArgAsInt32(0)) + "," << ofToString(m.getArgAsInt32(1)) + "} to " + m.getRemoteIp()
			<< ":" << ofToString(m.getRemotePort()) << endl;

	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
