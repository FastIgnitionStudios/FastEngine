#pragma once

#include "ObjectTypes.h"
#ifdef PLATFORM_WINDOWS

extern Engine::EngineApp* Engine::CreateApplication();

int main(int argc, char** argv)
{
    ObjectRegistry::Get().ForceRegistration();
    Engine::Log::Init();
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
}

#endif