/////////////////////////////////////////////////////////////////////////
// Sample using Google protocol buffers C++
//
#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <string>


#include "egm.pb.h" // generated by Google protoc.exe

#pragma comment(lib, "Ws2_32.lib")      // socket lib
#pragma comment(lib, "libprotobuf.lib") // protobuf lib

static int portNumber = 6510;
static unsigned int sequenceNumber = 0;

using namespace std;
using namespace abb::egm;


// Protobuf-C++ is supported by Google and no other third party libraries needed for the protobuf part. 
// It can be a bit tricky to build the Google tools in Windows but here is a guide on how to build 
// protobuf for Windows (http://eli.thegreenplace.net/2011/03/04/building-protobuf-examples-on-windows-with-msvc).
//
// When you have built libprotobuf.lib and protoc.exe:
//	 Run Google protoc to generate access classes, protoc --cpp_out=. egm.proto
//	 Create a win32 console application
//	 Add protobuf source as include directory
//	 Add the generated egm.pb.cc to the project (exclude the file from precompile headers)
//	 Copy the file below
//	 Compile and run
//
//
// Copyright (c) 2014, ABB
// All rights reserved.
//
// Redistribution and use in source and binary forms, with
// or without modification, are permitted provided that 
// the following conditions are met:
//
//    * Redistributions of source code must retain the 
//      above copyright notice, this list of conditions 
//      and the following disclaimer.
//    * Redistributions in binary form must reproduce the 
//      above copyright notice, this list of conditions 
//      and the following disclaimer in the documentation 
//      and/or other materials provided with the 
//      distribution.
//    * Neither the name of ABB nor the names of its 
//      contributors may be used to endorse or promote 
//      products derived from this software without 
//      specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

double x = 400; double y = 450.55; double z = 553.00;
double qx = 0.409197; double qy = -0.235415; double qz = 0.108835; double qw = 0;

SOCKET sockfd;
struct sockaddr_in serverAddr, clientAddr;


//////////////////////////////////////////////////////////////////////////
// Create a simple sensor message
void CreateSensorMessage(EgmSensor* pSensorMessage)
{
	EgmHeader* header = new EgmHeader();
	header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
	header->set_seqno(sequenceNumber++);
	header->set_tm(GetTickCount());

	pSensorMessage->set_allocated_header(header);

	EgmCartesian *pc = new EgmCartesian();
	float offset = 225;// .001;
	
	pc->set_x(x);// *1000);// 1.1);
	pc->set_y(y * 1000);// 2.2);
	pc->set_z(z * 1000);// 3.3);


	EgmQuaternion *pq = new EgmQuaternion();
	pq->set_u0(qx);
	pq->set_u1(qy);
	pq->set_u2(qz);
	pq->set_u3(qw);

	EgmPose *pcartesian = new EgmPose();
	pcartesian->set_allocated_orient(pq);
	pcartesian->set_allocated_pos(pc);

	EgmPlanned *planned = new EgmPlanned();
	planned->set_allocated_cartesian(pcartesian);

	pSensorMessage->set_allocated_planned(planned);
	printf("DUMMY POINT To Robot:\n");
	printf("\tDesired State:\n");
	printf("\t\tDesired Pose: [[ %f, %f, %f ], [ %f, %f, %f, %f ]]\n\n",
		planned->cartesian().pos().x(), planned->cartesian().pos().y(), planned->cartesian().pos().z(),
		planned->cartesian().orient().u0(), planned->cartesian().orient().u1(), planned->cartesian().orient().u2(), planned->cartesian().orient().u3());
}

