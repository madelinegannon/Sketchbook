#include "Curve.h"

//--------------------------------------------------------------
void Curve::setup(){
    setup_path();
}

//--------------------------------------------------------------
void Curve::set(ofNode _start, ofNode _goal, Type _type){
    start.setGlobalPosition(_start.getGlobalPosition());
    start.setGlobalOrientation(_start.getGlobalOrientation());
    goal.setGlobalPosition(_goal.getGlobalPosition());
    goal.setGlobalOrientation(_goal.getGlobalOrientation());
    type = _type;
    has_tangents = false;
    
    // update the tangents needed to generate the dubins path
    update_tangent_circles(start, goal);
    update_tangent_lines();
    
    setCircleResolution(360);
    ofSetCircleResolution(180);
}

//--------------------------------------------------------------
void Curve::generate_path(Type _type){
    
    clear();

    Circle * start_circle;
    Circle * goal_circle;
    
    switch (_type) {
        case Type::RSR:
        case Type::RLR:
            start_circle = &start_circle_R;
            goal_circle = &goal_circle_R;
            break;
        case Type::LSL:
        case Type::LRL:
            start_circle = &start_circle_L;
            goal_circle = &goal_circle_L;
            break;
        case Type::RSL:
            start_circle = &start_circle_R;
            goal_circle = &goal_circle_L;
            break;
        case Type::LSR:
            start_circle = &start_circle_L;
            goal_circle = &goal_circle_R;
            break;
        default:
            break;
    }
    
    if (_type == Type::RLR || _type == Type::LRL)
        generate_CCC(start_circle, goal_circle);
    else
        generate_CSC(start_circle, goal_circle);

}

//--------------------------------------------------------------
void Curve::generate_CSC(Circle* start_circle, Circle* goal_circle){
    
    ofVec3f centroid, arc_start, arc_end;
    ofVec3f x_axis;
    float arc_theta, orientation_angle, angle_begin, angle_end;
    double curve_length = 0;
    
    // set the directionality of each arc
    bool arc_1_left, arc_2_left, arc_3_left;
    switch (type) {
        case Type::RSR:
            arc_1_left = false;
            arc_3_left = false;
            break;
        case Type::LSL:
            arc_1_left = true;
            arc_3_left = true;
            break;
        case Type::RSL:
            arc_1_left = false;
            arc_3_left = true;
            break;
        case Type::LSR:
            arc_1_left = true;
            arc_3_left = false;
            break;
        default:
            break;
    }
    
    
    // check if a tangent pair exists
    if (has_tangents){
        
        // describe the first arc
        centroid = start_circle->pos;
        arc_start = start.getGlobalPosition();
        arc_end = outer_tangents.first;
        
        x_axis = start.getXAxis().normalize();
        orientation_angle = x_axis.angle(ofVec3f(1,0,0));
        
        // get the rotation and length of the  arc
//        arc_theta = ofRadToDeg(get_arc_theta(centroid, arc_start, arc_end, radius, arc_1_left));
//        curve_length += get_arc_length(centroid, arc_start, arc_end, radius, arc_1_left);
        arc_theta = ofRadToDeg(get_arc_theta(centroid, arc_start, arc_end, radius_start, arc_1_left));
        curve_length += get_arc_length(centroid, arc_start, arc_end, radius_start, arc_1_left);
        
        // set the start and end angles for the arc
        switch (type) {
            case Type::RSR:
            case Type::RSL:
                angle_begin = 90 + orientation_angle;
                angle_end   = angle_begin + arc_theta;
                break;
            case Type::LSL:
            case Type::LSR:
                angle_begin = orientation_angle - 90;
                angle_end   = angle_begin + arc_theta;
                break;
            default:
                break;
        }
        
        // add the arc to the path
        this->arc(centroid, radius_start, radius_start, angle_begin, angle_end, arc_1_left);
        
        
        // add the straight-away (second arc)
        curve_length += outer_tangents.first.distance(outer_tangents.second);
        
        
        // describe the third arc
        centroid = goal_circle->pos;
        arc_start = outer_tangents.second;
        arc_end = goal.getGlobalPosition();
        
        x_axis = goal.getXAxis().normalize();
        orientation_angle = x_axis.angle(ofVec3f(1,0,0));
        
        // get the rotation and length of the  arc
        arc_theta = ofRadToDeg(get_arc_theta(centroid, arc_start, arc_end, radius_goal, arc_3_left));
        curve_length += get_arc_length(centroid, arc_start, arc_end, radius_goal, arc_3_left);
        
        switch (type) {
            case Type::RSR:
            case Type::LSR:
                angle_end   = 90 + orientation_angle;
                angle_begin = angle_end - arc_theta;
                break;
            case Type::LSL:
            case Type::RSL:
                angle_end   = orientation_angle - 90;
                angle_begin =  angle_end - arc_theta;
                break;
            default:
                break;
        }
        
//        cout << "orientation_angle: {" << ofToString(orientation_angle) << "}" << endl;
//        cout << "arc_theta: " << ofToString(arc_theta) << "" << endl;
//        cout << "angle_begin: " << ofToString(angle_begin) << "" << endl;
//        cout << "angle_end: " << ofToString(angle_end) << "" << endl <<endl;
        
        this->arc(centroid, radius_goal, radius_goal, angle_begin, angle_end, arc_3_left);
        
    }
    
    // update the curve length
    length = curve_length;
}

