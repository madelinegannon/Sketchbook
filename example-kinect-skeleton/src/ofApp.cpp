#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    setup_gui();
    setup_camera();
    setup_world();

    setup_device();
}

//--------------------------------------------------------------
void ofApp::update()
{

    if (device.get_body_tracker()->get_num_bodies() > 0)
    {

        auto skeleton = device.get_body_tracker()->get_skeleton(0);

        auto pt_a = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_LEFT].position);
        auto pt_b = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_RIGHT].position);
        auto pt_c = glm::mix(pt_a, pt_b, .5);
        // ofDrawLine(pt_a, pt_b);



    }
}

//--------------------------------------------------------------
void ofApp::exit()
{
    if (device.isOpen())
        device.close();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackgroundGradient(background_inner, background_outer, OF_GRADIENT_CIRCULAR);

    ofEnableDepthTest();
    draw_world();
    ofDisableDepthTest();

    draw_gui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (!streaming)
    {
        switch (key)
        {
        case ' ':
        {
            play = !play;
            if (play)
            {
                device.play = true;
            }
            else
            {
                device.pause = false;
            }
            break;
        }
        case '+':
            play_head = ofClamp(play_head + .01, 0, 1);
            device.seek = play_head;
            break;
        case '-':
            play_head = ofClamp(play_head - .01, 0, 1);
            device.seek = play_head;
            break;
        }
    }

    switch (key)
    {
    case 'f':
    case 'F':
        ofToggleFullscreen();
        break;
    default:
        break;
    }

    keypressed_camera(key);
}

//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::keypressed_camera(int key)
{
    bool val = true;
    switch (key)
    {
    case '1':
        listener_show_top(val);
        break;
    case '2':
        listener_show_front(val);
        break;
    case '3':
        listener_show_side(val);
        break;
    case '4':
        listener_show_perspective(val);
        break;
    }
}

void ofApp::setup_device()
{
    // Load Body Tracking Settings
    auto btSettings = ofxAzureKinect::BodyTrackingSettings();
    btSettings.updateBodies = true;

    // Load a recording with Body Tracking Settings
    if (!streaming)
    {
        filename = ofToDataPath("output_3d_movements.mkv");
        if (!device.open(filename, btSettings))
        {
            exit();
        }
    }
    else
    {
        auto deviceSettings = ofxAzureKinect::DeviceSettings();
        deviceSettings.syncImages = false;
        deviceSettings.depthMode = K4A_DEPTH_MODE_NFOV_UNBINNED;
        deviceSettings.updateIr = false;
        deviceSettings.updateColor = false;
        //deviceSettings.colorResolution = K4A_COLOR_RESOLUTION_1080P;
        deviceSettings.updateWorld = true;
        deviceSettings.updateVbo = false;

        if (!device.open(deviceSettings, btSettings))
        {
            exit();
        }
    }

    // Start Playback
    device.startCameras();
}

//--------------------------------------------------------------
void ofApp::setup_world()
{
    show_front = true;
}

//--------------------------------------------------------------
void ofApp::draw_world()
{
    ofxAzureKinect::BodyTracker *tracker = device.get_body_tracker();
    // Draw 2D Segmentation Map
    if (show_body_map)
        tracker->draw_body_map();

    // Draw 3D World
    cam.begin();
    ofDrawAxis(1500);

    if (tracker->get_num_bodies() > 0)
    {
        // Draw Body Tracker Data
        if (show_bodies)
            tracker->draw_point_clouds(device.getDepthTex(), device.getDepthToWorldTex());

        if (show_skeltons)
            tracker->draw_skeletons();


        auto skeleton = &device.get_body_tracker()->get_skeleton(0);
        auto pt_a = toGlm(skeleton->joints[K4ABT_JOINT_KNEE_LEFT].position);
        auto pt_b = toGlm(skeleton->joints[K4ABT_JOINT_KNEE_RIGHT].position);
        auto pt_c = glm::mix(pt_a, pt_b, .5);
        pin_skeleton(0, pt_c);
    }

    cam.end();
}

