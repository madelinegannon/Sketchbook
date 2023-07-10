#pragma once

/*
* This is ofxAzureKinect's example-bodies example, 
* but with streaming out the closest skeleton over OSC.
 */

#include "ofMain.h"

#include "ofxAzureKinect.h"
#include "ofxOsc.h"

class ofApp 
	: public ofBaseApp 
{
public:
	void setup();
	void exit();

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

private:
	ofxAzureKinect::Device kinectDevice;

	ofEasyCam camera;

	ofVbo pointsVbo;
	ofShader shader;

	ofVboMesh skeletonMesh;

	ofxOscSender sender;
	int port = 55555;
	string ip_address = "192.168.0.11";

	void setup_osc(string ip_addr, int port);
	void send_skeleton();
	int get_closest_skeleton();
};
