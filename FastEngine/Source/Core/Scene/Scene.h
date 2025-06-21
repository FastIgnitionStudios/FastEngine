#pragma once

#include "Core.h"
#include "entt.hpp"


namespace Engine
{
    class ScriptEntity;
}

namespace Engine
{
    struct NativeScriptComponent;
}

static const char* DefaultComponents[] = { "TransformComponent",  "IDComponent", "NameComponent" };

namespace Engine
{
    class Entity;

    struct RegisteredComponent
    {
        std::string ComponentName;
        std::function<void(entt::registry&, entt::entity)> Constructor;
        std::function<bool(entt::registry&, entt::entity)> EntityHasComponent;
    };

    class Scene : public Engine
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);

        template <typename... Args>
        auto GetEntitiesByComponents()
        {
            return Registry.view<Args...>();
        }

        template <typename T, typename... Args>
        static bool RegisterComponentDeferred(Args&&... args)
        {
            auto& comps = GetRegisteredComponents();
            RegisteredComponent registat{};
            registat.ComponentName = GetClassName<T>();
            registat.Constructor = [&](entt::registry& registry, entt::entity entity)
            {
                registry.emplace<T>(entity, std::forward<Args>(args)...);
            };
            registat.EntityHasComponent = [&](entt::registry& registry, entt::entity entity)
            {
                return registry.any_of<T>(entity);
            };
            comps.push_back(registat);
            return true;
        }
        

        static std::vector<RegisteredComponent>& GetRegisteredComponents()
        {
            static std::vector<RegisteredComponent> RegisteredComponents;
            return RegisteredComponents;
        }
        

        Entity GetEntityByTag(const std::string& tag);

        void OnUpdate(float DeltaTime);

    private:
        entt::registry Registry;

        

        friend class Entity;
        friend class SceneHierarchyPanel;
        friend class SceneSerializer;
    };
}
