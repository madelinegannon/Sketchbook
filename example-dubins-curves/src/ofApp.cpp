#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    panel.setup(dubins_path.get_gui());
    panel.setPosition(15,15);
}

//--------------------------------------------------------------
void ofApp::update(){

    ofNode node = dubins_path.get_ticker_node();
    if (node.getGlobalPosition() != ofVec3f()){
        pts.push_back(node.getGlobalPosition());
    }
    if (pts.size() > 100)
        pts.erase(pts.begin());
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(20);

    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(5);
    ofSetColor(255, 200);
    if (pts.size() > 0){
        ofBeginShape();
        for (auto pt : pts)
            ofVertex(pt.x, pt.y);
        ofEndShape();
    }
    ofPopStyle();
    
    dubins_path.draw();
    
    panel.draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'c':
            dubins_path.clear();
            break;
        case 'd':
            show_debug = !show_debug;
            dubins_path.show_debug(show_debug);
            break;
        default:
            dubins_path.keyPressed(key);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
    dubins_path.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    dubins_path.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
   
    dubins_path.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    // generate a new way point for the dubins path
    if (!dubins_path.is_moving()){
    
        ofNode cp;
        cp.setGlobalPosition(ofVec3f(x,y,0));
        if (mouse_pressed_count % 2 == 0){
            cp.setGlobalOrientation(ofQuaternion(0, 0, 0, 1));
            cp.roll(30);
        }
        else{
            cp.setGlobalOrientation(ofQuaternion(0, 0, 0, 1));
            cp.roll(60);
        }
        
        dubins_path.add_waypoint(cp, 50);
        mouse_pressed_count++;
    }

    dubins_path.mouseReleased(x, y, button);
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
