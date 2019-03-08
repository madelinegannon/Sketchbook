#pragma once

/*
 * Linestrip.h
 *
 * Draw and Animate the ribbon-like line strip
 *
 * @author mad — 03.06.2091
 */

#include "ofMain.h"
#include "ofxGui.h"

class Linestrip {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
	
        void addVertex(ofVec2f pt) { line.addVertex(pt); }
        void set_target(ofVec2f _target) { target.set(_target); }
        void set_texture(ofTexture _tex);
    
        int get_num_points(){ return line.getVertices().size(); }

        ofPolyline line;
    
    
        //--------- GUI ----------
//        void setup_gui();
//        bool show_gui = true;
//        ofxPanel panel;
//        ofParameterGroup params;
        ofParameter<bool> show_debug;
        ofParameter<bool> show_mesh_faces;
        ofParameter<bool> show_wireframe;
        ofParameter<float> mesh_width;
        ofParameter<float> resample_dist;
    
//        ofParameterGroup params_spotlight;
        ofParameter<bool> show_spotlight;
        ofParameter<float> spotlight_dist;
    
//        ofParameterGroup params_pulse;
        ofParameter<bool> show_pulse;
        ofParameter<bool> do_wrap;
        ofParameter<float> look_ahead;
        ofParameter<ofVec4f> col_start;
        ofParameter<ofVec4f> col_end;
    
//        ofParameterGroup params_texture;
        ofParameter<bool> show_texture;
        ofParameter<int> line_width;
        ofParameter<bool> do_stripes;
    
    
        void listener_resample_dist(float &val);
        void listener_mesh_width(float &val);
        
    
    private:
    
        ofMesh mesh;
    
        ofVec2f target;
        int target_index = 0;
        ofTexture tex;
    
        //--------- GUI ----------
        void setup_gui();
        bool show_gui = true;
        ofParameterGroup params;
        ofParameterGroup params_spotlight;
        ofParameterGroup params_pulse;
        ofParameterGroup params_texture;
    
    
    
        //----- ANIMATIONS -------
        void do_pulse();
        void do_spotlight();
        void do_texture();
    
    void resample();//float _resample_dist);

    
};
