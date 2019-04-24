#include "MotionController.h"


//--------------------------------------------------------------
MotionController::MotionController()
{
}

//--------------------------------------------------------------
MotionController::~MotionController()
{
}

//--------------------------------------------------------------
void MotionController::setup(int num_agents)
{
	NUM_AGENTS = num_agents;
	
	for (int i = 0; i < NUM_AGENTS; i++) {
		targets.push_back(new ofMatrix4x4());
	}
    
//    setup_osc();
    setup_gui();

    setup_base_geometry();
    setup_gizmos();
    setup_motion_targets();



	// double check that we're paused
	mode = 0;
}

//--------------------------------------------------------------
void MotionController::update()
{

    update_mode();
    update_motion_targets();


	// set motion & lookAt targets
	for (int i = 0; i < NUM_AGENTS; i++) {

		ofNode tgt;
		tgt.setGlobalPosition(gizmos[i]->getMatrix().getTranslation());
		tgt.lookAt(gizmos[i]->getMatrix().getTranslation(), ofVec3f(0, -1, 0));

		targets[i]->setTranslation(tgt.getGlobalPosition());
		targets[i]->setRotate(tgt.getGlobalOrientation());

	}
}

//--------------------------------------------------------------
void MotionController::draw(ofEasyCam * cam)
{
	ofEnableDepthTest();
	ofPushStyle();
    
    if (show_gizmos){
        draw_halo();
    }
	
	draw_gizmos(cam);

    // show the final target
    ofPushStyle();
    for (int i = 0; i < NUM_AGENTS; i++) {
        ofEnableDepthTest();
//        ofFill();
//        ofSetColor(255);
//        ofDrawBox(gizmos[i]->getMatrix().getTranslation(), 35);
//        ofNoFill();
//        ofSetColor(ofColor::aquamarine);
//        ofDrawBox(gizmos[i]->getMatrix().getTranslation(), 38);
        
        ofNode temp;
        temp.setGlobalPosition(gizmos[i]->getMatrix().getTranslation());
        temp.setGlobalOrientation(gizmos[i]->getMatrix().getRotate());
        temp.draw();
        ofDisableDepthTest();
    }
    ofPopStyle();
    

	ofPopStyle();
	ofDisableDepthTest();
}

