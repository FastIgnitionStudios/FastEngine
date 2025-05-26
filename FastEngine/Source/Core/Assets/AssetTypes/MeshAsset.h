#pragma once

#include "Core.h"
#include "Assets/Asset.h"
#include "Rendering/Renderer.h"

namespace Engine 
{

    class MeshAsset : public Asset, public IRenderable
    {
    public:
        MeshAsset(const RenderObject& meshRenderObject);
        virtual ~MeshAsset();

        virtual void Load() override;
        virtual void Unload() override;

        static Ref<MeshAsset> LoadMeshFromFile(const std::string& path);

        virtual void Draw(const glm::mat4& topMatrix, DrawContext& context) override;

    private:
        RenderObject MeshRenderObject;
    };
}
