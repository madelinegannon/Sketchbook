#pragma once

#include "ofMain.h"
#include "ofxGui.h"

// Modify all the pixels within a given region of interest (roi)

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    
        ofVideoGrabber vidGrabber;
        ofImage img_src, img;
    
        int imgWidth = 640;
        int imgHeight = 480;
    
        // GUI
        void setup_gui();
        ofxPanel panel;
        ofParameter<bool> use_webcam;
        ofParameterGroup grid;
        ofParameter<bool> square_grid;
        ofParameter<int> cell_width;
        ofParameter<int> x_grid;
        ofParameter<int> y_grid;
};
