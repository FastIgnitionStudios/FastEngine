#pragma once

#include "Core/UUID.h"
#include "ScriptEntity.h"
#include "glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL

#include "gtx/transform.hpp"
#include "Components_RGen.h"

namespace Engine
{
    RCOMPONENT()
    struct IDComponent
    {
        RGEN()
        UUID ID;
        
        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
    };

    RCOMPONENT()
    struct MiscComponent
    {
        RGEN()
        bool Active = true;
    };

    
    RCOMPONENT()
    struct NameComponent
    {
        RGEN()
        std::string Name;

        NameComponent() = default;
        NameComponent(const std::string& name) : Name(name) {}
        NameComponent(const NameComponent&) = default;

        operator std::string() const { return Name; }
    };

    RCOMPONENT()
    struct TransformComponent
    {
        RGEN()
        glm::vec3 Translation = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.f); // Stored as radians
        glm::vec3 Scale = glm::vec3(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3 translation) : Translation(translation) {}

        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
            
        }
    };

    RCOMPONENT()
    struct NativeScriptComponent
    {
        RGEN()
        Ref<ScriptEntity> script;
        entt::type_info scriptType{std::in_place_type<void>};
        

        template<typename T, typename... Args>
        void CreateScript(Entity entity, Args&&... args)
        {
            script = Ref<T>::Create(entity, std::forward<Args>(args)...);
            scriptType = entt::type_id<T>();
        }
        

        template<typename T>
        bool IsType() const
        {
            return scriptType == entt::type_id<T>();
        }
    };
    
    
}
