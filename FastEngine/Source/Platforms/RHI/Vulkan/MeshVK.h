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
        MeshVK(Ref<MeshAsset> mesh, RendererVK* renderer);

        static std::vector<std::shared_ptr<MeshAssetVK>> CreateMeshAsset(Ref<MeshAsset> mesh, RendererVK* renderer);
        
    private:

        
        Ref<MeshAsset> mesh;
        MeshAssetVK asset;
        
    };
}
