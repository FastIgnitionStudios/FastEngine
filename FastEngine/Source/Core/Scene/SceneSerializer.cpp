#include "EnginePCH.h"
#include "SceneSerializer.h"

#include "Entity.h"

#include <fstream>

#include "Components.h"
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"

namespace Engine
{
    SceneSerializer::SceneSerializer()
    {
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : SceneContext(scene)
    {
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "EntityID";
        out << YAML::Value << entity.GetComponent<IDComponent>().ID;

        if (entity.HasComponent<NameComponent>())
        {
            out << YAML::Key << "Name";
            out << YAML::BeginMap; // Tag Component

            auto& tag = entity.GetComponent<NameComponent>().Name;
            out << YAML::Key << "Name" << YAML::Value << tag;

            out << YAML::EndMap; // Tag Component
        }

        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string& filePath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        auto eView = SceneContext->Registry.view<IDComponent>();
        for (auto& entityID : eView)
        {
            Entity entity = {entityID, SceneContext.Raw()};
            if (!entity) return;
            
            SerializeEntity(out, entity);
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filePath);
        fout << out.c_str();
        fout.close();
        
    }

    void SceneSerializer::SerializeRuntime(const std::string& filePath)
    {
        ENGINE_CORE_ASSERT(false, "Not Implemented");
    }

    bool SceneSerializer::Deserialize(const std::string& filePath)
    {
        return false;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filePath)
    {
        ENGINE_CORE_ASSERT(false, "Not Implemented");
        return false;
    }
}
