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

namespace Engine
{
    class Entity;

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
        static void RegisterComponentDeferred(Args&&... args)
        {
            RegisteredComponents[std::string(GetClassName<T>())] = [&](entt::registry& reg, entt::entity entity)
            {
                reg.emplace<T>(entity, std::forward<Args>(args)...);
            };
        }

        static std::function<void(entt::registry&, entt::entity)> GetRegisteredComponentByClass(const std::string& className);
        static std::map<std::string, std::function<void(entt::registry&, entt::entity)>>& GetRegisteredComponents() { return RegisteredComponents; }

        Entity GetEntityByTag(const std::string& tag);

        void OnUpdate(float DeltaTime);

    private:
        entt::registry Registry;

        static std::map<std::string, std::function<void(entt::registry&, entt::entity)>> RegisteredComponents;

        friend class Entity;
        friend class SceneHierarchyPanel;
        friend class SceneSerializer;
    };
}
