#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setup_gui();
    setup_camera();
    
    motion.setup(NUM_AGENTS);
    agents.setup(NUM_AGENTS);
    sensors.setup(false);
    
    
    panel.add(motion.params);
    panel_agents.add(agents.params);
    panel_sensors.add(sensors.params);
    
    // minimize the agent controller to start (takes up too much screen space)
    panel_agents.getGroup("Agent_Controller").minimize();

    setup_keyframes();
}
//--------------------------------------------------------------
void ofApp::update(){
    
    update_keyframes();
    
    if (sensors.attach_geom){
        if (sensors.get_attachment_node().getGlobalPosition() != ofVec3f()){
            motion.halo_pos.set(sensors.get_attachment_node().getGlobalPosition());
        }
    }
    if (sensors.attach_target){
//        if (sensors.get_target_node().getGlobalPosition() != ofVec3f()){
            motion.set_sensor_target(sensors.get_target_node().getGlobalPosition());
//        }
    }
    
    if (use_sensors)
        sensors.update();
    
    // update the motion controller
    motion.update();
    
    agents.set_targets(motion.get_targets());
    agents.update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(background_inner, background_outer, OF_GRADIENT_CIRCULAR);
    
    ofEnableDepthTest();
    cam.begin();
    
    // draw world
    ofPushStyle();
    ofSetColor(100, 100);
    ofPushMatrix();
    ofTranslate(0,0,-1);
    ofDrawPlane(7000, 7000);
    ofSetColor(ofColor::lightCyan);
    ofNoFill();
    ofDrawRectangle(-7005/2, -7005/2, 7005, 7005);
    ofPopMatrix();
    ofPopStyle();
    
    ofDrawAxis(1500);
    
    sensors.draw();
    motion.draw(&cam);
    agents.draw();

    cam.end();
    ofDisableDepthTest();
    
    panel.draw();
    panel_agents.draw();
    panel_sensors.draw();
    
    ofDrawBitmapStringHighlight("FPS: "+ofToString(ofGetFrameRate()), ofGetWidth()-100, 10);
    
    if (show_timeline)
        draw_keyframes();
}


//--------------------------------------------------------------
void ofApp::setup_keyframes(){
    //lets you use COMMAND+C and COMMAND+V actions on mac
    ofxTimeline::removeCocoaMenusFromGlut("bjork-cornucopia");
    
    timeline.setup();
    timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setDurationInSeconds(90);
    
    for (int i=0; i<3; i++){
        keyframes.push_back(new ofxTLGuiKeyframes());
    }
    keyframes[0]->setXMLFileName("keyframes_motion_controller.xml");
    keyframes[1]->setXMLFileName("keyframes_motion_modifiers.xml");
    keyframes[2]->setXMLFileName("keyframes_motion_positions.xml");
    
    keyframes[0]->setParameterGroup(motion.params);
    keyframes[1]->setParameterGroup(motion.params_modifiers);
    keyframes[2]->setParameterGroup(motion.params_positions);

    timeline.addTrack("Motion_Positions",  keyframes[2]);
    timeline.addTrack("Motion_Modifiers",  keyframes[1]);
    timeline.addTrack("Motion_Controller",  keyframes[0]);

    
    //    keyframes_param_00 = new ofxTLGuiKeyframes();
    //    keyframes_param_00->setXMLFileName("MyEmptyKeyframes.xml");
    //    keyframes_param_00->setParameterGroup(param_00);
    //    timeline.addTrack("My Custom Keyframes", keyframes_param_00);
    
    // move the timeline to the bottom of the window
    timeline.setOffset(ofVec2f(0,ofGetHeight() - timeline.getHeight() - 10));
}

//--------------------------------------------------------------
void ofApp::update_keyframes(){
    
    if(timeline.getIsPlaying()){
        
        for (auto &key : keyframes)
            key->updateParameterGroup();
    }
    
    
    // disable the camera is we are messing with the timeline
    if (timeline.getDrawRect().inside(ofGetMouseX(), ofGetMouseY())){
        cam.disableMouseInput();
    }
    else
        cam.enableMouseInput();
}

//--------------------------------------------------------------
void ofApp::draw_keyframes(){
    
    ofPushStyle();
    ofSetColor(54, 200);
    ofDrawRectangle(timeline.getDrawRect());
    
    timeline.draw();
    
    ofPopStyle();
}


