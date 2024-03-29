#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "MotionController.h"
#include "AgentController.h"

#define NUM_AGENTS 10

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
    
    
        MotionController motion;
        AgentController agents;
    
    
        void draw_world();
		
        ofEasyCam cam;
        void setup_camera();
        void keypressed_camera(int key);
    
    private:
    
        void setup_gui();
        ofxPanel panel, panel_agents;
        ofParameterGroup params;
        ofParameter<bool> show_gui, show_top, show_front, show_side, show_perspective;
        void listener_show_top(bool & val);
        void listener_show_front(bool & val);
        void listener_show_side(bool & val);
        void listener_show_perspective(bool & val);
    
    
        ofColor background_inner = ofColor(238);
        ofColor background_outer = ofColor(118);
};
