#pragma once

#include "ofMain.h"


class Particle {

public:
    
    Particle();
    
    Particle(int id = -1,
             glm::vec3 position = glm::vec3(),
             glm::vec3 velocity = glm::vec3(),
             glm::vec3 acceleration = glm::vec3(),
             float radius = 5,
             float max_speed = NULL
             );
    
    void update();
    void draw();
    void apply_force(glm::vec3 force);
    bool is_alive();
    void kill();
    string to_string();

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float radius;
    float max_speed;
    int id;

private:
    bool active = true;

};
