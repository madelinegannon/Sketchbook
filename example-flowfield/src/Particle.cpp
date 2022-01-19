#include "Particle.h"

Particle::Particle(int id,
                   glm::vec3 position,
                   glm::vec3 velocity,
                   glm::vec3 acceleration,
                   float radius,
                   float max_speed
                 ){
    this->id = id;
    this->position = position;
    this->velocity = velocity;
    this->acceleration = acceleration;
    this->radius = radius;
    this->max_speed = max_speed;
}

void Particle::update(){
    // move the particle
    velocity += acceleration;
    // clamp the velocity
    if (max_speed != NULL && glm::length2(velocity) > max_speed*max_speed){
        velocity = glm::normalize(velocity) * glm::vec3(max_speed);
//        velocity = scaled;
    }
        
    position += velocity;
    // reset the acceleration
    acceleration *= 0;
}

void Particle::apply_force(glm::vec3 force){
    acceleration += force;
}

bool Particle::is_alive(){
    return active;
}

void Particle::kill(){
    active = false;
}

void Particle::draw(){
    ofPushStyle();
    ofFill();
    ofSetColor(ofColor::magenta, 60);
    ofDrawEllipse(position, radius*2, radius*2);
    ofSetColor(ofColor::cyan);
    ofSetLineWidth(1);
    ofDrawLine(position, this->position+this->velocity);
    ofPopStyle();
}

string Particle::to_string(){
    stringstream ss;
    ss << "Particle " << id << ":";
    ss << "\n\tpos:\t{ " << ofToString(position) << " }" ;
    ss << "\n\tvel:\t{ " << ofToString(velocity) << " }" ;
    ss << "\n\taccel\t{ " << ofToString(acceleration) << " }" << endl;
    return ss.str();
}
