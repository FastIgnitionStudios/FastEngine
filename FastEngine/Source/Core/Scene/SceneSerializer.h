#pragma once

#include "Core.h"
#include "Scene.h"

namespace Engine
{
    class SceneSerializer : public Engine
    {
    public:
        SceneSerializer();
        SceneSerializer(const Ref<Scene>& scene);

        void Serialize(const std::string& filePath);
        void SerializeRuntime(const std::string& filePath);

        bool Deserialize(const std::string& filePath);
        bool DeserializeRuntime(const std::string& filePath);

    private:
        Ref<Scene> SceneContext;
    
    };
}
