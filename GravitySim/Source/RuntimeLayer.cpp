#include "RuntimeLayer.h"

#include "imgui.h"
#include "Rendering/PrimitiveRenderer.h"
#include "Scripts/PlanetManager.h"
#include "Scripts/PlanetScript.h"

static bool mesh = false;
static bool meshCreated = false;

void RuntimeLayer::OnAttach()
{
    ActiveScene = Ref<Engine::Scene>::Create();

    auto planet = ActiveScene->CreateEntity("Planet");
    planet.AddComponent<Engine::NativeScriptComponent>().CreateScript<PlanetScript>(planet, 1, glm::vec3(0.001, -0.001,0 ));
    auto planet2 = ActiveScene->CreateEntity("Planet2");
    planet2.AddComponent<Engine::NativeScriptComponent>().CreateScript<PlanetScript>(planet2, 1);
    auto& transform = planet.GetComponent<Engine::TransformComponent>();
    transform.Translation = glm::vec3(0.0f, 10.0f, 0);
    auto planetManager = ActiveScene->CreateEntity("PlanetManager");
    planetManager.AddComponent<Engine::NativeScriptComponent>().CreateScript<PlanetManager>(planetManager);
}

void RuntimeLayer::OnDetach()
{
    Layer::OnDetach();
}

void RuntimeLayer::OnUpdate(float deltaTime)
{
    ActiveScene->OnUpdate(deltaTime);
    
}

void RuntimeLayer::OnRender(float deltaTime)
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
