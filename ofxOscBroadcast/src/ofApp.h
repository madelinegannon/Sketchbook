#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

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

	//----------------------------------------
	// OSC Server:

	bool debugOsc = true;

	ofxOscReceiver serverReceiver; // OSC receiver
	int serverRecvPort; // port we're listening on: must match port from sender!
	int serverSendPort; // port we're SENDING on


						 // Message display variables
	vector<string>serverMessages; //vector containing the received messages for display
	unsigned int maxServerMessages; //nr of messages fitting on the screen

	void checkForNewClients();
	vector<string>knownClients; //collected IP's of chat participants
	ofxOscSender serverSender;

	//Distribute a received message among the known hosts
	void broadcastToClients();

};
