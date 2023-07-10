#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofSetLogLevel(OF_LOG_VERBOSE);

	ofLogNotice(__FUNCTION__) << "Found " << ofxAzureKinect::Device::getInstalledCount() << " installed devices.";

	if (this->kinectDevice.open())
	{
		auto deviceSettings = ofxAzureKinect::DeviceSettings();
		deviceSettings.syncImages = false;
		deviceSettings.depthMode = K4A_DEPTH_MODE_NFOV_UNBINNED;
		deviceSettings.updateIr = false;
		deviceSettings.updateColor = true;
		deviceSettings.colorResolution = K4A_COLOR_RESOLUTION_1080P;
		deviceSettings.updateWorld = true;
		deviceSettings.updateVbo = false;
		this->kinectDevice.startCameras(deviceSettings);
	
		auto bodyTrackerSettings = ofxAzureKinect::BodyTrackerSettings();
		bodyTrackerSettings.sensorOrientation = K4ABT_SENSOR_ORIENTATION_DEFAULT;
		//bodyTrackerSettings.processingMode =  K4ABT_TRACKER_PROCESSING_MODE_CPU; //K4ABT_TRACKER_PROCESSING_MODE_GPU_DIRECTML;
		this->kinectDevice.startBodyTracker(bodyTrackerSettings);
	}

	// Load shader.
	auto shaderSettings = ofShaderSettings();
	shaderSettings.shaderFiles[GL_VERTEX_SHADER] = "shaders/render.vert";
	shaderSettings.shaderFiles[GL_FRAGMENT_SHADER] = "shaders/render.frag";
	shaderSettings.intDefines["BODY_INDEX_MAP_BACKGROUND"] = K4ABT_BODY_INDEX_MAP_BACKGROUND;
	shaderSettings.bindDefaults = true;
	if (this->shader.setup(shaderSettings))
	{
		ofLogNotice(__FUNCTION__) << "Success loading shader!";
	}

	// Setup vbo.
	std::vector<glm::vec3> verts(1);
	this->pointsVbo.setVertexData(verts.data(), verts.size(), GL_STATIC_DRAW);

	setup_osc(this->ip_address, this->port);

}

//--------------------------------------------------------------
void ofApp::exit()
{
	this->kinectDevice.close();
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (this->kinectDevice.isStreaming() && this->kinectDevice.getBodySkeletons().size() > 0)
	{
		send_skeleton();
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(0);

	if (this->kinectDevice.isStreaming())
	{
		this->kinectDevice.getBodyIndexTex().draw(0, 0, 360, 360);
	}

	this->camera.begin();
	{
		ofPushMatrix();
		{
			ofRotateXDeg(180);

			ofEnableDepthTest();

			constexpr int kMaxBodies = 6;
			int bodyIDs[kMaxBodies];
			int i = 0;
			while (i < this->kinectDevice.getNumBodies())
			{
				bodyIDs[i] = this->kinectDevice.getBodyIDs()[i];
				++i;
			}
			while (i < kMaxBodies)
			{
				bodyIDs[i] = 0;
				++i;
			}

			this->shader.begin();
			{
				this->shader.setUniformTexture("uDepthTex", this->kinectDevice.getDepthTex(), 1);
				this->shader.setUniformTexture("uBodyIndexTex", this->kinectDevice.getBodyIndexTex(), 2);
				this->shader.setUniformTexture("uWorldTex", this->kinectDevice.getDepthToWorldTex(), 3);
				this->shader.setUniform2i("uFrameSize", this->kinectDevice.getDepthTex().getWidth(), this->kinectDevice.getDepthTex().getHeight());
				this->shader.setUniform1iv("uBodyIDs", bodyIDs, kMaxBodies);

				int numPoints = this->kinectDevice.getDepthTex().getWidth() * this->kinectDevice.getDepthTex().getHeight();
				this->pointsVbo.drawInstanced(GL_POINTS, 0, 1, numPoints);
			}
			this->shader.end();

			ofDisableDepthTest();

			auto& bodySkeletons = this->kinectDevice.getBodySkeletons();
			for (auto& skeleton : bodySkeletons)
			{
				// Draw joints.
				for (int i = 0; i < K4ABT_JOINT_COUNT; ++i)
				{
					auto joint = skeleton.joints[i];
					ofPushMatrix();
					{
						glm::mat4 transform = glm::translate(toGlm(joint.position)) * glm::toMat4(toGlm(joint.orientation));
						ofMultMatrix(transform);

						ofDrawAxis(50.0f);
					}
					ofPopMatrix();
				}

				// Draw connections.
				this->skeletonMesh.setMode(OF_PRIMITIVE_LINES);
				auto& vertices = this->skeletonMesh.getVertices();
				vertices.resize(50);
				int vdx = 0;

				// Spine.
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_NAVEL].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_NAVEL].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_CHEST].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_CHEST].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HEAD].position);

				// Head.
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HEAD].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NOSE].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NOSE].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_LEFT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EAR_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NOSE].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_RIGHT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_RIGHT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EAR_RIGHT].position);

				// Left Leg.
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_LEFT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_LEFT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_FOOT_LEFT].position);

				// Right leg.
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_RIGHT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_RIGHT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_RIGHT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_RIGHT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_FOOT_RIGHT].position);

				// Left arm.
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_LEFT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_LEFT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_LEFT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_LEFT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_WRIST_LEFT].position);

				// Right arm.
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_RIGHT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_RIGHT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_RIGHT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_RIGHT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_RIGHT].position);

				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_RIGHT].position);
				vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_WRIST_RIGHT].position);

				this->skeletonMesh.draw();
			}
		}
		ofPopMatrix();
	}
	this->camera.end();

	std::ostringstream oss;
	oss << ofToString(ofGetFrameRate(), 2) + " FPS" << std::endl;
	oss << "Joint Smoothing: " << this->kinectDevice.getBodyTracker().jointSmoothing;
	ofDrawBitmapStringHighlight(oss.str(), 10, 20);
}

