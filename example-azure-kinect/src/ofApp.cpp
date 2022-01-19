#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

    device.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
    // device.update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    auto color_tex = device.get_color_texture();
    auto depth_tex = device.get_depth_texture();
    auto ir_tex = device.get_ir_texture();


    if (color_tex->isAllocated())
        color_tex->draw(15, 15, 640, 360);
    if (depth_tex->isAllocated())
        depth_tex->draw(15, 380, 315, 315);
    if (ir_tex->isAllocated())
        ir_tex->draw(340, 380, 315, 315);

    // cout << ofGetFrameRate() << endl;
}

//--------------------------------------------------------------
void ofApp::exit()
{
    cout << "exiting." << endl;
    device.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
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
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
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
