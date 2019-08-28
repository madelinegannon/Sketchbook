#pragma once

#include "ofMain.h"

// Simple app to draw polylines with mouse, and to
// save and load premade polylines from a csv file.

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

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
    
        vector<ofPolyline*> paths;
        void save_paths(string filename="paths.csv");
        void load_paths(string filename="paths.csv");
};
