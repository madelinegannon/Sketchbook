#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0,0,0);
    setup_gui();

    // mbp webcam is 640x480 (initalize at this size)
    int imgWidth = x_resize;
    int imgHeight = y_resize;
    
    vidGrabber.setVerbose(true);
    vidGrabber.setup(imgWidth,imgHeight);
    
    img.load("snapshot.png");
    img.setImageType(OF_IMAGE_COLOR);
    
    vid_img.load("snapshot.png"); // just fill the vid_img with anything
    vid_img.setImageType(OF_IMAGE_COLOR);
    
    base_rect.setWidth(imgWidth);
    base_rect.setHeight(imgHeight);
    
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    grid.setName("Grid_Params");
    grid.add(square_grid.set("Square_Grid", true));
    grid.add(x_grid.set("X", 8, 1, 80));
    grid.add(y_grid.set("Y", 24, 1, 60));
    
    image.setName("Image_Params");
    image.add(x_resize.set("X_Resize", 640, 160, 1280));
    image.add(y_resize.set("Y_Resize", 480, 120, 960));
    
    panel.setup();
    panel.setName("Pixelator");
    panel.add(use_webcam.set("Use_Webcam", false));
    panel.add(image);
    panel.add(grid);
    
    // add target colors for tiling
    colors.push_back(col_0);
    colors.push_back(col_1);
    colors.push_back(col_2);
    colors.push_back(col_3);
    colors.push_back(col_4);
    colors.push_back(col_5);
    for (int i=0; i<colors.size(); i++){
        tgt_colors.push_back(new ofxColorSlider());
        panel.add(tgt_colors[i]->setup("col_"+ofToString(i), colors[i], ofColor(0,0,0), ofColor(255,255,255)));
        //tgt_colors[i]->maximize();
        
        Tile_Count tc;
        tile_counts.push_back(tc);
    }
    cout << "tile count size: " << tile_counts.size() << endl;
    
    
    
    panel.setPosition(10,10);
    
}

//------------------------------------------------- -------------
void ofApp::update(){
    if (use_webcam){
        if(!vidGrabber.isInitialized())
            vidGrabber.initGrabber(base_rect.getWidth(), base_rect.getHeight());
        vidGrabber.update();
    
        // the video grabber stream isn't scalable, but ofImage is :)
        // save camera stream as an ofImage so we can stretch & scale it
        vid_img.setFromPixels(vidGrabber.getPixels());
        vid_img.resize(x_resize, y_resize);
        vid_img.update();
    }
    else{
        if(vidGrabber.isInitialized())
            vidGrabber.close();
    }
    
    img.resize(x_resize, y_resize);
    img.update();
    
    base_rect.setWidth(x_resize);
    base_rect.setHeight(y_resize);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(5);
    ofSetColor(255, 0, 255, 120);
    ofDrawRectangle(base_rect);
    ofPopStyle();
    
    if (use_webcam){
//        vidGrabber.draw(0,0);
        vid_img.draw(0,0);  // video grabber stream isn't scalable, but ofImage is :)
    }
    else{
        img.draw(0,0);
    }
  
    // reset the tile counter
    tile_counts.clear();
    for (int i=0; i<colors.size(); i++){
        Tile_Count tc;
        tile_counts.push_back(tc);
    }
    
    // pixelate, get closest color tile, and tally the tile count
    draw_cells(x_grid, y_grid);
    

    for (int i=0; i<tile_counts.size(); i++){
        string s = "Tile " + ofToString(i) + ": color {" + ofToString(colors[i]) + "}\n";
        ofDrawBitmapString(s, 10, ofGetHeight()/2+50 + i*55 );
        ofDrawBitmapStringHighlight(tile_counts[i].to_string(), 20, ofGetHeight()/2 + i*55 +50 + 15);
    }

    panel.draw();
}

