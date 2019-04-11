#pragma once

#include "ofMain.h"
#include "ofxGui.h"

/*
 * PID Example
 * This is a PD Controller for the 2D trajectory of a point navigating to a target>
 *
 * The Basics:
 * Kp acts like a spring, Kd acts like a dampener, and Ki gets rid of steady-state error
 *
 * See PID Control: A Brief History https://www.youtube.com/watch?v=UR0hOmjaHp0
 * and Simple Examples of PID Controllers https://www.youtube.com/watch?v=XfAt6hNV8XM
 * for more details.
 *
 * @author mad
 * 04.10.2019
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
		
    
        ofNode vehicle;
    
    
    private:
        ofVec3f start, end;
        ofVec3f desired_pos, desired_vel, curr_pos, curr_vel;
    
        void update_target();
        void update_desired_vel();
        void update_current_vel();
        void update_position();
    
        // just use a regular time step for now
        float dt = 1/60.;
    
        ofVec3f accel;
        ofVec3f heading;
        ofVec3f vel_projected; // curr_vel projected onto the heading vector
    
        void setup_gui();
        ofxPanel panel;
        ofParameterGroup params;
        ofParameter<bool> show_debug;
        ofParameter<float> kp, kd;
        ofParameter<bool> use_moving_target;
        ofParameter<float> steering_scalar;
    
};
