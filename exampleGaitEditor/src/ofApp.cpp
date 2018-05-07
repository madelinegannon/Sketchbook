#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    setupGui();
    setupTimeline();
    setupGaitEditor();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(timeline.getIsPlaying()){
        for (auto &key : keyframes)
            key->updateParameterGroup();
    }
    
    if (gait_00.isValidPoint(mouseX,mouseY)){
        hoverPt.set(gait_00.getClosestPoint(gait_00.screenToWindow(mouseX,mouseY)));
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
        panel_gait.draw();
        
        timeline.draw();
        
        gait_00.draw();
        
        // highlight the hovering point over the bezier points
        ofPushMatrix();
        ofTranslate(gait_00.window.x, gait_00.window.y);
        if (hoverPt.x != 0 && hoverPt.y != 0){
            ofSetColor(ofColor::white, 60);
            ofDrawEllipse(hoverPt, 30, 30);
        }
        
        // cycle through the bezier within a given time
        if (!ofGetMousePressed()) // only update gait timing when we are not moving around the flags
            cycleTime = getGaitTiming();
        int step = ofGetElapsedTimeMillis() % cycleTime;
        float t = ofxTween::map(step, 0, cycleTime, 0, 1, true, *easing, ofxTween::easeInOut);
        
        ofFill();
        ofSetColor(255, 0, 0);
        ofDrawEllipse(gait_00.evaluate(t), 10, 10);

        ofPopMatrix();
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
    
    
    easings.setName("Easings");
    easings.add(easingFunction.set("Easing Function", 1, 0, easingNames.size()-1));
    easings.add(easingName.set(easingNames[1]));
    easingFunction.addListener(this, &ofApp::updateEasing);
    
    params_gait.setName("Gait Editor");
    params_gait.add(easings);
    panel_gait.setup(easings);
    panel_gait.setPosition(panel.getPosition().x + panel.getWidth() + 10, panel.getPosition().y);
    
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
    
    timeline.setPageName("GUI Ctrl");
    timeline.addPage("Gait Ctrl");
    timeline.addFlags("Gait Timing");
    timeline.setCurrentPage(0);
    
    // move the timeline to the bottom of the window
    timeline.setOffset(ofVec2f(0,ofGetHeight() - timeline.getHeight() - 10));

//    timeline.play();
    
}

//--------------------------------------------------------------
void ofApp::setupGaitEditor(){
    
    // initialize easing
    easing = new ofxEasingLinear();
    
    
    gait_00.setup(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetHeight() - timeline.getHeight() - 10);
    gait_00.load(gait_filename);
}


//--------------------------------------------------------------
float ofApp::getGaitTiming(){
    
    float start = timeline.getTrack("Gait Timing")->getEarliestTime();
    float end = timeline.getTrack("Gait Timing")->getLatestTime();
//    cout << "time span: " << ofToString(end - start) << endl;
    return (end - start);
}

//--------------------------------------------------------------
void ofApp::updateEasing(int &easingFunction){
    
    switch (easingFunction) {
        case Easing::CIRCULAR:
            easing = new ofxEasingCirc();
            break;
        case Easing::LINEAR:
            easing = new ofxEasingLinear();
            break;
        case Easing::QUADRATIC:
            easing = new ofxEasingQuad();
            break;
        case Easing::QUARTIC:
            easing = new ofxEasingQuart();
            break;
        case Easing::QUINTIC:
            easing = new ofxEasingQuint();
            break;
        case Easing::EXPONENTIAL:
            easing = new ofxEasingExpo();
            break;
        case Easing::SINE:
            easing = new ofxEasingSine();
            break;
        case Easing::BACK:
            easing = new ofxEasingBack();
            break;
        case Easing::BOUNCE:
            easing = new ofxEasingBounce();
            break;
        case Easing::ELASTIC:
            easing = new ofxEasingElastic();
            break;
        default:
            easing = new ofxEasingLinear();
            break;
    }
    easingName.set(easingNames[easingFunction]);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
   
    switch (key) {            
        case 'g':
            showGui = !showGui;
            break;
        case 'f':
            toggleFullscreen = !toggleFullscreen;
            
            int width, height;
            if (toggleFullscreen){
                width = ofGetScreenWidth() - 50;
                height = ofGetScreenHeight() - 50;
            }
            else{
                width = 1080;
                height = 720;
            }
            ofSetWindowShape(width, height);
            ofSetWindowPosition((ofGetScreenWidth()-width)/2, (ofGetScreenHeight()-height)/2);
            
            // update gui positions
            timeline.setOffset(ofVec2f(0,ofGetHeight() - timeline.getHeight() - 10));
            gait_00.updateWindow(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetHeight() - timeline.getHeight() - 10);
            
            break;
        case 'c':
            gait_00.clear();
            break;
            
        case 'l':
            gait_00.load("gait.xml");
            break;
            
        case OF_KEY_BACKSPACE:
            gait_00.handleDelete();
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
    gait_00.handleMouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    gait_00.handleMousePressed(x, y, button);
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

//--------------------------------------------------------------
void ofApp::exit(){
    gait_00.exit();
}
