#include "Trajectory.h"

Trajectory::Trajectory()
{
    setup_gui();
}

void Trajectory::update()
{    
    // move towards the oldest point in the trajectory
    if (path.getVertices().size() > 0) {

        // remove oldest target from the path if we're close
        float dist_sq = curr_pos.distanceSquared(path.getVertices()[0]);
        if (dist_sq <= look_ahead_radius.get() * look_ahead_radius.get() && get_num_targets() >= 2) {
            path.removeVertex(0);
        }
        
        // update the desired pos
        if (path.getVertices().size() > 0)
            desired_pos = path.getVertices()[0];
    }

    update_desired_vel();
    update_current_vel();
    update_position();
}

/**
 * @brief This update function overrides the current position with an external point, and then updates.
 * 
 * @param (glm::vec3)  pos: new current position
 */
void Trajectory::update(glm::vec3 pos)
{
    curr_pos.set(pos);
    update();
}

void Trajectory::draw(bool show_debug)
{
    ofPushStyle();

    // draw the path
    ofNoFill();
    ofSetLineWidth(5);
    ofSetColor(255, 0, 255, 120);
    path.draw();

    // draw the look_ahead radius
    ofFill();
    ofSetColor(255, 60);
    ofDrawEllipse(desired_pos, look_ahead_radius.get() * 2, look_ahead_radius.get() * 2);

    // draw the current position
    ofSetColor(ofColor::orangeRed);
    ofDrawEllipse(curr_pos, 35, 35);
    ofSetColor(ofColor::white);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawEllipse(curr_pos, 36, 36);

    // draw the debug vectors
    if (show_debug) {
        // draw the velocity vector
        ofSetLineWidth(2);
        ofDrawLine(curr_pos, curr_pos + curr_vel);
        
        // draw the desired heading
        ofSetColor(ofColor::lightGoldenRodYellow);
        ofDrawLine(curr_pos, curr_pos + heading);
        
        // draw the current vel
        ofSetColor(ofColor::red);
        ofDrawLine(curr_pos, curr_pos + curr_vel);
        
        // draw the projected velocity
        if (vel_projected != ofVec3f()) {
            ofSetColor(ofColor::cyan);
            ofDrawLine(curr_pos, vel_projected);
        }
    }
    ofPopStyle();
}

/**
 * @brief Resets the desired pos/vel & current pos/vel, clears the past target path,
 * resets accel/heading/vel_projected vectors, and resets rpm to 0.
 * 
 * @param (glm::vec3)  start: where to set the current position (default is (0, 0, 0))
 */
void Trajectory::reset(glm::vec3 start)
{
    curr_pos.set(start);
    desired_pos.set(start);
    path.clear();

    desired_vel = ofVec3f();
    curr_vel = ofVec3f();

    accel = ofVec3f();
    heading = ofVec3f();
    vel_projected = ofVec3f();

    rpm = 0;
}

/**
 * @brief Returns the last target to be added to the path. Returns (0, 0, 0) if path is empty.
 * 
 * @return (glm::vec3)  
 */
glm::vec3 Trajectory::get_last_target() {
    if (path.getVertices().size() > 0)
        return path.getVertices()[path.getVertices().size()-1];
    else {
        ofLogWarning("Trajectory::get_last_target()") << "No buffered targets! Returning (0, 0, 0)";
        return glm::vec3();
    }
}

/**
 * @brief Add a target to the trajectory's path.
 * 
 * @param (glm::vec3)  target: {X,Y,Z} coordinate
 */
void Trajectory::add_target(glm::vec3 target)
{
    path.addVertex(target.x, target.y, target.z);
}


//--------------------------------------------------------------
void Trajectory::update_desired_vel() {
    // update the heading and projected velocity
    heading = desired_pos - curr_pos;

    // update the desired vel
    ofVec3f a = curr_vel;
    ofVec3f b = heading;

    b.normalize();
    b.scale(a.dot(b));

    vel_projected = curr_pos + b;

    // if we are heading towards the target
    if (curr_pos.distance(desired_pos) > vel_projected.distance(desired_pos)) {
        // weight the desired velocity
        desired_vel = vel_projected - curr_pos;
    }
    else {
        // otherwise dampen: apply breaks by setting desired_vel in opposite direction
        desired_vel = curr_pos - vel_projected;
    }
}

