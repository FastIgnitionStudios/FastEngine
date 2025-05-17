#pragma once

#ifdef PLATFORM_WINDOWS

extern Engine::EngineApp* Engine::CreateApplication();

int main(int argc, char** argv)
{
    auto app = Engine::CreateApplication();
}

#endif