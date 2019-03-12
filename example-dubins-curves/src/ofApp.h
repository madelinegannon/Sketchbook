#pragma once

#include "ofMain.h"
#include "Dubins.h"
#include "ofxGui.h"

/*
 * TO DO:
 * - [x] Add arrow keys for selecting curve indices (LEFT/RIGHT) and types (UP/DOWN)
 * - [x] Add save button
 * - [ ] Set different start and goal radiuses?
 *    - [x] Working for CSC
 *    - [ ] Working for CCC
 * - [ ] Edit waypoint orientation (with gizmo)
 * - [ ] Add waypoint picker?
 */

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
		
    
        Dubins dubins_path;
        bool show_debug = true;
        int mouse_pressed_count = 0;
    
        vector<ofVec3f> pts;
    
        ofxPanel panel;
        bool show_gui = true;
};
