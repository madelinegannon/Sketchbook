#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Particle.h"

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

    vector<Particle> particles;

    vector<ofPolyline> lines;
    void create_flow_field(ofRectangle bounds, int num_particles);
    void update_flow_field();
    void draw_flow_field();
    float noise_z = 0;

    vector<ofRectangle> cells;
    ofRectangle bounds;
    void create_grid(int rows, int cols, float cell_size);
    void draw_grid();

    vector<glm::vec3> vectors;
    void create_vector_grid(int rows, int cols, float cell_size);
    void update_vector_grid();
    void draw_vector_grid();

    void setup_gui();
    bool show_gui = true;
    
    ofxPanel panel;
    ofParameterGroup params_grid;
    ofParameter<bool> show_grid = true;
    ofParameter<bool> ultra_low_res = true;
    ofParameter<bool> low_res = false;
    ofParameter<bool> high_res = false;
    ofParameter<int> rows = 10;
    ofParameter<int> cols = 10;
    ofParameter<int> cell_size = 20;
    

    ofParameterGroup params_field;
    ofParameter<bool> show_vectors = true;
    ofParameter<bool> pause_field = false;
    ofParameter<float> noise_scalar;
    ofParameter<float> noise_speed;
    
    ofParameterGroup params_particles;
    ofParameter<bool> show_particles = true;
    ofParameter<int> num_lines = 100;
    ofParameter<int> max_length = 100;
    ofParameter<float> speed_scalar = 1;
    ofParameter<float> max_speed = 4;
    
    void on_update_resolution_ultra_low(bool &val);
    void on_update_resolution_low(bool &val);
    void on_update_resolution_high(bool &val);
    void on_update_grid(int &val);
    void on_update_particles(float &val);
    
    enum Resolution{
        ULTRA_LOW,
        LOW,
        HIGH,
    }; Resolution res_mode = ULTRA_LOW;
    
    enum Output{
        PIXELS,
        POLYLINES,
        DEBUG,
    }; Output draw_mode = POLYLINES;
};