//--------------------------------------------------------------
void ofApp::draw_world(){
    
    // add scale figure for reference
    
}

//--------------------------------------------------------------
void ofApp::setup_camera(){
    cam.setFarClip(9999999);
    cam.setDistance(5000);
    ofNode tgt;
    tgt.setGlobalPosition(0, 400, 1000);
    tgt.setGlobalOrientation(ofQuaternion(0, 0, 0, 1));
    cam.setTarget(tgt);
    cam.lookAt(ofVec3f(0, 0, -1), ofVec3f(1, 0, 0));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case 't':
            show_timeline = !show_timeline;
            break;
        case 's':
            use_sensors = !use_sensors;
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        default:
            break;
    }
    
    keypressed_camera(key);
    motion.keyPressed(key);
    
}

//--------------------------------------------------------------
void ofApp::keypressed_camera(int key){
    
   
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    // disable camera if interacting with motion gizmos
    if (motion.has_gizmos_isInteracting())
        cam.disableMouseInput();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // disable camera if interacting with motion gizmos
    if (motion.has_gizmos_isInteracting())
        cam.disableMouseInput();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    // enable camera when done interacting with motion gizmos
    if (!cam.getMouseInputEnabled())
        cam.enableMouseInput();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    for (int i=0; i<NUM_AGENTS; i++){
        motion.update_gizmos(w, h);
    }

    // move the timeline to the bottom of the window
    timeline.setOffset(ofVec2f(0,ofGetHeight() - timeline.getHeight() - 10));
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    params.setName("Bird_Song_App");
    params.add(show_gui.set("Show_GUI", true));
    params.add(show_top.set("TOP", true));
    params.add(show_front.set("FRONT", false));
    params.add(show_side.set("SIDE", false));
    params.add(show_perspective.set("PERSP", false));
    
    show_top.addListener(this, &ofApp::listener_show_top);
    show_front.addListener(this, &ofApp::listener_show_front);
    show_side.addListener(this, &ofApp::listener_show_side);
    show_perspective.addListener(this, &ofApp::listener_show_perspective);
    
    panel.setup(params);
    panel.setPosition(10, 10);
    
    panel_agents.setup();
    panel_agents.setName("Agent_Params");
    panel_agents.setPosition(panel.getPosition().x+panel.getWidth()+5, panel.getPosition().y);
    
    panel_sensors.setup();
    panel_sensors.setName("Sensor_Params");
    panel_sensors.setPosition(panel_agents.getPosition().x+panel_agents.getWidth()+5, panel_agents.getPosition().y);
    
    ofSetCircleResolution(60);
}

//--------------------------------------------------------------
void ofApp::listener_show_top(bool & val)
{
    if (val) {
        
        int x = 0;
        int y = ((NUM_AGENTS - 1) * 800) / 2;
        int z;
        if (NUM_AGENTS < 5)
            z = 3000;
        else
            z = 6000;
        
        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(pos.x, pos.y, 0);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(1,0,0));
        
        show_front = false;
        show_side = false;
        show_perspective = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_show_front(bool & val)
{
    if (val) {
        
        int x = 0;
        int y = ((NUM_AGENTS - 1) * 800) / 2;
        int z = 600;
        if (NUM_AGENTS < 5)
            x = 3000;
        else
            x = 6000;
        
        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(0, pos.y, pos.z);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, 0, 1));
        
        show_top = false;
        show_side = false;
        show_perspective = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_show_side(bool & val)
{
    if (val) {
        
        int x = 900;
        int y = -2000;
        int z = 600;
        
        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(pos.x, 0, pos.z);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, 0, 1));
        
        show_top = false;
        show_front = false;
        show_perspective = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_show_perspective(bool & val)
{
    if (val) {
        
        int x = 0;
        int y = -2000;
        int z = 600;
        
        //if (NUM_AGENTS < 5) {
        //    x = 3000;
        //    z = 2000;
        //}
        //else {
        x = 4000;
        z = 2000;
        //}
        
        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(0, ((NUM_AGENTS - 1) * 800) / 2, 0);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, 0, 1));
        cam.setGlobalPosition(pos);
        
        show_top = false;
        show_front = false;
        show_side = false;
    }
}

