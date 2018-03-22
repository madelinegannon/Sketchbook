#pragma once

#include "ofMain.h"
#include "ofxEasing.h"


class Agent  {
	public:

		int id;
		ofVec3f pos;
		ofQuaternion orient;

		Agent() {};
		~Agent() {};

		void setup();
		void update();
		void draw();
		void seek(ofVec3f target);
		void arrive(ofVec3f &target);
		void applyForce(ofVec3f force);


		ofNode agent;
		ofNode agentOffset;
		ofNode randomOffset;

		ofVec3f	pos_prev;
		ofVec3f	vel;
		ofVec3f	accel;
		ofVec3f desire;

		ofColor color;
		ofPolyline agentTrail;

		ofVec3f bouceVec;

		// --------- EASING ---------

		float mapSpeed(float dist, float radius, float speedMax);

		vector<ofxeasing::function> easings{
			ofxeasing::linear::easeIn,
			ofxeasing::quad::easeIn,
			ofxeasing::cubic::easeIn,
			ofxeasing::quart::easeIn,
			ofxeasing::quint::easeIn,
			ofxeasing::circ::easeIn,
			ofxeasing::sine::easeIn,
			ofxeasing::exp::easeIn,
			ofxeasing::elastic::easeOut,
			ofxeasing::bounce::easeOut,
			ofxeasing::back::easeOut,
		};

		
		
		//// --------- PARAMS ---------

		ofParameterGroup params;

		ofParameter<float> radius;
		ofParameter<float> speedMax;
		ofParameter<float> forceMax;
		ofParameter<bool> noise;
		ofParameter<bool> bounce;
		ofParameter<bool> gravity;
		ofParameter<ofVec3f> gravityVec;

		//ofParameter<float> randomLerp;
		//float newRandomStartTime;

		ofParameter<int> easingType;
		//ofParameter<float> noiseScale;
		//ofParameter<bool> newRandomPos;
		//ofParameter<bool> useRandomPos;


		//ofParameter<ofVec3f> agentOffsetPos;


		void updateAgentRadius(float &radius);
		void updateAgentSpeed(float &speed);
		void updateAgentForce(float &force);
		void updateAgentBounce(bool &bounce);
		void updateAgentGravity(bool &gravity);
		void updateAgentGravityVec(ofVec3f &gravityVec);


		
};
