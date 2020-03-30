#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    img_src.load("snapshot.png");
    img_src.setImageType(OF_IMAGE_COLOR);
    
    img.setFromPixels(img_src.getPixels());
    img.setImageType(OF_IMAGE_COLOR);

    setup_gui();
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    grid.setName("Grid_Params");
    grid.add(square_grid.set("Square_Grid", true));
    grid.add(cell_width.set("Cell_Width", 160, 80, 640));
    grid.add(x_grid.set("X", 0, 0, 8));
    grid.add(y_grid.set("Y", 0, 0, 8));
    
    panel.setup();
    panel.setName("Pixel_Editor");
    panel.add(use_webcam.set("Use_Webcam", false));
    panel.add(grid);
    
    panel.setPosition(10,10);
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // draw the source image
    img_src.draw(0,0);
    
    // highlight the roi
    ofRectangle cell;
    cell.setWidth(cell_width);//imgWidth/x_grid);
    cell.setHeight(cell_width);
    cell.setX(cell.getWidth() * x_grid);
    cell.setY(cell.getWidth() * y_grid);
    
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(3);
    ofSetColor(ofColor::magenta, 120);
    ofDrawRectangle(cell);
    ofPopStyle();
    
    
    // modify the copy image
    
    // reset the pixels
    img.clear();
    img.setFromPixels(img_src.getPixels());
    
    // change all the pixels in the roi to cyan
    // ... image type is RGB, so we advance through the pixel array by 3
    // ... if it were RGBA, we'd advance by 4 ... and grayscale by 1
    for (int j=0; j<cell.getWidth()*3; j+=3){
        for (int i=0; i<cell.getWidth()*3; i+=3){
            
            // convert 2D coordinates of roi to 1D for pixel array
            int x = cell.getX()*3;
            int y = cell.getY()*3 * imgWidth;
            
            int index = x + y + j*imgWidth + i;
            
            img.getPixels()[index]     = 0;      // R
            img.getPixels()[index + 1] = 255;    // G
            img.getPixels()[index + 2] = 255;    // B
        }
    }
    // must call update after modifying pixel array
    img.update();
    
    img.draw(imgWidth,0);
    
    panel.draw();
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