//--------------------------------------------------------------
void Curve::generate_CCC(Circle* start_circle, Circle* goal_circle){
    /* test with only RLR for now */
    
    ofVec3f centroid, arc_start, arc_end;
    ofVec3f x_axis;
    float arc_theta, orientation_angle, angle_begin, angle_end;
    double curve_length = 0;
    
    ofVec3f start_tangent, goal_tangent;
    
    bool arc_1_left, arc_2_left, arc_3_left;                        // directionality of each arc
    
    // get the interior angle between the start and goal circles
    float dist = start_circle->pos.distance(goal_circle->pos);
    float tangent_radius = (radius_start + radius_goal)/2;
    
    // Law of Cosines
    // beta = acos( ( a^2 + b^2 - c^2 ) / (2ab) )
    // a = P1 to P3
    // b = P1 to P2
    // c = P3 to P2
    float a = radius_start + tangent_radius;
    float b = dist;
    float c = tangent_radius + radius_goal;
    float interior_theta = acos((a*a + b*b - c*c)/(2*a*b));
    if (type == Type::RLR){
        interior_theta += atan2(goal_circle->pos.y - start_circle->pos.y, goal_circle->pos.x - start_circle->pos.x);
        arc_1_left = false;
        arc_2_left = true;
        arc_3_left = false;
    }
    else if (type == Type::LRL){
        interior_theta = atan2(goal_circle->pos.y - start_circle->pos.y, goal_circle->pos.x -
                           start_circle->pos.x) - interior_theta;
        arc_1_left = true;
        arc_2_left = false;
        arc_3_left = true;
    }
    
    // Update the tangent circle
    tangent_circle.radius = tangent_radius;
    // Compute the tangent circle using law of cosines + atan2 of line between start and goal circles
    tangent_circle.pos = ofVec2f(start_circle->pos.x + ((tangent_radius+radius_start)*cos(interior_theta)), start_circle->pos.y + ((tangent_radius+radius_start)*sin(interior_theta)));
    
    
    // Compute tangent points ont the given tangent circle
    start_tangent.x = (tangent_circle.pos.x + start_circle->pos.x)/2.0;
    start_tangent.y = (tangent_circle.pos.y + start_circle->pos.y)/2.0;
    goal_tangent.x  = (tangent_circle.pos.x + goal_circle->pos.x)/2.0;
    goal_tangent.y  = (tangent_circle.pos.y + goal_circle->pos.y)/2.0;
    
    // describe the first arc
    centroid = start_circle->pos;
    arc_start = start.getGlobalPosition();
    arc_end = start_tangent;
    
    x_axis = start.getXAxis().normalize();
    orientation_angle = x_axis.angle(ofVec3f(1,0,0));
    
    // get the rotation and length of the arc
    arc_theta = ofRadToDeg(get_arc_theta(centroid, arc_start, arc_end, radius_start, arc_1_left));
    curve_length += get_arc_length(centroid, arc_start, arc_end, radius_start, arc_1_left);
    
    // set the start and end angles for the arc
    switch (type) {
        case Type::RLR:
            angle_begin = 90 + orientation_angle;
            angle_end   = angle_begin + arc_theta;
            break;
        case Type::LRL:
            angle_begin = orientation_angle - 90;
            angle_end   = angle_begin + arc_theta;
            break;
        default:
            break;
    }

    
    // add the arc to the path
    this->arc(centroid, radius_start, radius_start, angle_begin, angle_end, arc_1_left);
    
//    cout << "orientation_angle: {" << ofToString(orientation_angle) << "}" << endl;
//    cout << "arc_theta: " << ofToString(arc_theta) << "" << endl;
//    cout << "angle_begin: " << ofToString(angle_begin) << "" << endl;
//    cout << "angle_end: " << ofToString(angle_end) << "" << endl <<endl;
    

    
    // describe the second arc
    centroid = tangent_circle.pos;
    arc_start = start_tangent;
    arc_end = goal_tangent;
    
    x_axis = ofVec3f(1,0,0);
    orientation_angle = x_axis.angle((start_tangent - tangent_circle.pos).getNormalized());
    
    if (tangent_circle.pos.y > start_circle->pos.y)
        orientation_angle *= -1;
    
    // get the rotation and length of the arc
    arc_theta = ofRadToDeg(get_arc_theta(centroid, arc_start, arc_end, tangent_radius, arc_2_left));
    curve_length += get_arc_length(centroid, arc_start, arc_end, tangent_radius, arc_2_left);

    
    angle_begin = orientation_angle;
    angle_end   = angle_begin + arc_theta;

    
    // add the arc to the path
    this->arc(centroid, tangent_radius, tangent_radius, angle_begin, angle_end, arc_2_left);
    
    
    // describe the third arc
    centroid = goal_circle->pos;
    arc_start = goal_tangent;
    arc_end = goal.getGlobalPosition();
    
    x_axis = goal.getXAxis().normalize();
    orientation_angle = x_axis.angle(ofVec3f(1,0,0));
    
    // get the rotation and length of the arc
    arc_theta = ofRadToDeg(get_arc_theta(centroid, arc_start, arc_end, radius_goal, arc_3_left));
    curve_length += get_arc_length(centroid, arc_start, arc_end, radius_goal, arc_3_left);

    
    // set the start and end angles for the arc
    switch (type) {
        case Type::RLR:
            angle_end = 90 + orientation_angle;
            angle_begin   = angle_end - arc_theta;
            break;
        case Type::LRL:
            angle_end   = orientation_angle - 90;
            angle_begin = angle_end - arc_theta;
            break;
        default:
            break;
    }
    

    // add the arc to the path
    this->arc(centroid, radius_goal, radius_goal, angle_begin, angle_end, arc_3_left);
    
    
    // update the curve length
    length = curve_length;
}

