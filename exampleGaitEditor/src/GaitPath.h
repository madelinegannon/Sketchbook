//
//  ofxTLGuiKeyframes.h
//  exampleTimeline
//
//  Created by mad on 4/27/18.
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxEasing.h"


class GaitPath : public ofPath {
public:
    
    GaitPath();
    
    void setup(int x, int y, int width, int height);
    void update(int x, int y);
    void draw();
   
    void handleMousePressed(int x, int y, int button);
    void handleMouseDragged(int x, int y, int button);
    void handleDelete();
    ofPoint evaluate(float t);

    void exit();
    void load(string xmlFileName);
    void save();
    
    void updateWindow(int x, int y, int width, int height);
    void clear(){ ofPath::clear(); };
    bool isValidPoint(int x, int y) { return window.inside(x, y); }
 
    bool isHovering = false;
    ofPoint getClosestPoint(ofPoint p);
    
    ofRectangle window;
    ofPoint screenToWindow(int screenX, int screenY);
    
protected:
    void add(int x, int y);
    void updateClosestPoint(int x, int y);
    void removeControlPoint(); // remove the closestPointIndex
    void findClosestPoint(ofPoint p);
    struct CP {
        int p   = -1;
        int cp1 = -1;
        int cp2 = -1;
    } closestPointIndex;
    
    string xmlFileName = "gait.xml";
    
    void remove(int i);

};

