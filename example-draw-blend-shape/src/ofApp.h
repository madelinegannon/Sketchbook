#pragma once

#include "ofMain.h"
#include "ofxGui.h"

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
        glm::vec3 mouse;
        ofPlanePrimitive ground;
    
        vector<ofPolyline*> paths;
    
        void setup_gui();
        void setup_camera();
        void setup_world();
        void draw_world();
    
        void draw_none();
        void draw_offset();
        void draw_spiral();

        ofxPanel panel;
        ofParameterGroup params;
        ofParameter<bool> none, offset, spiral, blend, animate_step_over;
        ofParameter<float> step_over, resolution;
    
        void listener_none(bool &val);
        void listener_offset(bool &val);
        void listener_spiral(bool &val);
        void listener_resolution(float &val);
};
