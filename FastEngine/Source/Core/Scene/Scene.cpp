#include "EnginePCH.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Input/Input.h"
#include "Rendering/Camera.h"

namespace Engine
{
    Scene::Scene()
    {
        CreateEntity("Test");
        auto camera = CreateEntity("Camera");
        auto& cameraComp = camera.AddComponent<CameraComponent>();
        cameraComp.camera->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        cameraComp.camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
        cameraComp.camera->SetPitch(0);
        cameraComp.camera->SetYaw(0);
        cameraComp.camera->SetMousePosition(Input::GetMouseX(), Input::GetMouseY());
    }

    Scene::~Scene()
    {
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity(Registry.create(), this);
        entity.AddComponent<IDComponent>();
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<NameComponent>(name);
        tag.Name = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::OnUpdate(float DeltaTime)
    {
        auto view = Registry.view<TransformComponent>();
        for (auto entity : view)
        {
            auto& transform = view.get<TransformComponent>(entity);
        }
        

    }
}
