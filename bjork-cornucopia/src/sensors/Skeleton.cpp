#include "Skeleton.h"


//--------------------------------------------------------------
void Skeleton::setup(int _id_frame) {

}

//--------------------------------------------------------------
void Skeleton::update() {

}

//--------------------------------------------------------------
void Skeleton::draw() {

    ofPushStyle();
    ofNoFill();
    
    for (int i = 0; i < joints.size(); i++) {
        
        // ignore the hand nodes for now
        if (i != Joint::HAND_TIP_LEFT  && i != Joint::THUMB_LEFT &&
            i != Joint::HAND_TIP_RIGHT && i != Joint::THUMB_RIGHT ) {
            
            if (i == Joint::HAND_LEFT) {
                if (hand_state_L == 1)
                    ofSetColor(ofColor::magenta);
                else
                    ofSetColor(ofColor::white);
            }
            else if (i == Joint::HAND_RIGHT) {
                if (hand_state_R == 1)
                    ofSetColor(ofColor::magenta);
                else
                    ofSetColor(ofColor::white);
            }
            else {
                ofSetColor(ofColor::white);
            }
            
            ofDrawBox(joints[i].getGlobalPosition(), 50 / 1.);
            joints[i].draw();
            
        }
    }
    
    ofSetLineWidth(5);
    ofSetColor(200, 100);
    // draw spine
    ofDrawLine(joints[Joint::SPINE_BASE].getGlobalPosition(), joints[Joint::SPINE_MID].getGlobalPosition());
    ofDrawLine(joints[Joint::SPINE_MID].getGlobalPosition(), joints[Joint::NECK].getGlobalPosition());
    ofDrawLine(joints[Joint::NECK].getGlobalPosition(), joints[Joint::HEAD].getGlobalPosition());
    // draw left arm
    ofDrawLine(joints[Joint::SPINE_SHOULDER].getGlobalPosition(), joints[Joint::SHOULDER_LEFT].getGlobalPosition());
    ofDrawLine(joints[Joint::SHOULDER_LEFT].getGlobalPosition(), joints[Joint::ELBOW_LEFT].getGlobalPosition());
    ofDrawLine(joints[Joint::ELBOW_LEFT].getGlobalPosition(), joints[Joint::WRIST_LEFT].getGlobalPosition());
    ofDrawLine(joints[Joint::WRIST_LEFT].getGlobalPosition(), joints[Joint::HAND_LEFT].getGlobalPosition());
    // draw right arm
    ofDrawLine(joints[Joint::SPINE_SHOULDER].getGlobalPosition(), joints[Joint::SHOULDER_RIGHT].getGlobalPosition());
    ofDrawLine(joints[Joint::SHOULDER_RIGHT].getGlobalPosition(), joints[Joint::ELBOW_RIGHT].getGlobalPosition());
    ofDrawLine(joints[Joint::ELBOW_RIGHT].getGlobalPosition(), joints[Joint::WRIST_RIGHT].getGlobalPosition());
    ofDrawLine(joints[Joint::WRIST_RIGHT].getGlobalPosition(), joints[Joint::HAND_RIGHT].getGlobalPosition());
    // draw left leg
    ofDrawLine(joints[Joint::SPINE_BASE].getGlobalPosition(), joints[Joint::HIP_LEFT].getGlobalPosition());
    ofDrawLine(joints[Joint::HIP_LEFT].getGlobalPosition(), joints[Joint::KNEE_LEFT].getGlobalPosition());
    ofDrawLine(joints[Joint::KNEE_LEFT].getGlobalPosition(), joints[Joint::ANKLE_LEFT].getGlobalPosition());
    ofDrawLine(joints[Joint::ANKLE_LEFT].getGlobalPosition(), joints[Joint::FOOT_LEFT].getGlobalPosition());
    // draw right leg
    ofDrawLine(joints[Joint::SPINE_BASE].getGlobalPosition(), joints[Joint::HIP_RIGHT].getGlobalPosition());
    ofDrawLine(joints[Joint::HIP_RIGHT].getGlobalPosition(), joints[Joint::KNEE_RIGHT].getGlobalPosition());
    ofDrawLine(joints[Joint::KNEE_RIGHT].getGlobalPosition(), joints[Joint::ANKLE_RIGHT].getGlobalPosition());
    ofDrawLine(joints[Joint::ANKLE_RIGHT].getGlobalPosition(), joints[Joint::FOOT_RIGHT].getGlobalPosition());
    
    ofPopStyle();
    
}


//--------------------------------------------------------------
string Skeleton::toString(){    
    stringstream ss;
    ss << "Skeleton: " << endl;
    ss << "\tid: " << id_frame << endl;
    ss << "\tid_tracking: " << id_tracking << endl;
    ss << "\tis_tracked: " << is_tracked << endl;
    ss << "\tNumber of Joints: " << joints.size() << endl;
    return ss.str();
}