//--------------------------------------------------------------
void ofApp::draw_cells(int x_num, int y_num){
    
    float x_spacing = base_rect.getWidth() / x_num;
    float y_spacing = base_rect.getHeight() / y_num;
    
    if (square_grid){
        // recalculate grid_y based on x_spacing
        // ... y_num may not go all the way to bottom
        y_spacing = x_spacing;
        y_num = base_rect.getHeight() / y_spacing;
    }
    
    // reset the tile counter
    for (auto tc : tile_counts)
        tc.clear();

    // get the correct pixel stream
    ofRectangle cell;
    ofPixels pix;
    if (use_webcam){
        pix = vid_img.getPixels();//vidGrabber.getPixels();
    }
    else
        pix = img.getPixels();
    

    for (int i=0; i<x_num; i++){
        for (int j=0; j<y_num; j++){

            // calculate cell size and location
            cell.setPosition(i*x_spacing, j*y_spacing);
            cell.setWidth(x_spacing);
            cell.setHeight(y_spacing);

            // draw the cell with its average color
            ofPushStyle();
            ofFill();
            ofSetColor(get_avg_col(&cell, &pix));
            ofDrawRectangle(cell);
            ofPopStyle();

            
            // swap with the closest color
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(base_rect.getWidth(), 0);
            ofFill();
            ofSetColor(get_closest_color(&cell, get_avg_col(&cell, &pix)));
            ofDrawRectangle(cell);
            ofPopMatrix();
            ofPopStyle();
            
        }
    }
}

//--------------------------------------------------------------
ofColor ofApp::get_avg_col(ofRectangle * _cell, ofPixels * _pix){
    
    int r=0; int g=0; int b=0;
    int count=0;
    
    // change all the pixels in the roi to cyan
    for (int j=0; j<_cell->getWidth()*3; j+=3){
        for (int i=0; i<_cell->getWidth()*3; i+=3){

            // convert 2D coordinates of roi to 1D for pixel array
            int x = _cell->getX()*3;
            int y = _cell->getY()*3 * base_rect.getWidth();

            int index = x + y + j*base_rect.getWidth() + i;
            
            // get the pixel color and store sum of rgb values
            ofColor col;
            if (index > 0 && index < _pix->size())
                col = _pix->getColor(index);
            else
                col = ofColor::white;
            
            r += col.r;
            g += col.g;
            b += col.b;
            
            count++;

        }
    }
    
    ofColor avg_rgb = ofColor(r/count, g/count, b/count);
    
//    cout << "avg rgb: " << r/count << "," << g/count << "," << b/count  << endl;
//    cout << endl;
    
    return avg_rgb;
    
}

//--------------------------------------------------------------
ofColor ofApp::get_closest_color(ofRectangle * _cell, ofColor src_col){
    
    float min_dist = FLT_MAX;
    
    ofVec3f src = ofVec3f(src_col.r, src_col.g, src_col.b);
    int i=0;
    int index = -1;
    for (auto tgt_color : tgt_colors){
        
        ofColor tgt_col = *tgt_color;
        ofVec3f tgt = ofVec3f(tgt_col.r, tgt_col.g, tgt_col.b);;
        
        float sq_dist = tgt.squareDistance(src);
        if (sq_dist < min_dist){
            min_dist = sq_dist;
            index = i;
        }
        
        i++;
    }
    
    count_tile(_cell, index);
    
    return ofColor(*tgt_colors[index]);
}


//--------------------------------------------------------------
void ofApp::count_tile(ofRectangle * _cell, int index){
    
    // tally number of tiles for each color, and record their relative X,Y coordinates
    tile_counts[index].count++;
    
    
    int x_spacing = base_rect.getWidth() / x_grid;
    int y_spacing = base_rect.getHeight() / y_grid;
    
    if (square_grid){
        y_spacing = base_rect.getWidth() / x_grid;
    }
    tile_counts[index].coordinates.push_back(ofVec2f(_cell->getPosition().x/x_spacing, _cell->getPosition().y/y_spacing));
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        case ' ':
            use_webcam = !use_webcam;
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
