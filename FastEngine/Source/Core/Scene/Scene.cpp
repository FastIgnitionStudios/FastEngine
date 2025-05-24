#include "EnginePCH.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Engine
{
    Scene::Scene()
    {
        
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
