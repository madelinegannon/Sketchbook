#pragma once

#include "ofxGui.h"
#include "ofxRay.h"
#include "Motor.h"


class Limb : public ofNode {
	public:

		Limb() {};

		Limb(int id, ofVec3f pos) {
			this->id = id;
			shoulder.setGlobalPosition(pos);
			toe.setPosition(0,0,-140);

			groundPlane.setCenter(ofVec3f());
			groundPlane.setNormal(ofVec3f(0, 0, 1));
			groundPlane.setInfinite(true);

			touchPt = ofVec3f();
		};


		int id;
		float angle, angle_prev = 0;
		float extension, extension_prev = 0;
		ofColor color;
		
		ofNode shoulder, elbow_0, elbow_1, toe;
		ofRay extensionRay;
		ofPlane groundPlane;
		ofVec3f touchPt;	// where toe intersects with ground
		
		// parameters from Robot
		float splay;
		float minExtension;
		float maxExtension;
		bool showRadians = false;
		bool showMeters = false;
		bool showDebug = true;

		ofParameterGroup params; // how to connect to Robot --> ofApp (see Mimus SensorSet)
		ofParameter<float> gain_angle_pk = .8;
		ofParameter<float> gain_angle_pd = .03;
		ofParameter<float> gain_ext_pk = 120;
		ofParameter<float> gain_ext_pd = 4;

		Motor motor;

		void setup() {
			motor.setup();

			motor.setParent(*this);
			motor.setPosition(shoulder.getPosition());

			shoulder.setParent(motor);
			shoulder.setPosition(0, 0, 0);
			toe.setParent(shoulder);

			ofColor start = ofColor::purple;
			ofColor end = ofColor::orange;

			color = start.getLerped(end, id / 4.0);

			// setup GUI
			if (id == 0) { // turn off the other limb gain settings for now
				params.setName("Limb " + ofToString(id));
				params.add(gain_angle_pk.set("angle_pk_" + ofToString(id), .8f, 0., 2.));
				params.add(gain_angle_pd.set("angle_pd_" + ofToString(id), .03f, 0., .1));
				params.add(gain_ext_pk.set("ext_pk_" + ofToString(id), 120, 100., 200.));
				params.add(gain_ext_pd.set("ext_pd_" + ofToString(id), 4, 0., 10));
			}

		};
		
		void update() {

			// pass GUI input to each motor
			motor.showRadians = showRadians;
			motor.showDebug = showDebug;

			// have the legs angle straigt down
			ofVec3f dnDir = shoulder.getGlobalPosition();
			dnDir.z = 0; 

			// add splay to Y-Axis for front and back legs
			if (isBack()) {
				dnDir.y -= splay;
			}
			else if (isFront()) {
				dnDir.y += splay;
			}

			// To make sure the dnDir is in the YZ plane of the motors
			// first, rotate the shoulder to align the toe with the down direction
			// then, recalculate the toe-to-shoulder ray in-plane
			// ... a bit of a hack, but seems to be working

			// first, update the shoulder-to-down ray
			extensionRay.s = shoulder.getGlobalPosition();
			extensionRay.t = dnDir - shoulder.getGlobalPosition();
			// find the intersection point with the ground plane
			groundPlane.intersect(extensionRay, touchPt);

			// calculate & visualize motor angle
			ofVec3f v0 = shoulder.getGlobalPosition() - toe.getGlobalPosition();
			ofVec3f v1 = shoulder.getGlobalPosition() - touchPt;
			angle = angleBetween(ofVec2f(v0.y, v0.z), ofVec2f(v1.y, v1.z));
			
			// rotate the shoulder to look down (saggital?)
			shoulder.tilt(angle);

			// then, recalulate the shoulder-to-toe ray
			extensionRay.s = shoulder.getGlobalPosition();
			extensionRay.t = toe.getGlobalPosition() - shoulder.getGlobalPosition();
			// find the intersection point with the ground plane
			groundPlane.intersect(extensionRay, touchPt);

			
			// update the motor angle using the global Z axis of the shoulder
			ofVec3f pos = shoulder.getGlobalPosition();
			ofVec3f lookAt = pos + shoulder.getZAxis();
			ofVec3f globalZAxis = lookAt - pos;
			motor.update(globalZAxis);

			float tempExtension = shoulder.getGlobalPosition().distance(touchPt);

			// limit the extension to our [min,max] range
			if (tempExtension < minExtension)				
				toe.setPosition(0, 0, -minExtension);
			else if (tempExtension > maxExtension)
				toe.setPosition(0, 0, -maxExtension);
			else
				toe.setPosition(0, 0, -tempExtension);

			// set the final extension and angle
			extension = -1*toe.getZ();
			angle = motor.getAngle();
		};

		void draw() {
			ofPushStyle();

			// draw the connection between the shoulder and toe
			ofDrawLine(shoulder.getGlobalPosition(), toe.getGlobalPosition());
			shoulder.draw();
			toe.draw();
			

			// draw motor rotation
			motor.draw();
			

			if (showDebug) {
				// show the touch point
				ofSetColor(color);
				ofDrawBox(touchPt, 15);
				ofSetLineWidth(3);
				ofDrawLine(shoulder.getGlobalPosition(), touchPt);

				// show the extension of the leg
				ofPushMatrix();
				ofSetColor(ofColor::white);
				ofTranslate(shoulder.getGlobalPosition().interpolate(touchPt, .5));
				if (showMeters)
					ofDrawBitmapString(ofToString(extension / 1000.), 5, 5);
				else
					ofDrawBitmapString(ofToString(extension), 5, 5);
				ofPopMatrix();
			}		
			
			ofPopStyle();
		};
			
		void exit() {
			toe.clearParent();
			shoulder.clearParent();
			motor.clearParent();
			this->clearParent();
		};

		float getAngle() {
			return angle;
		};

		float getExtention() {
			return extension;
		};

		// Only works with co-planar vectors
		// from online code for atan2 https://math.stackexchange.com/questions/878785/how-to-find-an-angle-in-range0-360-between-2-vectors
		float angleBetween(ofVec2f v0, ofVec2f v1) {
			float theta, dot, det;

			v0.normalize();
			v1.normalize();

			dot = v0.x*v1.x + v0.y*v1.y;      // dot product
			det = v0.x*v1.y - v0.y*v1.x;      // determinant
			theta = ofRadToDeg(atan2(det, dot));  // atan2(y, x) or atan2(sin, cos)

			//cout << "v0: " << v0 << ", v1: " << v1 << ", rotAngle (deg): " << theta << endl;

			return theta;
		};
		

		bool isFront() {
			return id % 2 == 0;
		}

		bool isBack() {
			return id % 2 != 0;
		}

		bool isLeft() {
			return id < 2;
		}

		bool isRight() {
			return id >= 2;
		}
		
};
