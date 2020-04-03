#pragma once

#include "ofMain.h"
#include "ofxGui.h"

/// Pixelates an image or webcam stream with a few parameters:
/// – number of cells in X and Y
/// – width of cells in X and Y
/// – resizes image/stream in X and Y

/// Converts pixelated image to a fixed number of colors
/// ... good for making mosaics

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
		
    
    // Webcam/Image stuff
    ofVideoGrabber vidGrabber;
    ofImage img, vid_img;
    
    
    // Pixelator
    ofRectangle base_rect;
    void draw_cells(int x, int y);
    ofColor get_avg_col(ofRectangle * _cell, ofPixels * _pix);
    
    
    // Tiliing
    
    // load preset tile colors
    vector<ofColor> colors;
    ofColor col_0 = ofColor(143, 164, 191); // medium blue
    ofColor col_1 = ofColor(176, 198, 217); // light blue
    ofColor col_2 = ofColor( 57,  77,  89); // dark blue
    ofColor col_3 = ofColor(166, 158, 151); // light tan
    ofColor col_4 = ofColor(140, 123, 108); // tan
    ofColor col_5 = ofColor(191, 191, 189); // medium gray
    
    // return the tile color that most similar to the color of a cell
    ofColor get_closest_color(ofRectangle * _cell, ofColor src_col);
    
    struct Tile_Count{
        int count = 0;
        vector<ofVec2f> coordinates;
        void clear(){
            count = 0;
            coordinates.clear();
        }
        string to_string(){
            string s = "count : " + ofToString(count) + "\n";
            for (auto coord : coordinates)
                s += "{" + ofToString(coord) + "}, ";
            s +="\n";
            return s;
        }
    };
    vector<Tile_Count> tile_counts;
    
    void count_tile(ofRectangle * _cell, int index);
    ofColor get_avg_col(ofImage * _texture);
    
    
    vector<ofImage*> textures;
    vector<int> tgt_brightness;
    int get_avg_brightness(ofImage* _texture);
    int get_tgt_brightness(int src_brightness);
    
    int num_tiles = 0;
    vector<ofColor> tile_tgt_colors;
    void load_tiles();
    ofImage* get_closest_texture(ofRectangle * _cell, ofColor src_col);
    
    
    // GUI
    void setup_gui();
    ofxPanel panel;
    ofParameter<bool> use_webcam;
    ofParameterGroup image;
    ofParameter<int> x_resize;
    ofParameter<int> y_resize;
    
    ofParameterGroup grid;
    ofParameter<bool> square_grid;
    ofParameter<int> x_grid;
    ofParameter<int> y_grid;
    
    vector<ofxColorSlider*> tgt_colors;
    
};
