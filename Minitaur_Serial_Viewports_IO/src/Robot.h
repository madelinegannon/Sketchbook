#pragma once

#include "ofxGui.h"
#include "ofxGizmo.h"
#include "ofxRay.h"
#include "Limb.h"



class Robot : public ofNode {
	public:

		Robot() {};

		void setup();
		void setupGUI();
		void update();
		void draw();
		void reset();
		void exit();
		ofMatrix4x4 updateMat(ofMatrix4x4 prev, ofMatrix4x4 next);
		
		ofNode pivot;
		ofBoxPrimitive body;
	

		vector<Limb> limbs;

		ofxGizmo gizmo;

		static enum BEHAVIOR {NONE, POSTURE, FOLLOW_TRACKER, FOLLOW_AGENT, LOOKAT_TRACKER, LOOKAT_AGENT, PHYSICS};
		ofParameter<int> behaviorMode = PHYSICS;

		void setLookAtPt(ofVec3f lookAtPt);
		ofVec3f lookAtPt = ofVec3f();


		ofParameterGroup params;
		ofParameter<bool> showGizmo;
		ofParameter<bool> lockPivot;
		ofParameter<ofVec3f> PRZ; // pitch, roll, height
		ofParameter<bool> lockGains; // make all gains the same
		ofParameter<bool> resetGains; // make all gains the same
		ofParameter<bool> sendGains; // send out gains to minitaur


		void lockPivotListener(bool &flag);
		ofMatrix4x4 relOffset;

		void lockGainsListener(bool &flag);
		void resetGainsListener(bool &flag);
		void sendGainsListener(bool &flag);
		
};
