#pragma once
#include "spdlog/spdlog.h"
#include "Core/Core.h"
#include <memory>

namespace Engine
{
    class ENGINE_API Log
    {
    public:

        Log();
        ~Log();
        
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> CoreLogger;
        static std::shared_ptr<spdlog::logger> ClientLogger;

    };
}

// Log macros

#ifdef DEBUG_BUILD

#define ENGINE_CORE_TRACE(...) ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_CORE_INFO(...) ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_CORE_WARN(...) ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_CORE_ERROR(...) ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CORE_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define ENGINE_TRACE(...) ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...) ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...) ::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)

// Strip logging from release build

#else

#define ENGINE_CORE_TRACE
#define ENGINE_CORE_INFO
#define ENGINE_CORE_WARN
#define ENGINE_CORE_ERROR
#define ENGINE_CORE_CRITICAL

#define ENGINE_TRACE
#define ENGINE_INFO
#define ENGINE_WARN
#define ENGINE_ERROR
#define ENGINE_CRITICAL
#endif