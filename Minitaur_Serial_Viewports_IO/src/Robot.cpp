#include "Robot.h"

//--------------------------------------------------------------
void Robot::setup(){

	pivot.setGlobalPosition(0, 0, 250);

	body.setWidth(240);
	body.setHeight(410);
	body.setDepth(100);

	//body.setParent(pivot);

	// set up gizmo and attach to geometry
	gizmo.setDisplayScale(2);
	gizmo.enableMouseInput();
	gizmo.setNode(pivot);

	// setup Minitaur (temp numbers for now)
	limbs.push_back(Limb(0, ofVec3f(-150, 200, 0)));
	limbs.push_back(Limb(1, ofVec3f(-150, -200, 0)));
	limbs.push_back(Limb(2, ofVec3f(150, 200, 0)));
	limbs.push_back(Limb(3, ofVec3f(150, -200, 0)));
	for (auto &limb : limbs) {
		limb.setup();
		limb.setParent(body);
	}

	setupGUI();
}

//--------------------------------------------------------------
void Robot::update() {

	//// TEMP: update timer to rotate the whole body
	//// comment out below: pivot.setTransformMatrix(gizmo.getMatrix()); 
	//float timer = float(ofGetElapsedTimeMillis());
	//timer /= timerMax;
	//pivot.tilt(sin(timer + .5));

	// update geometry attached to gizmo		
	pivot.setTransformMatrix(gizmo.getMatrix());

	if (lockPivot) {

		// BUG: not working if you rotate the pivot during !lockOffset ... only translate for now
		if (behaviorMode == LOOKAT_TRACKER || behaviorMode == LOOKAT_AGENT) {

			pivot.lookAt(lookAtPt, ofVec3f(0, 0, 1));

			ofNode temp = pivot;
			temp.rotate(-90, temp.getXAxis()); // rotate to get the 'head' to look at the desired point
			//temp.rotate(90, temp.getZAxis());

			body.setGlobalPosition(relOffset.getTranslation() + pivot.getGlobalPosition());
			body.setOrientation(temp.getGlobalOrientation()); // still reorients based on the center of the body, not pivot :(
		
		}
		else if (behaviorMode == POSTURE || behaviorMode == PHYSICS) {
			body.setGlobalPosition(pivot.getGlobalPosition());
			body.setOrientation(pivot.getGlobalOrientation());
		}
		
	}


	for (auto &limb : limbs) {
		// pass GUI input to each limb
		limb.splay = splay;
		limb.minExtension = minExtension;
		limb.maxExtension = maxExtension;
		limb.showDebug = showDebug;
		limb.showRadians = showRadians;
		limb.showMeters = showMeters;
		
		limb.update();
		//limb.motor.update(body.getZAxis());
	}
	
	// converts from deg / mm to rad / m
	PRZ.set(ofVec3f(ofDegToRad(body.getPitch()), ofDegToRad(body.getHeading()), body.getGlobalPosition().z / 1000.));
}

//--------------------------------------------------------------
void Robot::draw() {

	pivot.draw();

	if (behaviorMode == LOOKAT_TRACKER || behaviorMode == LOOKAT_AGENT) {
		ofDrawLine(pivot.getGlobalPosition(), lookAtPt);
	}


	ofPushStyle();
	body.drawWireframe();
	//ofSetColor(ofColor::antiqueWhite, 50);
	//body.draw();
	ofPopStyle();

	for (auto &limb : limbs) {
		limb.draw();
	}


}

