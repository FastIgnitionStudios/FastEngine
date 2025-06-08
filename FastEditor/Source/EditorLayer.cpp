#include "EditorLayer.h"

#include "Engine.h"
#include "imgui.h"
#include "Panels/SceneHierachyPanel.h"
#include "Core/Scene/SceneSerializer.h"
#include "Rendering/PrimitiveRenderer.h"


namespace Engine
{
    static bool firstFrame = true;
    EditorLayer::EditorLayer()
    {
    }

    void EditorLayer::OnAttach()
    {
        ActiveScene = Ref<Scene>::Create();
        SceneHierarchy = Ref<SceneHierarchyPanel>::Create();
        SceneHierarchy->SetContext(ActiveScene);


    }

    void EditorLayer::OnDetach()
    {
        Layer::OnDetach();
    }

    void EditorLayer::OnUpdate(float deltaTime)
    {
        ActiveScene->OnUpdate(0.0f);
        PrimitiveRenderer::DrawQuad(glm::vec3(0, 0, 0));
        if (firstFrame)
        {
            auto meshes = ActiveScene->CreateEntity("Meshes");
            auto& meshComp = meshes.AddComponent<MeshComponent>("..\\FastEngine\\Source\\Assets\\Meshes\\basicmesh.glb");

            SceneSerializer serializer(ActiveScene);
            serializer.Serialize("TestScene.fescene");
            firstFrame = false;
        }
    }

    void EditorLayer::OnRender(float deltaTime)
    {
        SceneHierarchy->OnImGuiRender();   
    }

    void EditorLayer::OnEvent(Event& e)
    {
        Layer::OnEvent(e);
    }
}
