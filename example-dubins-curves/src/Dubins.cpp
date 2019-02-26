#include "Dubins.h"


//--------------------------------------------------------------
Dubins::Dubins(){
    setup();
}

//--------------------------------------------------------------
void Dubins::setup(){
    setup_gui();
    setup_path();
}

//--------------------------------------------------------------
void Dubins::update(vector<ofNode> _waypoints){
    // regenerate the existing dubins path based on new waypoints
    if (curves.size() > 1){
        
        for (int i=0; i<_waypoints.size()-1; i++){
            // update the stored waypoints
            waypoints[i].setGlobalPosition(_waypoints[i].getGlobalPosition());
            waypoints[i].setGlobalOrientation(_waypoints[i].getGlobalOrientation());
            waypoints[i+1].setGlobalPosition(_waypoints[i+1].getGlobalPosition());
            waypoints[i+1].setGlobalOrientation(_waypoints[i+1].getGlobalOrientation());
            
            // update the stored curves
            curves[i].set(waypoints[i], waypoints[i+1], curves[i].type);
            curves[i].update();
        }

        regenerate_path();
    }

}

//--------------------------------------------------------------
void Dubins::regenerate_path(){
    // remake the path
    path.clear();
    full_path.clear();
    for (auto &crv : curves)
        path.append(crv);
    
    // add all the segments to the full path polyline
    for (auto crv : curves){
        int j=0;
        for (auto v : crv.getOutline()[0].getVertices()){
            if (j!=0)
                full_path.addVertex(v);
            j++;
        }
    }

    full_path = full_path.getResampledBySpacing(resample_spacing);
}

//--------------------------------------------------------------
void Dubins::draw(bool _show_waypoints){
    
    
    
    if (_show_debug){
    
        // highlight any close or active waypoints
        ofPushStyle();
        ofFill();
        ofNode closest = get_closest_waypoint(ofGetMouseX(), ofGetMouseY(), 30);
        if (closest.getGlobalPosition() != ofVec3f()){
            ofSetColor(255, 80);
            ofDrawEllipse(closest.getGlobalPosition(), 40, 40);
        }
        if (is_moving() && !picker_curve){
            ofSetColor(0,255,255, 80);
            ofDrawEllipse(closest.getGlobalPosition(), 30, 30);
        }
        ofPopStyle();
        
        
        // show the tangent curves
        if (_show_debug){
            for (auto & curve : curves)
                curve.draw();
        }
        
        // show the waypoint nodes
        if (_show_waypoints)
            draw_waypoints();
        
        if (_save_pdf)
            ofBeginSaveScreenAsPDF("screenshot-"+ofGetTimestampString()+".pdf");
        
        // draw the path polyline
        ofPushStyle();
        ofNoFill();
        ofColor col;
        col.set(250,0,250);
        ofSetColor(col);
        ofSetLineWidth(5);
        full_path.draw();
        ofPopStyle();
        
        if (_save_pdf){
            ofEndSaveScreenAsPDF();
            _save_pdf = false;
        }
        
        // show the curve picker
        if (picker_curve){
            
            if (curves.size() > 0 && picker_curve_index < curves.size()){
                ofPushStyle();
                ofSetColor(0, 255, 255);
                ofSetLineWidth(5);
                path.getOutline()[picker_curve_index].draw();
                ofPopStyle();
            }

        }
    }
    
    // show the directionality of the path
    draw_ticker();

}

