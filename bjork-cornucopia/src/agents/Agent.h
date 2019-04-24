#pragma once

#include "ofMain.h"


class Agent  {
	public:
        Agent() {};
        ~Agent() {};

        void setup(int id);
        void setup(int id, ofNode _pose);
        void update();
        void draw();
        void reset(ofBoxPrimitive bounds);
    
        int id = 0;
        ofNode pose;

        void move(vector<Agent*> agents, ofNode target);   // move towards a given target using PD Controller
        ofVec3f separate(vector<Agent*> agents);
        void apply_forces(ofVec3f _gravity);
    
        string toString();    
    
        // ------------ GUI  ------------
        ofParameterGroup params;
        ofParameter<float> radius;
        ofParameter<float> kp, kd;
        ofParameter<float> steering_scalar;

        void set_params(float _radius, float _kp, float _kd, float _steering);

    
private:
    
        ofVec3f desired_pos, desired_vel, curr_pos, curr_vel;
        void update_desired_vel();
        void update_current_vel();
        void update_position();
    
        ofVec3f acceleration;
        ofVec3f heading;
        ofVec3f vel_projected; // curr_vel projected onto the heading vector
        ofVec3f gravity;
        
    
        // just use a regular time step for now
        float dt = 1/60.;
    
        void setup_gui();
    
        ofColor color;
        vector<ofNode> trail;
    
        void draw_body();
        void draw_radius();
        void draw_debugging();

        bool show_debug = false;
};
