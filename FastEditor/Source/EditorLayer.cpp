#include "EditorLayer.h"

#include "Engine.h"
#include "imgui.h"
#include "Panels/SceneHierachyPanel.h"
#include "Core/Scene/SceneSerializer.h"


namespace Engine
{
    EditorLayer::EditorLayer()
    {
    }

    void EditorLayer::OnAttach()
    {
        ActiveScene = Ref<Scene>::Create();
        SceneHierarchy = Ref<SceneHierarchyPanel>::Create();
        SceneHierarchy->SetContext(ActiveScene);

        auto meshes = ActiveScene->CreateEntity("Meshes");
        auto& meshComp = meshes.AddComponent<MeshComponent>();
        meshComp.filePath = "..\\FastEngine\\Source\\Assets\\Meshes\\basicmesh.glb";
        meshComp.id = meshes.GetComponent<IDComponent>().ID;

        SceneSerializer serializer(ActiveScene);
        serializer.Serialize("TestScene.fescene");
    }

    void EditorLayer::OnDetach()
    {
        Layer::OnDetach();
    }

    void EditorLayer::OnUpdate()
    {
        Layer::OnUpdate();
    }

    void EditorLayer::OnRender()
    {
        SceneHierarchy->OnImGuiRender();   
    }

    void EditorLayer::OnEvent(Event& e)
    {
        Layer::OnEvent(e);
    }
}
