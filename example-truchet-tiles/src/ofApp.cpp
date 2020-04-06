#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    load_tiles();
    
    for (int i=0; i<x_grid; i++){
        vector<int> random_turns;
        vector<int> zero_turns;
        for (int j=0; j<y_grid; j++){
            random_turns.push_back(int(ofRandom(0, 4)));
            zero_turns.push_back(0);
        }
        // start at 0 for current turn angles
        turns_curr.push_back(zero_turns);
        // add random angle to next turns
        turns_next.push_back(random_turns);
    }
    
    prev_turn_time = ofGetElapsedTimef();
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(ofColor::white, ofColor::lightGrey, OF_GRADIENT_CIRCULAR);
    

    // use a timer to update target angles
    if (ofGetElapsedTimef() - prev_turn_time > turn_time + wait_time){
            randomize_angles();
            prev_turn_time = ofGetElapsedTimef();
    }
    

    float x_spacing = ofGetWidth()/(x_grid-1);
    float y_spacing = x_spacing; // keep square aspect ratio
    
    float x_offset = -x_spacing/2;
    float y_offset = -y_spacing/2;
    
    for (int i=0; i<x_grid; i++){
        for (int j=0; j<y_grid; j++){
            
            ofImage img;
            img.setFromPixels(tiles[1]->getPixels());
           
            ofPushMatrix();
            ofTranslate(i*x_spacing,j*y_spacing);
            
            // animate the turn between target angles based on time
            float t = ofxeasing::map_clamp(ofGetElapsedTimef()-prev_turn_time, 0, turn_time, 0, 1, ofxeasing::cubic::easeInOut);
            
            // to get the angle, lerp between current and next turns
            float angle = 0;
            angle = ofLerp(90*turns_curr[i][j], 90*turns_next[i][j], t);
            
            // do the rotation
            ofRotateDeg(angle);
            img.draw(x_offset, y_offset, x_spacing, y_spacing);
            ofPopMatrix();
        }
    }
        
    
}


//--------------------------------------------------------------
void ofApp::load_tiles(){
    
    // load images of vitreous glass tiles
    std::string path = ofToDataPath("tiles");
    
    int i=0;
    for (const auto & entry : filesystem::directory_iterator(path)){
        
        //            string file = entry.path().string().substr();
        //            cout << entry.path() << endl;
        
        string front = "../../../data/";
        int start = front.size();
        int end = entry.path().string().size();
        string file = entry.path().string().substr(start, end);
        
        if (file.find("trechet_") != -1){
            cout << "\tFILE FOUND: " << file << endl;
            tiles.push_back(new ofImage());
            ofLoadImage(*tiles[i], file);
            tiles[i]->allocate(64, 64, OF_IMAGE_COLOR_ALPHA);
            i++;
        }
    }
    num_tiles = tiles.size();
    cout << endl << "Number of Tiles Loaded: " << num_tiles << endl << endl;
}

//--------------------------------------------------------------
void ofApp::randomize_angles(){
    for (int i=0; i<x_grid; i++){
        for (int j=0; j<y_grid; j++){
            turns_curr[i][j] = turns_next[i][j];
            turns_next[i][j] = (int(ofRandom(0, 4)));
        }
    }
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
