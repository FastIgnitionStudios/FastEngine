#pragma once
#include "BufferVK.h"
#include "Core.h"
#include "MaterialVK.h"
#include "UtilsVK.h"
#include "fastgltf/core.hpp"
#include "Rendering/Mesh.h"

#ifdef LoadImage
#undef LoadImage
#endif

namespace Engine
{

    class RendererVK;

    struct GLTFMaterial
    {
        MaterialInstanceVK data;
    };
    
    struct GeometryVK
    {
        uint32_t startIndex;
        uint32_t indexCount;
        GLTFMaterial material;
        glm::mat4 offset;
    };

    struct MeshAssetVK
    {
        std::string name;
        std::vector<GeometryVK> geometries;
        GPUMeshBuffers buffers;
    };
    
    class MeshVK : public Mesh
    {
    public:
        MeshVK();
        MeshVK(std::string filePath, RendererVK* renderer);
        MeshVK(std::vector<uint32_t> indices, std::vector<Vertex> vertices, Renderer* renderer);
        virtual ~MeshVK();
        
        TransformComponent& GetTransform() { return transform; }
        void SetTransform(TransformComponent transform) { this->transform = transform; }
        
        virtual void Draw(const glm::mat4& worldTransform, DrawContext& context) override;
        std::vector<std::shared_ptr<MeshAssetVK>> GetMeshes() { return meshes;}
    private:

        VkFilter ExtractFilter(fastgltf::Filter filter);
        VkSamplerMipmapMode ExtractMipmapMode(fastgltf::Filter filter);
        
        std::vector<std::shared_ptr<MeshAssetVK>> meshes;
        std::vector<VkSampler> samplers;
        std::vector<ImageVK::AllocatedImage> images;
        std::vector<std::shared_ptr<GLTFMaterial>> materials;
        
        TransformComponent transform;
        DescriptorAllocatorDynamic meshDescriptorPool;
        AllocatedBuffer materialDataBuffer;
        friend class RendererVK;
        
    };
}
