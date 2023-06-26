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
        float time_last = 0;    
        float time_diff = 1/60.;
        float circumference = 314.0;
        float max_vel_dist = 10.0;     // max distance per time step (based on max velocity)

        ofPolyline trajectory;
        void draw_trajectory();
    
    private:
        ofVec3f start, end;
        ofVec3f desired_pos, desired_vel, curr_pos, curr_vel;
    
        void update_target();
        void update_desired_vel();
        void update_current_vel();
        void compute_rpm(ofVec3f prev_pos, ofVec3f curr_pos);
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
        ofParameter<bool> use_trajectory;
        ofParameter<float> steering_scalar;

        ofParameterGroup params_rpm;
        ofParameter<float> wheel_diameter;
        ofParameter<string> info_circumfrence;
        ofParameter<string> info_rpm;

        ofParameterGroup params_trajectory;
        ofParameter<float> look_ahead_radius;
        ofParameter<float> max_vel;
        ofParameter<string> info_max_vel_dist;

        void on_wheel_diameter_changed(float& val);    
        void on_max_vel_changed(float& val);
        float get_max_dist_from_vel(float max_vel);
};
