#pragma once

#include "ofMain.h"


class Tracker : public ofNode {
	public:

		Tracker() {};

		int id;
		ofVec3f pos;
		ofQuaternion orient;
		ofVec3f  scale;

		ofMesh mesh;
		ofPolyline trail;

		bool isConnected = false;


		void update() {

			isConnected = true;

			trail.addVertex(pos);
			if (trail.getVertices().size() > 75)
				trail.getVertices().erase(trail.getVertices().begin());

		};

		void draw() {
			ofPushStyle();

			ofPushMatrix();
			ofMultMatrix(getGlobalTransformMatrix());

			ofFill();
			ofSetColor(ofColor::aquamarine, 50);
			//ofDrawBox(50);
			ofNoFill();
			ofSetColor(ofColor::antiqueWhite, 200);

			//ofDrawBox(50);
			mesh.drawWireframe();
			ofPopMatrix();


			ofSetLineWidth(5);
			ofSetColor(ofColor::paleVioletRed, 100);
			trail.draw();

			ofPopStyle();
		};
};
