#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    img.load("snapshot.png");
    img.allocate(img_width,img_height, OF_IMAGE_COLOR);
    
    make_grid(0, 0, x_grid, img_height/(img_width/x_grid), img_width, img_height);
    
    
    for (int i=0; i<_cells.size(); i++){
        if (_cells[i].avg_brightness > 120 && _cells[i].bounds.getWidth() > 7){
            cout << "Cell: " << i << endl;
            cout << "\t" << ofToString(_cells[i].bounds) << endl;
            cout << "\t" << ofToString(_cells[i].avg_col) << endl;
            cout << "\t" << ofToString(_cells[i].avg_brightness) << endl;
            make_grid(_cells[i].bounds.getX (), _cells[i].bounds.getY(),2,2,_cells[i].bounds.getWidth(), _cells[i].bounds.getHeight());
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

//    img.draw(0, 0);
    
    draw_grid();
}

//--------------------------------------------------------------
void ofApp::make_grid(int x, int y, int x_num, int y_num, int width, int height){
    
    float spacing = width / x_num;
    
    for (int i=0; i<x_num; i++){
        for (int j=0; j<y_num; j++){
            
            Cell cell;
            cell.bounds = ofRectangle(x + i*spacing, y+ j*spacing, spacing, spacing);
            set_avg_color(&cell);
            
            _cells.push_back(cell);
            
//            ofRectangle cell = ofRectangle(i*spacing, j*spacing, spacing, spacing);
//            cells.push_back(cell);
        }
    }
    
}


//--------------------------------------------------------------
void ofApp::draw_grid(){
    
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(3);
    for (auto cell : _cells){
        ofSetColor(cell.avg_brightness);
        ofSetColor(cell.avg_col);
        ofDrawRectangle(cell.bounds);
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::set_avg_color(Cell* cell){
    
    int r=0, g=0, b=0, count=0;
    
    float x = cell->bounds.getX();
    float y = cell->bounds.getY();
    float w = cell->bounds.getWidth();
    float h = cell->bounds.getHeight();
    
    for (int i=x; i<x+w; i++){
        for (int j=y; j<y+h; j++){
            ofColor col = img.getColor(i, j);
            
            r += col.r;
            g += col.g;
            b += col.b;
            
            count++;
        }
    }
    cout << "COlOR: Avg brightness for {" << x << "," << y << "}: " << ofColor(r/count, g/count, b/count).getBrightness() << endl;
    cell->avg_brightness = ofColor(r/count, g/count, b/count).getBrightness();
    
    cell->avg_col = ofColor(r/count, g/count, b/count);
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
