#include "Linestrip.h"

//--------------------------------------------------------------
void Linestrip::setup(){
    setup_gui();
    
    // allocate basic texture
    tex.allocate(100, 100, GL_RGBA);
    
    // setup a mesh for the newly created polyline
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
}

//--------------------------------------------------------------
void Linestrip::update(){
    
    if (show_spotlight){
        // set target to mouse
        set_target(ofVec2f(ofGetMouseX(), ofGetMouseY()));
        do_spotlight();
    }
    else if (show_pulse){
        // set target based on timer
        target_index=(ofGetElapsedTimeMillis()/10) % get_num_points();
        set_target(line.getVertices()[target_index]);
        do_pulse();
    }
    
}

//--------------------------------------------------------------
void Linestrip::draw(){
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::aqua, 200);
    
    // draw the mesh
    if (show_mesh_faces){
        ofPushStyle();
        mesh.draw();
        ofPopStyle();
    }
    
    if (show_wireframe){
        ofSetColor(ofColor::magenta,100);
        mesh.drawWireframe();
    }
    
    if (show_texture){
        tex.bind();
        mesh.draw();
        tex.unbind();
    }
    
    
    if (show_debug){
        
        // draw the lines
        ofSetLineWidth(1);
        ofSetColor(ofColor::whiteSmoke);
        
        line.draw();
        ofSetColor(ofColor::whiteSmoke,60);
        for (auto &v : line.getVertices())
            ofDrawEllipse(v, mesh_width/2, mesh_width/2);
        
        
        // show the closest point on the last polyline
        if (target != ofVec2f()){
            ofFill();
            ofSetColor(ofColor::yellow, 100);
            ofDrawEllipse(target, mesh_width*1.5, mesh_width*1.5);
        }
    }
    
    ofPopStyle();
    
//    if (show_gui){
//        panel.draw();
//    }
 
}

//--------------------------------------------------------------
void Linestrip::do_spotlight(){
    ofFloatColor col;
    
    for (int i=0; i<mesh.getNumColors(); i++){
        ofVec2f pos = mesh.getVertices()[i];
        float dist = pos.distance(target);
        float alpha = ofMap(dist, 0, spotlight_dist, 1, 0, true);
        col.set(1,1,1,alpha);
        mesh.getColors()[i].set(col);
    }
}

//--------------------------------------------------------------
void Linestrip::do_pulse(){
    ofFloatColor col;
    float r,g,b,a;
    
    for (int i=0; i<mesh.getNumColors(); i++){
        int head_index_pl = target_index;
        float length_to_head = line.getLengthAtIndex(head_index_pl);
        int tail_index_pl = line.getIndexAtLength(length_to_head + look_ahead);
        int num_vertices_pl = line.getVertices().size()-1;
        int total_length = line.getLengthAtIndex(num_vertices_pl);
        
        // there are two mesh vertices for every line.vertex
        int head_index_mesh = head_index_pl*2;
        int tail_index_mesh = tail_index_pl*2+1;
        
        // if the current index is less than the head index or more than the tail index,
        // interpolate the update the pulse color
        float length_to_index = line.getLengthAtIndex(i/2);
        if (i > head_index_mesh && i <= tail_index_mesh){
            
            r = ofMap(length_to_index, length_to_head, length_to_head+look_ahead, col_start.get().x, col_end.get().x);
            g = ofMap(length_to_index, length_to_head, length_to_head+look_ahead, col_start.get().y, col_end.get().y);
            b = ofMap(length_to_index, length_to_head, length_to_head+look_ahead, col_start.get().z, col_end.get().z);
            float lerp_offset = .25; // add lerp from 1.25.0 for a faster (linear) falloff
            a = ofMap(length_to_index, length_to_head, length_to_head+look_ahead, col_start.get().w + lerp_offset, col_end.get().w);
            
        }
        else{
            r, g, b, a = 0;
        }
        
        col.set(r,g,b,a);
        mesh.getColors()[i].set(col);
        
        // Special Case: if we are wrapping around the curves
        if (do_wrap){
            float wrap_length=0;
            if ((length_to_head + look_ahead) > total_length){
                // find the difference in distance
                wrap_length = (length_to_head + look_ahead) - total_length;
                // make a second head / tail at the start of the line
                head_index_mesh = 0;
                tail_index_mesh = line.getIndexAtLength(wrap_length)*2;
                if (i >= head_index_mesh && i <= tail_index_mesh){
                    // get alpha at the tail_index_mesh
                    float tail_alpha = mesh.getColor(mesh.getVertices().size()-2).a;
                    // lerp to zero
                    float wrap_alpha = ofMap(length_to_index, 0, wrap_length, tail_alpha, 0);
                    
                    ofFloatColor col_end_pt = mesh.getColor(mesh.getVertices().size()-2);
                    r = ofMap(length_to_index, 0, wrap_length, col_end_pt.r, col_end.get().x);
                    g = ofMap(length_to_index, 0, wrap_length, col_end_pt.g, col_end.get().y);
                    b = ofMap(length_to_index, 0, wrap_length, col_end_pt.b, col_end.get().z);
                    a = ofMap(length_to_index, 0, wrap_length, col_end_pt.a, col_end.get().w);
                    
                    col.set(r,g,b,a);
                    mesh.getColors()[i].set(col);
                }
            }
        }
    }
}




