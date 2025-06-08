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

        template<typename ...Args>
        auto GetEntitiesByComponents()
        {
            return Registry.view<Args...>();
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
