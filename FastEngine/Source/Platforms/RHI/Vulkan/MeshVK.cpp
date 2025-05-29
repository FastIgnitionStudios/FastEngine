#include "EnginePCH.h"
#include "MeshVK.h"

#include "RendererVK.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <ranges>

#include "glm/gtx/quaternion.hpp"

#include "fastgltf/glm_element_traits.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"


namespace Engine
{
    MeshVK::MeshVK()
    {
    }

    MeshVK::MeshVK(MeshComponent mesh, RendererVK* renderer)
    {
        meshes = CreateMeshAsset(mesh, renderer);
    }

    MeshVK::~MeshVK()
    {

    }

    std::vector<std::shared_ptr<MeshAssetVK>> MeshVK::CreateMeshAsset(MeshComponent meshComp, RendererVK* renderer)
    {

        fastgltf::Parser parser;

        auto data = fastgltf::GltfDataBuffer::FromPath(meshComp.filePath);
        if (data.error() != fastgltf::Error::None)
        {
            ENGINE_CORE_ERROR("File couldn't be loaded: {0}", meshComp.filePath);
        }

        auto asset = parser.loadGltf(data.get(), std::filesystem::path(meshComp.filePath).parent_path(), fastgltf::Options::LoadExternalBuffers);
        if (auto error = asset.error(); error != fastgltf::Error::None)
        {
            ENGINE_CORE_ERROR("File couldn't be loaded: {0}", meshComp.filePath);
        }

        

        
        std::vector<std::shared_ptr<MeshAssetVK>> meshes;

        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;

        for (fastgltf::Mesh& mesh : asset->meshes)
        {
            MeshAssetVK newMesh;

            newMesh.name = mesh.name;

            indices.clear();
            vertices.clear();

            for (auto&& p : mesh.primitives)
            {
                GeometryVK newSurface;
                newSurface.startIndex = (uint32_t)indices.size();
                newSurface.indexCount = (uint32_t)asset->accessors[p.indicesAccessor.value()].count;

                size_t initialVtx = vertices.size();

                {
                    fastgltf::Accessor& indexAccessor = asset->accessors[p.indicesAccessor.value()];
                    indices.reserve(indices.size() + indexAccessor.count);

                    fastgltf::iterateAccessor<std::uint32_t>(asset.get(), indexAccessor, [&](std::uint32_t idx)
                    {
                        indices.push_back(idx + initialVtx);
                    });
                }

                {
                    fastgltf::Accessor& posAccessor = asset->accessors[p.findAttribute("POSITION")->accessorIndex];
                    vertices.resize(vertices.size() + posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(asset.get(), posAccessor, [&](glm::vec3 v, size_t index)
                    {
                        Vertex newvtx;
                        newvtx.position = v;
                        newvtx.normal = {1, 0, 0};
                        newvtx.color = {1, 1, 1, 1};
                        newvtx.uv_x = 0;
                        newvtx.uv_y = 0;
                        vertices[initialVtx + index] = newvtx;
                    });
                }

                auto normals = p.findAttribute("NORMAL");
                if (normals != p.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(asset.get(), asset->accessors[(*normals).accessorIndex],
                                                                  [&](glm::vec3 v, size_t index)
                                                                  {
                                                                      vertices[initialVtx + index].normal = v;
                                                                  });
                }

                auto uv = p.findAttribute("TEXCOORD_0");
                if (uv != p.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(asset.get(), asset->accessors[(*uv).accessorIndex],
                                                                  [&](glm::vec2 v, size_t index)
                                                                  {
                                                                      vertices[initialVtx + index].uv_x = v.x;
                                                                      vertices[initialVtx + index].uv_y = v.y;
                                                                  });
                }

                auto color = p.findAttribute("COLOR_0");
                if (color != p.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(asset.get(), asset->accessors[(*color).accessorIndex],
                                                                  [&](glm::vec4 v, size_t index)
                                                                  {
                                                                      vertices[initialVtx + index].color = v;
                                                                  });
                }

                newMesh.geometries.push_back(newSurface);



            }

            constexpr bool OverrideColors = true;
            if (OverrideColors)
            {
                for (Vertex& vtx : vertices)
                {
                    vtx.color = {vtx.normal, 1};
                }
            }

            newMesh.buffers = renderer->UploadMeshes(indices, vertices);
            renderer->MainDeletionQueue.PushFunction([=, this]()
            {
                DestroyBuffer(newMesh.buffers.vertexBuffer, renderer->Allocator);
                DestroyBuffer(newMesh.buffers.indexBuffer, renderer->Allocator);
            });
            

            meshes.emplace_back(std::make_shared<MeshAssetVK>(std::move(newMesh)));
        
            
        }

        return meshes;
    }

    void MeshVK::Draw(const glm::mat4& worldTransform, DrawContext& context)
    {
        glm::mat4 transform = worldTransform;

        for (auto& meshAsset : meshes)
        {
            for (auto& mesh : meshAsset.get()->geometries)
            {
                RenderObject def;
                def.indexCount = mesh.indexCount;
                def.firstIndex = mesh.startIndex;
                def.indexBuffer = &meshAsset.get()->buffers.indexBuffer;
                def.material = &mesh.material.data;

                def.transform = transform;
                def.vertexBufferAddress = meshAsset.get()->buffers.vertexBufferAddress;

                context.OpaqueSurfaces.push_back(def);
            }
        }
    }
}
