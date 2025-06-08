#include "PlanetManager.h"

#include "PlanetScript.h"
#include "gtc/type_ptr.hpp"

void PlanetManager::OnCreate()
{
    ScriptEntity::OnCreate();
}

void PlanetManager::OnUpdate(float DeltaTime)
{
    std::vector<PlanetScript*> planets;

    for (auto planet : planets)
    {
        // F = (G*m1*m2)/d^2
        // Where F = the force
        // G = gravitational constant
        // m1 = mass of planet 1
        // m2 = mass of planet 2
        // d = distance between them
        for (auto other : planets)
        {
            auto otherTransform = other->GetComponent<::Engine::TransformComponent>();
            auto thisTransform = planet->GetComponent<::Engine::TransformComponent>();
            if (planet == other) continue;
            float vel = 0.0f;
            float acc1 = 0.0f;
            float mass = 0.000001f;
           
            
            float dx = otherTransform.Translation.x - thisTransform.Translation.x;
            float dy = otherTransform.Translation.y - thisTransform.Translation.y;
            float dz = otherTransform.Translation.z - thisTransform.Translation.z;
            float dist = sqrt(dx * dx + dy * dy + dz * dz);
            glm::vec3 dir {dx / dist, dy / dist, dz / dist};
            float force = (9.81 * mass * mass) / (dist * dist);
            acc1 = force / mass;
            glm::vec3 acc {acc1 * dir.x, acc1 * dir.y, acc1 * dir.z};

            planet->SetAcceleration(acc);
            
        }
    }
}
