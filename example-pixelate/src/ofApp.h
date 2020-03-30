#pragma once

#include "ofMain.h"
#include "ofxGui.h"

/// Pixelates an image or webcam stream with a few parameters:
/// – number of cells in X and Y
/// – width of cells in X and Y
/// – resizes image/stream in X and Y

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
		
    
    // Webcam stuff
    ofVideoGrabber vidGrabber;
    ofImage img, vid_img;
    
    
    // Pixelator
    ofRectangle base_rect;
    void draw_cells(int x, int y);
    ofColor get_avg_col(ofRectangle * _cell, ofPixels * _pix);
    
    
    
    // GUI
    void setup_gui();
    ofxPanel panel;
    ofParameter<bool> use_webcam;
    ofParameterGroup image;
    ofParameter<int> x_resize;
    ofParameter<int> y_resize;
    
    ofParameterGroup grid;
    ofParameter<bool> square_grid;
    ofParameter<int> x_grid;
    ofParameter<int> y_grid;
    
    // next step ... simplify: reduce down to 4 or 6 preset colors
    // ... look into color distance:    
    /**
     More generally, you can create a system that tracks a (single) spot with a specific color. A very simple way to achieve this is to find the pixel whose color has the shortest Euclidean distance (in "RGB space") to the target color. Here is a code fragment which shows this.
     https://openframeworks.cc/ofBook/chapters/image_processing_computer_vision.html#pointprocessingoperationsonimages
     */
    
};
