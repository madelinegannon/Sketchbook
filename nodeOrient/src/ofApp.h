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
    
    
    // reorient the body to look at the lookAt node
    ofNode pivot;
    ofNode lookAt;
    ofBoxPrimitive body;
    
    ofNode limb;
    ofNode toe;
    ofPath limbArc;
    
    float rotAngle =0;
    float timerMin = 0;
    float timerMax = 1000.;
    float rotMin = -.5;
    float rotMax =  .5;
    
    float yaw, pitch, roll, height;
    
    ofEasyCam cam;
    
    
    class Motor : public ofNode{
        
    public:
        
        Motor(){};
        
        ofNode motorZero; // holds the ZERO position of the motor
        float radius = 20;
        
        ofVec3f centroid;
        ofVec3f upVec = ofVec3f(0,0,1);
        float rotAngle =0;
        
        ofPath rotationArc;
        
        void setup(){
            
            rotationArc.setColor(ofColor::yellow);
        };
        
        void update(ofVec3f dir){

            
        };
        
        // Get the angle bewtween the motor's Zero and the toe position.
        // NOTE: all vectors should be normalized
        // NOTE: needs to be a signed angle (Ghost Example is -.5 rad to .5 rad)
        void calcAngle(ofVec3f pos){
            
            ofVec3f toGround = (getGlobalPosition() - pos).getNormalized(); // should just be a down vector
            ofVec3f toZero = (getGlobalPosition() - motorZero.getGlobalPosition()).getNormalized(); // should always be an angled vector
            
            //rotAngle = toZero.angle(toGround); // needs to go 360º
            
            // NOT CORRECT
//            ofMatrix4x4 mat;
//            mat.setTranslation(toZero);
//            mat *= getGlobalOrientation().conj();
//            toZero = mat.getTranslation();
//            toZero.normalize();
            
            toZero.
            
            // try online code fot atan2 https://math.stackexchange.com/questions/878785/how-to-find-an-angle-in-range0-360-between-2-vectors
            ofVec3f v1 = toGround;
            ofVec3f v0 = toZero;
            
            // NEED TO ROTATE v0 & v1 onto XZ plane
            
            float dot = v0.x*v1.x + v0.z*v1.z;      // dot product
            float det = v0.x*v1.z - v0.z*v1.x;      // determinant
            rotAngle = ofRadToDeg(atan2(det, dot));  // atan2(y, x) or atan2(sin, cos)
            
            
            cout << "toGround: "<< toGround << ", toZero: " << toZero << endl << ", rotAngle: " << rotAngle << endl;
           
            // reset the rotation arc
            rotationArc.clear();
            rotationArc.arc(ofVec3f(), 20, 20, 0, -rotAngle);
            
        };
        
        
        void draw(){
            
            ofPushStyle();
            motorZero.draw();
            
            
            ofPopStyle();
            
            ofPushStyle();
            ofPushMatrix();
            //ofMultMatrix(this->getGlobalTransformMatrix());
            ofTranslate(this->getGlobalPosition());
            
            
            ofRotateX(90);
            ofSetColor(ofColor::red);
//            ofDrawCircle(0,0,0,radius);
//            ofSetColor(ofColor::white);
//            ofDrawLine(0,0,0,0,motorZero.getZ(),0);
//
            ofFill();
            ofRotateZ(90);
            //ofSetColor(ofColor::yellow);
            rotationArc.draw();
            
            
            ofPopMatrix();
            ofPopStyle();
        };
        
        
    };
    
    Motor motor;
		
};