//--------------------------------------------------------------
void MotionController::keyPressed(int key)
{
    switch (key) {
        case 'r':
            mode = HOME;
//            reset_all();
            break;
        case ' ':
            // timeline in ofApp has been toggled
            // make sure we are in MOTION_MODE::TIMELINE mode
            if (mode != TIMELINE){
                mode = TIMELINE;
            }
            else if (mode == TIMELINE){
                // reset the MOTION_MODE
                mode = PAUSE;
            }
            break;
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void MotionController::update_mode(){
    // update the run-mode
    switch (mode)
    {
        case MOTION_MODE::HOME:
        {
            reset_all();
            mode = MOTION_MODE::RUN;
            break;
        }
        case MOTION_MODE::TIMELINE:
        {
            break;
        }
        default:
            break;
    }
    
    update_mode_string();
}

//--------------------------------------------------------------
void MotionController::reset(int i)
{
//    ofVec3f pos = home_target;
//    pos.y += (i * 800);    // convert from base to world
//    gizmos[i]->getMatrix().setTranslation(pos);
}

//--------------------------------------------------------------
void MotionController::reset_all()
{
    halo.clear();
    setup_motion_targets();
    setup_base_geometry();
//    for (int i = 0; i < NUM_AGENTS; i++) {
//        reset(i);
//    }
}

//--------------------------------------------------------------
void MotionController::setup_osc()
{
	receiver.setup(1200);
}

//--------------------------------------------------------------
void MotionController::update_osc()
{
	// check for incoming messages

	while (receiver.hasWaitingMessages()) {
		
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		if (m.getAddress() == "/target") {
		}
		else if (m.getAddress() == "/target2") {
		}

		else {
			// unrecognized message
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for (int i = 0; i < m.getNumArgs(); i++) {
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
					msg_string += m.getArgAsString(i);
				}
				else {
					msg_string += "unknown";
				}
			}
			ofLogNotice() << "UNKNOWN OSC MESSAGE: " << msg_string << endl;
		}
	}
}

//--------------------------------------------------------------
void MotionController::setup_base_geometry()
{
    float theta = 360. / float(NUM_AGENTS);
    for (int i=0; i<NUM_AGENTS; i++){
        ofVec3f pos = ofVec3f(halo_radius, 0, 0);
        pos.rotate(theta*i, ofVec3f(0,0,1));
        pos += halo_pos;
        halo.addVertex(pos);
    }
    halo.close();
}

//--------------------------------------------------------------
void MotionController::draw_halo(){
    ofPushStyle();
    ofNoFill();
    
    ofSetColor(ofColor::lightCyan,100);
    ofSetLineWidth(5);
    ofDrawEllipse(halo_pos, halo_radius*2, halo_radius*2);
    
    ofSetColor(ofColor::magenta);
    ofSetLineWidth(1);
    halo.draw();
    
    ofPopStyle();
}

//--------------------------------------------------------------
void MotionController::setup_motion_targets()
{
	double y_spacing = 800;
	for (int i = 0; i < NUM_AGENTS; i++) {

		motion_targets.push_back(ofVec3f());

        motion_targets[i].x = halo.getVertices()[i].x;
        motion_targets[i].y = halo.getVertices()[i].y;
        motion_targets[i].z = halo.getVertices()[i].z;
    }

	modifier = MOTION_MODIFIER::SINE;
}
//--------------------------------------------------------------
void MotionController::update_motion_targets()
{
    for (int i = 0; i < NUM_AGENTS; i++) {
        
        // if we are using geometry to generate motion targets
        if (use_geometry){
            
            // check if we should be spinning
            if (halo_spin != 0){
                halo.getVertices()[i].rotate(halo_spin, ofVec3f(0,0,1));
            }
            
            // check if we should be seeking a sensor target
            if (sensor_target != ofVec3f())
                motion_targets[i].set(sensor_target);
            
            // otherwise go to the geometry targets
            else
                motion_targets[i].set(halo.getVertices()[i]);
        }
        
        // if timeline is playing, then use recoded positions stored in params_positions
        else if (mode == TIMELINE){
            motion_targets[i].set(params_positions.get(i).cast<ofVec3f>().get());
        }
        
        // if we are manually moving each agent, seek its gizmo
        else{
            motion_targets[i].set(gizmos[i]->getMatrix().getTranslation());
        }
        
        // add a motion modifier, if any
        if (modifier != MOTION_MODIFIER::NONE) {
            add_motion_modifiers();
        }
        
        // set the gizmo to the new motion target
        gizmos[i]->getMatrix().setTranslation(motion_targets[i]);
        
        // update the corresponding parameter with the new position
        params_positions.get(i).cast<ofVec3f>().set(gizmos[i]->getMatrix().getTranslation());

    }

}

//--------------------------------------------------------------
void MotionController::set_sensor_target(ofVec3f tgt)
{
    sensor_target.set(tgt);
}

//--------------------------------------------------------------
void MotionController::add_motion_modifiers()
{
	for (int i = 0; i < NUM_AGENTS; i++) {
        
        if (use_noise) {
            motion_targets[i] += add_noise(i, noise_speed, noise_scalar);
        }
        
        if (use_sine){
            float scalar;
            if (mode == TIMELINE || use_geometry) scalar = 10;//use_geometry ? 10 : 1;
            else scalar = 1;
            
            
//            if (mode == TIMELINE)
//                cout << "BEFORE: " << motion_targets[i] << endl;
            
            motion_targets[i] += add_sine(i, sine_speed, sine_amplitude, sine_frequency, sine_scalar)*scalar;
            
//            if (mode == TIMELINE)
//                cout << "AFTER: " << motion_targets[i] << endl << endl;
        }

	}
}

//--------------------------------------------------------------
ofVec3f MotionController::add_noise(int i, float _speed, float _scalar)
{

	float offset = i;

	float x = ofSignedNoise((ofGetElapsedTimef() + offset) * _speed);
	float y = 0;
	float z = ofSignedNoise((ofGetElapsedTimef() + offset) * _speed * 2);

	ofVec3f pos = ofVec3f(x, y, z);
	pos *= _scalar;

	return pos;
}


//--------------------------------------------------------------
ofVec3f MotionController::add_sine(int i, float _speed, float _amplitude, float _frequency, float _scalar)
{
	float offset = i;

	float x = _amplitude * sin((offset + ofGetElapsedTimef()) * _speed);
	float y = 0;
	float z = _amplitude * cos((offset + ofGetElapsedTimef()) * _speed);

	ofVec3f pos = ofVec3f(x, y, z);	
	pos *= _scalar;

	return pos;
}

//--------------------------------------------------------------
void MotionController::setup_gui() 
{
	params.setName("Motion_Controller");
	params.add(use_sensors.set("Use_Sensors", false));
    params.add(use_geometry.set("Use_Geometry", true));
	params.add(show_gizmos.set("Show_Gizmos", true));
	params.add(mode_string.set("MODE: PAUSE"));
	params.add(_reset_all.set("Reset_All", false));
    
    params.add(halo_pos.set("Halo_Pos", ofVec3f(0,0,0), ofVec3f(-5000,-5000,0), ofVec3f(5000,5000,5000)));
    params.add(halo_radius.set("Halo_Radius",3000, 0, 5000));
    params.add(halo_spin.set("Halo_Spin",0, -2, 2));
    
    // add halo listeners
    halo_radius.addListener(this, &MotionController::listener_halo_radius);
    halo_pos.addListener(this, &MotionController::listener_halo_pos);

//    home_target.set("Home_TARGET", ofVec3f(450, 0, 650), ofVec3f(-900,-300,250), ofVec3f(900,300,1300));

	// add the min/max mapping ranges
//    params.add(easing_type.set("Easing_Type", 2, 0, 5));        // 2 is SINE::easeInOut


	params_modifiers.setName("Motion_Modifiers");
	params_modifiers.add(use_noise.set("Use_Noise", false));
	params_modifiers.add(noise_speed.set("Noise_Speed", .5, .01, 2));
	params_modifiers.add(noise_scalar.set("Noise_Scalar", 100, 1, 200));
	
	params_modifiers.add(use_sine.set("Use_Sine", true));
	params_modifiers.add(sine_preset_idle.set("Preset_Idle", false));
	params_modifiers.add(sine_speed.set("Sine_Speed", 5, 0, 10));
	params_modifiers.add(sine_amplitude.set("Sine_Amp", 15, 1, 150));
	params_modifiers.add(sine_frequency.set("Sine_Freq", 3, 1, 50));	// <--- not using, currently
	params_modifiers.add(sine_scalar.set("Sine_Scalar", .1, .01, 2));

//    params_timers.setName("Motion_Timers");
//    params_timers.add(timer_reset_timeout.set("Reset_Timeout", 7, 0, 15));

    use_sensors.addListener(this, &MotionController::listener_use_sensors);
	use_noise.addListener(this, &MotionController::listener_use_noise);
	use_sine.addListener(this, &MotionController::listener_use_sine);
	sine_preset_idle.addListener(this, &MotionController::listener_sine_preset_idle);
	_reset_all.addListener(this, &MotionController::listener_reset_all);


	panel.setup(params);
	panel.add(params_modifiers);
//    panel.add(params_timers);
	panel.setPosition(230, 20);

	// hide this: can't set target manually when updtating with MotionController::update_motion_targets()
//    panel.getGroup("Home_TARGET").minimize();

}

//--------------------------------------------------------------
void MotionController::listener_halo_radius(float & val)
{
    // update the radius of the halo
    for (int i=0; i<NUM_AGENTS; i++){
        float t;
        t = ofMap(val, 0, halo_pos.get().distance(halo.getVertices()[i]), 0, 1);
        ofVec3f pos = halo_pos.get().getInterpolated(halo.getVertices()[i], t);
        
        halo.getVertices()[i].set(pos);
    }
}

//--------------------------------------------------------------
void MotionController::listener_halo_pos(ofVec3f & val)
{
    // update the location of the halo
    ofVec3f centroid;
    for (int i=0; i<NUM_AGENTS; i++)
        centroid += halo.getVertices()[i];
    centroid /= NUM_AGENTS;
    
    ofVec3f diff = val - centroid;
    for (int i=0; i<NUM_AGENTS; i++)
        halo.getVertices()[i].set(halo.getVertices()[i] + diff);
}

//--------------------------------------------------------------
void MotionController::listener_use_sensors(bool &val)
{
}

//--------------------------------------------------------------
void MotionController::listener_use_noise(bool & val)
{
	if (val) {
		modifier = MOTION_MODIFIER::NOISE;
	}
	else if (!use_sine)
	{
		modifier = MOTION_MODIFIER::NONE;
	}
}

//--------------------------------------------------------------
void MotionController::listener_use_sine(bool & val)
{
	if (val) {
		modifier = MOTION_MODIFIER::SINE;
	}
	else if (!use_noise)
	{
		modifier = MOTION_MODIFIER::NONE;
	}
}

//--------------------------------------------------------------
void MotionController::listener_sine_preset_idle(bool & val)
{
	sine_speed = 5;
	sine_amplitude = 10;
	sine_scalar = 1;
	
	sine_preset_idle = false;
}

//--------------------------------------------------------------
void MotionController::listener_reset_all(bool &val)
{
    if (val){
        mode = MOTION_MODE::HOME;
        _reset_all = false;
    }
}

//--------------------------------------------------------------
void MotionController::update_mode_string()
{
	string temp = "MODE: ";
	switch (mode)
	{
	case MOTION_MODE::PAUSE:
		temp += "PAUSE";
		break;
	case MOTION_MODE::RUN:
		temp += "RUN";
		break;
	case MOTION_MODE::HOME:
		temp += "HOME";
		break;
    case MOTION_MODE::TIMELINE:
        temp += "TIMELINE";
        break;
    }
    
    mode_string = temp;


	// check if this is a new mode, start the timer
//    if (temp != mode_string.get()){
//
//        // only update without timer if the previous mode was PAUSE
//        if (temp == "MODE: RUN") {
//            if (mode_string.get() == "MODE: PAUSE" )//|| timer_mode_string.has_timedout)
//                mode_string = temp;
//        }
//        else {
////            timer_mode_string.t_last = ofGetElapsedTimef();
////            timer_mode_string.has_timedout = false;
//            // set the new mode string
//            mode_string = temp;
//        }
//    }
//
//    // if we're in a trigger-based motion mode, display the mode for a little bit
//    if (mode_string.get() != "MODE: RUN" || mode_string.get() != "MODE: PAUSE") {
//
////        // update the timer
////        if (ofGetElapsedTimef() - timer_mode_string.t_last > timer_mode_string.timeout)
////            timer_mode_string.has_timedout = true;
//
//    }

	
}

//--------------------------------------------------------------
void MotionController::draw_gui(int x, int y)
{

	if (panel.getPosition().x != x) panel.setPosition(x, panel.getPosition().y);
	if (panel.getPosition().y != y) panel.setPosition(panel.getPosition().x, y);

	ofPushStyle();
	if (use_sensors) {
		ofSetColor(ofColor::aquamarine, 60);
	}
	else {
		ofSetColor(0, 60);
	}
	int padding = 10;
	ofDrawRectangle(panel.getPosition().x - padding, panel.getPosition().y - padding, panel.getWidth() + 2 * padding, panel.getHeight() + 2 * padding);
	panel.draw();

//    draw_timers(true, true);

	ofPopStyle();
}

//--------------------------------------------------------------
void MotionController::setup_gizmos()
{

	// for some reason, pointers to ofxGizmo objects aren't initializing properly
	// ... so we have to manually add these to the ofxGizmo vectors
	gizmos.push_back(&gizmo_0);
	gizmos.push_back(&gizmo_1);
	gizmos.push_back(&gizmo_2);
	gizmos.push_back(&gizmo_3);
	gizmos.push_back(&gizmo_4);
	gizmos.push_back(&gizmo_5);
	gizmos.push_back(&gizmo_6);
	gizmos.push_back(&gizmo_7);
	gizmos.push_back(&gizmo_8);
	gizmos.push_back(&gizmo_9);
    gizmos.push_back(&gizmo_10);
    gizmos.push_back(&gizmo_11);

	// and then remove the ones we don't need
	gizmos.erase(gizmos.begin() + NUM_AGENTS, gizmos.end());


	// okay, now we should be good to go .... set the starting positions
	for (int i = 0; i < NUM_AGENTS; i++) {
        
        ofVec3f pos = halo.getVertices()[i];
		
		ofMatrix4x4 mat;
        mat.makeIdentityMatrix();
        mat.setTranslation(pos);
        mat.setRotate(ofQuaternion(.707, 0, 0, 0));
		
        gizmos[i]->setMatrix(mat);
		gizmos[i]->setDisplayScale(.5);
	}
    
    // add all the gizmos to a ParameterGroup
    for (int i = 0; i < NUM_AGENTS; i++) {
        params_positions.setName("Params_Positions");
        ofParameter<ofVec3f> pos;
        positions.push_back(pos);
        params_positions.add(positions[i].set("Target_Pos_"+ofToString(i), ofVec3f(), ofVec3f(-5000,-5000,-5000), ofVec3f(5000,5000,5000)));
    }
    panel.add(params_positions);
}

//--------------------------------------------------------------
bool MotionController::has_gizmos_isInteracting() {
    
    // update gizmo/cam interaction
    bool gizmo_active = false;
    for (int i = 0; i < NUM_AGENTS; i++) {
        if (gizmos[i]->isInteracting())
            gizmo_active = true;
    }
    
    return gizmo_active;
}

//--------------------------------------------------------------
void MotionController::update_gizmos(int view_dim_x, int view_dim_y){
    for (int i=0; i<NUM_AGENTS; i++){
        gizmos[i]->setViewDimensions(view_dim_x, view_dim_y);
    }
}

//--------------------------------------------------------------
void MotionController::draw_gizmos(ofEasyCam * cam)
{
	for (int i = 0; i < NUM_AGENTS; i++) {
        if (show_gizmos){
            ofEnableDepthTest();
            gizmos[i]->draw(*cam);
            ofDisableDepthTest();
        }
	}
}

