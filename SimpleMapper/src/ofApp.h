#pragma once

#include "ofMain.h"
#include "ofxGui.h"

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
		
        bool load_gui_from_file = false;
    
        //------- MAPPING --------
        ofTexture  texScreen;
        ofPolyline quad; // <-- should save/load to file
    
        // debugging
        void test_texture_warping();
        void render_dummy_texture();
        void warp_texture();
    
    
        // calibrating
        bool is_adjusting = false;
    
        void draw_quad();
    
        //--------- GUI ----------
        ofxPanel panel;
        ofParameterGroup params;
        ofParameter<bool> show_gui;
        ofParameter<bool> mode_project;
        ofParameter<bool> mode_calibrate;
        ofParameter<ofVec2f> p1, p2, p3, p4;
    
        void listener_p1(ofVec2f &val);
        void listener_p2(ofVec2f &val);
        void listener_p3(ofVec2f &val);
        void listener_p4(ofVec2f &val);
    
        void setup_gui();

    
};
