#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setup_gui();
    create_grid(rows, cols, cell_size);
    create_vector_grid(rows, cols, cell_size);
    
    bounds.width = (cols * cell_size);
    bounds.height = (rows * cell_size);
//    create_flow_field(bounds, num_lines);
    
    if (draw_mode == Output::PIXELS) ofSetBackgroundAuto(false);
    
    ofBackground(255);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    update_vector_grid();
    
    // this is all particle stuff that should get cleaned up
    
    int i=0;
    for (auto & particle : particles){
        // Wrap the particle around (should always be inside bounds)
        if (particle.position.x > bounds.getWidth()) particle.position.x = cell_size/2;
        if (particle.position.x < 0) particle.position.x = bounds.getWidth() - cell_size/2;
        if (particle.position.y > bounds.getHeight()) particle.position.y = cell_size/2;
        if (particle.position.y < 0) particle.position.y = bounds.getHeight() - cell_size/2;

        // check if we should stop the particle
        if (!bounds.inside(particle.position)){
            particle.kill();
        }
        else if (particle.is_alive()){
            // get the closest vector
            int x = int(particle.position.x / cell_size);
            int y = int(particle.position.y / cell_size);
            int index = y + x * rows;
            // apply force and update
            if (index < cells.size())// && bounds.inside(particle.position))
                particle.apply_force(vectors[index] * speed_scalar.get());
            particle.update();
            // add position to polyline
            if (lines[i].size() == 0 ||
                 glm::distance2(lines[i].getVertices()[lines[i].size()-1], particle.position) > 5*5){
                lines[i].addVertex(particle.position);
            }
            // delete old positions from polyline
            if (lines[i].size() > max_length){
                lines[i].getVertices().erase(lines[i].getVertices().begin());
            }
        }
        i++;
    }
    
    if (!pause_field) noise_z += .001;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if (draw_mode != Output::PIXELS){
        ofBackground(255);
        if (show_grid) draw_grid();
        if (show_vectors) draw_vector_grid();
        if (show_gui){
            panel.draw();
            ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), ofGetWidth()-60, 15);
        }
    }
    
    if (show_particles) draw_flow_field();
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    
    int _rows, _cols, _cell_size, _num_lines;
    float _noise_scalar, _noise_speed;
    if (res_mode == Resolution::HIGH){
        _rows = 140;
        _cols = 200;
        _cell_size = 7;
        _num_lines = 500;
        _noise_scalar = .03;
        _noise_speed = .01;
    }
    else if (res_mode == Resolution::LOW){
        _rows = 60;
        _cols = 80;
        _cell_size = 12;
        _num_lines = 250;
        _noise_scalar = .03;
        _noise_speed = .01;
    }
    else if (res_mode == Resolution::ULTRA_LOW){
        _rows = 10;
        _cols = 10;
        _cell_size = 60;
        _num_lines = 250;
        _noise_scalar = .05;
        _noise_speed = .01;
    }
    
    params_grid.setName("Grid Params");
    params_grid.add(show_grid.set("Show_Grid", true));
    params_grid.add(ultra_low_res.set("Ultra_Low_Res", true));
    params_grid.add(low_res.set("Low_Res", false));
    params_grid.add(high_res.set("High_Res", false));
    params_grid.add(rows.set("Rows", _rows, 1, 1000));
    params_grid.add(cols.set("Columns", _cols, 1, 1000));
    params_grid.add(cell_size.set("Cell Size", _cell_size, 1, 100));
    
    ultra_low_res.addListener(this, &ofApp::on_update_resolution_ultra_low);
    low_res.addListener(this, &ofApp::on_update_resolution_low);
    high_res.addListener(this, &ofApp::on_update_resolution_high);
    rows.addListener(this, &ofApp::on_update_grid);
    cols.addListener(this, &ofApp::on_update_grid);
    cell_size.addListener(this, &ofApp::on_update_grid);
    
    params_field.setName("Field Params");
    params_field.add(show_vectors.set("Show_Vectors", true));
    params_field.add(pause_field.set("Pause_Field", false));
    params_field.add(noise_scalar.set("Noise_Scalar", _noise_scalar, 0, 1));
    params_field.add(noise_speed.set("Noise_Speed", _noise_speed, 0, 5));
        
    params_particles.setName("Particle Params");
    params_particles.add(show_particles.set("Show_Particles", true));
    params_particles.add(num_lines.set("Num_Lines", _num_lines, 0, 1000));
    params_particles.add(max_length.set("Max_Length", 30, 0, 1000));
    params_particles.add(speed_scalar.set("Speed_Scalar", .25, 0, 2));
    params_particles.add(max_speed.set("Max_Speed", 3.5, 0, 10));
    max_speed.addListener(this, &ofApp::on_update_particles);
    
    panel.setup();
    panel.setName("Flow Fields");
    panel.add(params_grid);
    panel.add(params_field);
    panel.add(params_particles);
    panel.setPosition(15, 15);
}

