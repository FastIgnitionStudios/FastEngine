#include "EnginePCH.h"
#include "MeshAsset.h"
#include "Platforms/RHI/Vulkan/RendererVK.h"

namespace Engine
{
    MeshAsset::MeshAsset(const RenderObject& meshRenderObject) : MeshRenderObject(meshRenderObject)
    {
    }

    MeshAsset::~MeshAsset()
    {
    }

    void MeshAsset::Load()
    {
    }

    void MeshAsset::Unload()
    {
    }

    Ref<MeshAsset> MeshAsset::LoadMeshFromFile(const std::string& path)
    {
        RenderObject obj;
        return Ref<MeshAsset>::Create(obj);
    }

    void MeshAsset::Draw(const glm::mat4& topMatrix, DrawContext& context)
    {
        
    }
}
