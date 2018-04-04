#pragma once

#include "ofMain.h"


class SerialThread : public ofThread {
	public:

		SerialThread() { };
		
		ofSerial serial;
		string incomingMsg = "";
		string incomingMsg_prev = "";
		string *sendMsg;
		string sendMsg_prev = "";
		bool bNewMessage = false;

		void start(string sendMessage){
			sendMsg = &sendMessage; 
			startThread(); 
		};
		void stop() { stopThread(); };
		
		
		void threadedFunction()
		{
			while (isThreadRunning()){
				if (lock()){

					checkForMessage();
					sendMessage();
				
					unlock();
					sleep(100);
				}
				else {

				}
			}

		};

protected:

		void checkForMessage() {

			// check for incoming messages from Minitaur
			incomingMsg = "";
			stringstream ss;
			char ch;
			if (serial.isInitialized()) {// && serial.available()) {
				while ((ch = serial.readByte()) > 0 && ch != '\n')
					ss << ch;

				incomingMsg = ss.str();

				if (incomingMsg != "") {
					cout << "	Incoming Message from Minitaur: [ " << incomingMsg << " ]" << endl << endl;
				}

			}

			incomingMsg = ofGetTimestampString();

			bNewMessage = (incomingMsg != incomingMsg_prev);
		};
		
		void sendMessage() {

			if (*sendMsg != sendMsg_prev) {
				string msg = *sendMsg;
				cout << msg << endl;

				sendMsg_prev = msg;
			}

			//if (serial.isInitialized()) {// && serial.available()) {

			//	unsigned char* chars = (unsigned char*)sendMsg.c_str();
			//	serial.writeBytes(chars, sendMsg.length());

			//	//cout << "Sending Message [ " << msg << " ] to Minitaur" << endl;
			//}
			//else {
			//	cout << "Serial NOT Initialized" << endl;
			//}

		};
};
