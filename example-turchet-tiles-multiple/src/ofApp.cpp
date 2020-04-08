#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    setup_gui();
    
    // intialize tiles
    setup_tiles();
    
    // intialize rotations and tile types
    initialize_grid();
    
    curr_time = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::initialize_grid(){
    rotations.clear();
    tile_ids.clear();
    
    for (int i=0; i<x_grid; i++){
        vector<int> rots;
        vector<int> ids;
        for (int j=0; j<y_grid; j++){
            rots.push_back(int(ofRandom(0,4)) * 90);
            ids.push_back(int(ofRandom(0,base_tiles.size())));
        }
        rotations.push_back(rots);
        tile_ids.push_back(ids);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (ofGetElapsedTimef() - curr_time > wait_time && !pause){
        update_pattern();
        curr_time = ofGetElapsedTimef();
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float w=ofGetWidth()/x_grid;
    float h=ofGetWidth()/x_grid;
    
    for (int i=0; i<x_grid; i++){
        for (int j=0; j<y_grid; j++){
            
            // do the random rotation
            ofPushMatrix();
            ofTranslate(i*w, j*w);
            ofTranslate(w/2, h/2);
            ofRotateDeg(rotations[i][j]);
            ofTranslate(-w/2, -h/2);
            
            draw_tile(tile_ids[i][j]);
            
            ofPopMatrix();
        }
    }
    
    panel.draw();
}

//--------------------------------------------------------------
void ofApp::update_pattern(){
    
    for (int i=0; i<x_grid; i++){
        for (int j=0; j<y_grid; j++){
            
            // pick a random rotation
            rotations[i][j] = int(ofRandom(0,4))*90;
            
            // pick a random rotation
            tile_ids[i][j] = int(ofRandom(0,base_tiles.size()));

        }
    }
}

//--------------------------------------------------------------
void ofApp::draw_tile(int id){
    
    ofColor foreground = ofColor::black;
    ofColor background = ofColor::white;
    
    float w=ofGetWidth()/x_grid;
    float h=ofGetWidth()/x_grid;
    float d = w/3;
    
    ofPushStyle();
    
    // draw rectangle
    ofSetColor(background);
    ofFill();
    ofDrawRectangle(0,0,w,h);
    
    // draw end cap circles
    ofSetColor(0);
    ofDrawEllipse(w/2, 0, d, d);
    ofDrawEllipse(w, h/2, d, d);
    ofDrawEllipse(w/2, h, d, d);
    ofDrawEllipse(0, h/2, d, d);
    
    // draw pattern
    for (auto path : base_tiles[id].paths)
        path.draw();
    
    // draw the corner circles
    ofSetColor(background);
    ofDrawEllipse(0, 0, 2*d, 2*d);
    ofDrawEllipse(w, 0, 2*d, 2*d);
    ofDrawEllipse(w, h, 2*d, 2*d);
    ofDrawEllipse(0, h, 2*d, 2*d);
    
    // draw the debug view
    if (debug_view){
        base_tiles[id].spine.draw();
        ofSetColor(base_tiles[id].spine.getStrokeColor());
        ofNoFill();
        ofDrawRectangle(base_tiles[id].cell);
    }
    
    ofPopStyle();
    
}


//--------------------------------------------------------------
void ofApp::setup_gui(){
    panel.setup();
    panel.setName("Params");
    panel.setPosition(10, 10);

    panel.add(debug_view.set("Debug_View", true));
    panel.add(pause.set("Pause", false));
    panel.add(x_grid.set("x_grid", 15, 2, 100));
    panel.add(y_grid.set("y_grid", 12, 2, 100));
    
    tile_params.setName("Tile_Params");
    tile_params.add(tile_id_0.set("Tile_0: /", true));
    tile_params.add(tile_0_weight.set("Tile_0 Weight", 1, 1, 5));
    tile_params.add(tile_id_1.set("Tile_1: :(", true));
    tile_params.add(tile_1_weight.set("Tile_1 Weight", 1, 1, 5));
    tile_params.add(tile_id_2.set("Tile_2: 3|", true));
    tile_params.add(tile_2_weight.set("Tile_2 Weight", 1, 1, 5));
    tile_params.add(tile_id_3.set("Tile_3: 3-", true));
    tile_params.add(tile_3_weight.set("Tile_3 Weight", 1, 1, 5));
    
    panel.add(tile_params);
    
    x_grid.addListener(this, &ofApp::grid_listener);
    y_grid.addListener(this, &ofApp::grid_listener);
    
    tile_id_0.addListener(this, &ofApp::tile_id_listener);
    tile_id_1.addListener(this, &ofApp::tile_id_listener);
    tile_id_2.addListener(this, &ofApp::tile_id_listener);
    tile_id_3.addListener(this, &ofApp::tile_id_listener);
    
    tile_0_weight.addListener(this, &ofApp::tile_weight_listener);
    tile_1_weight.addListener(this, &ofApp::tile_weight_listener);
    tile_2_weight.addListener(this, &ofApp::tile_weight_listener);
    tile_3_weight.addListener(this, &ofApp::tile_weight_listener);

}

//--------------------------------------------------------------
void ofApp::grid_listener(int &val){
    setup_tiles();
    initialize_grid();
}

//--------------------------------------------------------------
void ofApp::tile_id_listener(bool &val){
    setup_tiles();
    initialize_grid();
}

//--------------------------------------------------------------
void ofApp::tile_weight_listener(int &val){
    setup_tiles();
    initialize_grid();
}

//--------------------------------------------------------------
void ofApp::setup_tiles(){
    
    base_tiles.clear();
    
    float width=ofGetWidth()/x_grid;
    float height=ofGetWidth()/x_grid;
    
    // Tile 0: '/' or '\'
    Tile tile_0;
    tile_0.cell = ofRectangle(0,0,width,height);
    
    ofPath arc_0, arc_1;
    arc_0.moveTo(2*width/3, 0);
    arc_0.lineTo(1*width/3, 0);
    arc_0.arc(0,0, width/3, width/3, 0, 90);
    arc_0.lineTo(0, 2*width/3);
    arc_0.arcNegative(0,0, 2*width/3, 2*width/3, 90,0);
    arc_0.close();
    
    arc_0.setFillColor(0);
    arc_0.setFilled(true);
    
    arc_1.moveTo(width, 1*width/3);
    arc_1.lineTo(width, 2*width/3);
    arc_1.arcNegative(width,width, 1*width/3, 1*width/3, -90, 180);
    arc_1.lineTo(1*width/3, width);
    arc_1.arc(width,width, 2*width/3, 2*width/3, 180, -90);
    arc_1.close();
    
    arc_1.setFillColor(0);
    arc_1.setFilled(true);

    tile_0.paths.push_back(arc_0);
    tile_0.paths.push_back(arc_1);
    
    tile_0.spine.moveTo(width/2, 0);
    tile_0.spine.arc(0,0, width/2, width/2, 0, 90);
    tile_0.spine.moveTo(width, width/2);
    tile_0.spine.arcNegative(width,width, width/2, width/2,-90,180);
    
    tile_0.spine.setStrokeColor(ofColor::orange);
    tile_0.spine.setFilled(false);
    tile_0.spine.setStrokeWidth(1);
    
    
    // Tile 1: ':(' or '):'
    Tile tile_1;
    tile_1.cell = ofRectangle(0,0,width,height);

    ofPath arc_2;
    arc_2.moveTo(2*width/3, 0);
    arc_2.lineTo(1*width/3, 0);
    arc_2.arc(0, 0, width/3, width/3, 0, 90);
    arc_2.lineTo(0, 2*width/3);
    arc_2.arcNegative(0, 0, 2*width/3, 2*width/3, 90,0);
    arc_2.close();
    
    arc_2.setFillColor(0);
    arc_2.setFilled(true);
    
    tile_1.paths.push_back(arc_2);
    
    tile_1.spine.moveTo(width/2, 0);
    tile_1.spine.arc(0, 0, width/2, width/2, 0, 90);
    tile_1.spine.setStrokeColor(ofColor::aquamarine);
    tile_1.spine.setFilled(false);
    tile_1.spine.setStrokeWidth(1);
    
    
    // Tile 2: '3|'
    Tile tile_2;
    tile_2.cell = ofRectangle(0,0,width,height);

    ofPath arc_3, arc_4, bar_0;
    arc_3.moveTo(2*width/3, 0);
    arc_3.lineTo(1*width/3, 0);
    arc_3.arc(0,0, width/3, width/3, 0, 90);
    arc_3.lineTo(0, 2*width/3);
    arc_3.arcNegative(0,0, 2*width/3, 2*width/3, 90,0);
    arc_3.close();
    
    arc_4.moveTo(2*width/3, width);
    arc_4.lineTo(1*width/3, width);
    arc_4.arcNegative(0,width, 1*width/3, 1*width/3, 0, -90);
    arc_4.lineTo(0, 1*width/3);
    arc_4.arc(0,width, 2*width/3, 2*width/3, -90, 0);
    arc_4.close();
    
    // add '|' section
    bar_0.moveTo(2*width/3, width);
    bar_0.lineTo(2*width/3, 0);
    bar_0.lineTo(1*width/3, 0);
    bar_0.lineTo(1*width/3, width);
    bar_0.lineTo(2*width/3, width);
    bar_0.close();
    
    arc_3.setFillColor(0);
    arc_3.setFilled(true);

    arc_4.setFillColor(0);
    arc_4.setFilled(true);
    
    bar_0.setFillColor(0);
    bar_0.setFilled(true);
    
    tile_2.paths.push_back(arc_3);
    tile_2.paths.push_back(arc_4);
    tile_2.paths.push_back(bar_0);
    
    tile_2.spine.moveTo(width/2, 0);
    tile_2.spine.arc(0,0, width/2, width/2, 0, 90);
    tile_2.spine.arc(0,width, width/2, width/2,-90,0);
    tile_2.spine.lineTo(width/2, 0);
    
    tile_2.spine.setStrokeColor(ofColor::red);
    tile_2.spine.setFilled(false);
    tile_2.spine.setStrokeWidth(1);
    
    
    // Tile 2: '3–'
    Tile tile_3;
    tile_3.cell = ofRectangle(0,0,width,height);

    ofPath arc_5, arc_6, bar_1;
    arc_5.moveTo(2*width/3, 0);
    arc_5.lineTo(1*width/3, 0);
    arc_5.arc(0,0, width/3, width/3, 0, 90);
    arc_5.lineTo(0, 2*width/3);
    arc_5.arcNegative(0,0, 2*width/3, 2*width/3, 90,0);
    arc_5.close();
    
    arc_6.moveTo(2*width/3, width);
    arc_6.lineTo(1*width/3, width);
    arc_6.arcNegative(0,width, 1*width/3, 1*width/3, 0, -90);
    arc_6.lineTo(0, 1*width/3);
    arc_6.arc(0,width, 2*width/3, 2*width/3, -90, 0);
    arc_6.close();
    
    // add '-' section
    bar_1.moveTo(0, 1*width/3);
    bar_1.lineTo(width, 1*width/3);
    bar_1.lineTo(width, 2*width/3);
    bar_1.lineTo(0, 2*width/3);
    bar_1.lineTo(0, 1*width/3);
    bar_1.close();
    
    arc_5.setFillColor(0);
    arc_5.setFilled(true);

    arc_6.setFillColor(0);
    arc_6.setFilled(true);
    
    bar_1.setFillColor(0);
    bar_1.setFilled(true);
    
    tile_3.paths.push_back(arc_5);
    tile_3.paths.push_back(arc_6);
    tile_3.paths.push_back(bar_1);
    
    tile_3.spine.moveTo(width/2, 0);
    tile_3.spine.arc(0,0, width/2, width/2, 0, 90);
    tile_3.spine.arc(0,width, width/2, width/2,-90,0);
    tile_3.spine.moveTo(0, width/2);
    tile_3.spine.lineTo(width, width/2);
    
    tile_3.spine.setStrokeColor(ofColor::purple);
    tile_3.spine.setFilled(false);
    tile_3.spine.setStrokeWidth(1);
    
    if (tile_id_0){
        for (int i=0; i<tile_0_weight; i++)
            base_tiles.push_back(tile_0);
    }
    if (tile_id_1){
        for (int i=0; i<tile_1_weight; i++)
            base_tiles.push_back(tile_1);
    }
    if (tile_id_2){
            for (int i=0; i<tile_2_weight; i++)
                base_tiles.push_back(tile_2);
        }
    if (tile_id_3){
        for (int i=0; i<tile_3_weight; i++)
            base_tiles.push_back(tile_3);
    }
    
    // if you want a higher probability of a specific tile
    // getting picked, add it again to the list:
//    base_tiles.push_back(tile_1);
//    base_tiles.push_back(tile_1);
//    base_tiles.push_back(tile_1);
//    base_tiles.push_back(tile_1);
//    base_tiles.push_back(tile_2);
//    base_tiles.push_back(tile_2);
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
