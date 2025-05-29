#pragma once

#include "Core.h"
#include "glm.hpp"

namespace Engine
{
    class Camera;
    
    struct CameraComponent
    {
        ::UUID id {::UUID()};
        Ref<Camera> camera;

        CameraComponent() { camera = Ref<Camera>::Create(); }
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(Ref<Camera> inCamera) : camera(inCamera) {}
        
    };

    class Camera : public Engine
    {
    public:
        Camera();
        virtual ~Camera();

        void Update(float deltaTime);

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetRotationMatrix() const;

        void SetPosition(const glm::vec3& position) { this->position = position;}
        void SetVelocity(const glm::vec3& velocity) { this->velocity = velocity;}
        void SetPitch(float pitch) { this->pitch = pitch; }
        void SetYaw(float yaw) { this->yaw = yaw; }
        void SetMousePosition(float x, float y) { mousePosition = {x, y}; }

    private:
        
        
        glm::vec3 velocity {};
        float sensitivity = 0.1f;
        glm::vec3 position {};

        float pitch {0};
        float yaw {0};

        glm::vec2 mousePosition;


        
    };
}
