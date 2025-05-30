#pragma once
#include "Core.h"
#include "Rendering/Mesh.h"

namespace Engine
{
    class GLTFImporter : public Engine
    {
    public:
        GLTFImporter();
        virtual ~GLTFImporter();

        std::vector<Ref<Mesh>>& GetMeshes() { return meshes; }

        static Ref<GLTFImporter> LoadGLTF(std::string filePath);
        
    private:

        std::vector<Ref<Mesh>> meshes;
    };
}
