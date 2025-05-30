#include "EnginePCH.h"
#include "GLTFImporter.h"

#include "fastgltf/core.hpp"
#include "Platforms/RHI/Vulkan/MeshVK.h"
#include "Rendering/RendererAPI.h"

namespace Engine
{
    static std::vector<Ref<Mesh>> LoadGLTF_Vulkan(std::string filePath)
    {
        ENGINE_CORE_TRACE("Loading GLTF: {0}", filePath);
        std::vector<Ref<Mesh>> meshes;

        fastgltf::Parser parser;

        auto data = fastgltf::GltfDataBuffer::FromPath(filePath);
        if (data.error() != fastgltf::Error::None)
        {
            ENGINE_CORE_ERROR("File couldn't be loaded: {0}", filePath);
        }

        auto asset = parser.loadGltf(data.get(), std::filesystem::path(filePath).parent_path(), fastgltf::Options::LoadExternalBuffers);
        if (auto error = asset.error(); error != fastgltf::Error::None)
        {
            ENGINE_CORE_ERROR("File couldn't be loaded: {0}", filePath);
        }

        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;

        for (fastgltf::Mesh& mesh : asset->meshes)
        {
            Ref<MeshVK> newMeshVK = Ref<MeshVK>::Create();
            MeshAssetVK newMesh;

            newMesh.name = mesh.name;

            indices.clear();
            vertices.clear();
        }
    }
    
    GLTFImporter::GLTFImporter()
    {
    }

    GLTFImporter::~GLTFImporter()
    {
    }

    Ref<GLTFImporter> GLTFImporter::LoadGLTF(std::string filePath)
    {
        switch (RendererAPI::GetRendererAPI()) {
        case RHI::None:
            break;
        case RHI::Vulkan:
            {
                auto importer = Ref<GLTFImporter>::Create();
                importer->meshes = LoadGLTF_Vulkan(filePath);
                return importer;
            }
        case RHI::OpenGL:
            break;
        case RHI::DirectX:
            break;
        }

        return nullptr;
    
    }
    
}
