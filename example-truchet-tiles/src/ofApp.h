#pragma once

#include "ofMain.h"
#include "ofxEasing.h"

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
    
        vector<ofImage*> tiles;
        void load_tiles();
        int num_tiles = 0;
    
        vector<vector<int>> turns_curr;
        vector<vector<int>> turns_next;
        void randomize_angles();
        int x_grid = 20;
        int y_grid = 16;
    
        float turn_time = 6;
        float wait_time = turn_time/3;
        float prev_turn_time=0;

};