//--------------------------------------------------------------
void Robot::setupGUI() {

	params.setName("Robot Params");
	params.add(showGizmo.set("Show Gizmo", true));
	params.add(lockPivot.set("Lock Pivot", true));
	params.add(showDebug.set("Show Debug", true));
	params.add(showRadians.set("Show Radians", false));
	params.add(showMeters.set("Show Meters", false));
	params.add(behaviorMode.set("Behavior Mode", 6, 0, 6));
	params.add(minExtension.set("Min Extension", 150, 140, 300));
	params.add(maxExtension.set("Max Extension", 300, 150, 330));
	params.add(splay.set("Foot Splay", 10, 0, 500));
	params.add(PRZ.set("Pitch, Roll, Height", ofVec3f(0, 0, 0), ofVec3f(-.5, -.5, .14), ofVec3f(.5, .5, .25)));

	params.add(lockGains.set("Lock Gains", false));
	params.add(resetGains.set("Reset Gains", false));
	params.add(sendGains.set("Send Gains", false));

	lockPivot.addListener(this, &Robot::lockPivotListener);
	lockGains.addListener(this, &Robot::lockGainsListener);
	resetGains.addListener(this, &Robot::resetGainsListener);
	sendGains.addListener(this, &Robot::sendGainsListener);

	for (int i = 0; i < 4; i++) {
		params.add(limbs[i].params);
	}
}

// align the robot's body to look at the end point
void Robot::setLookAtPt(ofVec3f lookAtPt){

	this->lookAtPt.set(lookAtPt);

}

//--------------------------------------------------------------
void Robot::lockPivotListener(bool &flag) {


	// BUG: not working if you rotate the pivot during !lockPivot
	//relOffset = pivot.getGlobalTransformMatrix().getInverse() * body.getGlobalTransformMatrix();
	
	ofVec3f offset = body.getGlobalPosition() - pivot.getGlobalPosition() ;
	relOffset.setTranslation(offset);


}

//--------------------------------------------------------------
void Robot::lockGainsListener(bool &flag) {

	if (lockGains) {
		float angle_pk = limbs[0].gain_angle_pk;
		float angle_pd = limbs[0].gain_angle_pd;
		float ext_pk = limbs[0].gain_ext_pk;
		float ext_pd = limbs[0].gain_ext_pd;

		for (int i = 0; i < 4; i++) {
			limbs[i].gain_angle_pk = angle_pk;
			limbs[i].gain_angle_pd = angle_pd;
			limbs[i].gain_ext_pk = ext_pk;
			limbs[i].gain_ext_pd = ext_pd;

			// disable events not really working ...?
			limbs[i].gain_angle_pk.disableEvents();
			limbs[i].gain_angle_pd.disableEvents();
			limbs[i].gain_ext_pk.disableEvents();
			limbs[i].gain_ext_pd.disableEvents();
		}
	}
	else {

		for (int i = 0; i < 4; i++) {
			limbs[i].gain_angle_pk.enableEvents();
			limbs[i].gain_angle_pd.enableEvents();
			limbs[i].gain_ext_pk.enableEvents();
			limbs[i].gain_ext_pd.enableEvents();
		}

	}

}


//--------------------------------------------------------------
void Robot::resetGainsListener(bool &flag) {

	float angle_pk = .8;
	float angle_pd = .03;
	float ext_pk = 120;
	float ext_pd = 4;

	for (int i = 0; i < 4; i++) {
		limbs[i].gain_angle_pk = angle_pk;
		limbs[i].gain_angle_pd = angle_pd;
		limbs[i].gain_ext_pk = ext_pk;
		limbs[i].gain_ext_pd = ext_pd;
	}

	// reset the value
	resetGains.set(false);

}

//--------------------------------------------------------------
void Robot::sendGainsListener(bool &flag) {

	// send the gains

	// reset the value
	sendGains.set(false);
}

//--------------------------------------------------------------
void Robot::reset() {
	pivot.setPosition(0, 0, 140);
	pivot.setOrientation(ofQuaternion());
	gizmo.setNode(pivot);

	body.setPosition(0, 0, 140);
	body.setOrientation(ofQuaternion());

	relOffset.makeIdentityMatrix();
}

//--------------------------------------------------------------
void Robot::exit() {
	for (auto &limb : limbs)
		limb.exit();
}

//--------------------------------------------------------------
ofMatrix4x4 Robot::updateMat(ofMatrix4x4 prev, ofMatrix4x4 next)
{
	// make mat from shoulder position
	ofMatrix4x4 mat;
	mat.makeIdentityMatrix();
	// undo rotation
	 mat *= prev;
	// apply new rotation
	mat *= next;
	// set new position and orientation


	return mat;
}

