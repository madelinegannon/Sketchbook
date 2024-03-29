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
		deviceSettings.updateColor = false;
		//deviceSettings.colorResolution = K4A_COLOR_RESOLUTION_1080P;
		deviceSettings.updateWorld = true;
		deviceSettings.updateVbo = false;
		this->kinectDevice.startCameras(deviceSettings);
	
		auto bodyTrackerSettings = ofxAzureKinect::BodyTrackerSettings();
		bodyTrackerSettings.sensorOrientation = K4ABT_SENSOR_ORIENTATION_DEFAULT;
		//bodyTrackerSettings.processingMode = K4ABT_TRACKER_PROCESSING_MODE_CPU;
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

#ifdef LOAD_CONFIG_FILE
	load_settings();
#endif // LOAD_CONFIG_FILE

	setup_comms();
}

//--------------------------------------------------------------
void ofApp::setup_comms()
{
	// open an outgoing connection to HOST:PORT
	//create the socket and set to send to 127.0.0.1:11999
	ofxUDPSettings settings;
	settings.sendTo(ip_address, port);
	settings.blocking = false;

	if (udpConnection.Setup(settings)) {
		ofLogNotice(__FUNCTION__) << "Streaming Skeletons to " << ip_address << ":" << ofToString(port);
	}
	else {
		ofLogWarning(__FUNCTION__) << "Not able to connect to " << ip_address << ":" << ofToString(port);
	}
}

//--------------------------------------------------------------
void ofApp::send_message(k4abt_skeleton_t body, int id)
{
	ofJson _body;
	ofJson _id;
	_id["id"] = id;
	_body.push_back(_id);
	for (int i = 0; i < K4ABT_JOINT_COUNT; ++i)
	{
		auto name = k4abt_joint_names[i];
		auto joint = body.joints[i];
		auto position = toGlm(joint.position);
		auto orientation = toGlm(joint.orientation);

		ofJson _joint;
		_joint["name"] = name;
		_joint["x"] = position.x;
		_joint["y"] = position.y;
		_joint["z"] = position.z;
		_joint["qw"] = orientation.w;
		_joint["qx"] = orientation.x;
		_joint["qy"] = orientation.y;
		_joint["qz"] = orientation.z;

		_body.push_back(_joint);
	}
	auto message = _body.dump();
	udpConnection.Send(message.c_str(),message.length());
}

//--------------------------------------------------------------
/**
 * Returns the index of the person who is closest to the sensor.
*/
int ofApp::get_closest_body(vector<k4abt_skeleton_t> bodies)
{
	int index = -1;
	if (this->kinectDevice.getNumBodies() == 1)
	{
		return 0;
	}
	else
	{
		// Use the centroid of the 2 hands as the query point
		auto min_dist = FLT_MAX;
		auto origin = glm::vec3(0.0);
		for (int i=0; i<bodies.size(); i++)
		{
			auto hand_left = toGlm(bodies[i].joints[K4ABT_JOINT_WRIST_LEFT].position);
			auto hand_right = toGlm(bodies[i].joints[K4ABT_JOINT_WRIST_RIGHT].position);
			auto centroid = (hand_left + hand_right) / 2.0;
			auto dist = glm::distance2(centroid, origin);
			if (dist < min_dist)
			{
				min_dist = dist;
				index = i;
			}
		}
	}
	return index;
}

//--------------------------------------------------------------
void ofApp::exit()
{
	this->kinectDevice.close();
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (this->kinectDevice.getNumBodies() > 0)
	{
		auto& bodySkeletons = this->kinectDevice.getBodySkeletons();
		int closest = get_closest_body(bodySkeletons);
		int id = this->kinectDevice.getBodyIDs()[closest];
		//cout << "Closest is Body Index: " << ofToString(closest) << ", ID: " << ofToString(id) << endl;
		send_message(bodySkeletons[closest], id);		
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
			// send_message(get_closest_body(bodySkeletons));
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


/**
 * @brief Saves system settings to a file.
 *
 * @param (string)  filename: file saved to local /bin/data folder (must end in .xml). Defaults to "settings.xml"
 */
void ofApp::save_settings(string filename)
{
	ofxXmlSettings config;
	config.addTag("config");
	config.pushTag("config");
	config.addValue("ip_address", this->ip_address);
	config.addValue("port", this->port);
	config.popTag();

	if (filename == "")
		filename = "settings.xml";
	config.saveFile(filename);
	ofLogNotice("ofApp::save_settings") << "Saving settings file to: /bin/data/" << filename;
}

/**
 * @brief Loads system settings from a file.
 *
 * @param (string)  filename: file must be in local /bin/data folder (must end in .xml). Defaults to "settings.xml"
 */
void ofApp::load_settings(string filename)
{
	ofxXmlSettings config;
	if (filename == "")
		filename = "settings.xml";

	if (config.loadFile(filename)) {
		this->ip_address = config.getValue("config:ip_address", "");
		this->port = config.getValue("config:port", 0);
	}
	else {
		ofLogWarning("ofApp::load_settings") << "No settings file found at: /bin/data/" << filename;
	}
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key)
	{
	case 's':
	case 'S':
		save_settings();
		break;
	default:
		break;
	}
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