//--------------------------------------------------------------
void Trajectory::update_current_vel() {
    // move based on the difference in position, and
    // dampen based on the difference in velocity
    accel = kp * (desired_pos - curr_pos) + kd * (desired_vel - curr_vel);

    // update the current velocity (convert accel from m/s/s to m/s)
    time_diff = 1 / 60.0;// ofGetElapsedTimef() - time_last; // <-- dynamic time_diff causing instability with multiple robots
    get_max_dist_from_vel(max_vel.get());
    curr_vel = curr_vel + accel * time_diff;
    time_last = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void Trajectory::compute_rpm(ofVec3f prev_pos, ofVec3f curr_pos)
{
    // convert from mm per time_step to rev per minute
    // float mm_per_second =  dist / time_diff;     // convert from distance per time_step to seconds
    // float mm_per_minute = mm_per_second * 60;    // convert from seconds to minutes
    // float rpm = mm_per_minute / circumference;   // convert from linear distance to revolutions
    float dist = prev_pos.distance(curr_pos);
    rpm = dist / time_diff * 60.0 / circumference;
    info_rpm.set(ofToString(rpm));
}

void Trajectory::update_position() {
    // update the vehicle position (convert vel from m/s to m)
    ofVec3f pos = curr_pos + curr_vel * time_diff * steering_scalar;
    // cap to a max RPM
    float dist = pos.distance(curr_pos);

    if (dist > max_vel_dist) {
        auto scaled = curr_vel * time_diff * steering_scalar;
        scaled.normalize();
        scaled.scale(max_vel_dist);
        pos = curr_pos + scaled;
    }
    compute_rpm(curr_pos, pos);
    curr_pos.set(pos);
}


void Trajectory::on_wheel_diameter_changed(float& val) 
{
    circumference = val * PI;
    //info_circumfrence.set(ofToString(circumference));
}


void Trajectory::on_max_vel_changed(float& val)
{
    max_vel_dist = get_max_dist_from_vel(val);
    look_ahead_radius.set(max_vel * 0.75);
    info_max_vel_dist.set(ofToString(max_vel_dist));
}

/**
 * @brief Finds the max distance the vehicle can travel based on a maximum RPM.
 *
 * @param (float)  max_vel: maximum velocity (in RPM)
 * @return (float) distance (in mm)
 */
float Trajectory::get_max_dist_from_vel(float max_vel)
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

void Trajectory::setup_gui() {

    params_pid.setName("PD_Controller");

    params_pid.add(kp.set("Proportional_Gains", 300, 0, 500));  // gains for Propotional Component
    params_pid.add(kd.set("Derivative_Gains", 25, 0, 100));     // gains for Derivitive Component
    params_pid.add(steering_scalar.set("Steering_Scalar", 1.0, 0, 25));

    params_rpm.setName("RPM_Parameters");
    params_rpm.add(wheel_diameter.set("Wheel_Diameter_(mm)", 100, 1, 1000));
    params_rpm.add(look_ahead_radius.set("Look_Ahead_Radius", 100, 1, 1000));
    params_rpm.add(max_vel.set("Max_Vel", 50, 1, 1000));

    params_info.setName("Info");
    params_info.add(info_rpm.set("RPM", ""));
    params_info.add(info_max_vel_dist.set("Max_Vel_Dist", ""));
    params_rpm.add(params_info);

    circumference = wheel_diameter.get() * PI;

    max_vel_dist = get_max_dist_from_vel(max_vel.get());
    info_max_vel_dist.set(ofToString(max_vel_dist));

    wheel_diameter.addListener(this, &Trajectory::on_wheel_diameter_changed);
    max_vel.addListener(this, &Trajectory::on_max_vel_changed);
    
    params.setName("Trajectory");
    params.add(params_pid);
    params.add(params_rpm);


    panel.setup();
    panel.setName("Trajectory");
    panel.setWidthElements(250);
    panel.add(params_pid);
    panel.add(params_rpm);

    panel.setPosition(10, 10);
}