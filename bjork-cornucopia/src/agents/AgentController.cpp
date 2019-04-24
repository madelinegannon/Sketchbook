#include "AgentController.h"


//--------------------------------------------------------------
AgentController::AgentController(){}

//--------------------------------------------------------------
AgentController::~AgentController(){}

//--------------------------------------------------------------
void AgentController::setup(int num_agents)
{
	NUM_AGENTS = num_agents;
	for (int i=0; i<NUM_AGENTS; i++){
        agents.push_back(new Agent());
        agents[i]->setup(i);
        
        targets.push_back(ofNode());
        targets[i] = agents[i]->pose;
    }
	
    setup_gui();
}

//--------------------------------------------------------------
void AgentController::update()
{
    for (int i=0; i<NUM_AGENTS; i++){
        
        agents[i]->apply_forces(gravity);
        
        agents[i]->move(agents, targets[i]);
        
        // update the  agent params with agent 0
        if (match_all){
            agents[i]->set_params(agents[0]->radius, agents[0]->kp, agents[0]->kd, agents[0]->steering_scalar);
        }

        agents[i]->update();
    }    
}

//--------------------------------------------------------------
void AgentController::draw()
{
    for (int i=0; i<NUM_AGENTS; i++){
        agents[i]->draw();
    }
}

//--------------------------------------------------------------
void AgentController::setup_gui()
{
    params.setName("Agent_Controller");
    params.add(gravity.set("Gravity", ofVec3f(0,0,0), ofVec3f(-20,-20,-20), ofVec3f()));
    params.add(match_all.set("Match_All_Params", true));
    
    for (int i=0; i<NUM_AGENTS; i++){
        params.add(agents[i]->params);
    }
}

//--------------------------------------------------------------
void AgentController::set_targets(vector<ofMatrix4x4*> tgts)
{
    for (int i=0; i<NUM_AGENTS; i++){
        ofNode tgt;
        //    tgt.setGlobalOrientation(target->getRotate());
        tgt.setGlobalPosition(tgts[i]->getTranslation());
        targets[i] = tgt;
    }
}

