#include "Robot.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void Robot::setup(int id){
	this->id = id;

	if (id == MINITAUR) {
		buildMesh();
	}
	
}


//--------------------------------------------------------------
void Robot::exit() {

}

//--------------------------------------------------------------
void Robot::update(){
	
}

//--------------------------------------------------------------
void Robot::draw(){

	ofPushStyle();
	
	ofSetColor(ofColor::black, 200);
	mesh.drawFaces();
	ofSetLineWidth(3);
	ofSetColor(ofColor::lightCyan);
	mesh.drawWireframe();

	ofPopStyle();
}

//--------------------------------------------------------------
void Robot::buildMesh() {

	float length, width, height;

	if (id == MINITAUR) {
		length = 70+145+70 + 100;
		width = 145;
		height = -55;
	}
	else {
		length = 0;
		width = 0;
		height = 0;
	}
	
	// build the body cavity
	ofVec3f a, b, c, d;
	a.set(-width / 2, length / 2, -height);
	b.set(-width / 2,-length / 2, -height);
	c.set( width / 2,-length / 2, -height);
	d.set( width / 2, length / 2, -height);

	mesh.addVertex(a); // 0
	mesh.addVertex(b); // 1
	mesh.addVertex(c); // 2
	mesh.addVertex(d); // 3

	// top
	mesh.addTriangle(0, 1, 2);	
	mesh.addTriangle(0, 2, 3);

	a.set(-width / 2, length / 2, 0);
	b.set(-width / 2, -length / 2,0);
	c.set(width / 2, -length / 2, 0);
	d.set(width / 2, length / 2, 0);

	mesh.addVertex(a); // 4
	mesh.addVertex(b); // 5
	mesh.addVertex(c); // 6
	mesh.addVertex(d); // 7

	// bottom
	mesh.addTriangle(4, 5, 6);
	mesh.addTriangle(4, 6, 7);

	// sides
	mesh.addTriangle(0, 4, 5);
	mesh.addTriangle(0, 5, 1);

	mesh.addTriangle(1, 5, 6);
	mesh.addTriangle(1, 6, 2);

	mesh.addTriangle(2, 6, 7);
	mesh.addTriangle(2, 7, 3);

	mesh.addTriangle(3, 7, 4);
	mesh.addTriangle(3, 4, 0);

	////////////////////////////////////////
	// MINITAUR
	// build the leg frames
	length = 155;
	width = 95+145+45;
	int offset = mesh.getVertices().size();

	a.set(-width / 2, length / 2, -height);
	b.set(-width / 2, -length / 2, -height);
	c.set(width / 2, -length / 2, -height);
	d.set(width / 2, length / 2, -height);

	mesh.addVertex(a); // 4
	mesh.addVertex(b); // 5
	mesh.addVertex(c); // 6
	mesh.addVertex(d); // 7
	
	// top
	mesh.addTriangle(0 + offset, 1 + offset, 2 + offset);
	mesh.addTriangle(0 + offset, 2 + offset, 3 + offset);

	a.set(-width / 2, length / 2, 0);
	b.set(-width / 2, -length / 2, 0);
	c.set(width / 2, -length / 2, 0);
	d.set(width / 2, length / 2, 0);

	mesh.addVertex(a); // 4
	mesh.addVertex(b); // 5
	mesh.addVertex(c); // 6
	mesh.addVertex(d); // 7

	// bottom
	mesh.addTriangle(4 + offset, 5 + offset, 6 + offset);
	mesh.addTriangle(4 + offset, 6 + offset, 7 + offset);

	// sides
	mesh.addTriangle(0 + offset, 4 + offset, 5 + offset);
	mesh.addTriangle(0 + offset, 5 + offset, 1 + offset);

	mesh.addTriangle(1 + offset, 5 + offset, 6 + offset);
	mesh.addTriangle(1 + offset, 6 + offset, 2 + offset);

	mesh.addTriangle(2 + offset, 6 + offset, 7 + offset);
	mesh.addTriangle(2 + offset, 7 + offset, 3 + offset);

	mesh.addTriangle(3 + offset, 7 + offset, 4 + offset);
	mesh.addTriangle(3 + offset, 4 + offset, 0 + offset);

}
