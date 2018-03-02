#pragma once

#include "ofMain.h"



//--------------------------------------------------------------
class Robot {

public:

	void setup(int id);
	void exit();
	void update();
	void draw();

	int id;
	enum Species{
		GENERIC,
		MINITAUR
	};

private:

	ofMesh mesh;
	void buildMesh();
	
};
