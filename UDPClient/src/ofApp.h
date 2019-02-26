#pragma once

#include "egm.pb.h" // generated by Google protoc.exe

//#pragma comment(lib, "Ws2_32.lib")      // socket lib
#pragma comment(lib, "libprotobuf.lib") // protobuf lib

#include "ofMain.h"
#include "ofxNetwork.h"

static int portNumber = 6510;
static unsigned int sequenceNumber = 0;

using namespace abb::egm;

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
		
		ofxUDPManager udpConnection;
		
		void SendSensorMessage();
		void CreateSensorMessage(EgmSensor *pSensorMessage);

		void ReceiveRobotMessage();
		void DisplayRobotMessage(EgmRobot* pRobotMessage);

		void ReceiveSensorMessage();
		void DisplaySensorMessage(EgmSensor *pSensorMessage);

};
