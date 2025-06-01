#pragma once
#include "Core.h"
#include "Rendering/Mesh.h"

namespace Engine
{

    class RendererVK;
    
    class GLTFImporter : public Engine
    {
    public:
        GLTFImporter(std::string filePath, Renderer* renderer);
        virtual ~GLTFImporter();

        std::vector<Ref<Mesh>>& GetMeshes() { return meshes; }

        static Ref<GLTFImporter> LoadGLTF(std::string filePath, Renderer* renderer);
        
    private:

        std::vector<Ref<Mesh>> meshes;
    };
}
