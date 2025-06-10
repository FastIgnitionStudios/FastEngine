#pragma once
#include "Buffer.h"
#include "Core.h"
#include "Renderer.h"
#include "Asset/GLTFImporter.h"
#include "Core/UUID.h"
namespace Engine
{
    class Mesh;
    struct MeshComponent
    {
        std::vector<Ref<Mesh>> meshes;

        MeshComponent(std::string filePath)
        {
            auto importer = GLTFImporter::LoadGLTF(filePath, Renderer::GetRenderer().Raw());
            meshes = importer->GetMeshes();
        }
        MeshComponent(std::vector<uint32_t> indices, std::vector<Vertex> vertices);
        MeshComponent() = default;

        void AssignMesh(std::string filePath);
        void AssignMesh(std::vector<uint32_t> indices, std::vector<Vertex> vertices);
    };

    class Mesh : public Engine, public IRenderable
    {
    public:
        Mesh() = default;

        virtual void Draw(const glm::mat4& worldMatrix, DrawContext& context) override = 0;

        static Ref<Mesh> Create(std::vector<uint32_t> indices, std::vector<Vertex> vertices, Renderer* renderer);

    protected:

    };
}
