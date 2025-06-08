#include "RuntimeLayer.h"

#include "imgui.h"
#include "Rendering/PrimitiveRenderer.h"
#include "Scripts/PlanetScript.h"

static bool mesh = false;
static bool meshCreated = false;
static int frame = 0;

void RuntimeLayer::OnAttach()
{
    ActiveScene = Ref<Engine::Scene>::Create();

    auto entity = ActiveScene->CreateEntity("Test");
    entity.AddComponent<Engine::NativeScriptComponent>().Bind<PlanetScript>();
    
}

void RuntimeLayer::OnDetach()
{
    Layer::OnDetach();
}

void RuntimeLayer::OnUpdate()
{
    ActiveScene->OnUpdate(0.0f);

    Engine::PrimitiveRenderer::DrawSphere(glm::vec3(5.f, 0, 0.f), 2);
    frame++;
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
        auto& meshComp = entity.AddComponent<Engine::MeshComponent>("..\\FastEngine\\Source\\Assets\\Meshes\\basicmesh.glb");
    }
}

void RuntimeLayer::OnEvent(Engine::Event& e)
{
    Layer::OnEvent(e);
}