//--------------------------------------------------------------
void Curve::update(){
    
    // regenerate curve from waypoints and existing type and radius
    generate_path(type);
}

//--------------------------------------------------------------
void Curve::draw(){
    
    if (show_debug){
        // if Type is a CCC, show the tangent circle
        if (type == Type::LRL || type == Type::RLR){
            draw_CCC();
        }
        // if Type is a CSC, show the tangent line pairs
        else{
            draw_CSC();
        }
    }

//    // manually draw the path
//    // ... (ofPath::draw() wasn't rendering correctly)
//    ofPushStyle();
//    ofNoFill();
////    ofColor col;
////    col.set(0,255,255., 150);
////    ofSetColor(col);
//    ofSetLineWidth(7);
//    const vector<ofPolyline> & outlines = this->getOutline();
//    ofBeginShape();
//    for(int i=0; i<outlines[0].getVertices().size(); i++){
//        if (i != 0){
//            ofVertex(outlines[0].getVertices()[i].x, outlines[0].getVertices()[i].y);
//        }
//    }
//    ofEndShape();
//    ofPopStyle();
//    
}


//--------------------------------------------------------------
void Curve::update_tangent_circles(ofNode _start, ofNode _goal){
    
    double theta = ofDegToRad(_start.getXAxis().angle(ofVec3f(1,0,0)));
    theta += PI/2.0;
    if (theta > PI)
        theta -= 2.0*PI;
    
    start_circle_L.pos = ofVec3f(_start.getGlobalPosition().x + radius_start*cos(theta), _start.getGlobalPosition().y + radius_start*sin(theta));
    start_circle_L.radius = radius_start;
    
    theta = ofDegToRad(_start.getXAxis().angle(ofVec3f(1,0,0)));
    theta -= PI/2.0;
    if (theta < -PI)
        theta += 2.0*PI;
    start_circle_R.pos = ofVec3f(_start.getGlobalPosition().x + radius_start*cos(theta), _start.getGlobalPosition().y + radius_start*sin(theta));
    start_circle_R.radius = radius_start;
    
    theta = ofDegToRad(_goal.getXAxis().angle(ofVec3f(1,0,0)));
    theta += PI/2.0;
    if (theta > PI)
        theta -= 2.0*PI;
    
    goal_circle_L.pos = ofVec3f(_goal.getGlobalPosition().x + radius_goal*cos(theta), _goal.getGlobalPosition().y + radius_goal*sin(theta));
    goal_circle_L.radius = radius_goal;
    
    theta = ofDegToRad(_goal.getXAxis().angle(ofVec3f(1,0,0)));
    theta -= PI/2.0;
    if (theta < -PI)
        theta += 2.0*PI;
    
    goal_circle_R.pos = ofVec3f(_goal.getGlobalPosition().x + radius_goal*cos(theta), _goal.getGlobalPosition().y + radius_goal*sin(theta));
    goal_circle_R.radius = radius_goal;

}

