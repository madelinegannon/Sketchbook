#include "SensorController.h"


//--------------------------------------------------------------
SensorController::SensorController(){}

//--------------------------------------------------------------
SensorController::~SensorController(){}

//--------------------------------------------------------------
void SensorController::setup(bool _use_live)
{
    setup_gui();
    
    use_live = _use_live;
    if (!use_live){
        setup_canned_data(filenames[0]);
    }
    else{
        
    }
}

//--------------------------------------------------------------
void SensorController::update()
{
    if (!use_live){
        int frame_count = end_frame - start_frame;
        counter = (counter + 1) % frame_count;
    }
 
}

//--------------------------------------------------------------
void SensorController::draw()
{
    if (!use_live){
        canned[counter]->draw();
    }

}

//--------------------------------------------------------------
ofNode SensorController::get_attachment_node()
{
     if (!use_live){
         return canned[counter]->joints[Skeleton::Joint::SPINE_MID];
     }
    else
        return ofNode();
}

//--------------------------------------------------------------
ofNode SensorController::get_target_node()
{
    if (!use_live){
        ofNode head = canned[counter]->joints[Skeleton::Joint::HEAD];
        ofNode hand_L = canned[counter]->joints[Skeleton::Joint::HAND_LEFT];
        ofNode hand_R = canned[counter]->joints[Skeleton::Joint::HAND_RIGHT];
        if (hand_L.getGlobalPosition().z > head.getGlobalPosition().z)
            return hand_L;
        else if (hand_R.getGlobalPosition().z > head.getGlobalPosition().z)
            return hand_R;
        else
            return ofNode();
    }
    else
        return ofNode();
}

//--------------------------------------------------------------
void SensorController::setup_gui()
{
    params.setName("Sensor_Controller");
    params.add(attach_geom.set("Attach_Geometry", false));
    params.add(attach_target.set("Attach_Target", false));
    
//    attach_geom.addListener(this, &SensorController::listener_attach_geom);
//    attach_target.addListener(this, &SensorController::listener_attach_target);
}

//--------------------------------------------------------------
void SensorController::listener_attach_geom(bool &val)
{
    if (val){
        attach_target.set(false);
    }
}

//--------------------------------------------------------------
void SensorController::listener_attach_target(bool &val)
{
    if (val){
        attach_geom.set(false);
    }
}

//--------------------------------------------------------------
void SensorController::setup_canned_data(string filename)
{
    if (result.open(filename))
    {
        // parse into bodies
        if (result.isArray()) {
            
            // set the start and end points for the recorded skeletons
            if (filename == "example_kinectron_skeleton_wave_walk.json"){
                start_frame = 250;
                end_frame = 1300;
                
                world_offset.set (-2500, 0, 1250);
            }
            else{
                start_frame = 0;
                end_frame = result.size();
            }

            // import the skeleton from the json file
            for (Json::ArrayIndex i = start_frame; i < end_frame; ++i) {
                const Json::Value& id = result[i]["bodyIndex"];
                const Json::Value& id_tracking = result[i]["trackingId"];
                const Json::Value& is_tracked = result[i]["tracked"];
                const Json::Value& timestamp = result[i]["record_timestamp"];
                const Json::Value& hand_state_L = result[i]["leftHandState"];
                const Json::Value& hand_state_R = result[i]["rightHandState"];
                
                const Json::Value& joints = result[i]["joints"];
                
                vector<ofNode> temp;
                float scalar;
                if (convert_mm)
                    scalar = 1000;
                else
                    scalar = 1;
                for (Json::ArrayIndex j = 0; j < joints.size(); ++j)
                {
                    
                    float x = joints[j]["cameraX"].asFloat() * scalar;
                    float y = joints[j]["cameraY"].asFloat() * scalar;
                    float z = joints[j]["cameraZ"].asFloat() * scalar;
                    float qx = joints[j]["orientationX"].asFloat();
                    float qy = joints[j]["orientationY"].asFloat();
                    float qz = joints[j]["orientationZ"].asFloat();
                    float qw = joints[j]["orientationW"].asFloat();
                    
                    ofNode joint;
                    joint.setGlobalOrientation(ofQuaternion(qx, qy, qz, qw));
                    joint.setGlobalPosition(ofVec3f(x, y, z));
                    
                    if (convert_world_coords) {
                        // convert to from kinect to robot's coordinate system
                        // Kinect camera space coordinates: https://medium.com/@lisajamhoury/understanding-kinect-v2-joints-and-coordinate-system-4f4b90b9df16
                        
                        //                        joint.pan(90); //rotate 90 about the Y
                        //                        joint.tilt(90); //rotate 90 about the X
                        
                        float _x = joint.getGlobalPosition().z;
                        float _y = joint.getGlobalPosition().x;
                        float _z = joint.getGlobalPosition().y;
                        
                        joint.setGlobalPosition(_x+world_offset.x, _y+world_offset.y, _z+world_offset.z);
                    }
                    
                    temp.push_back(joint);
                }
                
                canned.push_back(new Skeleton());
                canned.back()->id_frame = id.asInt();
                canned.back()->id_tracking = id_tracking.asString();
                canned.back()->is_tracked = is_tracked.asBool();
                canned.back()->timestamp = timestamp.asInt();
                canned.back()->hand_state_L = hand_state_L.asBool();
                canned.back()->hand_state_R = hand_state_R.asBool();
                for (auto & j : temp)
                    canned.back()->joints.push_back(j);
            }
        }
    }
    else{
        ofLogError("SensorController::setup_canned_data")  << "Failed to parse JSON ... Check your filename" << endl;
        return 1;
    }
    
    // quickly check that everything is loaded properly
    cout << "Canned Data Successfully Loaded: " << endl;
    cout << canned[0]->toString() << endl;
}

//--------------------------------------------------------------
void SensorController::update_canned_data(){
    
}

