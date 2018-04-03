#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    pivot.setPosition(0, 0, 0);

    float scalar = 2.;
    body.set(500/scalar, 250/scalar, 50/scalar);
    body.setPosition(0, 0, 0);
    
	// hard code and orientation for testing
    body.lookAt(ofVec3f(1.37,-2.4,-1)); 

	pivot = body;
    

	motor.setParent(body);
	motor.setPosition(500 / (2 * scalar), 250 / (2 * scalar), 0);
	motor.setup();

	limb.setParent(motor);
	limb.setPosition(0, 0, 0);

	toe.setParent(limb);
	toe.setPosition(0, 0, -240 / scalar);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    float timer = float(ofGetElapsedTimeMillis());
    
    timer /= timerMax;    

    limb.pan(sin(timer+.5));
    
	// calculate the motor angle based on limb orientation
	motor.update(limb.getZAxis());

    
}

//--------------------------------------------------------------
void ofApp::draw(){
        
    ofEnableDepthTest();
    cam.begin();
    
	// draw world
    ofDrawGrid(100,10,false, false, false,true);
    ofPushStyle();
    ofSetLineWidth(3);
    ofDrawAxis(150);
    ofPopStyle();
    

    ofPushStyle();
    
	ofSetColor(ofColor::antiqueWhite,100);
    body.drawWireframe();
  
    // draw leg
    limb.draw();
    toe.draw();
    ofSetLineWidth(5);
    ofDrawLine(limb.getGlobalPosition(), toe.getGlobalPosition());  
    
	ofPopStyle();
    
    
    // draw angular rotation
    motor.draw();

 
    ofNoFill();
    pivot.draw();
    
    
    cam.end();
    ofDisableDepthTest();
    
    
    // draw debug info onscreen
    stringstream ss;
    ss << endl << "{pos.x, pos.y, pos.z}: " << ofToString(body.getGlobalPosition()) << endl;
    //ss << "lockPivot: " << ofToString(lockPivot) << endl;
    // ss << "lookAtGizmo: " << ofToString(lookAtGizmo) << endl;
    ofDrawBitmapStringHighlight(ss.str(), 10,10);

    
    ofSetWindowTitle("FrameRate: "+ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
   
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
