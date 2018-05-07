#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxTimeline.h"
#include "ofxTLGuiKeyframes.h"
#include "GaitPath.h"
#include "ofxEasing.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

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
    
        ////////////////////////////////////
        // GUI Interface
        void setupGui();
        bool showGui = true;
        bool toggleFullscreen = false;
        ofxPanel panel;
        ofParameterGroup params;
    
        ofParameterGroup param_00;
        ofParameter<float> param_00_a;
        ofParameter<float> param_00_b;
    
        ofParameterGroup param_01;
        ofParameter<float> param_01_a;
        ofParameter<float> param_01_b;
    
        ofParameterGroup param_02;
        ofParameter<float> param_02_a;
        ofParameter<float> param_02_b;
    
        ofParameterGroup param_03;
        ofParameter<float> param_03_a;
        ofParameter<float> param_03_b;
    
        ////////////////////////////////////
        // Timeline Interface
        void setupTimeline();
        ofxTimeline timeline;
    
        ofxTLGuiKeyframes *keyframes_param_00;
        ofxTLGuiKeyframes *keyframes_param_01;
        ofxTLGuiKeyframes *keyframes_param_02;
        ofxTLGuiKeyframes *keyframes_param_03;
        vector<ofxTLGuiKeyframes *> keyframes;
    
        ////////////////////////////////////
        // Gait Editor
        void setupGaitEditor();
        GaitPath gait_00;
        string gait_filename = "gait.xml";
        ofPoint hoverPt;
    
        float getGaitTiming();
        int cycleTime = 0;
        int cycleTimer = 0;
    
        // Gait GUI
        ofxPanel panel_gait;
        ofParameterGroup params_gait;
        ofParameterGroup easings;
        ofParameter<int> easingFunction;
        ofParameter<string> easingName;
    
        // easing for different interpolations using ofxTween
        enum Easing {
            CIRCULAR,
            LINEAR,
            QUADRATIC,
            CUBIC,
            QUARTIC,
            QUINTIC,
            EXPONENTIAL,
            SINE,
            BACK,
            BOUNCE,
            ELASTIC
        };
        vector<string> easingNames{
            "   CIRCULAR",
            "   LINEAR",
            "   QUADRATIC",
            "   CUBIC",
            "   QUARTIC",
            "   QUINTIC",
            "   EXPONENTIAL",
            "   SINE",
            "   BACK",
            "   BOUNCE",
            "   ELASTIC"
        };
    
        enum EasingType {
            IN,
            OUT,
            IN_OUT
        };
    
        ofxTween::ofxEasingType easingType;
        ofxEasing* easing;
        void updateEasing(int &easingFunction);
};
