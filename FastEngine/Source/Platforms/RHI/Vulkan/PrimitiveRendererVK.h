#pragma once
#include "MaterialVK.h"
#include "MeshVK.h"
#include "UtilsVK.h"
#include "Rendering/PrimitiveRenderer.h"

namespace Engine
{

    class PrimitiveRendererVK : public PrimitiveRenderer
    {
    public:
        PrimitiveRendererVK();
        virtual ~PrimitiveRendererVK();
    protected:
        void DrawQuad_Implementation(const TransformComponent& transform) override;
        void DrawQuad_Implementation(const TransformComponent& transform, glm::vec4 color) override;
        void DrawCube_Implementation(const TransformComponent& transform) override;
        void DrawCube_Implementation(const TransformComponent& transform, glm::vec4 color) override;
        void DrawSphere_Implementation(const TransformComponent& transform, float radius) override;
        void DrawSphere_Implementation(const TransformComponent& transform, float radius, glm::vec4 color) override;

    private:
    };
}
