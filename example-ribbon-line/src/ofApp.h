#pragma once

#include "ofMain.h"
#include "Linestrip.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

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
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        vector<Linestrip> linestrips;
        void set_params_linestrips(vector<Linestrip> _linestrips);
        void set_params_linestrip(Linestrip ls);
        void set_linestrip_params();
    
        void draw_texture(int x, int y);
        void update_texture();
        ofFbo fbo_texture;
    
        //--------- GUI ----------
        void setup_gui();
        bool show_gui = true;
        ofxPanel panel;
        ofParameterGroup params;
        ofParameter<bool> show_debug;
        ofParameter<bool> show_mesh_faces;
        ofParameter<bool> show_wireframe;
    
        //------ CONTROLLER ------
        ofParameter<float> mesh_width;
        ofParameter<float> resample_dist;

        ofParameterGroup params_spotlight;
        ofParameter<bool> show_spotlight;
        ofParameter<float> spotlight_dist;

        ofParameterGroup params_pulse;
        ofParameter<bool> show_pulse;
        ofParameter<bool> do_wrap;
        ofParameter<float> look_ahead;
        ofParameter<ofVec4f> col_start;
        ofParameter<ofVec4f> col_end;

        ofParameterGroup params_texture;
        ofParameter<bool> use_texture;
        ofParameter<bool> show_texture;
        ofParameter<int> line_width;
        ofParameter<bool> do_stripes;
    
        void setup_listeners();
    
        void listener_show_debug(bool &val);
        void listener_mesh_width(float &val);
        void listener_resample_dist(float &val);
    
        void listener_spotlight(bool &val);
        void listener_spotlight_dist(float &val);
    
        void listener_pulse(bool &val);
        void listener_do_wrap(bool &val);
        void listener_look_ahead(float &val);
        void listener_col_start(ofVec4f &val);
        void listener_col_end(ofVec4f &val);
    
        void listener_texture(bool &val);
        void listener_line_width(int &val);
        void listener_do_stripes(bool &val);
    
};