//--------------------------------------------------------------
void ofApp::create_grid(int rows, int cols, float cell_size){
    cells.clear();
    for (int x=0; x<cols; x++){
        for (int y=0; y<rows; y++){
            cells.push_back(ofRectangle(x*cell_size, y*cell_size, cell_size, cell_size));
        }
    }

    bounds.width = (cols * cell_size);
    bounds.height = (rows * cell_size);
}

//--------------------------------------------------------------
void ofApp::create_flow_field(ofRectangle bounds, int num_particles){
    particles.clear();
    lines.clear();
    for (int i=0; i<num_particles; i++){
        particles.push_back(Particle(i, glm::vec3(ofRandom(bounds.width), ofRandom(bounds.height), 0)));
        particles[i].max_speed = max_speed.get();
        lines.push_back(ofPolyline());
    }
}

//--------------------------------------------------------------
void ofApp::draw_flow_field(){
    float offset_x = (ofGetWidth() - (cols * cell_size)) / 2;
    float offset_y = (ofGetHeight() - (rows * cell_size)) / 2;

    ofPushMatrix();
    ofTranslate(offset_x, offset_y);
    ofPushStyle();
    
    if (draw_mode == Output::PIXELS){
        ofFill();
        ofSetColor(ofNoise(ofGetFrameNum() / 100.0)*255, 0, ofNoise(ofGetFrameNum() / 300.0)*255, 10);
        for (auto particle : particles) ofDrawEllipse(particle.position, 3, 3);
    }
    else if (draw_mode == Output::POLYLINES){
        // manually draw each polyline, so we skip drawing the
        // line between wrapped points
        ofNoFill();
        ofSetColor(0, 50);
        for (auto line : lines){
            int i=1;
            for (auto v : line.getVertices()){
                if (i < line.getVertices().size()){
                    // filter by distance to neighbor
                    auto v1 = line.getVertices()[i];
                    if (glm::distance2(v, v1) < cell_size*cell_size)
                        ofDrawLine(v, v1);
                }
            i++;
            }
        }
    }
    else if (draw_mode == Output::DEBUG){
        int i=0;
        for (auto particle : particles){
            particle.draw();
            // show connection to the closest vector
            int x = int(particle.position.x / cell_size);
            int y = int(particle.position.y / cell_size);
            int index = y + x * rows;
            ofSetColor(255,0,0);
            if (index < cells.size() && bounds.inside(particle.position)){
                ofDrawLine(particle.position, cells[index].getCenter());
            }
        }
        i++;
    }
    ofPopMatrix();
    ofPopStyle();
}


