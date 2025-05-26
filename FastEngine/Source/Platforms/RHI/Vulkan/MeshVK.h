#pragma once
#include "BufferVK.h"
#include "Core.h"
#include "Rendering/Mesh.h"

namespace Engine
{

    class RendererVK;
    
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

        static std::vector<std::shared_ptr<MeshAssetVK>> CreateMeshAsset(MeshComponent component, RendererVK* renderer);
        
    private:

        
        MeshComponent component;
        MeshAssetVK asset;
        
    };
}
