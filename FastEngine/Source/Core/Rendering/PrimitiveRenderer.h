#pragma once

#include "Core.h"
#include "Scene/Components.h"

namespace Engine
{
    class PrimitiveRenderer : public Engine
    {
    public:
        PrimitiveRenderer() = default;
        virtual ~PrimitiveRenderer() = default;

        static void InitPrimitiveRenderer();

        static void DrawQuad(const TransformComponent& transform) { Instance->DrawQuad_Implementation(transform); }
        static void DrawQuad(const TransformComponent& transform, glm::vec4 color) { Instance->DrawQuad_Implementation(transform, color);}
        static void DrawCube(const TransformComponent& transform) { Instance->DrawCube_Implementation(transform);}
        static void DrawCube(const TransformComponent& transform, glm::vec4 color) { Instance->DrawCube_Implementation(transform, color);}
        static void DrawSphere(const TransformComponent& transform, float radius) { Instance->DrawSphere_Implementation(transform, radius);}
        static void DrawSphere(const TransformComponent& transform, float radius, glm::vec4 color) { Instance->DrawSphere_Implementation(transform, radius, color);}
        
    protected:
        virtual void DrawQuad_Implementation(const TransformComponent& transform) = 0;
        virtual void DrawQuad_Implementation(const TransformComponent& transform, glm::vec4 color) = 0;
        virtual void DrawCube_Implementation(const TransformComponent& transform) = 0;
        virtual void DrawCube_Implementation(const TransformComponent& transform, glm::vec4 color) = 0;
        virtual void DrawSphere_Implementation(const TransformComponent& transform, float radius) = 0;
        virtual void DrawSphere_Implementation(const TransformComponent& transform, float radius, glm::vec4 color) = 0;

    private:
        static PrimitiveRenderer* Instance;
    
    };
}
