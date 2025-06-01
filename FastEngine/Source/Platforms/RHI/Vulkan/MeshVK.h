#pragma once
#include "BufferVK.h"
#include "Core.h"
#include "MaterialVK.h"
#include "UtilsVK.h"
#include "fastgltf/core.hpp"
#include "Rendering/Mesh.h"

#ifdef LoadImage()
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
        MeshVK(MeshComponent mesh, RendererVK* renderer);
        MeshVK(std::string filePath, RendererVK* renderer);
        virtual ~MeshVK();

        std::vector<std::shared_ptr<MeshAssetVK>> CreateMeshAsset(MeshComponent mesh, RendererVK* renderer);

        virtual void Draw(const glm::mat4& worldTransform, DrawContext& context) override;
        
    private:

        VkFilter ExtractFilter(fastgltf::Filter filter);
        VkSamplerMipmapMode ExtractMipmapMode(fastgltf::Filter filter);
        
        MeshComponent meshComp;
        std::vector<std::shared_ptr<MeshAssetVK>> meshes;
        std::vector<VkSampler> samplers;
        std::vector<ImageVK::AllocatedImage> images;
        std::vector<std::shared_ptr<GLTFMaterial>> materials;
        
        DescriptorAllocatorDynamic meshDescriptorPool;
        AllocatedBuffer materialDataBuffer;
        friend class RendererVK;
        
    };
}
