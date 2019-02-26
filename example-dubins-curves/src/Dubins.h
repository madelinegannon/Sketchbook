#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Curve.h"

#define DELTA 1
#define WHEELBASE 2.6


/**
 * Dubins.h
 * Non-holomonic path/curve generator
 *
 * Motivated by Zach Lieberman's Daily Sketches 2017: https://medium.com/@zachlieberman/daily-sketches-in-2017-1b4234b0615d
 * Explained through A Comprehensive, Step-by-Step Tutorial to Computing Dubin’s Paths: https://gieseanw.wordpress.com/2012/10/21/a-comprehensive-step-by-step-tutorial-to-computing-dubins-paths/
 * Adapted from code from the author: https://github.com/gieseanw/Dubins
 *
 * @author mad
 * 02.23.2019
 */

class Dubins {

    public:
        Dubins();
        void setup();
        void update(vector<ofNode> waypoints);
        void draw(bool _show_waypoints=true);
        void clear();

        void keyPressed(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
    
        void add_waypoint(ofNode waypoint, double _radius=50);
        ofNode get_closest_waypoint(int x, int y, double _radius);
        void close();
        bool is_closed();
        bool is_moving();
    
        ofPath final_path;
        void show_debug(bool flag);
        void use_longest(bool flag);
    
        ofParameterGroup get_gui(){ return params; }
    
        ofNode get_ticker_node() { return ticker_node; }
    
    void save_pdf();
    
    private:
    
        vector<Curve> curves;
        vector<ofNode> waypoints;
    
        int active_waypoint_index = -1;

        ofPath path;
        ofPolyline full_path;
    
        void setup_path();
        void resample_full_path(int _resample_spacing=5);
        int resample_spacing;
    
        void draw_waypoints();
        void draw_ticker();
        int ticker_index = 0;
        ofNode ticker_node;
    
        void regenerate_path();
        Curve generate_segment(ofNode _start, ofNode _goal, Curve::Type _type, double _radius=50);
    
        void setup_gui();
        ofParameterGroup params;
        ofParameter<bool> _use_longest;
        ofParameter<bool> _show_debug;
        ofParameter<double> radius;
        ofParameter<float> speed;
    
        // dynamically change the type of a given curve
        ofParameterGroup params_picker;
        ofParameter<bool> picker_curve;
        ofParameter<int> picker_curve_index;
        ofParameter<int> picker_curve_type;
        ofParameter<string> picker_curve_type_name;
        ofParameter<double> picker_curve_radius;
    
        void listener_picker_curve_type(int &val);
        void listener_picker_curve_radius(double &val);
    
        bool _save_pdf = false;
    
};
