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
    
    vid_img.load("snapshot.png"); // just fill the vid_img with anything to start
    vid_img.setImageType(OF_IMAGE_COLOR);
    
    base_rect.setWidth(imgWidth);
    base_rect.setHeight(imgHeight);
    
    load_tiles();
    for (auto tex : textures){
        ofRectangle cell = ofRectangle(0, 0, tex->getWidth(), tex->getHeight());
        tile_tgt_colors.push_back(get_avg_col(tex));
    }
    
//    // load textures, store the average brightness of each texture
//    for (int i=0; i<6; i++){
//        textures.push_back(new ofImage());
//        ofLoadImage(*textures[i], "tex_"+ofToString(i)+".jpg");
//        textures[i]->allocate(64, 64, OF_IMAGE_COLOR);
//
//        tgt_brightness.push_back(get_avg_brightness(textures[i]));
//        cout << "brightness of " << i << ": " << tgt_brightness[i] << endl;
//    }
    
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
        tgt_colors[i]->setup("col_"+ofToString(i), colors[i], ofColor(0,0,0), ofColor(255,255,255));
//        panel.add(tgt_colors[i]);
        //tgt_colors[i]->maximize();
        
        Tile_Count tc;
        tile_counts.push_back(tc);
    }
    cout << "tile count size: " << tile_counts.size() << endl;
    
    
    panel.setPosition(10,10);
    
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
        
        if (file.find("VG") != -1){
            cout << "\tFILE FOUND: " << file << endl;
            textures.push_back(new ofImage());
            ofLoadImage(*textures[i], file);
            textures[i]->allocate(120, 120, OF_IMAGE_COLOR);
            i++;
        }
    }
    num_tiles = textures.size();
    cout << endl << "Number of Tiles Loaded: " << num_tiles << endl << endl;
}

//--------------------------------------------------------------
ofColor ofApp::get_avg_col(ofImage * _texture){
    
    int w = _texture->getWidth();
    int h = _texture->getHeight();
    
    int r=0; int g=0; int b=0;
    
    for (int i=0; i<w; i++){
        for(int j=0; j<h; j++){
            ofColor col = _texture->getColor(i, j);
            r += col.r;
            g += col.g;
            b += col.b;
        }
    }
    
    ofColor col = ofColor(r/(w*h), g/(w*h), b/(w*h));
    
//    cout << "average color: " << col << endl;
    
    return col;
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
    
    // pixelate, get closest color tile, and tally the tile count
    draw_cells(x_grid, y_grid);
    
    // draw textures along bottom of screen
    for (int i=0; i<textures.size(); i++){
        int w = textures[i]->getWidth()/4;
        int h = textures[i]->getHeight()/4;
        textures[i]->draw(i*w+(i*5), ofGetHeight()-h);
        
        ofPushStyle();
        ofSetColor(tile_tgt_colors[i]);
        ofDrawRectangle(i*w+(i*5), ofGetHeight()-h-h-10, w+5, h);
        ofPopStyle();
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

            ofColor col = get_avg_col(&cell, &pix);

            // draw the cell with its average color
            ofPushStyle();
            ofFill();
            ofSetColor(col);
            ofDrawRectangle(cell);
            ofPopStyle();

            
            // swap with the closest color
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(base_rect.getWidth(), 0);
            
            // draw texture in correct cell
//            ofImage* tex = textures[get_tgt_brightness(col.getBrightness())];
//            tex->draw(cell);
            

            // draw the cell with its average color
            ofFill();
            
            get_closest_texture(&cell, col)->draw(cell.getX(),cell.getY(),cell.getWidth(), cell.getHeight());
//            ofDrawRectangle(cell);
            
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
int ofApp::get_avg_brightness(ofImage* _texture){
    
    int brightness = 0;
    int count = 0;
    
    for (int i=0; i<_texture->getWidth(); i++){
        for (int j=0; j<_texture->getHeight(); j++){
            brightness += _texture->getColor(i, j).getBrightness();
            count++;
        }
    }

    return brightness/count;
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
    
//    count_tile(_cell, index);
    
    return ofColor(*tgt_colors[index]);
}

//--------------------------------------------------------------
ofImage* ofApp::get_closest_texture(ofRectangle * _cell, ofColor src_col){
    
    float min_dist = FLT_MAX;
    
    ofVec3f src = ofVec3f(src_col.r, src_col.g, src_col.b);
    int i=0;
    int index = -1;
    for (auto tgt_color : tile_tgt_colors){
        
        ofColor tgt_col = tgt_color;
        ofVec3f tgt = ofVec3f(tgt_col.r, tgt_col.g, tgt_col.b);;
        
        float sq_dist = tgt.squareDistance(src);
        if (sq_dist < min_dist){
            min_dist = sq_dist;
            index = i;
        }
        
        i++;
    }
    
//    count_tile(_cell, index);
    
    return textures[index];
}

//--------------------------------------------------------------
int ofApp::get_tgt_brightness(int src_brightness){
    
    int min_diff = INT_MAX;

    int i=0;
    int index = -1;
    for (auto tgt : tgt_brightness){
        
        int diff = abs(tgt-src_brightness);
        
        if (diff < min_diff){
            min_diff = diff;
            index = i;
        }
        
        i++;
    }
    
    return index;
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
