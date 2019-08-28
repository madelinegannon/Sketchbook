#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setup_gui();
    setup_camera();
    setup_world();
}

//--------------------------------------------------------------
void ofApp::update(){
    
   

    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(background_inner, background_outer, OF_GRADIENT_CIRCULAR);
    
    ofEnableDepthTest();
    draw_world();
    ofDisableDepthTest();
    
    draw_gui();
}

//--------------------------------------------------------------
void ofApp::setup_world()
{
    gizmo_circle_0.getMatrix().setTranslation(ofVec3f(0,0,500));
    gizmo_circle_1.getMatrix().setTranslation(ofVec3f(0,0,0));
}

//--------------------------------------------------------------
void ofApp::draw_world(){
    cam.begin();
    ofDrawAxis(1500);
    
    // draw gizmos
    gizmo_circle_0.draw(cam);
    gizmo_circle_1.draw(cam);
    
    // draw geometry
    ofPushStyle();
    ofSetColor(ofColor::magenta);
    ofNoFill();
    
    float angle_incr = 360./cyl_resolution;
    // draw a circle around the first gizmo
    ofBeginShape();
    for (int i=0; i<cyl_resolution; i++){
        ofVec3f pos = ofVec3f(cos(ofDegToRad(i*angle_incr)), sin(ofDegToRad(i*angle_incr)), 0);
        pos = pos * cyl_radius;
        pos = pos * gizmo_circle_0.getMatrix();
        ofVertex(pos);
    }
    ofEndShape(OF_CLOSE);
    // draw a circle around the second gizmo
    ofBeginShape();
    for (int i=0; i<cyl_resolution; i++){
        ofVec3f pos = ofVec3f(cos(ofDegToRad(i*angle_incr)), sin(ofDegToRad(i*angle_incr)), 0);
        pos = pos * cyl_radius;
        pos = pos * gizmo_circle_1.getMatrix();
        ofVertex(pos);
    }
    ofEndShape(OF_CLOSE);
    
    // draw the axis between the two circles
    ofSetColor(ofColor::skyBlue);
    ofDrawLine(gizmo_circle_0.getMatrix().getTranslation(), gizmo_circle_1.getMatrix().getTranslation());
    
    // draw helix between the two circles
    ofVec3f axis = (gizmo_circle_0.getMatrix().getTranslation() - gizmo_circle_1.getMatrix().getTranslation());
    cyl_height.set(axis.length());
    axis.normalize();
    float angle_diff = (start_angle > end_angle) ? (start_angle - end_angle) : (end_angle - start_angle);
    int count = cyl_resolution / (360./(angle_diff + 360*num_turns));
    angle_incr = (angle_diff + 360*num_turns) / count;
    
    ofSetLineWidth(3);
    
    // if too steep, just draw the straight line
    if (count < 2){        
        ofDrawLine(ofVec3f(cos(ofDegToRad(start_angle)), sin(ofDegToRad(start_angle)), 0)*cyl_radius + gizmo_circle_1.getMatrix().getTranslation(),
                   ofVec3f(cos(ofDegToRad(end_angle)), sin(ofDegToRad(end_angle)), 0)*cyl_radius + gizmo_circle_0.getMatrix().getTranslation());
    }
    else{
        cout << endl;
        ofBeginShape();
        for (int i=0; i<=count; i++){
            // generate the x,y coordinate
            ofVec3f pos;
            if (start_angle <= end_angle)
                pos = ofVec3f(cos(ofDegToRad(start_angle + i*angle_incr)), sin(ofDegToRad(start_angle + i*angle_incr)), 0);
            else
                pos = ofVec3f(cos(ofDegToRad(start_angle - i*angle_incr)), sin(ofDegToRad(start_angle - i*angle_incr)), 0);
            // add the radius
            pos = pos * cyl_radius;
            // offset based on the axis between circles
            ofVec3f offset = axis.getScaled(i * (cyl_height / count));
            pos += offset;
            // move based on gizmo originally at {0,0,0}
            pos += gizmo_circle_1.getMatrix().getTranslation();
            ofVertex(pos);
        }
        ofEndShape();
    }
    ofPopStyle();

    cam.end();
}