//--------------------------------------------------------------
void Linestrip::setup_gui(){
    
    params.setName("Params");
    params.add(show_debug.set("Show_Debug", true));
    params.add(show_mesh_faces.set("Show_Mesh_Faces", true));
    params.add(show_wireframe.set("Show_Wireframe",false));
    params.add(mesh_width.set("Mesh_Width", 50, 1,500));
    params.add(resample_dist.set("Resample_Dist", 15, 1,100));
    
    resample_dist = 15;
    
//    resample_dist.addListener(this, &Linestrip::listener_resample_dist);
//    mesh_width.addListener(this, &Linestrip::listener_mesh_width);
    
    params_spotlight.setName("Params_Spotlight");
    params_spotlight.add(show_spotlight.set("Show_Spotlight", false));
    
    params_pulse.setName("Params_Pulse");
    params_pulse.add(show_pulse.set("Show_Pulse", true));
    params_pulse.add(do_wrap.set("Wrap_Pulse", true));
    params_pulse.add(look_ahead.set("Look_Ahead", 1500, 0, 5000));
    params_pulse.add(col_start.set("Start_Color", ofVec4f(1,1,1,1), ofVec4f(0,0,0,0), ofVec4f(1,1,1,1)));
    params_pulse.add(col_end.set("End_Color", ofVec4f(1,1,1,1), ofVec4f(0,0,0,0), ofVec4f(1,1,1,1)));
    
    params_texture.setName("Params_Texture");
    params_texture.add(show_texture.set("Show_Texture", true));
    params_texture.add(line_width.set("Line_Width", 15, 0, 50));
    params_texture.add(do_stripes.set("Striped_Texture", true));
    
}

//--------------------------------------------------------------
void Linestrip::listener_mesh_width(float &val){
    mesh_width = val;
    if (get_num_points() > 1){
        resample();
    }
}

//--------------------------------------------------------------
void Linestrip::listener_resample_dist(float &val){
    resample_dist = val;
    if (get_num_points() > 1){
        resample();
    }
}

//--------------------------------------------------------------
void Linestrip::resample(){
    
    // resample the polyline
    auto temp = line.getResampledBySpacing(resample_dist.get());
    line.clear();
    line.addVertices(temp.getVertices());
    
    // rebuild the mesh based on the resampled polyline
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    ofFloatColor col;
    for (int i=1; i<line.getVertices().size(); i++){
        
        // get the mesh vertices
        ofVec2f perp_prev = line.getNormalAtIndex(i-1) * mesh_width/2;
        ofVec2f pos_prev = line.getVertices()[i-1];
        
        mesh.addVertex(pos_prev + perp_prev);      // top
        float r,g,b,a;
        r = col_start.get().x; g = col_start.get().y; b = col_start.get().z; a = col_start.get().w;
        col.set(r,g,b,a);                          // add a default color
        mesh.addColor(col);
        mesh.addTexCoord(ofVec2f(0, 0));
        
        mesh.addVertex(pos_prev - perp_prev);      // btm
        r = col_end.get().x; g = col_end.get().y; b = col_end.get().z; a = col_end.get().w;
        col.set(r,g,b,a);                          // add a default color
        mesh.addColor(col);
        mesh.addTexCoord(ofVec2f(tex.getWidth(), tex.getHeight()));
        
        // add the last mesh face
        if (i==line.getVertices().size()-1){
            mesh.addVertex(line.getVertices()[i] + (line.getNormalAtIndex(i) * mesh_width/2));
            r = col_start.get().x; g = col_start.get().y; b = col_start.get().z; a = col_start.get().w;
            col.set(r,g,b,a);
            mesh.addColor(col);
            mesh.addTexCoord(ofVec2f(0, 0));//tex.getHeight()));
            mesh.addVertex(line.getVertices()[i] - (line.getNormalAtIndex(i) * mesh_width/2));
            r = col_end.get().x; g = col_end.get().y; b = col_end.get().z; a = col_end.get().w;
            col.set(r,g,b,a);
            mesh.addColor(col);
            mesh.addTexCoord(ofVec2f(tex.getWidth(), tex.getHeight()));
        }
    }
    
}

//--------------------------------------------------------------
void Linestrip::set_texture(ofTexture _tex){
    
    if (_tex.getWidth() != tex.getWidth() && _tex.getHeight() != tex.getHeight()){
        ofLogError() << "Error: incoming texture has different dimensions than internal texture" << endl;
    }else{
        tex = _tex;
    }
    
}


//--------------------------------------------------------------
void Linestrip::keyPressed(int key){
    switch (key) {
        case 'c':
            line.clear();
            mesh.clear();
            target = ofVec2f();
            break;
//        case 'd':
//            show_debug.set(!show_debug);
//            break;
        case 'g':
            show_gui = !show_gui;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void Linestrip::mouseDragged(int x, int y, int button){
    
    // add vertices to the polyine an build the line strip mesh
    ofVec2f pos = ofVec2f(x,y);
    line.addVertex(pos);
    
    // get the mesh vertices
    int i = line.getVertices().size()-1;
    ofVec2f perp_prev = line.getNormalAtIndex(i-1) * mesh_width/2;
    ofVec2f pos_prev = line.getVertices()[line.getVertices().size()-2];
    
    // construct the mesh face
    mesh.addVertex(pos_prev - perp_prev);
    mesh.addVertex(pos_prev + perp_prev);
    
}

//--------------------------------------------------------------
void Linestrip::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void Linestrip::mouseReleased(int x, int y, int button){
    
    if (line.getVertices().size() > 1)
        resample();
    
}
