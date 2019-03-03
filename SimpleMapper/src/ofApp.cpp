#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetCircleResolution(100);
    texScreen.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
    ofBackground(230,230,240);
    
    
    setup_gui();
    
    quad.addVertex(p1.get());
    quad.addVertex(p2.get());
    quad.addVertex(p3.get());
    quad.addVertex(p4.get());
    quad.close();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (is_adjusting && mode_calibrate){
        auto temp = quad.getClosestPoint(ofVec2f(mouseX,mouseY));
        float fluff_dist = 10;
        int i=0;
        for (auto &v : quad.getVertices()){
            if (v.squareDistance(temp) < fluff_dist*fluff_dist){
                v.set(mouseX, mouseY);
                
                // update the gui
                switch (i) {
                    case 0:
                        p1.set(v);
                        break;
                    case 1:
                        p2.set(v);
                        break;
                    case 2:
                        p3.set(v);
                        break;
                    case 4:
                        p3.set(v);
                        break;
                    default:
                        break;
                }
                break;
                
            }
            i++;
        }
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    test_texture_warping();
    
    // draw stuff you don't want projected afterwards
    
    
    if (!mode_project){
        
        // draw the outline of the quad
        if (mode_calibrate)
            draw_quad();
        
        // draw the mouse pressed
        ofPushStyle();
        ofFill();
        ofSetColor(ofColor::yellow, 100);
        if (is_adjusting){
            ofDrawEllipse(mouseX, mouseY, 30, 30);
        }
        ofPopStyle();

    }

    if (show_gui)
        panel.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){    
    panel.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::test_texture_warping(){
    // render what should be projected
    render_dummy_texture();
    
    // map the screen to the warped quad
    if (mode_calibrate || mode_project)
        warp_texture();
}

//--------------------------------------------------------------
void ofApp::render_dummy_texture(){
    ofPushStyle();
    ofSetHexColor(0x66CC33);
    ofDrawRectangle(100,100,300,300);
    
    ofSetHexColor(0xffffff);
    ofPushMatrix();
    ofTranslate(200,200,0);

    ofDrawCircle(0,0,80);
    ofDrawCircle(100,0,10);    // a small one
    ofPopMatrix();
    ofSetHexColor(0x333333);
    ofDrawBitmapString("(a) on screen", 150,200);
    ofSetHexColor(0xFFCC33);
    ofDrawCircle(mouseX, mouseY,20);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::warp_texture(){
    ofPushStyle();
    texScreen.loadScreenData(0,0,ofGetWidth(),ofGetHeight());
    
    // cancel out the backgroud
    if (mode_project){
        ofSetColor(0);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    }
    

  /*
     Texture coordinate orderings:
       (P4)–––––––(P3)
        |           |
        |     +     |
        |           |
       (P1)–––––––(P2)
       texture.getCoordFromPercent(0, 0) = (+) CENTROID
       texture.getCoordFromPercent(-.5, -.5) = (P1)
   */
    ofPoint p1, p2, p3, p4;
    p1.set(quad.getVertices()[0]);
    p2.set(quad.getVertices()[1]);
    p3.set(quad.getVertices()[2]);
    p4.set(quad.getVertices()[3]);
    

    ofSetHexColor(0xffffff);
    texScreen.draw(p1,p2,p3,p4);

    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::draw_quad(){
    
    ofPushStyle();
    ofSetColor(255, 0, 255);
    ofNoFill();
    ofSetLineWidth(3);
    quad.draw();
    
    ofSetColor(60);
    float offset = 15;
    ofDrawBitmapString("(P1)", p1.get().x - offset*2, p1.get().y + offset);
    ofDrawBitmapString("(P2)", p2.get().x + offset/2, p2.get().y + offset);
    ofDrawBitmapString("(P3)", p3.get().x + offset/2, p3.get().y - offset/2);
    ofDrawBitmapString("(P4)", p4.get().x - offset*2, p4.get().y - offset/2);
    
    ofSetColor(0, 255, 255);
    for (auto v : quad.getVertices())
        ofDrawEllipse(v, 4, 4);
    
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    params.setName("Simple_Mapper");
    params.add(show_gui.set("Show_Gui", true));
    params.add(mode_project.set("Project_Mode", false));
    params.add(mode_calibrate.set("Calibrate_Mode", true));
    
    
    int width = 600;
    int height = 300;
    int x = ofGetWidth()/2 - width/2;
    int y = ofGetHeight()/2 + height/2;
    
    /*
     Texture coordinate orderings:
         (P4)–––––––(P3)
          |           |
          |     +     |
          |           |
         (P1)–––––––(P2)
         texture.getCoordFromPercent(0, 0) = (+) CENTROID
         texture.getCoordFromPercent(-.5, -.5) = (P1)
     */
    
    params.add(p1.set("Quad_P1", ofVec2f(x, y), ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight())));
    params.add(p2.set("Quad_P2", ofVec2f(x + width, y), ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight())));
    params.add(p3.set("Quad_P3", ofVec2f(x + width, y - height), ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight())));
    params.add(p4.set("Quad_P4", ofVec2f(x, y - height), ofVec2f(0, 0), ofVec2f(ofGetWidth(), ofGetHeight())));
    
    p1.addListener(this, &ofApp::listener_p1);
    p2.addListener(this, &ofApp::listener_p2);
    p3.addListener(this, &ofApp::listener_p3);
    p4.addListener(this, &ofApp::listener_p4);
    
    
    panel.setup(params);
    panel.setPosition(10,10);
    
    if (load_gui_from_file)
        panel.loadFromFile("settings.xml");
    
}

//--------------------------------------------------------------
void ofApp::listener_p1(ofVec2f &val){
    if (mode_calibrate)
        quad.getVertices()[0].set(val);
}
void ofApp::listener_p2(ofVec2f &val){
    if (mode_calibrate)
        quad.getVertices()[1].set(val);
}
void ofApp::listener_p3(ofVec2f &val){
    if (mode_calibrate)
        quad.getVertices()[2].set(val);
}
void ofApp::listener_p4(ofVec2f &val){
    if (mode_calibrate)
        quad.getVertices()[3].set(val);
}
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case ' ':
            mode_project.set(!mode_project);
            break;
        case 'c':
            mode_calibrate.set(!mode_calibrate);
            break;
        case 'g':
            show_gui.set(!show_gui);
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

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    float dist = 100;
    ofVec2f mouse = ofVec2f(x,y);
    ofVec2f temp = quad.getClosestPoint(mouse);
    if (mouse.squareDistance(temp) > dist*dist){
        is_adjusting = false;
    }

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    float dist = 30;
    ofVec2f mouse = ofVec2f(x,y);
    ofVec2f temp = quad.getClosestPoint(mouse);
    if (mouse.squareDistance(temp) < dist*dist){

        is_adjusting = true;
    }

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

    is_adjusting = false;
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
