//
//  ofxTLGuiKeyframes.h
//  exampleTimeline
//
//  Created by mad on 4/27/18.
//

#pragma once

#include "ofMain.h"
#include "ofxTLEmptyKeyframes.h"
#include "ofxTimeline.h"
#include "ofxGui.h"

//custom keyframe container
//inherits time and value from the super class
class ofxTLGuiKeyframe : public ofxTLKeyframe {
public:
    
    float value_float;
    string name;
    vector<string> names;
    vector<float> values;
    
};

class ofxTLGuiKeyframes : public ofxTLEmptyKeyframes {
  
public:
    ofxTLGuiKeyframes();
    virtual ~ofxTLGuiKeyframes();
    
    void setParameterGroup(ofParameterGroup &params);
    void updateParameterGroup();
    void updateParameterGroupAtTime(unsigned long long sampleTime);
    
    //draw your keyframes
    virtual void draw();
    
    virtual bool mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseMoved(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
    virtual void mouseReleased(ofMouseEventArgs& args, long millis);
    
    //keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
    virtual void keyPressed(ofKeyEventArgs& args);
    
    //how to get colors from our example, linearly interpolated RGB
    //ofColor getCurrentColor();
    //ofColor getColorAtTime(unsigned long long millis);
    
    //time range contains MIN and MAX time in milliseconds
    //valueRange is 0 at the bottom of the track, and 1 at the top
    //if you have anything other than small dots keyframes you'll want to override
    //the default behavior
    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);
    
    //return a custom name for this keyframe
    virtual string getTrackType();
    
protected:
    ofParameterGroup *params;
    
    //always return the type for your track, in our case ofxTLEmptyKeyframe;
    //this will enusre that all keyframe objects passed to this class are of this type
    virtual ofxTLKeyframe* newKeyframe();
    //load this keyframe out of xml, which is alraedy pushed to the right level
    //only need to save custom properties that our subclass adds
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    //save custom properties into the xml
    virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    
    //return keyframe at this mouse point if you have non circular keyframes
    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
    
    //responde to right clicks on keyframes
    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args);
    
    //you can responde to a keyframe about to be killed with this function right before the parent class deletes it
    virtual void willDeleteKeyframe(ofxTLKeyframe* keyframe);
    
};