//--------------------------------------------------------------
vector<Circle> Curve::get_tangent_circles(){
    vector<Circle> tangent_circles;
    
    tangent_circles.push_back(start_circle_L);
    tangent_circles.push_back(start_circle_R);
    tangent_circles.push_back(goal_circle_L);
    tangent_circles.push_back(goal_circle_R);
    
    return tangent_circles;
}

//--------------------------------------------------------------
void Curve::draw_CCC(){
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::yellowGreen, 200);
    ofDrawEllipse(tangent_circle.pos, 2*tangent_circle.radius, 2*tangent_circle.radius);
    ofPopStyle();
    
    draw_tangent_circles();
}

//--------------------------------------------------------------
void Curve::draw_CSC(){
    ofPushStyle();
    ofNoFill();
    // draw the outer tangent lines (if they exist)
    ofVec3f outer_tangent_start;
    ofVec3f outer_tangent_goal;
    switch (type) {
        case Type::RSR:
            if (tangent_lines.size() > 0){
                outer_tangent_start = tangent_lines.at(0).first;
                outer_tangent_goal  = tangent_lines.at(0).second;
            }
            break;
        case Type::LSL:
            if (tangent_lines.size() > 1){
                outer_tangent_start = tangent_lines.at(1).first;
                outer_tangent_goal  = tangent_lines.at(1).second;
            }
            break;
        case Type::RSL:
            if (tangent_lines.size() > 2){
                outer_tangent_start = tangent_lines.at(2).first;
                outer_tangent_goal  = tangent_lines.at(2).second;
            }
            break;
        case Type::LSR:
            if (tangent_lines.size() > 3){
                outer_tangent_start = tangent_lines.at(3).first;
                outer_tangent_goal  = tangent_lines.at(3).second;
            }
            break;
        default:
            break;
    }
    ofSetColor(color_minor);
    ofSetLineWidth(3);
    ofDrawLine(outer_tangents.first, outer_tangents.second);
    
    // show the tangent points
    float dot_size = 7;
    ofFill();
    ofSetColor(start_bold);
    ofDrawEllipse(outer_tangent_start, dot_size, dot_size);
    ofSetColor(goal_bold);
    ofDrawEllipse(outer_tangent_goal, dot_size, dot_size);
    ofPopStyle();
    
    draw_tangent_circles();

}

