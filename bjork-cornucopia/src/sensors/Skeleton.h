#pragma once

#include "ofMain.h"


class Skeleton  {
	public:
        Skeleton() {};
        ~Skeleton() {};

        void setup(int _id_frame);
        void update();
        void draw();
    
        string toString();    

        int id_frame, timestamp;
        string id_tracking;
    
        bool is_tracked, hand_state_L, hand_state_R;
    
        vector<ofNode> joints;
    
        // joint index ordering from Kinectron {https://kinectron.github.io/docs/api2.html}
        enum Joint{
            SPINE_BASE,
            SPINE_MID,
            NECK,
            HEAD,
            SHOULDER_LEFT,
            ELBOW_LEFT,
            WRIST_LEFT,
            HAND_LEFT,
            SHOULDER_RIGHT,
            ELBOW_RIGHT,
            WRIST_RIGHT,
            HAND_RIGHT,
            HIP_LEFT,
            KNEE_LEFT ,
            ANKLE_LEFT,
            FOOT_LEFT,
            HIP_RIGHT,
            KNEE_RIGHT,
            ANKLE_RIGHT,
            FOOT_RIGHT,
            SPINE_SHOULDER,
            HAND_TIP_LEFT,
            THUMB_LEFT,
            HAND_TIP_RIGHT,
            THUMB_RIGHT
        };
};
