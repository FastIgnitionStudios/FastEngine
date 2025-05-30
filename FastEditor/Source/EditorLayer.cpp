#include "EditorLayer.h"

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
