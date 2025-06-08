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
            std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
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
        Renderer::GetRenderer().As<RendererVK>()->EnquePrimitive([=, this]()
        {
            auto renderer = Renderer::GetRenderer();
            std::vector<uint32_t> indices;
            std::vector<Vertex> vertices;
            constexpr uint32_t resolution{16};
            const uint32_t phi_count{resolution};
            const uint32_t theta_count{resolution};
            const glm::float32_t theta_step{glm::pi<glm::float32_t>() / theta_count};
            const glm::float32_t phi_step{glm::two_pi<glm::float32_t>() / phi_count};
            const uint32_t index_count{2 * 3 * phi_count + 2 * 3 * phi_count * (theta_count - 2)};
            const uint32_t vertex_count{2 + phi_count * (theta_count - 1)};

            // Compute vertices for sphere
            
            // top vertex
            vertices.push_back({glm::vec3(0, radius, 0)});
            
            for (uint32_t i = 1; i <= (theta_count - 1); ++i)
            {
                const glm::float32_t theta = (float)i * theta_step;
                for (uint32_t j = 0; j < phi_count; ++j)
                {
                    const glm::float32_t phi{(float)j * phi_step};
                    glm::float32_t x = radius * glm::sin(theta) * glm::cos(phi);
                    glm::float32_t y = radius * glm::cos(theta);
                    glm::float32_t z = -radius * glm::sin(theta) * glm::sin(phi);
                    vertices.push_back({{x, y, z}});
                }
            }
            // Bottom vertex
            vertices.push_back({glm::vec3(0, -radius, 0)});

            // Indices for top cap
            for (uint32_t i = 0; i < phi_count - 1; ++i)
            {
                indices.push_back(0);
                indices.push_back(i + 1);
                indices.push_back(i + 2);
            }

            indices.push_back(0);
            indices.push_back(phi_count);
            indices.push_back(1);
            //indices for middle section between top and bottom caps
            for (uint32_t i = 0; i < (theta_count - 2); ++i)
            {
                for (uint32_t j = 0; j < (phi_count - 1); ++j)
                {
                    const uint32_t index[4]
                    {
                        1 + j + i * phi_count,
                        1 + j + (i + 1) * phi_count,
                        1 + (j + 1) + (i + 1) * phi_count,
                        1 + (j + 1) + i * phi_count
                    };

                    indices.push_back(index[0]);
                    indices.push_back(index[1]);
                    indices.push_back(index[2]);

                    indices.push_back(index[2]);
                    indices.push_back(index[3]);
                    indices.push_back(index[0]);
                }

                const uint32_t index[4]
                {
                    phi_count + i * phi_count,
                    phi_count + (i + 1) * phi_count,
                    1 + (i + 1) * phi_count,
                    1 + i * phi_count
                };

                indices.push_back(index[0]);
                indices.push_back(index[1]);
                indices.push_back(index[2]);

                indices.push_back(index[2]);
                indices.push_back(index[3]);
                indices.push_back(index[0]);
            }

            // Indices for bottom cap
            const uint32_t south_pole_index{(uint32_t)vertices.size() - 1};
            for (uint32_t i = 0; i < (phi_count - 1); ++i)
            {
                indices.push_back(south_pole_index);
                indices.push_back(south_pole_index - phi_count + i + 1);
                indices.push_back(south_pole_index - phi_count + i);
            }

            indices.push_back(south_pole_index);
            indices.push_back(south_pole_index - phi_count);
            indices.push_back(south_pole_index - 1);

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

    void PrimitiveRendererVK::DrawSphere_Implementation(const TransformComponent& transform, float radius,
                                                        glm::vec4 color)
    {
    }
}