//--------------------------------------------------------------
void Curve::draw_tangent_circles(){
    
    Circle * start_circle;
    Circle * goal_circle;
    string _type = "";

    ofPushStyle();
    ofNoFill();
    switch (type) {
        case Type::RSR:
            start_circle = &start_circle_R;
            goal_circle = &goal_circle_R;
            _type = "RSR";
            break;
        case Type::RLR:
            start_circle = &start_circle_R;
            goal_circle = &goal_circle_R;
             _type = "RLR";
            break;
        case Type::LRL:
            start_circle = &start_circle_L;
            goal_circle = &goal_circle_L;
            _type = "LRL";
            break;
        case Type::LSL:
            start_circle = &start_circle_L;
            goal_circle = &goal_circle_L;
            _type = "LSL";
            break;
        case Type::RSL:
            start_circle = &start_circle_R;
            goal_circle = &goal_circle_L;
            _type = "RSL";
            break;
        case Type::LSR:
            start_circle = &start_circle_L;
            goal_circle = &goal_circle_R;
             _type = "LSR";
            break;
        default:
            break;
    }

    ofSetColor(color_minor);
    for (auto & circle : get_tangent_circles())
        ofDrawCircle(circle.pos, circle.radius);
    // highlight the start and goal circles
    ofSetColor(start_bold);
    ofDrawCircle(start_circle->pos, start_circle->radius);
    ofSetColor(goal_bold);
    ofDrawCircle(goal_circle->pos, goal_circle->radius);
//     ofDrawCircle(start_circle->pos, start_circle->radius);
//    ofDrawCircle(goal_circle->pos, goal_circle->radius);
    ofPopStyle();
    
    // show the type of the Curve
    ofSetColor(color_minor);
    ofVec3f mid = start_circle->pos.getInterpolated(goal_circle->pos, .5);
    if (type == Type::RLR || type == Type::LRL)
        mid = tangent_circle.pos;
    ofDrawBitmapString(_type, mid.x, mid.y);
}

//--------------------------------------------------------------
void Curve::setup_path(){
    setMode(ofPath::Mode::COMMANDS);
    setFilled(false);
    ofColor col;
    col.set(255,0,255., 150);
    setStrokeColor(col);
    setStrokeWidth(3);
    setCircleResolution(720);
    setCurveResolution(720);
}

//--------------------------------------------------------------
void Curve::clear(){
//    if (this->getOutline().size() > 0){
        ofPath::clear();
//    }
}


//--------------------------------------------------------------
void Curve::keyPressed(int key){

}

