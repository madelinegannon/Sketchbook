#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    setup_gui();
    
    start.set(ofGetWidth()/4, ofGetHeight()/2, 0);
    end.set(3*ofGetWidth()/4, ofGetHeight()/2, 0);
    
    vehicle.setGlobalPosition(start);
    
    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    update_target();
    
    // update current pos and desired pos
    ofVec3f pos = vehicle.getGlobalPosition();
    curr_pos.set(pos);
    desired_pos.set(end);
    
    update_desired_vel();
    update_current_vel();
    update_position();

    stringstream ss;
    ss << "curr_pos: { " << curr_pos << " }, curr_vel: { " << curr_vel  << " }" << endl;
    ss << "desired_pos: {" << desired_pos << " }, desired_vel: { " << desired_vel << " }" << endl;
    ss << "accel: " << accel << endl << endl;
    //cout << ss.str();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(238), ofColor(118), OF_GRADIENT_CIRCULAR);
    
    ofPushStyle();
    
    // draw the start and end path
    ofDrawEllipse(start, 15, 15);
    ofDrawEllipse(end, 15, 15);
    ofSetLineWidth(3);
    ofSetColor(80);
    ofDrawLine(start, end);
    
    
    ofVec3f pos = vehicle.getGlobalPosition();
    
    // draw the vehicle
    ofSetColor(ofColor::orangeRed);
    ofDrawEllipse(pos, 35, 35);
    ofSetColor(ofColor::white);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawEllipse(pos, 36, 36);
    
    if (show_debug){
        // draw the velocity vector
        ofSetLineWidth(2);
        ofDrawLine(pos, pos + curr_vel);
        
        // draw the desired heading
        ofSetColor(ofColor::lightGoldenRodYellow);
        ofDrawLine(pos, pos+heading);
        
        // draw the current vel
        ofSetColor(ofColor::red);
        ofDrawLine(pos, pos + curr_vel);
        
        // draw the projected velocity
        ofSetColor(ofColor::cyan);
        ofDrawLine(pos, vel_projected);
    }
    
    ofPopStyle();
    
    panel.draw();
}


//--------------------------------------------------------------
void ofApp::update_target(){
    // update the target pos
    if (use_moving_target)
        end.set(mouseX, mouseY, 0);
    else
        end.set(3*ofGetWidth()/4, ofGetHeight()/2, 0);
}

//--------------------------------------------------------------
void ofApp::update_desired_vel(){
    ofVec3f pos = vehicle.getGlobalPosition();
    
    // update the heading and projected velocity
    heading =  desired_pos - curr_pos;
    
    // update the desired vel
    ofVec3f a = curr_vel;
    ofVec3f b = heading;
    
    b.normalize();
    b.scale(a.dot(b));
    
    vel_projected = pos + b;
    
    // if we are heading towards the target
    if (pos.distance(end) > vel_projected.distance(end)){
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
void ofApp::update_current_vel(){
    // move based on the difference in position, and
    // dampen based on the difference in velocity
    accel = kp*(desired_pos - curr_pos) + kd*(desired_vel - curr_vel);
    
    // update the current velocity (convert accel from m/s/s to m/s)
    curr_vel = curr_vel + accel*dt;
}

//--------------------------------------------------------------
void ofApp::update_position(){
    // update the vehicle position (convert vel from m/s to m)
    ofVec3f pos = curr_pos + curr_vel*dt*steering_scalar;
    vehicle.setGlobalPosition(pos);
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    params.setName("PID_Tuner");
    params.add(show_debug.set("Show_Debug", true));
    params.add(kp.set("Proportional_Gains", .125, 0, 1));     // gains for Propotional Component
    params.add(kd.set("Derivative_Gains", 1, 0, 5));     // gains for Derivitive Component
    params.add(use_moving_target.set("Use_Moving_Tgt", false));
    params.add(steering_scalar.set("Steering_Scalar", 1.0, 0,5));
    
    panel.setup();
    panel.setName("PID_Example");
    panel.add(params);
    
    panel.setPosition(10,10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'r':
            // reset vehicle position
            vehicle.setGlobalPosition(start);
            accel = ofVec3f();
            curr_vel = ofVec3f();
            curr_pos = vehicle.getGlobalPosition();
            break;
        case ' ':
            use_moving_target.set(!use_moving_target);
        default:
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    start.set(ofGetWidth()/4, ofGetHeight()/2, 0);
    end.set(3*ofGetWidth()/4, ofGetHeight()/2, 0);
    keyPressed('r');
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
