#include "EnginePCH.h"
#include "MeshVK.h"

#include "glm/gtx/quaternion.hpp"

#include "fastgltf/glm_element_traits.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"


namespace Engine
{
    MeshVK::MeshVK()
    {
    }

    MeshVK::MeshVK(MeshComponent component, RendererVK* renderer)
    {
        CreateMeshAsset(component, renderer);
    }

    std::vector<std::shared_ptr<MeshAssetVK>> MeshVK::CreateMeshAsset(MeshComponent component, RendererVK* renderer)
    {
        fastgltf::GltfDataBuffer data;
        data.FromPath(component.filePath);

        constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

        fastgltf::Asset gltf;
        fastgltf::Parser parser;

        auto load = parser.loadGltfBinary(data, component.filePath, gltfOptions);
        if (load)
        {
            gltf = std::move(load.get());
        }
        else
        {
            ENGINE_CORE_ERROR("Failed to load mesh: {0}", component.filePath);
            return {};
        }

        std::vector<std::shared_ptr<MeshAssetVK>> meshes;

        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;

        for (fastgltf::Mesh& mesh : gltf.meshes)
        {
            MeshAssetVK newMesh;

            newMesh.name = mesh.name;

            indices.clear();
            vertices.clear();

            for (auto&& p : mesh.primitives)
            {
                GeometryVK newSurface;
                newSurface.startIndex = (uint32_t)indices.size();
                newSurface.indexCount = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;

                size_t initialVtx = vertices.size();

                {
                    fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];
                    indices.reserve(indices.size() + indexAccessor.count);

                    fastgltf::iterateAccessor<std::uint32_t>(gltf, indexAccessor, [&](std::uint32_t idx)
                    {
                        indices.push_back(idx + initialVtx);
                    });
                }

                {
                    fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
                    vertices.resize(vertices.size() + posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor, [&](glm::vec3 v, size_t index)
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
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).accessorIndex],
                                                                  [&](glm::vec3 v, size_t index)
                                                                  {
                                                                      vertices[initialVtx + index].normal = v;
                                                                  });
                }

                auto uv = p.findAttribute("TEXCOORD_0");
                if (uv != p.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).accessorIndex],
                                                                  [&](glm::vec2 v, size_t index)
                                                                  {
                                                                      vertices[initialVtx + index].uv_x = v.x;
                                                                      vertices[initialVtx + index].uv_y = v.y;
                                                                  });
                }

                auto color = p.findAttribute("COLOR_0");
                if (color != p.attributes.end())
                {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*color).accessorIndex],
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

            meshes.emplace_back(std::make_shared<MeshAssetVK>(std::move(newMesh)));

            
        }

        return meshes;
    }
}
