#pragma once

#include "Engine.h"

class PlanetScript : public Engine::ScriptEntity
{
public:
    PlanetScript(::Engine::Entity entity);
    void SetTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    void SetVelocity(glm::vec3 vel) { this->velocity = vel;}
    void SetAcceleration(glm::vec3 acc) { this->acceleration = acc;}
protected:
    void OnCreate() override;
    
    void OnUpdate(float DeltaTime) override;

private:
    glm::vec3 velocity{0};
    glm::vec3 acceleration{0};
    
};
