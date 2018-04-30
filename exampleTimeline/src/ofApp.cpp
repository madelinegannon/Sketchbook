#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    setupGui();
    setupTimeline();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(timeline.getIsPlaying()){
        for (auto &key : keyframes)
            key->updateParameterGroup();
    //        keyframes_param_00->updateParameterGroup();
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(60);
    
    
    
    if (showGui){
        
        ofPushStyle();
        ofSetColor(0, 60);
        ofDrawRectangle(10, 10, panel.getWidth()+20, panel.getHeight()+20);
        ofPopStyle();
        
        panel.draw();
        
        timeline.draw();
    }
    
}

//--------------------------------------------------------------
void ofApp::setupGui(){
    
    param_00.setName("param_00");
    param_00.add(param_00_a.set("param_00_a", 0, -PI, PI));
    param_00.add(param_00_b.set("param_00_b", .5, 0, 1));
    
    param_01.setName("param_01");
    param_01.add(param_01_a.set("param_01_a", 0, -PI, PI));
    param_01.add(param_01_b.set("param_01_b", .5, 0, 1));
    
    param_02.setName("param_02");
    param_02.add(param_02_a.set("param_02_a", 0, -PI, PI));
    param_02.add(param_02_b.set("param_02_b", .5, 0, 1));
    
    param_03.setName("param_03");
    param_03.add(param_03_a.set("param_03_a", 0, -PI, PI));
    param_03.add(param_03_b.set("param_03_b", .5, 0, 1));
    
    params.setName("Parameters");
    params.add(param_00);
    params.add(param_01);
    params.add(param_02);
    params.add(param_03);
    
    panel.setup(params);
    panel.setPosition(20, 20);
    
}

//--------------------------------------------------------------
void ofApp::setupTimeline(){
    
    //lets you use COMMAND+C and COMMAND+V actions on mac
    ofxTimeline::removeCocoaMenusFromGlut("Gui Track");
    
    timeline.setup();
    timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setDurationInSeconds(30);
    
    for (int i=0; i<4; i++){
        keyframes.push_back(new ofxTLGuiKeyframes());
        keyframes[i]->setXMLFileName("keyframes_param_"+ofToString(i));
    }
    
    keyframes[0]->setParameterGroup(param_00);
    keyframes[1]->setParameterGroup(param_01);
    keyframes[2]->setParameterGroup(param_02);
    keyframes[3]->setParameterGroup(param_03);
    
    timeline.addTrack("Param Group 0",  keyframes[0]);
    timeline.addTrack("Param Group 1",  keyframes[1]);
    timeline.addTrack("Param Group 2",  keyframes[2]);
    timeline.addTrack("Param Group 3",  keyframes[3]);
    
//    keyframes_param_00 = new ofxTLGuiKeyframes();
//    keyframes_param_00->setXMLFileName("MyEmptyKeyframes.xml");
//    keyframes_param_00->setParameterGroup(param_00);
//    timeline.addTrack("My Custom Keyframes", keyframes_param_00);
    
    // move the timeline to the bottom of the window
    timeline.setOffset(ofVec2f(0,ofGetHeight() - timeline.getHeight() - 10));

//    timeline.play();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
   
    switch (key) {            
        case 'g':
            showGui = !showGui;
            break;
        case 'L':
            //track_param_00->lockCameraToTrack = !track_param_00->lockCameraToTrack;
            break;
        case 'T':
            //track_param_00->addKeyframe();
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
