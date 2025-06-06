#include "EnginePCH.h"
#include "MeshVK.h"

#include "RendererVK.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <ranges>

#include "EngineApp.h"
#include "MaterialVK.h"
#include "glm/gtx/quaternion.hpp"

#include "fastgltf/glm_element_traits.hpp"
#include "fastgltf/core.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/math.hpp"
#include "STBImage/include/stb_image.h"


namespace Engine
{
    MeshVK::MeshVK()
    {
    }

    MeshVK::MeshVK(MeshComponent mesh, RendererVK* renderer)
    {
    }

    MeshVK::MeshVK(std::string filePath, RendererVK* renderer)
    {
        fastgltf::Parser parser;

        constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
            fastgltf::Options::LoadExternalImages | fastgltf::Options::LoadExternalBuffers;

        auto data = fastgltf::GltfDataBuffer::FromPath(filePath);
        if (data.error() != fastgltf::Error::None)
            ENGINE_CORE_ERROR("File couldn't be loaded: {0}", filePath);

        fastgltf::Asset gltf;


        auto type = fastgltf::determineGltfFileType(data.get());
        if (type == fastgltf::GltfType::glTF)
        {
            auto load = parser.loadGltf(data.get(), std::filesystem::path(filePath).parent_path(),
                                        gltfOptions);
            if (!load)
                ENGINE_CORE_ERROR("File couldn't be loaded: {0}", filePath);
            gltf = std::move(load.get());
        }
        else if (type == fastgltf::GltfType::GLB)
        {
            auto load = parser.loadGltfBinary(data.get(), std::filesystem::path(filePath).parent_path(), gltfOptions);
            if (!load)
                ENGINE_CORE_ERROR("File couldn't be loaded: {0}", filePath);
            gltf = std::move(load.get());
        }
        else
        {
            ENGINE_CORE_ASSERT(false, "Unknown file type");
            return;
        }

        std::vector<DescriptorAllocatorDynamic::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}
        };

        meshDescriptorPool.Init(renderer->GetDevice(), gltf.materials.size(),
                                sizes);

        for (fastgltf::Sampler& sampler : gltf.samplers)
        {
            VkSamplerCreateInfo samplerInfo = {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, .pNext = nullptr};
            samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
            samplerInfo.minLod = 0;

            samplerInfo.magFilter = ExtractFilter(sampler.magFilter.value_or(fastgltf::Filter::Nearest));
            samplerInfo.minFilter = ExtractFilter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

            samplerInfo.mipmapMode = ExtractMipmapMode(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

            VkSampler newSampler;
            vkCreateSampler(renderer->GetDevice(), &samplerInfo, nullptr,
                            &newSampler);

            samplers.push_back(newSampler);
        }


        materialDataBuffer = CreateBuffer(sizeof(PBRMaterialVK::MaterialConstants) * gltf.materials.size(),
                                          renderer->Allocator,
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
        int indexData = 0;
        PBRMaterialVK::MaterialConstants* sceneMaterialConstants = (PBRMaterialVK::MaterialConstants*)materialDataBuffer
            .allocationInfo.pMappedData;

        for (fastgltf::Image& image : gltf.images)
        {
            images.push_back(renderer->errorCheckerboardImage);
        }

        for (fastgltf::Material& mat : gltf.materials)
        {
            std::shared_ptr<GLTFMaterial> newMat = std::make_shared<GLTFMaterial>();
            materials.push_back(newMat);

            PBRMaterialVK::MaterialConstants constants;
            constants.colorFactors.x = mat.pbrData.baseColorFactor[0];
            constants.colorFactors.y = mat.pbrData.baseColorFactor[1];
            constants.colorFactors.z = mat.pbrData.baseColorFactor[2];
            constants.colorFactors.w = mat.pbrData.baseColorFactor[3];

            constants.metallicRoughnessFactors.x = mat.pbrData.metallicFactor;
            constants.metallicRoughnessFactors.y = mat.pbrData.roughnessFactor;

            sceneMaterialConstants[indexData] = constants;

            MaterialPass passType = MaterialPass::MainColor;
            if (mat.alphaMode == fastgltf::AlphaMode::Blend)
                passType = MaterialPass::Transparent;

            PBRMaterialVK::MaterialResources materialResources;

            materialResources.colorImage = renderer->GetWhiteImage();
            materialResources.colorSampler = renderer->GetDefaultSamplerLinear();
            materialResources.metallicRoughnessImage = renderer->GetWhiteImage();
            materialResources.metalRoughnessSampler = renderer->GetDefaultSamplerLinear();

            materialResources.dataBuffer = materialDataBuffer.buffer;
            materialResources.dataBufferOffset = sizeof(PBRMaterialVK::MaterialConstants) * indexData;

            if (mat.pbrData.baseColorTexture.has_value())
            {
                size_t img = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
                size_t sampler = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

                materialResources.colorImage = images[img];
                materialResources.colorSampler = samplers[sampler];
            }

            newMat->data = renderer->GetMetalRoughnessMaterial().WriteMaterial(
                renderer->GetDevice(), passType, materialResources,
                meshDescriptorPool);

            indexData++;
        }


        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;

        for (fastgltf::Mesh& mesh : gltf.meshes)
        {
            std::shared_ptr<MeshAssetVK> newMesh = std::make_shared<MeshAssetVK>();
            meshes.push_back(newMesh);
            newMesh->name = mesh.name;

            indices.clear();
            vertices.clear();

            for (auto&& p : mesh.primitives)
            {
                GeometryVK newSurface;
                newSurface.startIndex = (uint32_t)indices.size();
                newSurface.indexCount = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;
                newSurface.offset = glm::mat4(1) + glm::translate(glm::mat4(1), glm::vec3(5, 0, 0));

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
                                                                  }
                    );
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
                                                                  }
                    );
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


                if (p.materialIndex.has_value())
                    newSurface.material = *materials[p.materialIndex.value()];
                else
                    newSurface.material = *materials[0];


                newMesh->geometries.push_back(newSurface);
            }


            newMesh->buffers = renderer->UploadMeshes(indices, vertices);

            renderer->MainDeletionQueue.PushFunction([=, this]()
            {
                DestroyBuffer(newMesh->buffers.vertexBuffer, renderer->Allocator);
                DestroyBuffer(newMesh->buffers.indexBuffer, renderer->Allocator);
            });
        }

        DestroyBuffer(materialDataBuffer, renderer->Allocator);
    }

    MeshVK::MeshVK(std::vector<uint32_t> indices, std::vector<Vertex> vertices, RendererVK* renderer)
    {

        std::vector<DescriptorAllocatorDynamic::PoolSizeRatio> sizes = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}
        };

        meshDescriptorPool.Init(renderer->GetDevice(), 3,
                                sizes);
        
        std::shared_ptr<MeshAssetVK> newMesh = std::make_shared<MeshAssetVK>();
        meshes.push_back(newMesh);
        newMesh->name = "Mesh";

        materialDataBuffer = CreateBuffer(sizeof(PBRMaterialVK::MaterialConstants),
                                          renderer->Allocator,
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
        int indexData = 0;
        PBRMaterialVK::MaterialConstants* sceneMaterialConstants = (PBRMaterialVK::MaterialConstants*)materialDataBuffer
            .allocationInfo.pMappedData;

        images.push_back(renderer->errorCheckerboardImage);

        std::shared_ptr<GLTFMaterial> newMat = std::make_shared<GLTFMaterial>();
        materials.push_back(newMat);

        PBRMaterialVK::MaterialConstants constants;
        constants.colorFactors.x = 0;
        constants.colorFactors.y = 0;
        constants.colorFactors.z = 0;
        constants.colorFactors.w = 0;

        constants.metallicRoughnessFactors.x = 0;
        constants.metallicRoughnessFactors.y = 0;

        sceneMaterialConstants[indexData] = constants;

        MaterialPass passType = MaterialPass::MainColor;

        PBRMaterialVK::MaterialResources materialResources;

        materialResources.colorImage = renderer->GetWhiteImage();
        materialResources.colorSampler = renderer->GetDefaultSamplerLinear();
        materialResources.metallicRoughnessImage = renderer->GetWhiteImage();
        materialResources.metalRoughnessSampler = renderer->GetDefaultSamplerLinear();

        materialResources.dataBuffer = materialDataBuffer.buffer;
        materialResources.dataBufferOffset = sizeof(PBRMaterialVK::MaterialConstants) * indexData;


        newMat->data = renderer->GetMetalRoughnessMaterial().WriteMaterial(
            renderer->GetDevice(), passType, materialResources,
            meshDescriptorPool);

        GeometryVK newSurface;
        newSurface.startIndex = 0;
        newSurface.indexCount = (uint32_t)indices.size();
        newMesh->geometries.push_back(newSurface);
        newMesh->buffers = renderer->UploadMeshes(indices, vertices);

        renderer->MainDeletionQueue.PushFunction([=, this]()
        {
            DestroyBuffer(newMesh->buffers.vertexBuffer, renderer->Allocator);
            DestroyBuffer(newMesh->buffers.indexBuffer, renderer->Allocator);
        });

        DestroyBuffer(materialDataBuffer, renderer->Allocator);
    }

    MeshVK::~MeshVK()
    {
        meshes.clear();
        samplers.clear();
        images.clear();
        materials.clear();
    }

    VkFilter MeshVK::ExtractFilter(fastgltf::Filter filter)
    {
        switch (filter)
        {
        case fastgltf::Filter::Nearest:
        case fastgltf::Filter::NearestMipMapNearest:
        case fastgltf::Filter::NearestMipMapLinear:
            return VK_FILTER_NEAREST;

        case fastgltf::Filter::Linear:
        case fastgltf::Filter::LinearMipMapNearest:
        case fastgltf::Filter::LinearMipMapLinear:
        default:
            return VK_FILTER_LINEAR;
        }
    }

    VkSamplerMipmapMode MeshVK::ExtractMipmapMode(fastgltf::Filter filter)
    {
        switch (filter)
        {
        case fastgltf::Filter::NearestMipMapNearest:
        case fastgltf::Filter::LinearMipMapNearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case fastgltf::Filter::NearestMipMapLinear:
        case fastgltf::Filter::LinearMipMapLinear:
        default:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
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
