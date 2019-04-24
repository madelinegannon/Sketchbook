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
            // hightlight the selected keyframe
            ofColor col = (isKeyframeSelected(emptyKeyframe)) ?  ofColor(10,204,201) : ofColor(255,231,1);
            ofSetColor(col);
//            ofSetColor(10,204,201);
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
       
       // update each slider with the current parameter  value
       // update each bool parameter
       for (int j=0; j<key->params_bool.size(); j++){
           params->get(key->params_bool[j].first).cast<bool>().set(key->params_bool[j].second);
       }
       
       // update each float parameter
       for (int j=0; j<key->params_float.size(); j++){
           params->get(key->params_float[j].first).cast<float>().set(key->params_float[j].second);
       }
       
       // update each ofVec3f parameter
       for (int j=0; j<key->params_ofVec3f.size(); j++){
           params->get(key->params_ofVec3f[j].first).cast<ofVec3f>().set(key->params_ofVec3f[j].second);
       }
       
       return;
    }
    
    //sampling after the last we return the last
    if(keyframes[keyframes.size()-1]->time <= sampleTime){
        
        ofxTLGuiKeyframe* key  = (ofxTLGuiKeyframe*)keyframes[keyframes.size()-1];
        
        // update each slider with the current float value
        // update each bool parameter
        for (int j=0; j<key->params_bool.size(); j++){
            params->get(key->params_bool[j].first).cast<bool>().set(key->params_bool[j].second);
        }
        
        // update each float parameter
        for (int j=0; j<key->params_float.size(); j++){
            params->get(key->params_float[j].first).cast<float>().set(key->params_float[j].second);
        }
        
        // update each ofVec3f parameter
        for (int j=0; j<key->params_ofVec3f.size(); j++){
            params->get(key->params_ofVec3f[j].first).cast<ofVec3f>().set(key->params_ofVec3f[j].second);
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
            
            // update each slider with the interpolated parameter value
            
            // update each bool parameter (don't interpolate, just trigger next when close)
            for (int j=0; j<prevKey->params_bool.size(); j++){
                if (alpha > .95){
                    params->get(nextKey->params_bool[j].first).cast<bool>().set(nextKey->params_bool[j].second);
                }
            }
            
            // update each float parameter
            for (int j=0; j<prevKey->params_float.size(); j++){
                float val = ofLerp(prevKey->params_float[j].second, nextKey->params_float[j].second, alpha);
                params->get(nextKey->params_float[j].first).cast<float>().set(val);
            }
            
            // update each ofVec3f parameter
            for (int j=0; j<prevKey->params_ofVec3f.size(); j++){
                float x = ofLerp(prevKey->params_ofVec3f[j].second.x, nextKey->params_ofVec3f[j].second.x, alpha);
                float y = ofLerp(prevKey->params_ofVec3f[j].second.y, nextKey->params_ofVec3f[j].second.y, alpha);
                float z = ofLerp(prevKey->params_ofVec3f[j].second.z, nextKey->params_ofVec3f[j].second.z, alpha);
                
                params->get(nextKey->params_ofVec3f[j].first).cast<ofVec3f>().set(ofVec3f(x,y,z));
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
    
    // store the current value of each parameter
    for (int i=0; i<params->size(); i++){
        
//        cout << "type id: " << params->getType(i) << endl;
        
        if (params->getType(i) == "11ofParameterIbE"){
//            cout << "adding a new BOOL param: " << params->get(i).getName() << ", " << params->get(i).cast<bool>() << endl;
            newKey->params_bool.push_back(make_pair(params->get(i).getName(), params->get(i).cast<bool>()));
            
        }
        else if(params->getType(i) == "11ofParameterIfE"){
//            cout << "adding a new FLOAT param: " << params->get(i).getName() << ", " << params->get(i).cast<float>() << endl;
            newKey->params_float.push_back(make_pair(params->get(i).getName(), params->get(i).cast<float>()));
        }
        else if (params->getType(i) == "11ofParameterI7ofVec3fE"){
//            cout << "adding a new VEC3 param: " << params->get(i).getName() << ", " << params->get(i).cast<ofVec3f>() << endl;
            newKey->params_ofVec3f.push_back(make_pair(params->get(i).getName(), params->get(i).cast<ofVec3f>()));
        }
    }
    return newKey;
}

void ofxTLGuiKeyframes::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLGuiKeyframe* emptyKey = (ofxTLGuiKeyframe*)key;

    // restore each float parameter
    for (int i=0; i<emptyKey->params_float.size(); i++){
        emptyKey->params_float[i].second = xmlStore.getValue(emptyKey->params_float[i].first, 0.);
    }
    
    // restore each bool parameter
    for (int i=0; i<emptyKey->params_bool.size(); i++){
        emptyKey->params_bool[i].second = xmlStore.getValue(emptyKey->params_bool[i].first, 0);
    }

    // restore each ofVec3f parameter (which is saved as a string)
    for (int i=0; i<emptyKey->params_ofVec3f.size(); i++){
        string s = xmlStore.getValue(emptyKey->params_ofVec3f[i].first, "0, 0, 0");
        emptyKey->params_ofVec3f[i].second = ofFromString<ofVec3f>(s);
    }
}

void ofxTLGuiKeyframes::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLGuiKeyframe* emptyKey = (ofxTLGuiKeyframe*)key;
    for (int i=0; i<emptyKey->params_float.size(); i++){
        xmlStore.addValue(emptyKey->params_float[i].first,emptyKey->params_float[i].second);
    }
    for (int i=0; i<emptyKey->params_bool.size(); i++){
        xmlStore.addValue(emptyKey->params_bool[i].first,emptyKey->params_bool[i].second);
    }    
    // store each ofVec3f parameter as a string
    for (int i=0; i<emptyKey->params_ofVec3f.size(); i++){
        xmlStore.addValue(emptyKey->params_ofVec3f[i].first, ofToString(emptyKey->params_ofVec3f[i].second));
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

