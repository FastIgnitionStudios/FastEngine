#include "EnginePCH.h"
#include "GLTFImporter.h"

#include "EngineApp.h"
#include "fastgltf/core.hpp"
#include "Platforms/RHI/Vulkan/MeshVK.h"
#include "Rendering/RendererAPI.h"


namespace Engine
{

    
    GLTFImporter::GLTFImporter(std::string filePath, Renderer* renderer)
    {
        
        switch (RendererAPI::GetRendererAPI()) {
        case RHI::None:
            break;
        case RHI::Vulkan:
            {
                auto mesh = Ref<MeshVK>::Create(filePath, (RendererVK*)renderer);
                meshes.push_back(mesh);
                return;
            }
        case RHI::OpenGL:
            break;
        case RHI::DirectX:
            break;
        }
    }

    GLTFImporter::~GLTFImporter()
    {
    }

    Ref<GLTFImporter> GLTFImporter::LoadGLTF(std::string filePath, Renderer* renderer)
    {
        return Ref<GLTFImporter>::Create(filePath, renderer);
    }
    
}
