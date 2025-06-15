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
        static bool RegisterComponentDeferred(Args&&... args)
        {
           GetRegisteredComponents()[std::string(GetClassName<T>())] = [&](entt::registry& reg, entt::entity entity)
            {
                reg.emplace<T>(entity, std::forward<Args>(args)...);
            };
            return true;
        }

        static std::map<std::string, std::function<void(entt::registry&, entt::entity)>>& GetRegisteredComponents()
        {
            static std::map<std::string, std::function<void(entt::registry&, entt::entity)>> RegisteredComponents;
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