//--------------------------------------------------------------
void Dubins::draw_waypoints(){
    if (waypoints.size() > 0){
        ofPushStyle();
        ofSetColor(200);
        int i=0;
        for (auto cp : waypoints){
            ofDrawEllipse(cp.getGlobalPosition(), 10, 10);
            ofDrawBitmapString(ofToString(i), cp.getGlobalPosition().x + 15, cp.getGlobalPosition().y + 15);
            cp.draw();
            i++;
        }
        
        if (_show_debug){
            ofSetColor(ofColor::cyan, 50);
            ofNoFill();
            ofSetLineWidth(3);
            ofBeginShape();
            for (auto cp : waypoints)
                ofVertex(cp.getGlobalPosition().x,cp.getGlobalPosition().y);
            ofEndShape();
        }
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void Dubins::draw_ticker(){
    ofPushStyle();
    if (full_path.getVertices().size() > 0){
        ofPushStyle();
        ofFill();
        ofSetColor(255);
        int i = (int)(ticker_index * speed) % full_path.getVertices().size();
        ofVec3f pos = full_path.getVertices()[i];
        ofDrawEllipse(pos, 13, 13);
        ofPopStyle();
        
        // find the orientation of the current index position
        if (i == full_path.getVertices().size() - 1){
            // ... figure this out later
        }
        else{
            ticker_node.setGlobalPosition(pos);
            ticker_node.lookAt(full_path.getVertices()[i+1]);
            
            // show the orientation of the line
            ofPushStyle();
            ofSetLineWidth(3);
            ofSetColor(0,255,255,200);
            // show the z-axis of the ticker node
            auto z_axis = ticker_node.getZAxis().getNormalized();
            z_axis.scale(50);
            ofDrawLine(ticker_node.getGlobalPosition(), ticker_node.getGlobalPosition() - z_axis);
            ofPopStyle();
        }
    }
    ofPopStyle();
    
    ticker_index++;
}

//--------------------------------------------------------------
void Dubins::clear(){
    waypoints.clear();
    curves.clear();
    full_path.clear();
    path.clear();
}

//--------------------------------------------------------------
void Dubins::add_waypoint(ofNode waypoint, double _radius){
    
    // override with GUI's radius
    _radius = radius;
    
    waypoints.push_back(waypoint);
    
    bool use_CCC = false;
    if (waypoints.size() >= 2){
        
        auto pos1 = waypoints[waypoints.size()-2].getGlobalPosition();
        auto pos2 = waypoints[waypoints.size()-1].getGlobalPosition();
        
        if (pos1.distance(pos2) < 4*_radius)
            use_CCC = true;
        
        if (curves.size() == 0){
            // start with R'S by default
            if (use_CCC)
                curves.push_back(generate_segment(waypoints[waypoints.size()-2], waypoints[waypoints.size()-1], Curve::Type::RLR, _radius));
            else
                curves.push_back(generate_segment(waypoints[0], waypoints[1], Curve::Type::RSR, _radius));
            
            path.append(curves[curves.size()-1]);
        }
        else{
            // for each additional point, use the previous goal waypoint
            // (and curve direction) as the start for the next segment
            
            // get the type of the previous curve
            Curve::Type prev_type = curves[curves.size()-1].type;
            Curve::Type next_type;
            // categorize by directionality (left or right turning)
            
            // if ending in R, the type of the next segement needs to start with R
            // example: RSR, RLR, LSR
            if (prev_type == Curve::Type::RSR || prev_type == Curve::Type::RLR || prev_type == Curve::Type::LSR){
                
                if (use_CCC)
                    next_type = Curve::Type::RLR;
                
                // use the longest valid curve next
                else if (_use_longest){
                    double length_a = generate_segment(waypoints[waypoints.size()-2], waypoints[waypoints.size()-1], Curve::Type::RSR, _radius).get_length();
                    double length_b = generate_segment(waypoints[waypoints.size()-2], waypoints[waypoints.size()-1], Curve::Type::RSL, _radius).get_length();
                    if (length_a > length_b)
                        next_type = Curve::Type::RSR;
                    else
                        next_type = Curve::Type::RSL;
                }
                // otherwise, switch the dirction of the goal circle
                else{
                    next_type = Curve::Type::RSL;
                }
            }
            // if ending in L, the type of the next segement needs to start with L
            // example: LSL, LSR, LRL
            else
            if (prev_type == Curve::Type::LSL || prev_type == Curve::Type::LRL || prev_type == Curve::Type::RSL){
                
                if (use_CCC)
                    next_type = Curve::Type::LRL;
                // use the longest valid curve next
                else if (_use_longest){
                    double length_a = generate_segment(waypoints[waypoints.size()-2], waypoints[waypoints.size()-1], Curve::Type::LSL, _radius).get_length();
                    double length_b = generate_segment(waypoints[waypoints.size()-2], waypoints[waypoints.size()-1], Curve::Type::LSR, _radius).get_length();
                    if (length_a > length_b)
                        next_type = Curve::Type::LSL;
                    else
                        next_type = Curve::Type::LSR;
                }
                // otherwise, switch the dirction of the goal circle
                else{
                    next_type = Curve::Type::LSR;
                }
           
            }

            // store the curve segment and add it to the path
            curves.push_back(generate_segment(waypoints[waypoints.size()-2], waypoints[waypoints.size()-1], next_type, _radius));
            path.append(curves[curves.size()-1]);
        }
    
        // add the newly generated segment to the full path and resample
        resample_full_path();
    }


    if (curves.size() > 1)
        picker_curve_index.setMax(curves.size()-1);
}


//--------------------------------------------------------------
void Dubins::resample_full_path(int _resample_spacing){
    
    resample_spacing = _resample_spacing;
    
    if (curves.size() > 0){
        // add the previously generated segement to the path
        int i=0;
        for (auto v : curves[curves.size()-1].getOutline()[0].getVertices()){
            if (i!=0)
                full_path.addVertex(v);
            i++;
        }
    }

    full_path = full_path.getResampledBySpacing(resample_spacing);
}

//--------------------------------------------------------------
void Dubins::close(){
    // add the first node in the curve as the last one
    if (waypoints.size() > 2){
        add_waypoint(waypoints[0]);
    }
    else{
        ofLogNotice() << "Not enough waypoints to close Dubins Curve: {" << ofToString(waypoints.size())<< "}" << endl;
    }
    
}

//--------------------------------------------------------------
void Dubins::setup_path(){

    path.setMode(ofPath::Mode::COMMANDS);
    path.setFilled(false);
    ofColor col;
    col.set(200,0,200);
    path.setStrokeColor(col);
    path.setStrokeWidth(3);
    path.setCircleResolution(300);//2*radius);
    path.setCurveResolution(300);
}

//--------------------------------------------------------------
void Dubins::setup_gui(){

    params.setName("Dubins_Params");
    params.add(_show_debug.set("Show_Debug", true));
    params.add(_use_longest.set("Use_Longest_Segments", false));
    params.add(radius.set("Turning_Radius", 50,10,100));
    params.add(speed.set("Speed", 1,0,5));
    
    params_picker.setName("Dubins_Curve_Picker");
    params_picker.add(picker_curve.set("Pick_Curve", false));
    params_picker.add(picker_curve_index.set("Curve_Index", 0,0,10));
    params_picker.add(picker_curve_type.set("Curve_Type", 1,1,6));
    params_picker.add(picker_curve_type_name.set("\tCurve::Type::"));
    params_picker.add(picker_curve_radius.set("Curve_Radius", 50,10,150));
    
    params.add(params_picker);
    
    picker_curve_type.addListener(this, &Dubins::listener_picker_curve_type);
    picker_curve_radius.addListener(this, &Dubins::listener_picker_curve_radius);
}

//--------------------------------------------------------------
void Dubins::listener_picker_curve_type(int &val){
    
    if (picker_curve){
        if (curves.size() > 0 && picker_curve_index < curves.size()){

            Curve::Type _type = static_cast<Curve::Type>(val);
            
            // update the stored curves
            curves[picker_curve_index].set(waypoints[picker_curve_index], waypoints[picker_curve_index+1], _type);
            curves[picker_curve_index].update();
            
            regenerate_path();
            
            // show the picker curve type number
            string picker_type = "";
            switch (picker_curve_type) {
                case Curve::Type::RSL:
                    picker_type = "RSL";
                    break;
                case Curve::Type::RSR:
                    picker_type = "RSR";
                    break;
                case Curve::Type::LSR:
                    picker_type = "LSR";
                    break;
                case Curve::Type::LSL:
                    picker_type = "LSL";
                    break;
                case Curve::Type::RLR:
                    picker_type = "RLR";
                    break;
                case Curve::Type::LRL:
                    picker_type = "LRL";
                    break;
                default:
                    break;
            }
            picker_curve_type_name.set("\tCurve::Type::"+picker_type);
        }
    }
}

//--------------------------------------------------------------
void Dubins::listener_picker_curve_radius(double &val){
    
    if (picker_curve){
        if (curves.size() > 0 && picker_curve_index < curves.size()){
            
            // update the stored curves
            curves[picker_curve_index].set_radius(val);
            curves[picker_curve_index].set(waypoints[picker_curve_index], waypoints[picker_curve_index+1],  curves[picker_curve_index].type);
            curves[picker_curve_index].update();
            
            regenerate_path();
            
        }
    }
}

//--------------------------------------------------------------
void Dubins::show_debug(bool flag){
    _show_debug = flag;
    for (auto &curve : curves)
        curve.is_debugging(flag);
}

//--------------------------------------------------------------
void Dubins::use_longest(bool flag){
    _use_longest = flag;
}

//--------------------------------------------------------------
Curve Dubins::generate_segment(ofNode _start, ofNode _goal, Curve::Type _type, double _radius){
    Curve curve;
    curve.set_radius(_radius);
    curve.set(_start, _goal, _type);
    curve.generate_path(_type);
    return curve;
}

//--------------------------------------------------------------
ofNode Dubins::get_closest_waypoint(int x, int y, double _radius){
    ofVec2f pos = ofVec2f(x,y);
    float dist_sq;
    float min_dist = FLT_MAX;
    int i = 0;
    int index = -1;
    for (auto node : waypoints){
        dist_sq = node.getGlobalPosition().squareDistance(pos);
        if (dist_sq < _radius*_radius && dist_sq < min_dist){
            min_dist = dist_sq;
            index = i;
        }
        i++;
    }
    
    // update the active_waypoint_index (can be -1)
    if (ofGetMousePressed())
        active_waypoint_index = index;  // can stll be -1
    else
        active_waypoint_index = -1;
    
    if (index != -1)
        return waypoints[index];
    else
        return ofNode();
}

//--------------------------------------------------------------
void Dubins::keyPressed(int key){
    switch (key) {
        case ' ':
            if (waypoints.size() > 2){
                close();
            }
            break;
        case 's':
            _save_pdf = true;
        default:
            break;
    }
}

//--------------------------------------------------------------
void Dubins::mouseMoved(int x, int y ){
       
}

//--------------------------------------------------------------
void Dubins::mouseDragged(int x, int y, int button){
    
    ofVec2f pos = ofVec2f(x,y);
    
    if (active_waypoint_index != -1){
        waypoints[active_waypoint_index].setPosition(pos);
        // regenerate dubins curve based on updated waypoints
        update(waypoints);
    }
}

//--------------------------------------------------------------
void Dubins::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void Dubins::mouseReleased(int x, int y, int button){
    active_waypoint_index = -1;
}

//--------------------------------------------------------------
bool Dubins::is_moving(){
    
    if (picker_curve)
        return true;
    
    return active_waypoint_index != -1;
}

//--------------------------------------------------------------
bool Dubins::is_closed(){
    if (waypoints.size() < 2)
        return false;
    else
        return waypoints[0].getGlobalPosition() == waypoints[waypoints.size()-1].getGlobalPosition();
}

