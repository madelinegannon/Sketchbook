#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxGizmo.h"

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
    
        ofEasyCam cam;

    
        vector<ofVec3f> circle_0, circle_1, helix;
        ofxGizmo gizmo_circle_0, gizmo_circle_1;
    
    private:
        void setup_gui();
        void draw_gui();
    
        void setup_world();
        void draw_world();
    
        void setup_camera();
        void keypressed_camera(int key);
        bool do_disable_camera();
    
        void keypressed_gizmo(int key);
    
        ofxPanel panel;
        ofParameterGroup params;
        ofParameter<bool> show_gui, show_top, show_front, show_side, show_perspective;
        void listener_show_top(bool & val);
        void listener_show_front(bool & val);
        void listener_show_side(bool & val);
        void listener_show_perspective(bool & val);
        
        ofxPanel panel_cyl;
        ofParameterGroup params_cyl;
        ofParameter<float> cyl_height, cyl_radius, cyl_resolution;
        ofParameter<float> start_angle, end_angle;
        ofParameter<int> num_turns;

        ofColor background_inner = ofColor(238);
        ofColor background_outer = ofColor(118);
};