//////////////////////////////////////////////////////////////////////////
// Display inbound robot message
void DisplayRobotMessage(EgmRobot *pRobotMessage)
{
	printf("From Robot:");
	if (pRobotMessage->has_header() && pRobotMessage->header().has_seqno() && pRobotMessage->header().has_tm() && pRobotMessage->header().has_mtype())
	{
		printf("\tSeqNo=%d Tm=%u Type=%d\n", pRobotMessage->header().seqno(), pRobotMessage->header().tm(), pRobotMessage->header().mtype());

		if (pRobotMessage->has_feedback()) {
			printf("\tActual State:\n");
			// Pose Data
			if (pRobotMessage->feedback().has_cartesian() && pRobotMessage->feedback().cartesian().has_pos() && pRobotMessage->feedback().cartesian().has_orient()) {
				double __x = pRobotMessage->feedback().cartesian().pos().x();
				y = pRobotMessage->feedback().cartesian().pos().y();
				z = pRobotMessage->feedback().cartesian().pos().z();
				qx = pRobotMessage->feedback().cartesian().orient().u0();
				qy = pRobotMessage->feedback().cartesian().orient().u1();
				qz = pRobotMessage->feedback().cartesian().orient().u2();
				qz = pRobotMessage->feedback().cartesian().orient().u3();

				printf("\t\tActual Pose: [[ %f, %f, %f ], [ %f, %f, %f, %f ]]\n", __x, y, z, qx, qy, qz, qw);
			}
			// Joint Data
			if (pRobotMessage->feedback().has_joints()) {
				printf("\t\tJoints: [ ");
				for (auto &joint : pRobotMessage->feedback().joints().joints())
					printf("%f ", joint);
				printf(" ]\n ");
			}
		}
		else {
			printf("\tNo Actual State Data\n");
		}
		if (pRobotMessage->has_planned()) {
			printf("\tNext State:\n");
			// Planned Pose
			if (pRobotMessage->planned().has_cartesian() && pRobotMessage->planned().cartesian().has_pos() && pRobotMessage->feedback().cartesian().has_orient()) {
				double __x = pRobotMessage->planned().cartesian().pos().x();
				double __y = pRobotMessage->planned().cartesian().pos().y();
				double __z = pRobotMessage->planned().cartesian().pos().z();
				double __qx = pRobotMessage->feedback().cartesian().orient().u0();
				double __qy = pRobotMessage->feedback().cartesian().orient().u1();
				double __qz = pRobotMessage->feedback().cartesian().orient().u2();
				double __qw = pRobotMessage->feedback().cartesian().orient().u3();

				printf("\t\tPlanned Pose: [[ %f %f %f ], [ %f %f %f %f ]]\n", __x, __y, __z, __qx, __qy, __qz, __qw);
			}
			// Joint Data
			if (pRobotMessage->planned().has_joints()) {
				printf("\t\tPlanned Joints: [ ");
				for (auto &joint : pRobotMessage->planned().joints().joints())
					printf("%f ", joint);
				printf(" ]\n ");
			}
		}
		else {
			printf("No Planned State Data\n");
		}
		// Motor State
		if (pRobotMessage->has_motorstate()) {
			printf("\tMotorstate: %s\n\n", pRobotMessage->motorstate().MotorStateType_Name(pRobotMessage->motorstate().state()));
		}
	}
	else
	{
		printf("Fuck ... No header\n");
	}

}

////--------------------------------------------------------------------------------
//bool ofxUDPManager::GetRemoteAddr(string& address, int& port) const
//{
//	if (m_hSocket == INVALID_SOCKET) return(false);
//	if (canGetRemoteAddress == false) return (false);
//
//	//	get the static-winsock-allocated address-conversion string and make a copy of it
//	const char* AddressStr = inet_ntoa((in_addr)saClient.sin_addr);
//	address = AddressStr;
//
//	//	get the port
//	port = ntohs(saClient.sin_port);
//
//	return true;
//}


