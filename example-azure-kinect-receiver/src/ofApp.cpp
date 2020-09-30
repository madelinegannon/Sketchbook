#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

	// setup the server to listen on 11999
	ofxTCPSettings settings(11999);

	// set other options
	//settings.blocking = false;
	//settings.reuse = true;
	settings.messageDelimiter = "\n";

	TCP.setup(settings);

	lastSent = 0;
}

//--------------------------------------------------------------
void ofApp::update()
{
	// for each client lets send them a message letting them know what port they are connected on
	// we throttle the message sending frequency to once every 100ms
	uint64_t now = ofGetElapsedTimeMillis();
	if (now - lastSent >= 100)
	{
		for (int i = 0; i < TCP.getLastID(); i++)
		{
			if (!TCP.isClientConnected(i))
				continue;

			TCP.send(i, "hello client - you are connected on port - " + ofToString(TCP.getClientPort(i)));
		}
		lastSent = now;
	}

	for (unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++)
	{
		if (!TCP.isClientConnected(i))
			continue;

		// give each client its own color
		ofSetColor(255 - i * 30, 255 - i * 20, 100 + i * 40);

		// calculate where to draw the text
		int xPos = 15;
		int yPos = 80 + (12 * i * 4);

		// get the ip and port of the client
		string port = ofToString(TCP.getClientPort(i));
		string ip = TCP.getClientIP(i);
		string info = "client " + ofToString(i) + " -connected from " + ip + " on port: " + port;

		// if we don't have a string allocated yet
		// lets create one
		if (i >= storeText.size())
		{
			storeText.push_back(string());
		}

		/////////////////////////////////////////
		// TESTING RECEIVING IR PIXELS
		int receiveSize = 524301; //<-- what if I don't know the receive size?
		ofBuffer buffer;
		buffer.allocate(receiveSize);
		int totalBytes = 0;
		while (totalBytes < receiveSize)
		{
			int result = TCP.receiveRawBytes(i, &buffer.getBinaryBuffer()[totalBytes], receiveSize - totalBytes);
			if (result > 0)
			{
				totalBytes += result;
				if (totalBytes == receiveSize)
				{
					kinect::Image *msg = new kinect::Image();
					msg->InitAsDefaultInstance();
					msg->ParseFromArray(buffer.getData(), receiveSize);

					// cout << msg->GetTypeName() << endl;
					// cout << "width: " << msg->width() << ", height: " << msg->height();
					// cout << ", msg->image_data().length(): " << msg->image_data().length() << endl;

					if (msg->image_data().length() != 0)
						update_pixels_ir(msg->image_data());

					delete msg;
				}
			}
		}

	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(220);
	ofDrawBitmapString("TCP SERVER Example \n\nconnect on port: " + ofToString(TCP.getPort()), 10, 20);

	ofSetColor(0);
	ofDrawRectangle(10, 60, ofGetWidth() - 24, ofGetHeight() - 65 - 15);

	ofSetColor(220);

	// for each connected client lets get the data being sent and lets print it to the screen
	for (unsigned int i = 0; i < (unsigned int)TCP.getLastID(); i++)
	{

		if (!TCP.isClientConnected(i))
			continue;

		// give each client its own color
		ofSetColor(255 - i * 30, 255 - i * 20, 100 + i * 40);

		// calculate where to draw the text
		int xPos = 15;
		int yPos = 80 + (12 * i * 4);

		// get the ip and port of the client
		string port = ofToString(TCP.getClientPort(i));
		string ip = TCP.getClientIP(i);
		string info = "client " + ofToString(i) + " -connected from " + ip + " on port: " + port;

		// if we don't have a string allocated yet
		// lets create one
		if (i >= storeText.size())
		{
			storeText.push_back(string());
		}

		/////////////////////////////////////////
		// TESTING RECEIVING IR PIXELS
		/*
		int receiveSize = 524301; //524308;
		ofBuffer buffer;
		buffer.allocate(receiveSize);
		int totalBytes = 0;
		while (totalBytes < receiveSize)
		{
			int result = TCP.receiveRawBytes(i, &buffer.getBinaryBuffer()[totalBytes], receiveSize - totalBytes);
			if (result > 0)
			{
				totalBytes += result;
				if (totalBytes == receiveSize)
				{
					cout << "made it!" << endl;
					kinect::Image *msg = new kinect::Image();
					msg->InitAsDefaultInstance();
					msg->ParseFromArray(buffer.getData(), receiveSize);

					cout << msg->GetTypeName() << endl;
					cout << "width: " << msg->width() << ", height: " << msg->height();
					cout << ", msg->image_data().length(): " << msg->image_data().length() << endl;

					if (msg->image_data().length() != 0)
						update_pixels_ir(msg->image_data());

					delete msg;
				}
			}
			*/
		// }
		/////////////////////////////////////////

		// receive all the available messages, separated by \n
		// and keep only the last one
		// string str;
		// string tmp;
		// do
		// {
		// 	str = tmp;
		// 	tmp = TCP.receive(i);
		// } while (tmp != "");

		// // if there was a message set it to the corresponding client
		// if (str.length() > 0)
		// {
		// 	storeText[i] = str;
		// }

		// // draw the info text and the received text bellow it
		ofDrawBitmapString(info, xPos, yPos);
		// ofDrawBitmapString(storeText[i], 25, yPos + 20);
	}

	// ... testing ... testing ...
	ofPushStyle();
	ofSetColor(255);
	if (ir_tex.isAllocated())
	{
		ir_tex.draw(340, 385, 315, 315);
	}
	ofPopStyle();
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
	memcpy(image_data, (const uint8_t *)output.c_str(), 512 * 512 * 2);

	// convert from uint8_t* buffer to ofPixels
	if (!ir_pix.isAllocated())
	{
		ir_pix.allocate(dims.x, dims.y, num_channels);
	}
	ir_pix.setFromPixels((uint16_t *)(output.c_str()), dims.x, dims.y, num_channels);

	if (!ir_tex.isAllocated())
	{
		ir_tex.allocate(ir_pix);
		ir_tex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	}
	ir_tex.loadData(ir_pix);
	// }
	free(image_data);
}

//--------------------------------------------------------------
void ofApp::exit()
{
	google::protobuf::ShutdownProtobufLibrary();
}

//--------------------------------------------------------------
void ofApp::set_texture(uint8_t *image_data, ofShortPixels *pix, ofTexture *tex)
{
	// hard code dims and channels for now
	auto dims = glm::ivec2(512, 512);
	int num_channels = 1;

	if (image_data == NULL)
		return;

	if (!pix->isAllocated())
	{
		pix->allocate(dims.x, dims.y, num_channels);
	}
	pix->setFromPixels(reinterpret_cast<uint16_t *>(image_data), dims.x, dims.y, num_channels);

	if (!tex->isAllocated())
	{
		tex->allocate(*pix);
		tex->setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	}
	tex->loadData(*pix);
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
