#include "EnginePCH.h"
#include "SceneSerializer.h"

#include "Entity.h"

#include <fstream>

#include "Components.h"
#define YAML_CPP_STATIC_DEFINE
#include "Rendering/Camera.h"
#include "yaml-cpp/yaml.h"

namespace Engine
{
    YAML::Emitter& operator<< (YAML::Emitter& out, const glm::mat4& matrix)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << YAML::Newline << YAML::BeginSeq << matrix[0].x << matrix[0].y << matrix[0].z << matrix[0].w << YAML::EndSeq << YAML::Newline << YAML::BeginSeq << matrix[1].x << matrix[1].y << matrix[1].z << matrix[1].w << YAML::EndSeq << YAML::Newline << YAML::BeginSeq << matrix[2].x << matrix[2].y << matrix[2].z << matrix[2].w << YAML::EndSeq << YAML::Newline << YAML::BeginSeq << matrix[3].x << matrix[3].y << matrix[3].z << matrix[3].w << YAML::EndSeq << YAML::EndSeq;
        return out;
    }
    
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

        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "Camera";
            out << YAML::BeginMap; // Camera Component

            auto& cameraComp = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComp.camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "Rotation Matrix" << YAML::Value << camera->GetRotationMatrix();
            out << YAML::Key << "View Matrix" << YAML::Value << camera->GetViewMatrix();
            
            out << YAML::EndMap; // Camera
            
            out << YAML::EndMap; // Camera Component
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
