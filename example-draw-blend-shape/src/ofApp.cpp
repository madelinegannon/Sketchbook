#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setup_camera();
    setup_gui();
    setup_world();
}

//--------------------------------------------------------------
void ofApp::setup_camera(){
    ofNode target;
    target.setGlobalPosition(0, 0, -40);
    target.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 0, 1));
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
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    panel.setup();
    panel.setName("Blend_Shape");
    panel.add(blend.set("Blend", true));
    
    params.setName("Line_Type");
    params.add(none.set("None", false));
    params.add(offset.set("Offset", false));
    params.add(spiral.set("Spiral", true));
    params.add(step_over.set("Step_Over", 5, 1, 30));
    params.add(animate_step_over.set("Animate_Step_Over", false));
    params.add(resolution.set("Resolution", 50, 5, 100));
    
    none.addListener(this, &ofApp::listener_none);
    offset.addListener(this, &ofApp::listener_offset);
    spiral.addListener(this, &ofApp::listener_spiral);
    resolution.addListener(this, &ofApp::listener_resolution);
    
    panel.add(params);
    panel.setPosition(10, 10);
    
}

//--------------------------------------------------------------
void ofApp::setup_world(){

    ground.set(ofGetWidth(), ofGetHeight());
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (animate_step_over){
        step_over = ofMap(sin(ofGetElapsedTimef()), -1, 1, step_over.getMin(), step_over.getMax());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofEnableDepthTest();
    cam.begin();
    ofDrawAxis(150);
    ofPushStyle();
    ofSetColor(100, 100, 100);
    ground.draw();
    
    ofSetColor(ofColor::magenta,100);
    ofPushMatrix();
    ofTranslate(mouse);
    ofDrawBox(25);
    ofPopMatrix();
    
    // move the geometry off the ground plane
    ofPushMatrix();
    ofTranslate(0, 0, 5);
    
    if (offset)
        draw_offset();
    else if (spiral)
        draw_spiral();
    else
        draw_none();
    
    ofPopMatrix();
    
    ofPopStyle();
    cam.end();
    ofDisableDepthTest();
    
    panel.draw();
    ofDrawBitmapStringHighlight("FPS: "+ofToString(ofGetFrameRate()), ofGetWidth()-100, 15);
    
}

//--------------------------------------------------------------
void ofApp::draw_none(){
    ofPushStyle();
    ofSetLineWidth(3);
    ofSetColor(ofColor::magenta, 100);
    for (auto path : paths)
        path->draw();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::draw_offset(){
    
    // for each path, offset towards the centroid a given number of steps
    ofPushStyle();
    if (blend){
        int i=0;
        for (auto &path : paths){
            ofSetLineWidth(3);
            ofSetColor(ofColor::magenta, 100);
            path->draw();
            
            if (i%2 == 0 && i<paths.size()-1){
                auto path_next = paths[i+1];
               
                vector<ofPolyline> inner_paths;
                ofSetLineWidth(1);
                ofSetColor(ofColor::cyan, 100);
                
                int numVerts = path->getVertices().size();
                int numVerts_next = path_next->getVertices().size();
                for (int j=0; j<step_over; j++){
                    float t = j / step_over;
                    ofPolyline inner_path;
                    for (int k=0; k<numVerts; k++){
                        // draw lines from each vertex to the centroid
                        ofDrawLine(path->getVertices()[k], path->getCentroid2D());
                        // lerp towards the contour pair to draw the offset path
                        if (k < numVerts_next)
                            inner_path.addVertex(glm::mix(path->getVertices()[k], path_next->getVertices()[k], t));
                    }
                    inner_path.close();
                    inner_paths.push_back(inner_path);
                }
                
                ofSetColor(ofColor::magenta, 200);
                for (auto p : inner_paths)
                    p.draw();
                
                ofDrawEllipse(path->getCentroid2D(), 15, 15);
            }
            i++;
        }
    }
    else{
        for (auto &path : paths){
            ofSetLineWidth(3);
            ofSetColor(ofColor::magenta, 100);
            path->draw();
            
            vector<ofPolyline> inner_paths;
            ofSetLineWidth(1);
            ofSetColor(ofColor::cyan, 100);
            int numVerts = path->getVertices().size();
            for (int j=0; j<step_over; j++){
                float t = j / step_over;
                ofPolyline inner_path;
                for (int k=0; k<numVerts; k++){
                    // draw lines from each vertex to the centroid
                    ofDrawLine(path->getVertices()[k], path->getCentroid2D());
                    // lerp towards the centroid to draw the offset path
                    inner_path.addVertex(glm::mix(path->getVertices()[k], path->getCentroid2D(), t));
                }
                inner_path.close();
                inner_paths.push_back(inner_path);
            }
            
            ofSetColor(ofColor::magenta, 200);
            for (auto p : inner_paths)
                p.draw();

            ofDrawEllipse(path->getCentroid2D(), 15, 15);
        }
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::draw_spiral(){
    
    ofPushStyle();
    if (blend){
        int i=0;
        for (auto path : paths){
            auto path_next = paths[i+1];
            ofSetLineWidth(3);
            ofSetColor(ofColor::magenta, 100);
            path->draw();
            
            if (i%2 == 0 && i<paths.size()-1){
                
                ofSetLineWidth(1);
                ofSetColor(ofColor::cyan, 100);
                ofPolyline inner_path;
                
                int numVerts = path->getVertices().size();
                int numVerts_next = path_next->getVertices().size();
                for (int j=0; j<step_over; j++){
                    float t;
                    for (int k=0; k<numVerts; k++){
                        if (k < numVerts_next){ // check that we're in bounds
                            // draw lines from each contour pair's vertex
                            ofDrawLine(path->getVertices()[k], path_next->getVertices()[k]);
                            
                            t = ofMap(k, 0, numVerts, j/step_over, (j+1)/step_over, true);
                            if (t < 1.0)                                inner_path.addVertex(glm::mix(path->getVertices()[k], path_next->getVertices()[k], t));
                        }
                    }
                }
                ofSetColor(ofColor::magenta, 200);
                inner_path.draw();
//                if (path->getArea() > path_next->getArea())
//                    ofDrawEllipse(path->getCentroid2D(), 15, 15);
//                else
//                ofDrawEllipse(path_next->getCentroid2D(), 15, 15);
            }
            i++;
        }
    }
    else{
        for (int i=0; i<paths.size(); i++){
            auto path = paths[i];
            ofSetLineWidth(3);
            ofSetColor(ofColor::magenta, 100);
            path->draw();
       
            ofSetLineWidth(1);
            ofSetColor(ofColor::cyan, 100);
            ofPolyline inner_path;
            int numVerts = path->getVertices().size();
            for (int j=0; j<step_over; j++){
                float t;
                for (int k=0; k<numVerts; k++){
                    // draw lines from each vertex to the centroid
                    ofDrawLine(path->getVertices()[k], path->getCentroid2D());
                    t = ofMap(k, 0, numVerts, j/step_over, (j+1)/step_over);
                    inner_path.addVertex(glm::mix(path->getVertices()[k], path->getCentroid2D(), t));
                }
            }
            
            ofSetColor(ofColor::magenta, 200);
            inner_path.draw();
            
            ofDrawEllipse(path->getCentroid2D(), 15, 15);
        }
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::draw_world(){
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
            cam.setPosition(.00001, 0, 3);
            break;
        case 'c':
            paths.clear();
            cout << paths.size() << endl;
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
    mouse.y = mouseRay.y;
    mouse.z = 0;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (button == OF_MOUSE_BUTTON_LEFT && paths.size() > 0){
        float dist_threshold = 10;
        int index = paths[paths.size()-1]->getVertices().size()-1;
        ofVec3f vert_prev = ofVec3f(paths[paths.size()-1]->getVertices()[index]);
        auto pt = cam.screenToWorld(glm::vec3 ((float)x,(float)y,0));
        if (vert_prev.squareDistance(pt) > dist_threshold*dist_threshold){
            paths[paths.size()-1]->addVertex(pt.x,pt.y,.1);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == OF_MOUSE_BUTTON_LEFT){ // should check if we're interacting with the gui
        paths.push_back(new ofPolyline());
        // screenToWorld only works in ortho mode like this
        auto pt = cam.screenToWorld(glm::vec3 ((float)x,(float)y,0));
        paths[paths.size()-1]->addVertex(pt.x,pt.y,.1);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (button == OF_MOUSE_BUTTON_LEFT && paths.size() > 0){
        // close the path
        paths[paths.size()-1]->close();
        // resample
        auto smoothed = paths[paths.size()-1]->getResampledByCount(resolution);
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

//--------------------------------------------------------------
void ofApp::listener_none(bool &val){
    if (val){
        offset = false;
        spiral = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_offset(bool &val){
    if (val){
        none = false;
        spiral = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_spiral(bool &val){
    if (val){
        none = false;
        offset = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_resolution(float &val){
    for (auto path : paths){
        // resample
        auto smoothed = path->getResampledByCount(resolution);
        // replace with smoothed vertices
        path->clear();
        path->addVertices(smoothed.getVertices());
        // reclose the path
        path->close();
    }
}
