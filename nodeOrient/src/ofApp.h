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
			ofColor color = ofColor::yellow;
			color.a = 100;
            rotationArc.setColor(color);
        };
        
        void update(ofVec3f rotDir){

			// rotDir is already normalized on the XZ plane
			// local upDir is always (0,0,1)

			rotAngle = angleBetween(ofVec2f(rotDir.x, rotDir.z), ofVec2f(0, 1));

			// reset the rotation arc
			rotationArc.clear();
			if (rotAngle > 0)
				rotationArc.arc(ofVec3f(), 20, 20, 0, rotAngle);
			else 
				rotationArc.arc(ofVec3f(), 20, 20, rotAngle, 0);
            
        };
        
		// from online code for atan2 https://math.stackexchange.com/questions/878785/how-to-find-an-angle-in-range0-360-between-2-vectors
		float angleBetween(ofVec2f v0, ofVec2f v1) {
			float theta, dot, det;

			v0.normalize();
			v1.normalize();

			dot = v0.x*v1.x + v0.y*v1.y;      // dot product
			det = v0.x*v1.y - v0.y*v1.x;      // determinant
			theta = ofRadToDeg(atan2(det, dot));  // atan2(y, x) or atan2(sin, cos)

			//cout << "upVec: " << v1 << ", rotDir: " << v0 << ", rotAngle: " << theta << endl;

			return theta;
		};
        
        
        void draw(){
            
            ofPushStyle();
            motorZero.draw();
            
            
            ofPopStyle();
            
            ofPushStyle();
            ofPushMatrix();
            ofMultMatrix(this->getGlobalTransformMatrix());
            //ofTranslate(this->getGlobalPosition());
            
            
            //ofRotateX(90);
            ofSetColor(ofColor::red);
//            ofDrawCircle(0,0,0,radius);
//            ofSetColor(ofColor::white);
//            ofDrawLine(0,0,0,0,motorZero.getZ(),0);
//
            ofFill();
            //ofRotateZ(90);
            //ofSetColor(ofColor::yellow);
           // rotationArc.draw();
			drawMotor();
            
            ofPopMatrix();
            ofPopStyle();
        };

		void drawMotor() {
			ofPushStyle();
			ofPushMatrix();
			
			// orient the 0° to Z+ and 90° to X+
			ofRotateY(-90);
			ofRotateX(-90);

			// draw the circle & rotation arc
			ofFill();
			rotationArc.draw();
			
			ofNoFill();
			ofSetLineWidth(2);
			ofSetColor(ofColor::yellow);
			ofDrawCircle(0,0,0,radius);

			// draw the degree tick marks and reference angles
			ofSetColor(ofColor::ghostWhite);

			int deg = 360/10;
			for (int i = 0; i < deg; i++) {
				ofPushMatrix();
				ofRotate(i*(360 / deg));
				ofTranslate(radius + 5, 0);
				if (i*(360 / deg) == 0 || i*(360 / deg) == 90 || i * (360 / deg) == 180 || i * (360 / deg) == 270) {
					if (i * (360 / deg) == 270) {
						ofDrawBitmapString(ofToString(-90), 0, 0, 0);
					}
					else
						ofDrawBitmapString(ofToString(i*(360 / deg)), 0,0,0);
					ofDrawLine(ofVec3f(-10, 0, 0), ofVec3f(3, 0, 0));
				}
				else {
					ofDrawLine(ofVec3f(-5, 0, 0), ofVec3f(-10, 0, 0));
				}

				ofPopMatrix();
			}

			// draw the rotation angle in the center
			ofDrawBitmapString(ofToString(rotAngle), 0, 0, 0);

			ofPopMatrix();
			ofPopStyle();
		};
        
        
    };
    
    Motor motor;
		
};