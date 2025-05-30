#include "EnginePCH.h"
#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "ext/quaternion_trigonometric.hpp"
#include "gtx/transform.hpp"
#include "gtx/quaternion.hpp"
#include "Input/Input.h"

namespace Engine
{
    Camera::Camera()
    {
    }

    Camera::~Camera()
    {
    }

    void Camera::Update(float deltaTime)
    {
        float yawDelta = 0;
        float pitchDelta = 0;
        velocity = glm::vec3(0);
        
        if (Input::IsMouseButtonPressed(FAST_MOUSE_BUTTON_RIGHT))
        {
            if (Input::IsKeyPressed(FAST_KEY_A))
                velocity.x = -.1;
            if (Input::IsKeyPressed(FAST_KEY_D))
                velocity.x = .1;
            if (Input::IsKeyPressed(FAST_KEY_W))
                velocity.z = -.1;
            if (Input::IsKeyPressed(FAST_KEY_S))
                velocity.z = .1;

            yawDelta = (Input::GetMouseX() - mousePosition.x) * 0.003f;
            pitchDelta = (Input::GetMouseY() - mousePosition.y) * 0.003f;

        }

        
        mousePosition.x = Input::GetMouseX();
        yaw += yawDelta;

        
        mousePosition.y = Input::GetMouseY();
        pitch -= pitchDelta;

        glm::mat4 rotation = GetRotationMatrix();
        position += glm::vec3(rotation * glm::vec4(velocity * 0.5f, 0.0f));
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 cameraRotation = GetRotationMatrix();
        return glm::inverse(cameraTranslation * cameraRotation);
        
    }

    glm::mat4 Camera::GetRotationMatrix() const
    {
        glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
        glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3(0, -1, 0));

        return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
        
    }

    void Camera::OnMouseScroll(Event& e)
    {
        
    }

    void Camera::OnEvent(Event& e)
    {
        if (e.GetEventType() == EventType::MouseScrolled)
            OnMouseScroll(e);
    }
}
