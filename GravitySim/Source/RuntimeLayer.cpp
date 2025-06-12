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
    planet.AddComponent<Engine::NativeScriptComponent>().CreateScript<PlanetScript>(planet, 1.f, glm::vec3(0.001, -0.001,0 ));
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
    ImGui::SetNextWindowBgAlpha(.5);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Begin("Timing", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Delta Time: %f", deltaTime);
    ImGui::Text("FPS: %f", (1.0f / deltaTime) * 1000);
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void RuntimeLayer::OnEvent(Engine::Event& e)
{
    Layer::OnEvent(e);
}
