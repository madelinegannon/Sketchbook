#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    setup_gui();
    
    // allocate texture
    fbo_texture.allocate(100, 100, GL_RGBA);
    fbo_texture.begin();
    ofClear(255,255,255, 0);
    fbo_texture.end();
}

//--------------------------------------------------------------
void ofApp::update(){
    update_texture();
    
    if (linestrips.size() > 0 && linestrips[linestrips.size()-1].get_num_points() > 0){
        for (auto &ls : linestrips){
            ls.update();
        }        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushStyle();
    
    for (auto &ls : linestrips)
        ls.draw();
    
    ofPopStyle();

    if (show_texture){
        int x = panel.getPosition().x + panel.getWidth() + 10;
        int y = panel.getPosition().y;
        draw_texture(x, y);
    }
    
    if (show_gui){
        panel.draw();
    }
}

//--------------------------------------------------------------
void ofApp::draw_texture(int x, int y){
    fbo_texture.draw(x,y);
}

//--------------------------------------------------------------
void ofApp::update_texture(){

    fbo_texture.begin();
    int w = fbo_texture.getWidth();
    int h = fbo_texture.getHeight();
    
    ofPushStyle();
    
    // draw the pixel swatch
    ofSetColor(255);
    ofFill();
    ofDrawRectangle(0, 0, w, h);
    if (use_texture){
        ofSetColor(0);
        ofSetLineWidth(line_width);
        ofDrawLine(0,0+(line_width/2), w,0+(line_width/2));
        ofDrawLine(0,h-(line_width/2), w,h-(line_width/2));
        
        if (do_stripes){
            ofDrawLine(0,1*h/3, w,1*h/3);
            ofDrawLine(0,2*h/3, w,2*h/3);
        }
    }
    
    ofPopStyle();
    fbo_texture.end();
    
    for (auto &ls : linestrips)
        ls.set_texture(fbo_texture.getTexture());
}

//--------------------------------------------------------------
void ofApp::setup_gui(){
    params.setName("Params");
    params.add(show_debug.set("Show_Debug", true));
    params.add(show_mesh_faces.set("Show_Mesh_Faces", true));
    params.add(show_wireframe.set("Show_Wireframe",false));
    params.add(mesh_width.set("Mesh_Width", 50, 1,500));
    params.add(resample_dist.set("Resample_Dist", 15, 1,100));
    
    
    params_spotlight.setName("Params_Spotlight");
    params_spotlight.add(show_spotlight.set("Show_Spotlight", false));
    
    params_pulse.setName("Params_Pulse");
    params_pulse.add(show_pulse.set("Show_Pulse", true));
    params_pulse.add(do_wrap.set("Wrap_Pulse", true));
    params_pulse.add(look_ahead.set("Look_Ahead", 1000, 0, 5000));
    params_pulse.add(col_start.set("Start_Color", ofVec4f(1,1,1,1), ofVec4f(0,0,0,0), ofVec4f(1,1,1,1)));
    params_pulse.add(col_end.set("End_Color", ofVec4f(1,1,1,1), ofVec4f(0,0,0,0), ofVec4f(1,1,1,1)));
    
    params_texture.setName("Params_Texture");
    params_texture.add(use_texture.set("Use_Texture", true));
    params_texture.add(show_texture.set("Show_Texture", true));
    params_texture.add(line_width.set("Line_Width", 5, 0, 50));
    params_texture.add(do_stripes.set("Striped_Texture", false));
    
    setup_listeners();
    

    panel.setup(params);
    panel.add(params_spotlight);
    panel.add(params_pulse);
    panel.add(params_texture);
    panel.setPosition(10,10);
    
    
    ofSetBackgroundColor(60);
    ofSetCircleResolution(180);
    
}

//--------------------------------------------------------------
void ofApp::setup_listeners(){
    show_debug.addListener(this, &ofApp::listener_show_debug);
    mesh_width.addListener(this, &ofApp::listener_mesh_width);
    resample_dist.addListener(this, &ofApp::listener_resample_dist);
    
    show_spotlight.addListener(this, &ofApp::listener_spotlight);
    spotlight_dist.addListener(this, &ofApp::listener_spotlight_dist);
    
    show_pulse.addListener(this, &ofApp::listener_pulse);
    do_wrap.addListener(this, &ofApp::listener_do_wrap);
    look_ahead.addListener(this, &ofApp::listener_look_ahead);
    col_start.addListener(this, &ofApp::listener_col_start);
    col_end.addListener(this, &ofApp::listener_col_end);
    
    
    line_width.addListener(this, &ofApp::listener_line_width);
}


//--------------------------------------------------------------
void ofApp::set_params_linestrips(vector<Linestrip> _linestrips){
    for (auto &ls : _linestrips)
        set_params_linestrip(ls);
}

//--------------------------------------------------------------
void ofApp::set_params_linestrip(Linestrip ls){
    ls.show_debug.set(show_debug.get());
    ls.resample_dist.set(resample_dist.get());
    ls.mesh_width.set(mesh_width.get());
    ls.show_spotlight.set(show_spotlight.get());
    ls.spotlight_dist.set(spotlight_dist.get());
    ls.show_pulse.set(show_pulse.get());
    ls.do_wrap.set(do_wrap.get());
    ls.look_ahead.set(look_ahead.get());
    ls.col_start.set(col_start.get());
    ls.col_end.set(col_end.get());
    ls.line_width.set(line_width.get());
    ls.do_stripes.set(do_stripes.get());
    
    if (ls.get_num_points() > 1){
        float rs = resample_dist;
        ls.listener_resample_dist(rs);
        float mw = mesh_width;
        ls.listener_mesh_width(mw);
    }
}

//--------------------------------------------------------------
void ofApp::listener_show_debug(bool &val){
    for (auto &ls : linestrips)
        ls.show_debug.set(val);
}
void ofApp::listener_resample_dist(float &val){
    resample_dist = val;
    for (auto &ls : linestrips){
        if (ls.get_num_points() > 1)
            ls.listener_resample_dist(val);
    }
}
void ofApp::listener_mesh_width(float &val){
    for (auto &ls : linestrips){
        if (ls.get_num_points() > 1)
            ls.listener_mesh_width(val);
    }
}
void ofApp::listener_spotlight(bool &val){
    for (auto &ls : linestrips)
        ls.show_spotlight.set(val);
}
void ofApp::listener_spotlight_dist(float &val){
    for (auto &ls : linestrips)
        ls.spotlight_dist.set(val);
}
void ofApp::listener_pulse(bool &val){
    for (auto &ls : linestrips)
        ls.show_pulse.set(val);
}
void ofApp::listener_do_wrap(bool &val){
    for (auto &ls : linestrips)
        ls.do_wrap.set(val);
}
void ofApp::listener_look_ahead(float &val){
    for (auto &ls : linestrips)
        ls.look_ahead.set(val);
}
void ofApp::listener_col_start(ofVec4f &val){
    for (auto &ls : linestrips)
        ls.col_start.set(val);
}
void ofApp::listener_col_end(ofVec4f &val){
    for (auto &ls : linestrips)
        ls.col_end.set(val);
}
void ofApp::listener_line_width(int &val){
    for (auto &ls : linestrips)
        ls.line_width.set(val);
}
void ofApp::listener_do_stripes(bool &val){
    for (auto &ls : linestrips)
        ls.do_stripes.set(val);
}
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'c':
            linestrips.clear();
            break;
        case 'd':
            show_debug.set(!show_debug);
            break;
        case 'g':
            show_gui = !show_gui;
            break;
        default:
            break;
    }
    for (auto &ls : linestrips)
        ls.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    if (linestrips.size() > 0)
        linestrips[linestrips.size()-1].mouseDragged(x, y, button);

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    // create and store a new linestrip
    Linestrip linestrip;
    linestrip.setup();
    set_params_linestrip(linestrip);
    linestrip.addVertex(ofVec2f(x,y));
    linestrips.push_back(linestrip);

    linestrips[linestrips.size()-1].mousePressed(x,y,button);    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
     if (linestrips.size() > 0)
        linestrips[linestrips.size()-1].mouseReleased(x, y, button);
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
