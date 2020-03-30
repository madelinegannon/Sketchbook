#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setup_world();
}

//--------------------------------------------------------------
void ofApp::setup_world(){
    
    ofNode target;
    target.setGlobalPosition(.000001, 4, 0);
    target.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));
    cam.setTarget(target);
    cam.setNearClip(.001);
    cam.setFarClip(999999);
    cam.enableOrtho();
    cam.removeAllInteractions();
    // pan with middle button
    cam.addInteraction(ofEasyCam::TRANSFORM_TRANSLATE_XY, OF_MOUSE_BUTTON_MIDDLE);
    // zoom up/dn with right button
    // ... one odd thing: if you zoom too much, you go through the ground plane, and you end up looking at the plane from the other side.
    cam.addInteraction(ofEasyCam::TRANSFORM_TRANSLATE_Z, OF_MOUSE_BUTTON_RIGHT);
    cam.sensitivityScroll = 50;
    
    ground.set(ofGetHeight(), ofGetWidth());
    ground.tiltDeg(90);
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofEnableDepthTest();
    cam.begin();
    ofDrawAxis(15);
    ofPushStyle();
    ofSetColor(100, 100, 100);
    ground.draw();
    
    ofSetColor(ofColor::magenta,100);
    ofPushMatrix();
    ofTranslate(mouse);
    ofDrawBox(25);
    ofPopMatrix();
    
    draw_world();
    
    ofPopStyle();
    cam.end();
    ofDisableDepthTest();
    
}

//--------------------------------------------------------------
void ofApp::draw_world(){
    ofPushStyle();
    ofSetLineWidth(3);
    ofSetColor(ofColor::magenta, 100);
    for (auto path : paths)
        path->draw();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case 'd':
            if (cam.getMouseInputEnabled())
                cam.disableMouseInput();
            else
                cam.enableMouseInput();
            break;
        case 'o':
            if (cam.getOrtho())
                cam.disableOrtho();
            else
                cam.enableOrtho();
            break;
        case 'r':
            // you have to zoom in/out a little for the ground plane to show up
            cam.setPosition(.00001, 3, 0);
            break;
        case 'c':
            paths.clear();
            break;
        default:
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    // screenToWorld only works in ortho mode like this
    auto mouseRay = cam.screenToWorld(glm::vec3 ((float)x,(float)y,0));
    
    mouse.x = mouseRay.x;
    mouse.y = 0;
    mouse.z = mouseRay.z;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (button == OF_MOUSE_BUTTON_LEFT){
        float dist_threshold = 10;
        int index = paths[paths.size()-1]->getVertices().size()-1;
        ofVec3f vert_prev = ofVec3f(paths[paths.size()-1]->getVertices()[index]);
        auto pt = cam.screenToWorld(glm::vec3 ((float)x,(float)y,0));
        if (vert_prev.squareDistance(pt) > dist_threshold*dist_threshold){
            paths[paths.size()-1]->addVertex(pt.x,-.1,pt.z);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == OF_MOUSE_BUTTON_LEFT){
        paths.push_back(new ofPolyline());
        // screenToWorld only works in ortho mode like this
        auto pt = cam.screenToWorld(glm::vec3 ((float)x,(float)y,0));
        paths[paths.size()-1]->addVertex(pt.x,-.1,pt.z);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (button == OF_MOUSE_BUTTON_LEFT){
        // close the path
        paths[paths.size()-1]->close();
        // resample
        auto smoothed = paths[paths.size()-1]->getResampledByCount(50);
        // replace with smoothed vertices
        paths[paths.size()-1]->clear();
        paths[paths.size()-1]->addVertices(smoothed.getVertices());
        // reclose the path
        paths[paths.size()-1]->close();
        
        cout << "path: " << paths.size()-1 << ", vert count: " << paths[paths.size()-1]->getVertices().size() << endl;
    }
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
