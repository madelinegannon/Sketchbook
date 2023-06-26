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

    if (use_trajectory) {
        ofNoFill();
        ofSetLineWidth(5);
        ofSetColor(255, 0, 255, 120);
        trajectory.draw();

        ofFill();
        ofSetColor(255, 60);
        ofDrawEllipse(end, look_ahead_radius.get() * 2, look_ahead_radius.get() * 2);
    }    
    
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
    else if (use_trajectory && trajectory.getVertices().size() > 1) {
        // remove a vertex if we are close
        ofVec3f pos = vehicle.getGlobalPosition();
        float dist_sq = pos.distanceSquared(trajectory.getVertices()[0]);
        if (dist_sq < look_ahead_radius.get() * look_ahead_radius.get())
            trajectory.removeVertex(0);
        // move towards the oldest point in the trajectory
        end.set(trajectory.getVertices()[0]);
    }
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
void ofApp::compute_rpm(ofVec3f prev_pos, ofVec3f curr_pos)
{
    // convert from mm per time_step to rev per minute
    // float mm_per_second =  dist / time_diff;     // convert from distance per time_step to seconds
    // float mm_per_minute = mm_per_second * 60;    // convert from seconds to minutes
    // float rpm = mm_per_minute / circumference;   // convert from linear distance to revolutions
    float dist = prev_pos.distance(curr_pos);
    time_diff = ofGetElapsedTimef() - time_last;
    float rpm = dist / time_diff * 60.0 / circumference; 
    time_last = ofGetElapsedTimef();
    info_rpm.set(ofToString(rpm));
}

//--------------------------------------------------------------
void ofApp::update_position(){
    // update the vehicle position (convert vel from m/s to m)
    ofVec3f pos = curr_pos + curr_vel*dt*steering_scalar;
    // cap to a max RPM
    if (pos.distance(curr_pos) > max_vel_dist) {
        auto scaled = curr_vel * dt * steering_scalar;
        scaled.normalize();
        scaled.scale(max_vel_dist);
        pos = curr_pos + scaled;
    }
    compute_rpm(curr_pos, pos);
    vehicle.setGlobalPosition(pos);    
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    params.setName("PID_Tuner");
    params.add(show_debug.set("Show_Debug", true));
    params.add(kp.set("Proportional_Gains", 300, 0, 500));  // gains for Propotional Component
    params.add(kd.set("Derivative_Gains", 25, 0, 100));     // gains for Derivitive Component
    params.add(use_moving_target.set("Use_Moving_Tgt", false));
    params.add(use_trajectory.set("Use_Trajectory", false));
    params.add(steering_scalar.set("Steering_Scalar", 1.0, 0, 25));

    params_rpm.setName("RPM_Parameters");
    params_rpm.add(wheel_diameter.set("Wheel_Diameter_(mm)", 60, 1, 1000));
    params_rpm.add(info_circumfrence.set("Circumference_(mm)", ""));
    params_rpm.add(info_rpm.set("RPM", ""));

    circumference = wheel_diameter.get() * PI;
    info_circumfrence.set(ofToString(circumference));

    params_trajectory.setName("Trajectory_Parameters");
    params_trajectory.add(look_ahead_radius.set("Look_Ahead_Radius", 100, 1, 1000));
    params_trajectory.add(max_vel.set("Max_Vel", 50, 1, 1000));
    params_trajectory.add(info_max_vel_dist.set("Max_Vel_Dist", ""));
    params_trajectory.add(params_rpm);

    max_vel_dist = get_max_dist_from_vel(max_vel.get());
    info_max_vel_dist.set(ofToString(max_vel_dist));


    wheel_diameter.addListener(this, &ofApp::on_wheel_diameter_changed);
    max_vel.addListener(this, &ofApp::on_max_vel_changed);
    
    panel.setup();
    panel.setName("PID_Example");
    panel.setWidthElements(250);
    panel.add(params);
    panel.add(params_trajectory);
    
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
        case ' ': {
            if (use_moving_target) {
                use_moving_target.set(false);
                use_trajectory.set(true);
            }
            else if (use_trajectory) {
                use_trajectory.set(false);
            }
            else {
                use_moving_target.set(!use_moving_target);
            }
            break;
        }
        case 'f':
        case 'F':
            ofToggleFullscreen();
            break;
        default:
            break;
    }

}

//--------------------------------------------------------------
void ofApp::on_wheel_diameter_changed(float& val) {
    circumference = val * PI;
    info_circumfrence.set(ofToString(circumference));
}

//--------------------------------------------------------------
void ofApp::on_max_vel_changed(float& val)
{
    max_vel_dist = get_max_dist_from_vel(val);
    look_ahead_radius.set(max_vel_dist + 10);
    info_max_vel_dist.set(ofToString(max_vel_dist));
}

//--------------------------------------------------------------
/**
 * @brief Finds the max distance the vehicle can travel based on a maximum RPM.
 * 
 * @param (float)  max_vel: maximum velocity (in RPM)
 * @return (float) distance (in mm)
 */
float ofApp::get_max_dist_from_vel(float max_vel)
{
    // convert from RPM to RPSeconds
    float max_dist_per_minute = max_vel * circumference;
    float max_dist_per_second = max_dist_per_minute * (1 / 60.0);
    float max_dist_per_time_step = max_dist_per_second * time_diff;
    //cout << "Caluclating Max Distance for velocity:" << endl;
    //cout << "\tmax_dist_per_minute: " << max_dist_per_minute << endl;
    //cout << "\tmax_dist_per_second: " << max_dist_per_second << endl;
    //cout << "\tmax_dist_per_time_step: " << max_dist_per_time_step << endl << endl;
    return max_dist_per_time_step;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

    if (trajectory.getVertices().size() == 0){
        trajectory.addVertex(x, y, 0);
    }
    else {
        ofVec3f pos = ofVec3f(x, y, 0);
        float dist_sq = pos.distanceSquared(trajectory.getVertices().back());
        float dist_filter = 1;
        if (dist_sq > dist_filter * dist_filter)
            trajectory.addVertex(x, y, 0);
    }
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
