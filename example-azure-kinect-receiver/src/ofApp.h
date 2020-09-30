#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
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

	ofxTCPServer TCP;
	vector<string> storeText;
	uint64_t lastSent;

	ofShortPixels ir_pix;
	ofTexture ir_tex;
	
	void update_pixels_ir(string output);
	void set_texture(uint8_t* image_data, ofShortPixels * pix, ofTexture * tex);
};
