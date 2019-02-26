#pragma once

#include "ofMain.h"

/**
 * Curve.h
 * Non-holomonic path/curve generator
 *
 * Motivated by Zach Lieberman's Daily Sketches 2017: https://medium.com/@zachlieberman/daily-sketches-in-2017-1b4234b0615d
 * Explained through Andy G's Comprehensive, Step-by-Step Tutorial to Computing Dubin’s Paths: https://gieseanw.wordpress.com/2012/10/21/a-comprehensive-step-by-step-tutorial-to-computing-dubins-paths/
 * Adapted from code from the author: https://github.com/gieseanw/Dubins
 *
 * @author mad
 * 02.23.2019
 */

struct Circle{
    ofVec2f pos;
    float radius;
};

class Curve : public ofPath{

	public:
		void setup();
        void update();
        void draw();
        void clear();
        void keyPressed(int key);
    
    
        vector<Circle> get_tangent_circles();
        void set_radius(double _radius) { radius = _radius; }
        double get_length(){ return length; }
    
        enum Type{
            NONE=0,
            LRL,
            RLR,
            LSL,
            LSR,
            RSL,
            RSR
        };
        Type type = Type::NONE;
    
        void set(ofNode _start, ofNode _goal, Type _type);
        void generate_path(Type _type);
        void is_debugging(bool flag) { show_debug = flag; }
        double get_radius(){ return radius; }
    private:
        ofNode start, goal;
        double radius = 50;
        double length = 0;
    
        pair<ofVec3f,ofVec3f> outer_tangents;
        vector<pair<ofVec3f,ofVec3f>> tangent_lines;
        void update_tangent_lines();
        bool has_tangents;
        vector<pair<ofVec3f,ofVec3f>> get_tangent_lines(Circle start_circle, Circle goal_circle);
        Circle start_circle_L, start_circle_R, goal_circle_L, goal_circle_R, tangent_circle;
        void update_tangent_circles(ofNode _start, ofNode _goal);
    
        void generate_CSC(Circle* start_circle, Circle* goal_circle);
        void generate_CCC(Circle* start_circle, Circle* goal_circle);
    
        //--------------------------------------------------------------
        // Math Helpers
        double get_arc_theta(const ofVec3f& _center, const ofVec3f& _lhs, const ofVec3f& _rhs, double _radius, bool left);
        double get_arc_length(const ofVec3f& _center, const ofVec3f& _lhs, const ofVec3f& _rhs, double _radius, bool left);

        //--------------------------------------------------------------
        // GUI and Graphics
        bool show_debug = true;
        void draw_CCC();
        void draw_CSC();
        void draw_tangent_circles();
    
        void setup_path();
    
        ofColor start_bold = ofColor(ofColor::deepSkyBlue);
        ofColor goal_bold = ofColor(ofColor::mediumVioletRed);
        ofColor color_minor = ofColor(255, 120);
};
