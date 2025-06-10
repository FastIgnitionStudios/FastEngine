#include "EnginePCH.h"
#include "Mesh.h"

#include "EngineApp.h"
#include "RendererAPI.h"
#include "Platforms/RHI/Vulkan/MeshVK.h"

Engine::MeshComponent::MeshComponent(std::vector<uint32_t> indices, std::vector<Vertex> vertices)
{
    Ref<Mesh> newMesh = Mesh::Create(indices, vertices, EngineApp::Get()->GetRenderer().Raw());
    meshes.push_back(newMesh);
}

void Engine::MeshComponent::AssignMesh(std::string filePath)
{
    auto importer = GLTFImporter::LoadGLTF(filePath, Renderer::GetRenderer().Raw());
    meshes = importer->GetMeshes();
}

void Engine::MeshComponent::AssignMesh(std::vector<uint32_t> indices, std::vector<Vertex> vertices)
{
    Ref<Mesh> newMesh = Mesh::Create(indices, vertices, EngineApp::Get()->GetRenderer().Raw());
    meshes.push_back(newMesh);
}

Ref<Engine::Mesh> Engine::Mesh::Create(std::vector<uint32_t> indices, std::vector<Vertex> vertices, Renderer* renderer)
{
    switch (RendererAPI::GetRendererAPI())
    {
    case RHI::None:
        break;
    case RHI::Vulkan:
        return Ref<MeshVK>::Create(indices, vertices, renderer);
    case RHI::OpenGL:
        break;
    case RHI::DirectX:
        break;
    }
    return nullptr;
}
