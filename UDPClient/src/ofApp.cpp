#include "ofApp.h"

#define RECONNECT_TIME 400

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	udpConnection.Create();
	udpConnection.SetNonBlocking(false);
	//udpConnection.Bind(portNumber + 1);
	udpConnection.Connect("127.0.0.1", portNumber);
}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	SendSensorMessage();
	ReceiveRobotMessage();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
// Create a simple sensor message
void ofApp::CreateSensorMessage(EgmSensor* pSensorMessage)
{
	EgmHeader* header = new EgmHeader();
	header->set_mtype(EgmHeader_MessageType_MSGTYPE_CORRECTION);
	header->set_seqno(sequenceNumber++);
	header->set_tm(GetTickCount());

	pSensorMessage->set_allocated_header(header);

	EgmCartesian *pc = new EgmCartesian();
	float offset = 225;// .001;
	double x = 400; double y = 0; double z = 600;
	pc->set_x(x * 1000);// 1.1);
	pc->set_y(y * 1000);// 2.2);
	pc->set_z(z * 1000);// 3.3);


	EgmQuaternion *pq = new EgmQuaternion();
	pq->set_u0(1);
	pq->set_u1(0);
	pq->set_u2(0);
	pq->set_u3(0);

	EgmPose *pcartesian = new EgmPose();
	pcartesian->set_allocated_orient(pq);
	pcartesian->set_allocated_pos(pc);

	EgmPlanned *planned = new EgmPlanned();
	planned->set_allocated_cartesian(pcartesian);

	pSensorMessage->set_allocated_planned(planned);
	printf("To Robot:\n");
	printf("\tDesired State:\n");
	printf("\t\tDesired Pose: [[ %f, %f, %f ], [ %f, %f, %f, %f ]]\n\n",
		planned->cartesian().pos().x(), planned->cartesian().pos().y(), planned->cartesian().pos().z(),
		planned->cartesian().orient().u0(), planned->cartesian().orient().u1(), planned->cartesian().orient().u2(), planned->cartesian().orient().u3());
}

//--------------------------------------------------------------
void ofApp::SendSensorMessage()
{
	string messageBuffer;

	// create and send a sensor message
	EgmSensor *pSensorMessage = new EgmSensor();
	CreateSensorMessage(pSensorMessage);
	pSensorMessage->SerializeToString(&messageBuffer);

	//send 
	udpConnection.Send(messageBuffer.c_str(), messageBuffer.length());

	delete pSensorMessage;
}

//--------------------------------------------------------------
void ofApp::DisplayRobotMessage(EgmRobot* pRobotMessage)
{
	printf("From Robot:");
	if (pRobotMessage->has_header() && pRobotMessage->header().has_seqno() && pRobotMessage->header().has_tm() && pRobotMessage->header().has_mtype())
	{
		printf("\tSeqNo=%d Tm=%u Type=%d\n\n", pRobotMessage->header().seqno(), pRobotMessage->header().tm(), pRobotMessage->header().mtype());
	}
}

//--------------------------------------------------------------
void ofApp::ReceiveRobotMessage()
{

	//if (udpConnection.PeekReceive() > 0) {
		char udpMessage[100000];
		auto ret = udpConnection.Receive(udpMessage, 100000);
		if (ret > 0) {
			// deserialize inbound message
			EgmRobot *pRobotMessage = new EgmRobot();
			pRobotMessage->ParseFromArray(udpMessage, udpConnection.GetReceiveBufferSize());
			DisplayRobotMessage(pRobotMessage);
			delete pRobotMessage;
		}
		else {
			ofLogError() << "error while receiving" << endl;
		}
	//}

}


//--------------------------------------------------------------
void ofApp::ReceiveSensorMessage()
{

	//if (udpConnection.PeekReceive() > 0) {
	char udpMessage[100000];
	auto ret = udpConnection.Receive(udpMessage, 100000);
	if (ret > 0) {
		// deserialize inbound message
		EgmSensor *pSensorMessage = new EgmSensor();
		pSensorMessage->ParseFromArray(udpMessage, udpConnection.GetReceiveBufferSize());
		DisplaySensorMessage(pSensorMessage);
		delete pSensorMessage;
	}
	else {
		ofLogError() << "error while receiving" << endl;
	}
	//}
}

//--------------------------------------------------------------
void ofApp::DisplaySensorMessage(EgmSensor *pSensorMessage)
{
	printf("From Sensor:");
	if (pSensorMessage->has_header() && pSensorMessage->header().has_seqno() && pSensorMessage->header().has_tm() && pSensorMessage->header().has_mtype())
	{
		printf("\tSeqNo=%d Tm=%u Type=%d\n", pSensorMessage->header().seqno(), pSensorMessage->header().tm(), pSensorMessage->header().mtype());

		if (pSensorMessage->has_planned()) {

			printf("\tPlanned State:\n");
			// Planned Pose
			if (pSensorMessage->planned().has_cartesian() && pSensorMessage->planned().cartesian().has_pos() && pSensorMessage->planned().cartesian().has_orient()) {
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
}