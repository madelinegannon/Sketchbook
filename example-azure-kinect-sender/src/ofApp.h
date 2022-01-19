#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "DeviceCapturer.h"
#include "image.pb.h"

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();
	void exit();

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

	ofxTCPClient tcpClient;

	string connection_info;

	string msgTx, msgRx;
	float counter;

private:
	void reconnect();
	int connectTime;
	int deltaTime;
	int size;

	bool is_streaming = false;

	ofShortPixels test_buffer;
	ofTexture test_texture;
	void update_pixels_ir(string output);
	uint8_t * ir_raw_buffer;

	DeviceCapturer device;
};
