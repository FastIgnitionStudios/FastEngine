#pragma once
#include "BufferVK.h"
#include "Core.h"
#include "MaterialVK.h"
#include "UtilsVK.h"
#include "Rendering/Mesh.h"

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
        virtual ~MeshVK();

        std::vector<std::shared_ptr<MeshAssetVK>> CreateMeshAsset(MeshComponent mesh, RendererVK* renderer);

        virtual void Draw(const glm::mat4& worldTransform, DrawContext& context) override;
        
    private:
        
        MeshComponent meshComp;
        std::vector<std::shared_ptr<MeshAssetVK>> meshes;
        std::shared_ptr<MeshAssetVK> StaticMesh;
        friend class RendererVK;
        
    };
}