//--------------------------------------------------------------
void ofApp::setup_osc(string ip_addr, int port)
{
	sender.setup(ip_addr, port);
	cout << "Sending Closest Skeleton to " << ip_addr << ":" << port << endl;
}

//--------------------------------------------------------------
void ofApp::send_skeleton()
{
	int index = get_closest_skeleton();

	// send full skeleton
	ofxOscMessage m;
	m.setAddress("/skeleton");
	m.addInt32Arg(this->kinectDevice.getBodyIDs()[index]);
	auto body = this->kinectDevice.getBodySkeletons()[index];
	int i = 0;
	for (auto& joint : body.joints) {
		auto x = joint.position.xyz.x;
		auto y = joint.position.xyz.y;
		auto z = joint.position.xyz.z;
		auto qw = joint.orientation.wxyz.w;
		auto qx = joint.orientation.wxyz.x;
		auto qy = joint.orientation.wxyz.y;
		auto qz = joint.orientation.wxyz.z;
		m.addInt32Arg(i); // joint index
		m.addFloatArg(x);
		m.addFloatArg(y);
		m.addFloatArg(z);
		m.addFloatArg(qw);
		m.addFloatArg(qx);
		m.addFloatArg(qy);
		m.addFloatArg(qz);
		i++;
	}
	sender.sendMessage(m);
}

//--------------------------------------------------------------
int ofApp::get_closest_skeleton()
{
	int index = 0;
	if (this->kinectDevice.getBodySkeletons().size() > 1) {
		float min_dist = FLT_MAX;
		int min_index = 0;
		for (int i = 0; i < this->kinectDevice.getBodySkeletons().size(); i++) {
			auto temp = this->kinectDevice.getBodySkeletons()[i].joints[K4ABT_JOINT_PELVIS].position;
			glm::vec3 pos = glm::vec3(temp.xyz.x, temp.xyz.y, temp.xyz.z);
			float dist = glm::distance2(pos, glm::vec3());
			if (dist < min_dist){
				min_dist = dist;
				min_index = i;
			}
		}
		index = min_index;
	}
	return index;
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
void ofApp::mouseDragged(int x, int y, int button)
{
	if (button == 1)
	{
		this->kinectDevice.getBodyTracker().jointSmoothing = ofMap(x, 0, ofGetWidth(), 0.0f, 1.0f, true);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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
