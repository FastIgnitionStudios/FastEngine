#include "EnginePCH.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Engine
{
    std::shared_ptr<spdlog::logger> Log::CoreLogger;
    std::shared_ptr<spdlog::logger> Log::ClientLogger;

    Log::Log()
    {
    }

    Log::~Log()
    {
    }

    void Log::Init()
    {
        spdlog::set_pattern("%^[%D %T] [%n]: %v%$");

        CoreLogger = spdlog::stdout_color_mt("Engine");
        CoreLogger->set_level(spdlog::level::trace);

        ClientLogger = spdlog::stdout_color_mt("Client");
        ClientLogger->set_level(spdlog::level::trace);

        CoreLogger->trace("Engine Log Initialized");
        ClientLogger->trace("Client Log Initialized");
    }
}