void SendMsg(string messageBuffer) {
	
	int n;
	
	// create and send a sensor message
	EgmSensor *pSensorMessage = new EgmSensor();

	if (messageBuffer == "")
		CreateSensorMessage(pSensorMessage);
	pSensorMessage->SerializeToString(&messageBuffer);

	// send a message to the robot
	n = sendto(sockfd, messageBuffer.c_str(), messageBuffer.length(), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
	if (n < 0)
	{
		printf("Error send message\n");
	}
	delete pSensorMessage;

}

//--------------------------------------------------------------
void DisplaySensorMessage(EgmSensor *pSensorMessage)
{
	printf("From Sensor:");
	if (pSensorMessage->has_header() && pSensorMessage->header().has_seqno() && pSensorMessage->header().has_tm() && pSensorMessage->header().has_mtype())
	{
		printf("\tSeqNo=%d Tm=%u Type=%d\n", pSensorMessage->header().seqno(), pSensorMessage->header().tm(), pSensorMessage->header().mtype());

		if (pSensorMessage->has_planned()) {

			printf("\tPlanned State:\n");
			// Planned Pose
			if (pSensorMessage->planned().has_cartesian() && pSensorMessage->planned().cartesian().has_pos() && pSensorMessage->planned().cartesian().has_orient()) {

				x = pSensorMessage->planned().cartesian().pos().x();

				double __x = pSensorMessage->planned().cartesian().pos().x();
				double __y = pSensorMessage->planned().cartesian().pos().y();
				double __z = pSensorMessage->planned().cartesian().pos().z();
				double __qx = pSensorMessage->planned().cartesian().orient().u0();
				double __qy = pSensorMessage->planned().cartesian().orient().u1();
				double __qz = pSensorMessage->planned().cartesian().orient().u2();
				double __qw = pSensorMessage->planned().cartesian().orient().u3();

				printf("\t\tPlanned Pose: [[ %f %f %f ], [ %f %f %f %f ]]\n", __x, __y, __z, __qx, __qy, __qz, __qw);
			}
		}
		else {
			printf("No Planned State Data\n");
		}
	}
	else
	{
		printf("Fuck ... No header\n");
	}

	printf("x,y,z: { %f, %f, %f }", x, y, z);
}

void ReceiveMessage() {

	int n;
	int len;
	char protoMessage[1400];

	// receive and display message from robot
	len = sizeof(clientAddr);
	n = recvfrom(sockfd, protoMessage, 1400, 0, (struct sockaddr *)&clientAddr, &len);
	if (n < 0)
	{
		printf("Error receive message\n");
		return;
	}

	//const char* AddressStr = inet_ntop((in_addr)clientAddr.sin_addr);
	//string address (AddressStr);
	int port = ntohs(clientAddr.sin_port);
	printf( "CLIENT ADDR %s : %i \n", "", port);


	// deserialize inbound message
	EgmRobot *pRobotMessage = new EgmRobot();
	pRobotMessage->ParseFromArray(protoMessage, n);

	// whoops this is a sensor message
	if (!pRobotMessage->has_motorstate()) {

		
		EgmSensor *pSensorMessage = new EgmSensor(); // incoming sensor message from PC
		pSensorMessage->ParseFromArray(protoMessage, n);

		DisplaySensorMessage(pSensorMessage);

		// relay the sensor message to the robot
		string messageBuffer;
		pSensorMessage->SerializeToString(&messageBuffer);

		// send a message to the robot
		n = sendto(sockfd, messageBuffer.c_str(), messageBuffer.length(), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
		if (n < 0)
		{
			printf("Error send message\n");
		}
		delete pSensorMessage;
	}
	else {
		DisplayRobotMessage(pRobotMessage);
		delete pRobotMessage;

		// send dummy message
		SendMsg("");
	}


}


int _tmain(int argc, _TCHAR* argv[])
{
	//SOCKET sockfd;
	//int n;
	//struct sockaddr_in serverAddr, clientAddr;
	//int len;
	//char protoMessage[1400];


	/* Init winsock */
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		exit(0);
	}

	// create socket to listen on
	sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);

	memset(&serverAddr, sizeof(serverAddr), 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(portNumber);

	// listen on all interfaces
	bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

	string messageBuffer;
	//for (int messages = 0; messages < 10000; messages++)
	for (;;)
	{
		// receive and display message from robot
		//len = sizeof(clientAddr);
		//n = recvfrom(sockfd, protoMessage, 1400, 0, (struct sockaddr *)&clientAddr, &len);
		//if (n < 0)
		//{
		//	printf("Error receive message\n");
		//	continue;
		//}

		//// deserialize inbound message
		//EgmRobot *pRobotMessage = new EgmRobot();
		//pRobotMessage->ParseFromArray(protoMessage, n);
		//DisplayRobotMessage(pRobotMessage);
		//delete pRobotMessage;

		//// create and send a sensor message
		//EgmSensor *pSensorMessage = new EgmSensor();
		//CreateSensorMessage(pSensorMessage);
		//pSensorMessage->SerializeToString(&messageBuffer);

		//// send a message to the robot
		//n = sendto(sockfd, messageBuffer.c_str(), messageBuffer.length(), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
		//if (n < 0)
		//{
		//	printf("Error send message\n");
		//}
		//delete pSensorMessage;

		ReceiveMessage();

		

	}
	return 0;
}

