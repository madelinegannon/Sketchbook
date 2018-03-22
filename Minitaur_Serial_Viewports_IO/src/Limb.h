#pragma once

#include "ofxGui.h"
#include "ofxRay.h"



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

		ofParameterGroup params; // how to connect to Robot --> ofApp (see Mimus SensorSet)
		ofParameter<float> gain_angle_pk = .8;
		ofParameter<float> gain_angle_pd = .03;
		ofParameter<float> gain_ext_pk = 120;
		ofParameter<float> gain_ext_pd = 4;


		void setup() {
			shoulder.setParent(*this);
			toe.setParent(shoulder);

			ofColor start = ofColor::purple;
			ofColor end = ofColor::orange;

			color = start.getLerped(end, id / 4.0);

			// setup GUI
			params.setName("Limb " + ofToString(id));
			params.add(gain_angle_pk.set("angle_pk_" + ofToString(id),.8f, 0., 2.));
			params.add(gain_angle_pd.set("angle_pd_" + ofToString(id), .03f, 0., .1));
			params.add(gain_ext_pk.set("ext_pk_" + ofToString(id), 120, 100., 200.));
			params.add(gain_ext_pd.set("ext_pd_" + ofToString(id), 4, 0., 10));
		};
		
		void update() {
			// update the shoulder-to-toe ray
			extensionRay.s = shoulder.getGlobalPosition();
			extensionRay.t = toe.getGlobalPosition() - shoulder.getGlobalPosition();
			// find the intersection point with the ground plane
			groundPlane.intersect(extensionRay, touchPt);

			// get the extension length
			extension = shoulder.getGlobalPosition().distance(touchPt);
		};

		void draw() {
			ofPushStyle();

			ofDrawLine(shoulder.getGlobalPosition(), toe.getGlobalPosition());
			shoulder.draw();
			toe.draw();
			
			ofSetColor(color);



			ofDrawBox(touchPt, 10);
			ofSetLineWidth(3);
			
			// draw motor rotation
			ofPushMatrix();
			ofMatrix4x4 mat = shoulder.getGlobalTransformMatrix();
			ofMultMatrix(mat);
			ofRotateY(90);
			ofDrawCircle(0,0,0,25);
			ofPopMatrix();
			
			ofDrawLine(toe.getGlobalPosition(), touchPt);

			ofPushMatrix();
			ofSetColor(ofColor::white);
			ofTranslate(shoulder.getGlobalPosition().interpolate(touchPt, .5));
			ofDrawBitmapString(ofToString(extension), 5, 5);
			ofPopMatrix();
			
			
			ofPopStyle();
		};
			
		void exit() {
			toe.clearParent();
			shoulder.clearParent();
			this->clearParent();
		};

		float getAngle() {
			angle_prev = angle;

			ofVec3f p1 = (shoulder.getGlobalPosition() - toe.getGlobalPosition()).normalize(); // shoulder to toe vector
			ofVec3f ground = touchPt;
			ofVec3f p2 = ofVec3f(0, 0, -1);			// shoulder to ground vector

			angle = p1.angle(p2);
			return angle;
		};

		float getExtention() {
			return extension;
		};
		
		
};
