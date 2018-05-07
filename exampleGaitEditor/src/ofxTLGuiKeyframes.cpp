//
//  ofxTLGuiKeyframes.cpp
//  exampleTimeline
//
//  Created by mad on 4/27/18.
//

#include "ofxTLGuiKeyframes.h"

ofxTLGuiKeyframes::ofxTLGuiKeyframes(){
    
}

ofxTLGuiKeyframes::~ofxTLGuiKeyframes(){
    
}

void ofxTLGuiKeyframes::setParameterGroup(ofParameterGroup &params){
    
    this->params = &params;
    
}

void ofxTLGuiKeyframes::draw(){
    
    ofPushStyle();
    ofFill();
    //show the current color as background based on the playhead position
    ofSetColor(getCurrentColor(), 100);
    ofRect(bounds);

    ofSetRectMode(OF_RECTMODE_CENTER);
    for(int i = 0; i < keyframes.size(); i++){
        //make sure it's on screen
        if(isKeyframeIsInBounds(keyframes[i])){
            //we know the type because we created it in newKeyframe()
            //so we can safely cast
            ofxTLGuiKeyframe* emptyKeyframe = (ofxTLGuiKeyframe*)keyframes[i];
            if(hoverKeyframe == emptyKeyframe){
                ofSetColor(timeline->getColors().highlightColor);
            }
            else if(isKeyframeSelected(emptyKeyframe)){
                ofSetColor(timeline->getColors().textColor);
            }
            else{
                ofSetColor(timeline->getColors().keyColor);
            }
            
            ofVec2f screenPoint = screenPositionForKeyframe(emptyKeyframe);
            
            ofPushMatrix();
            ofTranslate(screenPoint);
            ofRotate(45);
            ofSetColor(10,204,201);
            ofDrawRectangle(0,0,10,10);
            ofSetColor(255,231,1);
            ofDrawRectangle(0,0,7,7);
            ofPopMatrix();

            ofSetLineWidth(1);
            ofDrawLine(screenPoint.x,bounds.y, screenPoint.x, bounds.y+bounds.height);
        }
    }
    
    ofPopStyle();
    
}

void ofxTLGuiKeyframes::updateParameterGroup(){
     if(keyframes.size() > 0)
         updateParameterGroupAtTime(timeline->getCurrentTimeMillis());
}


void ofxTLGuiKeyframes::updateParameterGroupAtTime(unsigned long long sampleTime){
    // log error if there are no frames
    if(keyframes.size() == 0){
        ofLog(OF_LOG_ERROR, "Error: No Keyframes have been set.");
        return;
    }
    
    // update to the first keyframe, there there is just one keyframe
   if(keyframes.size() == 1 || keyframes[0]->time >= sampleTime){
        
        ofxTLGuiKeyframe* key  = (ofxTLGuiKeyframe*)keyframes[0];
        
        // update each slider with the current float value
        for (int i=0; i<params->size(); i++){
            params->get(i).cast<float>().set(key->values[i]);
        }
        return;
    }
    
    //sampling after the last we return the last
   if(keyframes[keyframes.size()-1]->time <= sampleTime){
        
        ofxTLGuiKeyframe* key  = (ofxTLGuiKeyframe*)keyframes[keyframes.size()-1];
        
        // update each slider with the current float value
        for (int i=0; i<params->size(); i++){
            params->get(i).cast<float>().set(key->values[i]);
        }
        return;
    }

        //now we are somewhere in between, search
        //keyframes will always be sorted
        for(int i = 1; i < keyframes.size(); i++){
            if(keyframes[i]->time >= sampleTime){
                ofxTLGuiKeyframe* prevKey  = (ofxTLGuiKeyframe*)keyframes[i-1];
                ofxTLGuiKeyframe* nextKey  = (ofxTLGuiKeyframe*)keyframes[i];
                
                float alpha = ofMap(sampleTime, prevKey->time, nextKey->time, 0, 1.0);

                // update each slider with the interpolated float value
                for (int i=0; i<params->size(); i++){
                    
                    float val = ofLerp(prevKey->values[i], nextKey->values[i], alpha);
                    params->get(i).cast<float>().set(val);
                    
                }
                break;
            }
        }    
}


bool ofxTLGuiKeyframes::mousePressed(ofMouseEventArgs& args, long millis){
    return ofxTLKeyframes::mousePressed(args, millis);
}

void ofxTLGuiKeyframes::mouseMoved(ofMouseEventArgs& args, long millis){
    ofxTLKeyframes::mouseMoved(args, millis);
}

void ofxTLGuiKeyframes::mouseDragged(ofMouseEventArgs& args, long millis){
    ofxTLKeyframes::mouseDragged(args, millis);
}

void ofxTLGuiKeyframes::mouseReleased(ofMouseEventArgs& args, long millis){
    ofxTLKeyframes::mouseReleased(args, millis);
}

//keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
void ofxTLGuiKeyframes::keyPressed(ofKeyEventArgs& args){
    ofxTLKeyframes::keyPressed(args);
}

void ofxTLGuiKeyframes::regionSelected(ofLongRange timeRange, ofRange valueRange){
    //you can override the default to select other things than just dots
    ofxTLKeyframes::regionSelected(timeRange, valueRange);
}

string ofxTLGuiKeyframes::getTrackType(){
    return "GuiKeyframes";
}

ofxTLKeyframe* ofxTLGuiKeyframes::newKeyframe(){
    
    ofxTLGuiKeyframe* newKey = new ofxTLGuiKeyframe();
    
    // store the current value of each slider
    for (int i=0; i<params->size(); i++){
        newKey->names.push_back(params->get(i).getName());
        newKey->values.push_back(params->get(i).cast<float>());
    }
    
    return newKey;
}

void ofxTLGuiKeyframes::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLGuiKeyframe* emptyKey = (ofxTLGuiKeyframe*)key;
    
    for (int i=0; i<emptyKey->names.size(); i++){
        emptyKey->values[i] = xmlStore.getValue(emptyKey->names[i], 0.);
    }

}

void ofxTLGuiKeyframes::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLGuiKeyframe* emptyKey = (ofxTLGuiKeyframe*)key;
     for (int i=0; i<emptyKey->values.size(); i++){
         xmlStore.addValue(emptyKey->names[i],emptyKey->values[i]);
     }
}

ofxTLKeyframe* ofxTLGuiKeyframes::keyframeAtScreenpoint(ofVec2f p){
    return ofxTLKeyframes::keyframeAtScreenpoint(p);
}

void ofxTLGuiKeyframes::selectedKeySecondaryClick(ofMouseEventArgs& args){
    //you can make a popup window start here
    //    timeline->presentedModalContent(this);
    //and then when you want to get rid of it call somewhere else
    //    timeline->dismissedModalContent();
    //this will lock all other timeline interaction and feed all things into your track
    //so feel free to draw out of bounds, but don't go off the screen or out of the timeline
}


void ofxTLGuiKeyframes::willDeleteKeyframe(ofxTLKeyframe* keyframe){
    //do any cleanup before this keyframe gets hosed
}