//--------------------------------------------------------------
void Curve::update_tangent_lines(){
    switch (type) {
        case Type::RSR:
            tangent_lines = get_tangent_lines(start_circle_R, goal_circle_R);
            break;
        case Type::LSL:
            tangent_lines = get_tangent_lines(start_circle_L, goal_circle_L);
            break;
        case Type::RSL:
            tangent_lines = get_tangent_lines(start_circle_R, goal_circle_L);
            break;
        case Type::LSR:
            tangent_lines = get_tangent_lines(start_circle_L, goal_circle_R);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
vector<pair<ofVec3f,ofVec3f>> Curve::get_tangent_lines(Circle start_circle, Circle goal_circle){
    double x1 = start_circle.pos.x;
    double y1 = start_circle.pos.y;
    double x2 = goal_circle.pos.x;
    double y2 = goal_circle.pos.y;
    double r1 = start_circle.radius;
    double r2 = goal_circle.radius;
    double d_sq = pow(x2-x1,2) + pow(y2-y1,2);
    vector<pair<ofVec3f,ofVec3f>> returnVec;
    
    if (d_sq < (r1-r2)*(r1-r2)){
        //we may have a problem, the circles are either intersecting, one is within the other, but still tangent
        //at one point, or one is completely in the other. We only have a problem if one is within the other, but
        //not tangent to it anywhere
        if (d_sq != max(r1,r2) && d_sq < max(r1,r2)){
            cerr << "Circles are contained with each other and not tangent. No tangent lines exist" << endl;
            has_tangents = false;
            return returnVec;
        }//else they are intersecting or one is within the other, but still tangent to it
        //in the other two cases, either 1 or 2 external tangent lines remain, but there are no internal tangent
        //lines
    }
    
    double d = sqrt(d_sq);
    double vx = (x2 - x1) / d;
    double vy = (y2 - y1) / d;
    for (int sign1 = +1; sign1 >= -1; sign1 -= 2) {
        double c = (r1 - sign1 * r2) / d;
        if (c*c > 1.0) continue; //want to be subtracting small from large, not adding
        double h = sqrt(max(0.0, 1.0 - c*c));
        
        for (int sign2 = +1; sign2 >= -1; sign2 -= 2) {
            double nx = vx * c - sign2 * h * vy;
            double ny = vy * c + sign2 * h * vx;
            returnVec.push_back(make_pair(ofVec3f(x1 + r1 * nx, y1 + r1 * ny, 0), ofVec3f(x2 + sign1 * r2 * nx, y2 + sign1 * r2 * ny,0)));
        }
    }
    switch (type) {
        case Type::RSR:
            if (returnVec.size() > 0){
                outer_tangents.first = returnVec.at(0).first;
                outer_tangents.second  = returnVec.at(0).second;
                has_tangents = true;
            }
            break;
        case Type::LSL:
            if (returnVec.size() > 1){
                outer_tangents.first = returnVec.at(1).first;
                outer_tangents.second  = returnVec.at(1).second;
                has_tangents = true;
            }
            break;
        case Type::RSL:
            if (returnVec.size() > 2){
                outer_tangents.first = returnVec.at(2).first;
                outer_tangents.second = returnVec.at(2).second;
                has_tangents = true;
            }
            break;
        case Type::LSR:
            if (returnVec.size() > 3){
                outer_tangents.first = returnVec.at(3).first;
                outer_tangents.second  = returnVec.at(3).second;
                has_tangents = true;
            }
            break;
        default:
            break;
    }
    
    return returnVec;
}

//--------------------------------------------------------------
double Curve::get_arc_theta(const ofVec3f& _center, const ofVec3f& _lhs, const ofVec3f& _rhs, double _radius, bool left){
    ofVec3f vec1,vec2;
    vec1.x = _lhs.x - _center.x;
    vec1.y = _lhs.y - _center.y;
    
    vec2.x = _rhs.x - _center.x;
    vec2.y = _rhs.y - _center.y;
    
    double theta = atan2(vec2.y, vec2.x) - atan2(vec1.y,vec1.x);
    if (theta < -1e-6 && left)
        theta += 2.0*PI;
    else if (theta > 1e-6 && !left)
        theta -= 2.0 * PI;
    
    return theta;
}

//--------------------------------------------------------------
double Curve::get_arc_length(const ofVec3f& _center, const ofVec3f& _lhs, const ofVec3f& _rhs, double _radius, bool left){
    
    return fabs(get_arc_theta(_center, _lhs, _rhs, _radius, left)*_radius);
    
}

