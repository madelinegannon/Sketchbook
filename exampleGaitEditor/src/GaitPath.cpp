
//
//  GaitPath.cpp
//  exampleGaitEditor
//
//  Created by mad on 5/1/18.
//

#include "GaitPath.h"

//--------------------------------------------------------------
GaitPath::GaitPath(){
    
}

//--------------------------------------------------------------
void GaitPath::setup(int x, int y, int width, int height){
    window.set(x, y, width, height);
    ofPath::setFilled(false);
    ofPath::setStrokeWidth(2);
}


//--------------------------------------------------------------
void GaitPath::draw(){
    ofPushStyle();
    
    ofNoFill();
    
    ofSetColor(ofColor::whiteSmoke);
    ofDrawRectangle(window);
    
    
    ofPushMatrix();
    ofTranslate(window.x, window.y);
    
    ofPath::draw();
    
    vector<ofPath::Command> commands;
    commands = getCommands();
    
    for (int j = 1; j < commands.size(); j++) {
        
        float t = ofMap(255-(j*j*10), 255, 0, 0,1);
        ofColor c = ofColor::orange;
        ofSetColor(c.getLerped(ofColor::aqua, t));
        
        // draw the control point and its handle
        ofCircle(commands[j].to, 8);
        ofCircle(commands[j].cp2, 5);
        
        // draw the line to its control point handle
        ofLine(commands[j].to , commands[j].cp2);
        
        // draw the first control pont handle to the previous CP
        if (j > 0){
            t = ofMap(255-((j-1)*(j-1)*10), 255, 0, 0,1);
            ofColor c = ofColor::orange;
            ofSetColor(c.getLerped(ofColor::aqua, t));
            ofCircle(commands[j].cp1, 5);
            ofLine(commands[j].cp1 , commands[j-1].to);
        }
        // go back and draw the first CP (it's a duplicate)
        if (j == 1){
            ofCircle(commands[0].to, 8);
            ofDrawBitmapString("0", commands[0].to.x+5, commands[0].to.y+5);
        }
        
        // show the index
        ofDrawBitmapString(ofToString(j), commands[j].to.x+5, commands[j].to.y+5);
    }
  
    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void GaitPath::handleMousePressed(int x, int y, int button){

    if (isValidPoint(x,y) && !isHovering){
        add(x, y);
    }
}

//--------------------------------------------------------------
void GaitPath::handleMouseDragged(int x, int y, int button){
    
    if (isValidPoint(x,y) && isHovering){
            updateClosestPoint(x, y);
    }
}

//--------------------------------------------------------------
void GaitPath::handleDelete(){
    
    if (isHovering){
        removeControlPoint();
    }
}

//--------------------------------------------------------------
void GaitPath::add(int x, int y){
    
    ofPoint p0 = screenToWindow(x, y);
    ofPoint cp1, cp2;
    if (getCommands().size() > 1){
        auto prevCP = getCommands()[getCommands().size()-1];
        
        // cp1 is relative to the previous CP
        //   add, but keep in line with previous CP.cp2 (as a scaled extension)
        // cp2 goes to this CP
        
        ofPoint dir = prevCP.to - prevCP.cp2;
        cp1 = prevCP.to + dir/2;
        cp2 = p0; cp2.y += 25;
    }
    
    else{
        cp1 = p0;
        cp2 = p0;
    }
    
    ofPath::bezierTo(cp1, cp2, p0);
}


//--------------------------------------------------------------
ofPoint GaitPath::evaluate(float t){
    if (getOutline().size() > 0)
        return getOutline()[0].getPointAtPercent(t);
    else{
        ofLog(OF_LOG_ERROR, "Not enough points to evaluate.");
        return ofPoint();
    }
}

//--------------------------------------------------------------
void GaitPath::exit(){
    save();
}

//--------------------------------------------------------------
void GaitPath::load(string xmlFileName){
    
    this->xmlFileName = xmlFileName;
    
    ofxXmlSettings settings;
    if(!settings.loadFile(xmlFileName)){
        ofLog(OF_LOG_VERBOSE, "GaitPath — couldn't load GAIT settings file: " + xmlFileName);
        return;
    }

    // clear the current points stored in the path
    clear();

    settings.pushTag("gait");
    int cp_size = settings.getNumTags("control_point");
    for (int j=1; j<cp_size; j++){
        
        ofPoint p0, cp1, cp2;
        
        settings.pushTag("control_point", j);
        
        settings.pushTag("p0");
        p0.x = settings.getValue("X", 0);
        p0.y = settings.getValue("Y", 0);
        p0.z = settings.getValue("Z", 0);
        settings.popTag();
        
        settings.pushTag("cp1");
        cp1.x = settings.getValue("X", 0);
        cp1.y = settings.getValue("Y", 0);
        cp1.z = settings.getValue("Z", 0);
        settings.popTag();
        
        settings.pushTag("cp2");
        cp2.x = settings.getValue("X", 0);
        cp2.y = settings.getValue("Y", 0);
        cp2.z = settings.getValue("Z", 0);
        settings.popTag();
        
        ofPath::bezierTo(cp1, cp2, p0);
        
        settings.popTag();
    }
    
    settings.popTag();
    
    // delete the first point ... it's a weird., auto-generated duplicate
    if (getCommands().size() > 0){
        remove(0);
    }

}

//--------------------------------------------------------------
void GaitPath::save(){
    
    ofxXmlSettings settings;
    settings.addTag("gait");
    settings.pushTag("gait");
    
    vector<ofPath::Command> commands;
    commands = getCommands();
    
    for (int j = 0; j < commands.size(); j++) {
        settings.addTag("control_point");//_" + ofToString(j));
        settings.pushTag("control_point",j);//_" + ofToString(j));
        
        settings.addTag("p0");
        settings.pushTag("p0");
        settings.addValue("X", commands[j].to.x);
        settings.addValue("Y", commands[j].to.y);
        settings.addValue("Z", commands[j].to.z);
        settings.popTag();
        
        settings.addTag("cp1");
        settings.pushTag("cp1");
        settings.addValue("X", commands[j].cp1.x);
        settings.addValue("Y", commands[j].cp1.y);
        settings.addValue("Z", commands[j].cp1.z);
        settings.popTag();
        
        settings.addTag("cp2");
        settings.pushTag("cp2");
        settings.addValue("X", commands[j].cp2.x);
        settings.addValue("Y", commands[j].cp2.y);
        settings.addValue("Z", commands[j].cp2.z);
        settings.popTag();
        
        settings.popTag();
    }
    
    settings.popTag();
    settings.saveFile(xmlFileName);
}


//--------------------------------------------------------------
void GaitPath::updateWindow(int x, int y, int width, int height){
    window.set(x, y, width, height);
}


//--------------------------------------------------------------
ofPoint GaitPath::getClosestPoint(ofPoint p){
    findClosestPoint(p);
    
    if (getCommands().size() > 0 && closestPointIndex.p != -1){
        isHovering = true;
        
        if (closestPointIndex.cp1 == -1 && closestPointIndex.cp2 == -1)
            return getCommands()[closestPointIndex.p].to;
        else if (closestPointIndex.cp1 == -1)
            return getCommands()[closestPointIndex.p].cp2;
        else if (closestPointIndex.cp2 == -1)
            return getCommands()[closestPointIndex.p].cp1;
        
    }
    else{
        isHovering = false;
        return ofPoint();
    }
}

//--------------------------------------------------------------
void GaitPath::updateClosestPoint(int x, int y){
    if (getCommands().size() > 0 && isHovering){
        ofPoint p = screenToWindow(x, y);
        
        // we are near the main control point
        if(closestPointIndex.cp1 == -1 && closestPointIndex.cp2 == -1){
            getCommands()[closestPointIndex.p].to.x = p.x;
            getCommands()[closestPointIndex.p].to.y = p.y;
        }
        // we are near the main control point's handle
        else if (closestPointIndex.cp1 == -1){
            getCommands()[closestPointIndex.p].cp2.x = p.x;
            getCommands()[closestPointIndex.p].cp2.y = p.y;
        }
        // we are near the previous control point's handle
        else{
            getCommands()[closestPointIndex.p].cp1.x = p.x;
            getCommands()[closestPointIndex.p].cp1.y = p.y;
        }
    }
}

//--------------------------------------------------------------
void GaitPath::findClosestPoint(ofPoint p){
    int index = 0;
    float minDist = FLT_MAX;
    
    vector<ofPath::Command> commands;
    commands = getCommands();
    
    int c = 0;
    for (auto &command : commands){
        
        float dist0 = p.squareDistance(command.to);     // the current control point
        float dist1 = p.squareDistance(command.cp1);    // the previous control point's handle
        float dist2 = p.squareDistance(command.cp2);    // the current control point's handle
        
        // check the distance for the control points too
        if (dist0 < minDist || dist1 < minDist || dist2 < minDist){
            
            // get the smallest distance
            minDist = min({dist0, dist1, dist2});
            
            // if we are within a certain radius
            float maxRadius = 25;
            if (minDist < maxRadius*maxRadius){
                if (minDist == dist0){
                    closestPointIndex.p = c;
                    closestPointIndex.cp1 = -1;
                    closestPointIndex.cp2 = -1;
                }
                else if (minDist == dist1){
                    closestPointIndex.p = c;
                    closestPointIndex.cp1 =  1;
                    closestPointIndex.cp2 = -1;
                }
                else if (minDist == dist2){
                    closestPointIndex.p = c;
                    closestPointIndex.cp1 = -1;
                    closestPointIndex.cp2 =  1;
                }
                isHovering = true;
            }
            // otherwise, set everything to -1
            else{
                closestPointIndex.p = -1;
                closestPointIndex.cp1 = -1;
                closestPointIndex.cp2 = -1;
                isHovering = false;
            }
        }
        c++;
    }
}

// remove closest control point
void GaitPath::removeControlPoint(){
    int i = closestPointIndex.p;
    if (i == -1){
        ofLog(OF_LOG_ERROR, "Cannot Remove: Your mouse is too far away from a control point.");
    }
    else{
        remove(i);
    }
}

// remove control point based on index
void GaitPath::remove(int i){
    
    ofPath temp;
    
    vector<ofPath::Command> commands;
    commands = getCommands();
    for (int j = 0; j < commands.size(); j++) {
        if (j != 0 && j != i){ // there is a bug in ofPath that creates a duplicate point at 0
            temp.bezierTo(commands[j].cp1, commands[j].cp2, commands[j].to);
        }
    }
    
    // clear this path and swap with temp
    clear();
    append(temp);
}

//--------------------------------------------------------------
ofPoint GaitPath::screenToWindow(int x, int y){
    // convert screen-space point relative to top left corner of window
    return ofPoint(x - window.x, y - window.y);
}


