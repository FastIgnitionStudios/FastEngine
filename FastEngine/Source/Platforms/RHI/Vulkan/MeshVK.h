#pragma once
#include "BufferVK.h"
#include "Core.h"
#include "RendererVK.h"
#include "Scene/Mesh.h"

namespace Engine
{

    struct GeometryVK
    {
        uint32_t startIndex;
        uint32_t indexCount;
    };

    struct MeshAssetVK
    {
        std::string name;
        
        std::vector<GeometryVK> geometries;
        GPUMeshBuffers buffers;
    };
    
    class MeshVK : public Engine
    {
    public:
        MeshVK();
        MeshVK(MeshComponent component, RendererVK* renderer);

        std::vector<std::shared_ptr<MeshAssetVK>> CreateMeshAsset(MeshComponent component, RendererVK* renderer);
        
    private:

        
        MeshComponent component;
        MeshAssetVK asset;
        
    };
}
