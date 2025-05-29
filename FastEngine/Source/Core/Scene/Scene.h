#pragma once

#include "Core.h"
#include "entt.hpp"



namespace Engine 
{
    class Entity;
    
    class Scene : public Engine
    {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());
        entt::registry& GetRegistry() { return Registry; }

        void OnUpdate(float DeltaTime);

    private:
        entt::registry Registry;
        
        friend class Entity;
        friend class SceneHierarchyPanel;
    };

 
}
