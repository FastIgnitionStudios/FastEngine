#pragma once

#include "Core/UUID.h"

#include "glm.hpp"

namespace Engine
{
    struct IDComponent
    {
        UUID ID;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
    };
    
    struct NameComponent
    {
        std::string Name;

        NameComponent() = default;
        NameComponent(const std::string& name) : Name(name) {}
        NameComponent(const NameComponent&) = default;

        operator std::string() const { return Name; }
    };

    struct TransformComponent
    {
        glm::mat4 Transform = glm::mat4(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& transform) : Transform(transform) {}

        operator glm::mat4& () { return Transform; }
        operator const glm::mat4& () const { return Transform; }
        
    };
}
