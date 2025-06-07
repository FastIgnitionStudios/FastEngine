#include "EnginePCH.h"
#include "PrimitiveRendererVK.h"
#include "glm.hpp"
#include "EngineApp.h"
#include "Filesystem.h"
#include "MeshVK.h"
#include "GraphicsPipelineVK.h"
#include "RendererVK.h"
#include "ShaderVK.h"
#include "Rendering/Mesh.h"
#include "Rendering/Renderer.h"

namespace Engine
{
    PrimitiveRendererVK::PrimitiveRendererVK()
    {
    }

    PrimitiveRendererVK::~PrimitiveRendererVK()
    {
    }

    void PrimitiveRendererVK::DrawQuad_Implementation(const TransformComponent& transform)
    {
        Renderer::GetRenderer().As<RendererVK>()->EnquePrimitive([=, this]()
        {
            auto renderer = Renderer::GetRenderer();
            std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
            std::vector<Vertex> vertices = {
                {{0, 0, 0,}},
                {{0, 1, 0,}},
                {{1, 1, 0,}},
                {{1, 0, 0,}},
            };
            
            auto mesh = Ref<MeshVK>::Create(indices, vertices, renderer.Raw());
            for (auto& m : mesh->GetMeshes())
            {
                for (auto& geo : m->geometries)
                {
                    geo.offset = transform.GetTransform();
                }
            }
            mesh->SetTransform(transform);
            return mesh;
        });
    }

    void PrimitiveRendererVK::DrawQuad_Implementation(const TransformComponent& transform, glm::vec4 color)
    {
    }

    void PrimitiveRendererVK::DrawCube_Implementation(const TransformComponent& transform)
    {
    }

    void PrimitiveRendererVK::DrawCube_Implementation(const TransformComponent& transform, glm::vec4 color)
    {
    }

    void PrimitiveRendererVK::DrawSphere_Implementation(const TransformComponent& transform, float radius)
    {
    }

    void PrimitiveRendererVK::DrawSphere_Implementation(const TransformComponent& transform, float radius, glm::vec4 color)
    {
    }
}
