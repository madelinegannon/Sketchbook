#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    pivot.setPosition(0, 0, 0);

    float scalar = 2.;
    body.set(500/scalar, 250/scalar, 50/scalar);
    body.setPosition(0, 0, 0);
    
    body.lookAt(ofVec3f(1.37,-.4,-1)); // works in XZ plane only :(
    

    //limb.setParent(body);
    //limb.setPosition(500/(2*scalar), 250/(2*scalar), 0);
    //
    //toe.setParent(limb);
    //toe.setPosition(0, 0, -240/scalar);
    //
    //motor.setParent(limb);
    //motor.setPosition(0,0,0);
    //motor.motorZero.setParent(motor);
    //motor.motorZero.setPosition(0,0,100);
    //motor.setup();



	motor.setParent(body);
	motor.setPosition(500 / (2 * scalar), 250 / (2 * scalar), 0);

	// ... motorZero is body Up direction
	//motor.motorZero.setParent(motor);
	//motor.motorZero.setPosition(0, 0, 100);
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
    
    rotAngle = ofMap(timer, timerMin, timerMax, rotMin, rotMax);
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
    ofSetColor(ofColor::cyan);
    //ofDrawLine(pivot.getGlobalPosition(), lookAt.getGlobalPosition());
    ofSetColor(ofColor::antiqueWhite,100);
    body.drawWireframe();
    ofSetColor(ofColor::antiqueWhite);
    //body.drawWireframe();
    
    // draw leg
    //limb.draw();
    //toe.draw();
    ofSetLineWidth(5);
    ofDrawLine(limb.getGlobalPosition(), toe.getGlobalPosition());
    
    ofPopStyle();
    
    
    // draw angular rotation
    motor.draw();

    
    
    
    ofNoFill();
    pivot.draw();
    //lookAt.draw();
    
    
    cam.end();
    ofDisableDepthTest();
    
    
    // draw debug info onscreen
    stringstream ss;
    ss << endl << "{yaw, pitch, roll}: {" << yaw << "," << pitch << "," << roll << "}" << endl;
    ss << "{pos.x, pos.y, pos.z}: " << ofToString(body.getGlobalPosition()) << endl;
    //ss << "lockPivot: " << ofToString(lockPivot) << endl;
    // ss << "lookAtGizmo: " << ofToString(lookAtGizmo) << endl;
    ofDrawBitmapStringHighlight(ss.str(), 10,10);

    
    ofSetWindowTitle("FrameRate: "+ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == ' '){
        float max = 1000;
        float min = -max;
        ofVec3f pt = ofVec3f (ofRandom(min, max),ofRandom(min, max),ofRandom(min, max));
    
        lookAt.setGlobalPosition(pt);
        
        pivot.lookAt(lookAt.getGlobalPosition(), ofVec3f(0,0,1));
        lookAt.setOrientation(pivot.getGlobalOrientation());

        //        pivot.rotate(-90, pivot.getXAxis());
        //        pivot.rotate(90, pivot.getZAxis());
        ofNode temp = pivot;
        temp.rotate(-90, temp.getXAxis());
        temp.rotate(90, temp.getZAxis());
  
        body.setGlobalOrientation(temp.getGlobalOrientation());
        
        yaw = body.getHeading();
        pitch = body.getPitch();
        roll = body.getRoll();
        height = body.getHeight();
    }
    if (key == 'c'){
        pivot.setOrientation(ofQuaternion());
        lookAt.setOrientation(pivot.getGlobalOrientation());
        body.setGlobalOrientation(pivot.getGlobalOrientation());
        
        yaw = body.getHeading();
        pitch = body.getPitch();
        roll = body.getRoll();
        height = body.getHeight();
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
