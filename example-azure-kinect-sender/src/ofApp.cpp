#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

    //create the client socket and set to send to 127.0.0.1:11999
    ofxTCPSettings settings("127.0.0.1", 11999);

    // set other options:
    //settings.blocking =false;
    settings.messageDelimiter = "\n";

    // connect to the server - if this fails or disconnects
    // we'll check every few seconds to see if the server exists
    tcpClient.setup(settings);

    device.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (tcpClient.isConnected())
    {
        // we are connected - lets try to receive from the server
        string str = tcpClient.receive();
        if (str.length() > 0)
        {
            connection_info = str;
        }
    }
    else
    {
        // try to reconnect ever 5 seconds
        connection_info = "";
        reconnect();
    }


    // get image_data buffer for IR image
    uint8_t *image_data_ir = (uint8_t *)malloc(512 * 512 * 2);
    // if (image_data_ir != NULL && device.get_image_data_ir() != NULL)
    if (device.get_image_data_ir() != NULL)
    {
        memcpy(image_data_ir, device.get_image_data_ir(), 512 * 512 * 2);

        // testing protobuf
        kinect::Image *image = new kinect::Image();
        image->InitAsDefaultInstance();

        size_t image_data_size = 512 * 512 * 2;

        image->set_width(512);
        image->set_height(512);
        image->set_stride(1024);
        image->set_image_data(reinterpret_cast<char *>(image_data_ir), image_data_size);
        // cout << "image pre serialize: " << image->image_data().length() << endl;

        auto size = image->ByteSizeLong();
        void *buffer = malloc(size);
        image->SerializeToArray(buffer, size);
        // cout << "msg size: " << size << endl;
        if (tcpClient.isConnected())
        {
            // send then full message in raw bytes
            tcpClient.sendRawBytes((char *)buffer, size);
            // cout << "sending " << size << " bytes." << endl;
        }

        // testing deserializing
        kinect::Image *msg = new kinect::Image();
        msg->InitAsDefaultInstance();
        msg->ParseFromArray(buffer, size);

        // if (msg->GetTypeName() == "kinect.Image"){
        //     cout << msg->GetTypeName() << endl;
        //     cout << "width: " << msg->width() << ", height: " << msg->height() << endl;
        //     cout << msg->image_data().length() << endl;
        // }

        update_pixels_ir(msg->image_data());

        delete image;
        delete msg;
        free(buffer);
    }
    free(image_data_ir);
}

//--------------------------------------------------------------
void ofApp::exit()
{
    google::protobuf::ShutdownProtobufLibrary();
    device.stop();

    delete ir_raw_buffer;
}

//--------------------------------------------------------------
void ofApp::update_pixels_ir(string output)
{
    auto dims = glm::ivec2(512, 512);
    int num_channels = 1;

    // kinect::Image *msg = new kinect::Image();
    // msg->InitAsDefaultInstance();

    // msg->ParseFromString(output);
    // if (msg->mutable_image_data() != NULL)
    // {
    uint8_t *image_data;
    image_data = (uint8_t *)malloc(512 * 512 * 2);
    // memcpy(image_data,(const uint8_t *)msg->image_data().c_str(), 512 * 512 * 2);
    memcpy(image_data, (const uint8_t *)output.c_str(), 512 * 512 * 2);

    // convert from uint8_t* buffer to ofPixels
    if (!test_buffer.isAllocated())
    {
        test_buffer.allocate(dims.x, dims.y, num_channels);
    }
    test_buffer.setFromPixels(reinterpret_cast<uint16_t *>(image_data), dims.x, dims.y, num_channels);

    if (!test_texture.isAllocated())
    {
        test_texture.allocate(test_buffer);
        test_texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    }
    test_texture.loadData(test_buffer);
    // }
    free(image_data);
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(255, 0, 0);
    // Kinect Stuff
    auto color_tex = device.get_color_texture();
    auto depth_tex = device.get_depth_texture();
    auto ir_tex = device.get_ir_texture();

    if (color_tex->isAllocated())
        color_tex->draw(15, 15, 640, 360);
    if (depth_tex->isAllocated())
        depth_tex->draw(15, 385, 315, 315);
    if (ir_tex->isAllocated())
        ir_tex->draw(340, 385, 315, 315);

    // this is the image buffer being sent from DeviceCapturer
    // NEXT ... serialize and deserialize through protobuf
    if (test_texture.isAllocated())
    {
        test_texture.draw(15, 15, 315, 315);
    }

    // TCP Stuff
    if (tcpClient.isConnected())
    {
        ofDrawBitmapString("Connected to server: \n" + connection_info, 15, 270);
    }
    else
    {
        ofDrawBitmapString("status: server not found. launch server app and check ports!\n\nreconnecting in " + ofToString((5000 - deltaTime) / 1000) + " seconds", 15, 55);
    }
}

//--------------------------------------------------------------
void ofApp::reconnect()
{
    // if we are not connected lets try and reconnect every 5 seconds
    deltaTime = ofGetElapsedTimeMillis() - connectTime;

    if (deltaTime > 5000)
    {
        tcpClient.setup("127.0.0.1", 11999);
        tcpClient.setMessageDelimiter("\n");
        connectTime = ofGetElapsedTimeMillis();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    // you can only type if you're connected
    // we accumulate 1 line of text and send every typed character
    // on the next character after a breakline we clear the buffer
    if (tcpClient.isConnected())
    {
        if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL)
        {
            if (!msgTx.empty())
            {
                msgTx = msgTx.substr(0, msgTx.size() - 1);
            }
        }
        else if (key != 0)
        {
            ofUTF8Append(msgTx, key);
        }
        // tcpClient.send(msgTx);
        if (!msgTx.empty() && msgTx.back() == '\n')
        {
            msgTx.clear();
        }
    }

    if (key == ' ')
        is_streaming = !is_streaming;
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