void ofApp::pin_skeleton(int index,  glm::vec3 pt)
{
    ofPushMatrix();
    ofTranslate(-pt);
    device.get_body_tracker()->draw_skeleton(index);
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::draw_gui()
{
    panel.draw();

    panel_playback.draw();
    panel_body_tracking.draw();

    ofDrawBitmapStringHighlight("FPS: " + ofToString(ofGetFrameRate()), 10, 10);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    // if (do_disable_camera() && cam.getMouseInputEnabled())
    //     cam.disableMouseInput();

    if (button == 1)
    {
        device.get_body_tracker()->set_joint_smoothing(ofMap(x, 0, ofGetWidth(), 0.0f, 1.0f, true));
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
    if (!cam.getMouseInputEnabled())
        cam.enableMouseInput();
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

//--------------------------------------------------------------
void ofApp::setup_gui()
{

    params.setName("Navigation");
    params.add(show_gui.set("Show_GUI", true));
    params.add(show_top.set("TOP", true));
    params.add(show_front.set("FRONT", false));
    params.add(show_side.set("SIDE", false));
    params.add(show_perspective.set("PERSP", false));

    show_top.addListener(this, &ofApp::listener_show_top);
    show_front.addListener(this, &ofApp::listener_show_front);
    show_side.addListener(this, &ofApp::listener_show_side);
    show_perspective.addListener(this, &ofApp::listener_show_perspective);

    panel.setup(params);
    panel.setPosition(ofGetWidth() - panel.getWidth() - 20, 20);

    setup_gui_playback();
    setup_gui_body_tracking();

    ofSetCircleResolution(60);
}

//--------------------------------------------------------------
void ofApp::setup_gui_playback()
{
    params_playback.setName("Playback");
    params_playback.add(play.set("Play", true));
    params_playback.add(seek.set("Seek", 0, 0, 1));

    play.addListener(this, &ofApp::listener_play);
    seek.addListener(this, &ofApp::listener_seek);

    panel_playback.setup(params_playback);
    panel_playback.setPosition(panel.getPosition().x, panel.getPosition().y + panel.getHeight());
}

//--------------------------------------------------------------
void ofApp::setup_gui_body_tracking()
{
    params_body_tracking.setName("Body_Tracking");
    params_body_tracking.add(show_bodies.set("Show_Bodies", true));
    params_body_tracking.add(show_skeltons.set("Show_Skeletons", true));
    params_body_tracking.add(show_body_map.set("Show_Body_Map", true));
    params_body_tracking.add(joint_smoothing.set("Joint_Smoothing", 0, 0, 1));

    joint_smoothing.addListener(this, &ofApp::listener_joint_smoothing);

    panel_body_tracking.setup(params_body_tracking);
    if (streaming)
        panel_body_tracking.setPosition(panel.getPosition().x, panel.getPosition().y + panel.getHeight());
    else
        panel_body_tracking.setPosition(panel_playback.getPosition().x, panel_playback.getPosition().y + panel_playback.getHeight());
}

//--------------------------------------------------------------
void ofApp::setup_camera()
{
    cam.setFarClip(9999999);
    cam.setDistance(5000);
    ofNode tgt;
    tgt.setGlobalPosition(0, 0, 0);
    tgt.setGlobalOrientation(ofQuaternion(0, 0, 0, 1));
    cam.setTarget(tgt);
    cam.lookAt(ofVec3f(0, 0, -1), ofVec3f(1, 0, 0));
}

//--------------------------------------------------------------
void ofApp::listener_show_front(bool &val)
{
    if (val)
    {
        int x = 0;
        int y = 0;
        int z = -100;

        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(pos.x, pos.y, 0);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, -1, 0));

        show_top = false;
        show_side = false;
        show_perspective = false;
    }
}

//--------------------------------------------------------------F
void ofApp::listener_show_top(bool &val)
{
    if (val)
    {
        int x = 0;
        int y = -4000;
        int z = 1000;

        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(0, 0, pos.z);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, 0, 1));

        show_front = false;
        show_side = false;
        show_perspective = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_show_side(bool &val)
{
    if (val)
    {

        int x = 4000;
        int y = 0;
        int z = 1000;

        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(0, 0, z);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, -1, 0));

        show_top = false;
        show_front = false;
        show_perspective = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_show_perspective(bool &val)
{
    if (val)
    {

        int x = 3000;
        int y = -2000;
        int z = -1000;

        ofVec3f pos = ofVec3f(x, y, z);
        ofVec3f tgt = ofVec3f(0, 0, 1000);
        cam.setGlobalPosition(pos);
        cam.setTarget(tgt);
        cam.lookAt(tgt, ofVec3f(0, -1, 0));
        cam.setGlobalPosition(pos);

        show_top = false;
        show_front = false;
        show_side = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_play(bool &val)
{
    if (val)
    {
        device.play = true;
    }
    else
    {
        device.pause = false;
    }
}

//--------------------------------------------------------------
void ofApp::listener_seek(float &val)
{
    play_head = val;
    device.seek = play_head;
}

//--------------------------------------------------------------
void ofApp::listener_joint_smoothing(float &val)
{
    device.get_body_tracker()->set_joint_smoothing(val);
}