//--------------------------------------------------------------
void ofApp::draw_grid(){

    float offset_x = (ofGetWidth() - (cols * cell_size)) / 2;
    float offset_y = (ofGetHeight() - (rows * cell_size)) / 2;
    
    ofPushMatrix();
    ofTranslate(offset_x, offset_y);
    
    ofPushStyle();
    ofFill();
    ofSetColor(255, 100);
    ofSetLineWidth(3);
    
    int i=0;
    for (auto cell : cells){
        int x = i / rows;
        int y = i % rows;

        ofSetColor(ofNoise(x * noise_scalar, y * noise_scalar, noise_z + noise_z * noise_speed) * 255);
        ofDrawRectangle(cell);
        
        // DEBUGGING:
//        cout << "i: " << i << "\t {x,y}: {" << i_x << ", " << i_y << "}" << endl;
//        int index = y + x * cols;
//        ofSetColor(ofColor::magenta);
//        string coord = "{" + ofToString(x) + "," + ofToString(y) + "}";
//        ofDrawBitmapString(coord, cell.getCenter());
        i++;
    }
    ofPopStyle();
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::on_update_resolution_ultra_low(bool &val){
    
    if (val){
        int _rows, _cols, _cell_size, _num_lines;
        float _noise_scalar, _noise_speed;
        _rows = 10;
        _cols = 10;
        _cell_size = 60;
        _num_lines = 250;
        _noise_scalar = .05;
        _noise_speed = .01;
        
        rows.set(_rows);
        cols.set(_cols);
        cell_size.set(_cell_size);
        num_lines.set(_num_lines);
        noise_scalar.set(_noise_scalar);
        noise_speed.set(_noise_speed);
        
        res_mode = Resolution::ULTRA_LOW;
        low_res = false;
        high_res = false;
    }
}

//--------------------------------------------------------------
void ofApp::on_update_resolution_low(bool &val){
    
    if (val){
        int _rows, _cols, _cell_size, _num_lines;
        float _noise_scalar, _noise_speed;
        _rows = 60;
        _cols = 80;
        _cell_size = 12;
        _num_lines = 250;
        _noise_scalar = .03;
        _noise_speed = .01;
        
        rows.set(_rows);
        cols.set(_cols);
        cell_size.set(_cell_size);
        num_lines.set(_num_lines);
        noise_scalar.set(_noise_scalar);
        noise_speed.set(_noise_speed);
        
        res_mode = Resolution::LOW;
        ultra_low_res = false;
        high_res = false;
    }
}

//--------------------------------------------------------------
void ofApp::on_update_resolution_high(bool &val){
    
    if (val){
        int _rows, _cols, _cell_size, _num_lines;
        float _noise_scalar, _noise_speed;
        _rows = 140;
        _cols = 200;
        _cell_size = 7;
        _num_lines = 500;
        _noise_scalar = .03;
        _noise_speed = .01;
        
        rows.set(_rows);
        cols.set(_cols);
        cell_size.set(_cell_size);
        num_lines.set(_num_lines);
        noise_scalar.set(_noise_scalar);
        noise_speed.set(_noise_speed);
        
        res_mode = Resolution::HIGH;
        ultra_low_res = false;
        low_res = false;
    }
}

//--------------------------------------------------------------
void ofApp::on_update_grid(int &val){
    create_grid(rows, cols, cell_size);
    create_vector_grid(rows, cols, cell_size);
}

//--------------------------------------------------------------
void ofApp::on_update_particles(float &val){
    for (auto &particle : particles){
        particle.max_speed = max_speed;
    }
}

//--------------------------------------------------------------
void ofApp::create_vector_grid(int rows, int cols, float cell_size){
    vectors.clear();
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            vectors.push_back(glm::vec3(1,0,0));
        }
    }
}

//--------------------------------------------------------------
void ofApp::update_vector_grid(){
    int i=0;
    for (auto & vector : vectors){
        int x = i / rows;
        int y = i % rows;

        float angle = ofNoise(x * noise_scalar, y * noise_scalar, noise_z + noise_z * noise_speed) * 2 * PI;
        // This rotates from the current angle
        // float rotated_x = vectors[i].x * cos(angle) - vectors[i].y * sin(angle);
        // float rotated_y = vectors[i].x * sin(angle) + vectors[i].y * cos(angle);
        // But when animating we need to rotate from 0º each time: auto v = glm::vec3(1,0,0);
        float angle_x = cos(angle);
        float angle_y = sin(angle);
        vectors[i] = glm::vec3(angle_x, angle_y, 0);
        
        i++;
    }
}

//--------------------------------------------------------------
void ofApp::draw_vector_grid(){
    float offset_x = (ofGetWidth() - (cols * cell_size)) / 2;
    float offset_y = (ofGetHeight() - (rows * cell_size)) / 2;
    
    ofPushMatrix();
    ofTranslate(offset_x, offset_y);
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 0, 255, 100);
    for (int i=0; i<vectors.size(); i++){
        auto scaled = vectors[i] * (cell_size/1.1);
        auto p = cells[i].getCenter() + scaled;
        ofDrawLine(cells[i].getCenter(), p);

        // turn this off for better performance
        float w = cell_size*.15;
        ofDrawEllipse(p.x,p.y,w,w);
    }
    ofPopStyle();
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'h':
        case 'H':
            show_gui = !show_gui;
            break;
        case 'f':
        case 'F':
            ofToggleFullscreen();
            break;
        case 'g':
        case 'G':
            show_grid = !show_grid;
            break;
        case 'p':
        case 'P':
            pause_field = !pause_field;
            break;
        case ' ':
            create_flow_field(bounds, num_lines);
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
