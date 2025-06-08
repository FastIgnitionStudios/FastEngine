#pragma once

#include "Core.h"
#include "Scene.h"
#include "entt.hpp"

namespace Engine
{
    class Entity
    {
    public:
        Entity(entt::entity id, Scene* scene);
        Entity() = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            ENGINE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            
            return Scene->Registry.emplace<T>(EntityID, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent()
        {
            ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            
            return Scene->Registry.get<T>(EntityID);
        }

        template<typename T>
        bool HasComponent()
        {
            return Scene->Registry.any_of<T>(EntityID);
        }

        template<typename T>
        void RemoveComponent()
        {
            ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            
            Scene->Registry.remove<T>(EntityID);
        }

        operator uint32_t() const { return (uint32_t)EntityID; }
        operator bool() const { return EntityID != entt::null; }
        bool operator==(const Entity& other) const
        {
            return EntityID == other.EntityID && Scene == other.Scene;
        }
        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

        Scene* GetScene() const { return Scene; }

    private:
        entt::entity EntityID = entt::null;
        Scene* Scene = nullptr;
    
    };
}
