#pragma once

#include "ofMain.h"

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
    
        ofImage img;
        int img_width = 640;
        int img_height = 480;
        vector<ofRectangle> cells;
    
        int x_grid = 8;
        int y_grid = 8;
        
        void make_grid(int x, int y, int x_num, int y_num, int width, int height);
        void draw_grid();
    
        
    
        struct Cell{
            ofRectangle bounds;
            ofColor avg_col;
            int avg_brightness;
            
        };
    
        vector<Cell> _cells;
        void set_avg_color(Cell* cell);
        void subdivide(Cell* cell);
};
