#include "RuntimeLayer.h"

#include "imgui.h"

static bool mesh = false;
static bool meshCreated = false;

void RuntimeLayer::OnAttach()
{
    ActiveScene = Ref<Engine::Scene>::Create();
    
}

void RuntimeLayer::OnDetach()
{
    Layer::OnDetach();
}

void RuntimeLayer::OnUpdate()
{
    Layer::OnUpdate();
}

void RuntimeLayer::OnRender()
{
    ImGui::Begin("Runtime");
    if (ImGui::Button("Test"))
    {
        mesh = true;
    }
    ImGui::End();
    if (mesh && meshCreated == false)
    {
        meshCreated = true;
        auto entity = ActiveScene->CreateEntity("Test");
        auto& meshComp = entity.AddComponent<Engine::MeshComponent>();
        meshComp.filePath = "..\\FastEngine\\Source\\Assets\\Meshes\\basicmesh.glb";
        meshComp.id = entity.GetComponent<Engine::IDComponent>().ID;
    }
}

void RuntimeLayer::OnEvent(Engine::Event& e)
{
    Layer::OnEvent(e);
}
