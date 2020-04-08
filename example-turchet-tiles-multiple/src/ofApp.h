#pragma once

#include "ofMain.h"
#include "ofxGui.h"

/**

Drawing Patterns with multiple Truchet Tiles

Next:
   - Animate between tiles
   - Integrate multi-scale
   - Make Interactive

Reference:
   Multi-Scale Truchet Patterns
   http://archive.bridgesmathart.org/2018/bridges2018-39.pdf
*/

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
		
        void setup_gui();
    
        
        struct Tile{
            ofRectangle cell;
            vector<ofPath> paths;
            ofPath spine;
            int id = 0;
            int level = 0;
            
            void draw(){
                
            }
        };
        vector<Tile> base_tiles;
        void setup_tiles();
        void draw_tile(int id);
        
    
        // keep track of rotations
        void initialize_grid();
        vector<vector<int>> rotations;
        void update_pattern();      // updates rotations and tile ids
        vector<vector<int>> tile_ids;
        int pick_tile();
    
    
        // Timer
        float curr_time = 0;
        float wait_time = 2;
    
        // GUI
    
        ofxPanel panel;
        ofParameter<bool> debug_view;
        ofParameter<bool> pause;
        ofParameter<int> x_grid;
        ofParameter<int> y_grid;
        ofParameterGroup tile_params;
        ofParameter<bool> tile_id_0;
        ofParameter<bool> tile_id_1;
        ofParameter<bool> tile_id_2;
        ofParameter<bool> tile_id_3;
        ofParameter<int> tile_0_weight;
        ofParameter<int> tile_1_weight;
        ofParameter<int> tile_2_weight;
        ofParameter<int> tile_3_weight;
    
        void grid_listener(int &val);
        void tile_id_listener(bool &val);
        void tile_weight_listener(int &val);
};