//--------------------------------------------------------------
void ofApp::draw_gui(){
    
    panel.draw();
    panel_cyl.draw();
    ofDrawBitmapStringHighlight("FPS: "+ofToString(ofGetFrameRate()), ofGetWidth()-100, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
        default:
            break;
    }
    
    keypressed_camera(key);
    keypressed_gizmo(key);
}

//--------------------------------------------------------------
void ofApp::keypressed_camera(int key){
    bool val = true;
    switch (key) {
        case '1':
            listener_show_top(val);
            break;
        case '2':
            listener_show_front(val);
            break;
        case '3':
            listener_show_side(val);
            break;
        case '4':
            listener_show_perspective(val);
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::keypressed_gizmo(int key){
    bool val = true;
    switch (key) {
        case 'e':
        case 'E':
            gizmo_circle_0.setType(ofxGizmo::OFX_GIZMO_ROTATE);
            gizmo_circle_1.setType(ofxGizmo::OFX_GIZMO_ROTATE);
            break;
        case 'w':
        case 'W':
            gizmo_circle_0.setType(ofxGizmo::OFX_GIZMO_MOVE);
            gizmo_circle_1.setType(ofxGizmo::OFX_GIZMO_MOVE);
            break;
        case 'r':
        case 'R':
            // reset the position & orientation
            gizmo_circle_0.getMatrix().setTranslation(ofVec3f(0,0,500));
            gizmo_circle_0.getMatrix().setRotate(ofQuaternion(0,0,0,1));
            gizmo_circle_1.getMatrix().setTranslation(ofVec3f(0,0,0));
            gizmo_circle_1.getMatrix().setRotate(ofQuaternion(0,0,0,1));
            break;
    }
    
}

//--------------------------------------------------------------
bool ofApp::do_disable_camera(){
    
    return (gizmo_circle_0.isInteracting() || gizmo_circle_1.isInteracting());
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (do_disable_camera() && cam.getMouseInputEnabled())
        cam.disableMouseInput();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    params.setName("Navigation");
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
    
    params_cyl.setName("Cylinder_Params");
    params_cyl.add(cyl_radius.set("Radius", 500, 1, 5000));
    params_cyl.add(cyl_height.set("Height", 800, 1, 5000));
    params_cyl.add(cyl_resolution.set("Resoultion", 36, 3, 360));
    params_cyl.add(start_angle.set("Start_Angle", 0, -720, 720));
    params_cyl.add(end_angle.set("End_Angle", 90, -720, 720));
    params_cyl.add(num_turns.set("Num_Turns", 0, 0, 10));
    
    panel_cyl.setup(params_cyl);
    panel_cyl.setPosition(panel.getPosition().x + panel.getWidth() + 5, panel.getPosition().y);
    
    ofSetCircleResolution(60);
}

//--------------------------------------------------------------
void ofApp::setup_camera(){
    cam.setFarClip(9999999);
    cam.setDistance(5000);
    ofNode tgt;
    tgt.setGlobalPosition(0, 0, 0);
    tgt.setGlobalOrientation(ofQuaternion(0, 0, 0, 1));
    cam.setTarget(tgt);
    cam.lookAt(ofVec3f(0, 0, -1), ofVec3f(1, 0, 0));
}

//--------------------------------------------------------------
void ofApp::listener_show_top(bool & val)
{
    if (val) {
        
        int x = 0;
        int y = 0;
        int z = 4000;

        
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
        
        int x = 2000;
        int y = 400;
        int z = 600;
        
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
        
        int x = 3000;
        int y = -2000;
        int z = 2000;
        
        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(0, 800 / 2, 0);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, 0, 1));
        cam.setGlobalPosition(pos);
        
        show_top = false;
        show_front = false;
        show_side = false;
    }
}
