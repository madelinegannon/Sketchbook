#include "Agent.h"


//--------------------------------------------------------------
void Agent::setup(int id) {
	this->id = id;
	setup_gui();
}

//--------------------------------------------------------------
void Agent::setup(int id, ofNode _pose) {
    this->id = id;
    this->pose = _pose;
    
    setup_gui();
}

//--------------------------------------------------------------
void Agent::update() {
    // update the agent's trail
    trail.push_back(ofNode(pose));
    if (trail.size() > 200)
        trail.erase(trail.begin());
}

//--------------------------------------------------------------
void Agent::draw() {

	ofPushStyle();
	ofSetColor(color);
    ofNoFill();

    draw_body();
    draw_radius();
    if (show_debug)
        draw_debugging();

    ofPopStyle();
}

//--------------------------------------------------------------
// move towards a given target using PD Controller
void Agent::move(vector<Agent*> agents, ofNode target) {
    
    // update current pos and desired pos
    ofVec3f pos = pose.getGlobalPosition();
    curr_pos.set(pos);
    desired_pos.set(target.getGlobalPosition());
    
    // check if we need to separate the agent from its neighbors
    curr_vel += separate(agents);
    if (curr_pos.z > 0)
        curr_vel += gravity;
    
    update_desired_vel();
    update_current_vel();
    update_position();
    
}

//--------------------------------------------------------------
void Agent::update_desired_vel(){
    ofVec3f pos = pose.getGlobalPosition();
    
    // update the heading and projected velocity
    heading =  desired_pos - curr_pos;
    
    // update the desired vel
    ofVec3f a = curr_vel;
    ofVec3f b = heading;
    
    b.normalize();
    b.scale(a.dot(b));
    
    vel_projected = pos + b;
    
    // if we are heading towards the target
    float dist_p_to_d = pos.distanceSquared(desired_pos);
    float dist_vp_to_d = vel_projected.distanceSquared(desired_pos);
    if (dist_p_to_d > dist_vp_to_d){
        // weight the desired velocity
        desired_vel = vel_projected - pos;
    }
    else{
        // otherwise dampen: apply breaks by setting desired_vel in opposite direction
        desired_vel = pos - vel_projected;//ofVec3f();
    }
    
    //    desired_vel *= steering_scalar; <-- makes turning very slow
}

//--------------------------------------------------------------
ofVec3f Agent::separate(vector<Agent*> agents){
    
    ofVec3f sum; // sum of direction vectors pointing away from neighbors
    int count = 0;
    
    for (int i=0; i<agents.size(); i++){
        if (i != id){
            float dist_sq = pose.getGlobalPosition().distanceSquared(agents[i]->pose.getGlobalPosition());
            if (dist_sq < (radius + agents[i]->radius)*(radius + agents[i]->radius)){
                ofVec3f diff = pose.getGlobalPosition() - agents[i]->pose.getGlobalPosition();
                sum+=diff;
                count++;
            }
        }
    }
    if (count != 0){
        // return the averaged sum 
        return sum / count;
    }
    return ofVec3f();
}

void Agent::apply_forces(ofVec3f _gravity){
    gravity.set(_gravity);
}

//--------------------------------------------------------------
void Agent::update_current_vel(){
    // move based on the difference in position, and
    // dampen based on the difference in velocity
    acceleration = kp*(desired_pos - curr_pos) + kd*(desired_vel - curr_vel);
    
    // update the current velocity (convert accel from m/s/s to m/s)
    curr_vel = curr_vel + acceleration*dt;
}

//--------------------------------------------------------------
void Agent::update_position(){
    // update the vehicle position (convert vel from m/s to m)
    ofVec3f pos = curr_pos + curr_vel*dt*steering_scalar;
    pose.setGlobalPosition(pos);
}

