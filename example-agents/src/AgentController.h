#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxEasing.h"
#include "Agent.h"

class AgentController
{
public:
	AgentController();
	~AgentController();

	void setup(int num_agents);
	void update();
	void draw();
    
    void set_targets(vector<ofMatrix4x4*> tgts);

    ofParameterGroup params;

private:
    int NUM_AGENTS;
    
    vector<Agent*> agents;
    vector<ofNode> targets;

    void setup_gui();

    ofParameter<ofVec3f> gravity;
    ofParameter<bool> match_all;
};

