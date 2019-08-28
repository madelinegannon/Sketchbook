#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(200);
    
    ofPushStyle();
    ofSetLineWidth(3);
    ofSetColor(ofColor::magenta, 100);
    for (auto path : paths)
        path->draw();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::exit(){
    save_paths();
}

//--------------------------------------------------------------
void ofApp::save_paths(string filename)
{
    // Create a simple csv for each path
    ofFile file;
    file.open(filename, ofFile::WriteOnly);
    int i=0;
    for (auto path : paths){
        int j=0;
        for (auto &v : path->getVertices()){
            file << v.x << "," << v.y << "," << v.z;
            if (j != path->getVertices().size()-1)
                file << ",";
            j++;
        }
        if (i != paths.size()-1)
            file << "\n";
        i++;
    }
    file.close();
    if (paths.size() == 0) ofLogNotice() << "EMPTY PATH LIST saved to '" << filename <<"'";
    else ofLogNotice() << "Paths Saved to '" << filename <<"'";
}

//--------------------------------------------------------------
void ofApp::load_paths(string filename)
{
    ofLogNotice() << "Loading file: '" << filename <<"'";
    auto lines = ofSplitString(ofBufferFromFile(filename).getText(), "\n");
    if (lines[0].size() == 0){
        ofLogError() << "ERROR: No file exists at '/bin/data/" << filename <<"'";
    }
    else{
        paths.clear();
        for (auto line : lines){
            paths.push_back(new ofPolyline());
            auto verts = ofSplitString(ofToString(line), ",");
            for (int i=0; i<=verts.size()-3; i+=3){
                float x = atof(verts[i].c_str());
                float y = atof(verts[i+1].c_str());
                float z = atof(verts[i+2].c_str());
                paths[paths.size()-1]->addVertex(x,y,z);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch(key){
        case 'c':
            paths.clear();
            break;
        case 's':
            save_paths();
            break;
        case 'l':
            load_paths();
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

    float dist_threshold = 10;
    int index = paths[paths.size()-1]->getVertices().size()-1;
    ofVec3f* vert_prev = &paths[paths.size()-1]->getVertices()[index];
    if (vert_prev->squareDistance(ofVec3f(x,y,0)) > dist_threshold*dist_threshold){
        paths[paths.size()-1]->addVertex(x,y,0);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    paths.push_back(new ofPolyline());
    paths[paths.size()-1]->addVertex(x,y,0);
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