//--------------------------------------------------------------
void Agent::setup_gui()
{
    // setup gui params
    string name = "Agent_Params_" + ofToString(id);
    params.setName(name);
    
    params.add(radius.set("radius", 120, 1, 1500));

    params.add(kp.set("Proportional_Gains", 1, 0, 3));    // PD controller gains for Propotional Component
    params.add(kd.set("Derivative_Gains", 2.5, 0, 5));      // PD controller gains for Derivitive Component
    params.add(steering_scalar.set("Steering_Scalar", 1.0, 0,5));
    
    color = ofColor::aqua;
}

//--------------------------------------------------------------
void Agent::set_params(float _radius, float _kp, float _kd, float _steering){
    radius.set(_radius);
    kp.set(_kp);
    kd.set(_kd);
    steering_scalar.set(_steering);
}

//--------------------------------------------------------------
void Agent::draw_body() {
    // draw the trail
    ofBeginShape();
    for (auto &v: trail)
        ofVertex(v.getGlobalPosition().x, v.getGlobalPosition().y, v.getGlobalPosition().z);
    ofEndShape();
    
    // draw the pose
    ofEnableDepthTest();
    ofFill();
    ofSetColor(255);
    ofDrawBox(pose.getGlobalPosition(), 35);
    ofNoFill();
    ofSetColor(color);
    ofDrawBox(pose.getGlobalPosition(), 38);
    ofDisableDepthTest();

//    pose.draw();
}

//--------------------------------------------------------------
void Agent::draw_radius() {
    // draw the agent radius
    ofMatrix4x4 mat = ofMatrix4x4(pose.getGlobalOrientation());
    mat.setTranslation(pose.getGlobalPosition());
    ofPushMatrix();
    ofMultMatrix(mat);
    ofSetLineWidth(3);
    ofSetColor(ofColor::red, 60);
    ofDrawCircle(ofVec3f(), radius);
    ofRotateX(90);
    ofSetColor(ofColor::green,60);
    ofDrawCircle(ofVec3f(), radius);
    ofRotateY(90);
    ofSetColor(ofColor::blue, 60);
    ofDrawCircle(ofVec3f(), radius);
    ofPopMatrix();
}

//--------------------------------------------------------------
void Agent::draw_debugging() {
    ofSetColor(ofColor::antiqueWhite);
    string str_pose = "  "+ofToString(id) + ": \n[[" + ofToString(pose.getGlobalPosition()) +"],[" + ofToString(pose.getGlobalOrientation()) + "]]";
    ofDrawBitmapString(str_pose, pose.getGlobalPosition());
    
}

//--------------------------------------------------------------
void Agent::reset(ofBoxPrimitive bounds) {
	trail.clear();

	// range {[0,width/2], [0,height/2], [0,depth/2]}
	float x = ofRandom(bounds.getGlobalPosition().x - bounds.getWidth()/2, bounds.getGlobalPosition().x + bounds.getWidth()/2);
	float y = ofRandom(bounds.getGlobalPosition().y - bounds.getHeight()/2, bounds.getGlobalPosition().y + bounds.getHeight()/2);
	float z = ofRandom(bounds.getGlobalPosition().z - bounds.getDepth()/2, bounds.getGlobalPosition().z + bounds.getDepth()/2);
	ofVec3f random_pt = ofVec3f(x, y, z);

	cout << "reset point: {" << ofToString(random_pt) << "}" << endl;

	pose.setGlobalPosition(random_pt);
	pose.setGlobalOrientation(ofQuaternion(0, 0, 0, 1));
	trail.push_back(ofNode(pose));
};

//--------------------------------------------------------------
string Agent::toString(){
    
    stringstream ss;
    ss << "Agent State" << endl;
    ss << "\tPose: [[" << pose.getGlobalPosition() << "],["<< pose.getGlobalOrientation() << "]]"<< endl;
    ss << "\tVelocity: ["<< curr_vel << "]" << endl;
    ss << "\tAcceleration: ["<< acceleration << "]" << endl;
    
    return ss.str();
}

