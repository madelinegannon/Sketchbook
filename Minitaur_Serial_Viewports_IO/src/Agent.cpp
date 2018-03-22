#include "Agent.h"

//--------------------------------------------------------------
void Agent::setup() {

	// setup params
	params.setName("Agent Parameters");

	params.add(radius.set("radius", 50, 1, 500));
	params.add(speedMax.set("speedMax", 10, 1, 25));
	params.add(forceMax.set("forceMax", 3.5, .1, 5));
	//params.add(tweenType.set("tweenType", 5, 0, 6));
	params.add(bounce.set("bounce", false));
	params.add(gravity.set("gravity", false));
	params.add(gravityVec.set("gravityVec", ofVec3f(0, 0, -1), ofVec3f(-5, -5, -5), ofVec3f(5, 5, 5)));
	//params.add(agentOffsetPos.set("agentOffsetPos", ofVec3f(0, 0, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	//params.add(noiseScale.set("Noise Scale", 1, 1, 1000));
	//params.add(newRandomPos.set("New Random Pos", false));
	//params.add(useRandomPos.set("useRandomPos", false));
	//params.add(randomLerp.set("Random Pos Lerp Time", 3, 1, 100));

	// add listeners

	speedMax.addListener(this, &Agent::updateAgentSpeed);
	forceMax.addListener(this, &Agent::updateAgentForce);
	bounce.addListener(this, &Agent::updateAgentBounce);
	gravity.addListener(this, &Agent::updateAgentGravity);
	gravityVec.addListener(this, &Agent::updateAgentGravityVec);
	radius.addListener(this, &Agent::updateAgentRadius);

	color = ofColor::aqua;

	agentOffset.setParent(agent);
	randomOffset.setParent(agentOffset);
	//newRandomStartTime = ofGetElapsedTimef();
}

ofVec3f Noise3d(float scale) {
	return ofVec3f(ofRandomf(), ofRandomf(), ofRandomf())*scale;
}

//--------------------------------------------------------------

void Agent::update() {

	pos_prev.set(pos);


	// this biases the final force to head in
	// the direction of the bounce vector
	if (bounce)
		vel += bouceVec; // bounce vector should be scaled based on velocity

						 // this biases the final force to head in
						 // the direction of the gravity vector
	if (gravity)
		vel += gravityVec;

	vel = vel + accel;
	vel.limit(speedMax);
	pos = pos + vel;
	accel = accel * 0;

	// update the agentTrail
	agentTrail.addVertex(pos);

	if (agentTrail.getVertices().size() > 50) {
		agentTrail.getVertices().erase(agentTrail.getVertices().begin());
	}


	agent.setPosition(agentTrail[0]);
	//agentOffset.setPosition(agentOffsetPos.get());
	//if (newRandomPos) {
	//	newRandomStartTime = ofGetElapsedTimef();
	//	newRandomPos = false;
	//	randomOffset.setPosition(randomOffset.getPosition().getInterpolated(Noise3d(noiseScale), 0.99f));
	//}
	//if (ofGetElapsedTimef() - newRandomStartTime < randomLerp) {
	//	agentOffset.setPosition(agentOffset.getPosition().interpolate(randomOffset.getPosition(), 0.25));
	//}
	//else {
	//	agentOffset.setPosition(agentOffset.getPosition().interpolate(agentOffsetPos, 0.25));
	//}
}

//--------------------------------------------------------------

void Agent::draw() {

	ofPushStyle();

	ofSetColor(color);
	agentTrail.draw();
	// show steering vector?
	ofSetLineWidth(1);
	ofSetColor(ofColor::red);
	ofVec3f heading;
	heading.set(vel);
	heading.scale(75);
	heading = heading + pos;
	ofDrawLine(pos, heading);

	ofNode node;
	node.setGlobalPosition(pos);
	node.lookAt(heading);
	ofMatrix4x4 mat = ofMatrix4x4(node.getGlobalOrientation());
	mat.setTranslation(pos);
	ofPushMatrix();
	ofMultMatrix(mat);
	ofSetColor(255, 0, 255, 120);
	ofNoFill();
	ofDrawBox(30);
	ofDrawCircle(ofVec3f(), radius);
	ofRotateX(90);
	ofDrawCircle(ofVec3f(), radius);
	ofRotateY(90);
	ofDrawCircle(ofVec3f(), radius);
	ofPopMatrix();


	ofPopStyle();

}

//--------------------------------------------------------------

void Agent::seek(ofVec3f target) {

	ofVec3f desired = target - pos;
	desired.normalize();
	desired.scale(speedMax);
	ofVec3f steer = desired - vel;

	steer.limit(forceMax);

	applyForce(steer);

}

//--------------------------------------------------------------

void Agent::arrive(ofVec3f &target) {

	ofVec3f desired = target - pos;
	float dist = desired.length();

	// if we are within the arriving radius, dampen the approach

	if (dist < radius) {
		//float speedMapped = ofMap(dist, 0, radius, 0, speedMax);
		desired.normalize().scale(speedMax);// mapSpeed(dist, radius, speedMax));
	}
	else {
		desired.normalize();
		desired.scale(speedMax);
	}
	ofVec3f steer = desired - vel;


	steer.limit(forceMax);

	applyForce(steer);

}

//--------------------------------------------------------------

void Agent::applyForce(ofVec3f force) {
	accel = accel + force;
}

float Agent::mapSpeed(float dist, float radius, float speedMax)
{
	float speedMapped;

	//if (tweenType == CIRCULAR)
	//	speedMapped = ofxTween::map(dist, 0, radius, 0, speedMax, clamp, easeCirc, easingType);
	//else if (tweenType == QUADRATIC)
	//	speedMapped = ofxTween::map(dist, 0, radius, 0, speedMax, clamp, easeQuad, easingType);
	//else if (tweenType == CUBIC)
	//	speedMapped = ofxTween::map(dist, 0, radius, 0, speedMax, clamp, easeCubic, easingType);
	//else if (tweenType == QUARTIC)
	//	speedMapped = ofxTween::map(dist, 0, radius, 0, speedMax, clamp, easeQuart, easingType);
	//else if (tweenType == QUINTIC)
	//	speedMapped = ofxTween::map(dist, 0, radius, 0, speedMax, clamp, easeQuint, easingType);
	//else
	//	speedMapped = ofxTween::map(dist, 0, radius, 0, speedMax, clamp, easeLinear, easingType);

	return speedMapped;
}

//--------------------------------------------------------------

void Agent::updateAgentRadius(float &radius)
{
	this->radius = radius;
}

//--------------------------------------------------------------

void Agent::updateAgentSpeed(float &speed)
{
	this->speedMax = speed;
}

//--------------------------------------------------------------

void Agent::updateAgentForce(float &force)
{
	this->forceMax = force;
}

//--------------------------------------------------------------

void Agent::updateAgentBounce(bool &bounce)
{
	this->bounce = bounce;
}

//--------------------------------------------------------------

void Agent::updateAgentGravity(bool &gravity)
{
	this->gravity = gravity;
	this->gravityVec = gravityVec;
}

//--------------------------------------------------------------

void Agent::updateAgentGravityVec(ofVec3f &gravityVec)
{
	this->gravityVec = gravityVec;
}
