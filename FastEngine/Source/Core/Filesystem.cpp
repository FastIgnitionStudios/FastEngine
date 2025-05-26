#include "EnginePCH.h"
#include "Filesystem.h"

#include <fstream>
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(push)
#pragma warning(disable : 4996)

#include "Core.h"
#include <cstdlib>

namespace Engine
{
    std::filesystem::path Filesystem::EngineDir = "";
    std::filesystem::path Filesystem::AppDir = "";
    
    void Filesystem::InitFilesystem()
    {
        std::string WorkingDir = std::filesystem::current_path().generic_string();
        EngineDir = WorkingDir.substr(0, WorkingDir.find("FastEditor")).append("FastEngine/");
        AppDir = WorkingDir;
        ENGINE_CORE_INFO("Engine Directory: {0}", EngineDir.generic_string());
    }

    std::string Filesystem::ReadFile(const std::filesystem::path& filePath)
    {
        std::ifstream file(filePath.string(), std::ifstream::binary | std::ifstream::ate);

        ENGINE_CORE_ASSERT(file.is_open(), "Could not open file");
        if (!file.is_open())
            ENGINE_CORE_ERROR("Could not open file");

        size_t fileSize = file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return std::string(buffer.data(), fileSize);
    }

    std::filesystem::path Filesystem::GetEngineAssetDir()
    {
        std::filesystem::path engineAssetDir = EngineDir;
        engineAssetDir.append("Source/Assets/");
        return engineAssetDir;
    }

    std::filesystem::path Filesystem::GetAppAssetDir()
    {
        std::filesystem::path appAssetDir = AppDir;
        appAssetDir.append("Source/Assets/");
        return appAssetDir;
    }

    std::filesystem::path Filesystem::GetEngineAssetPath(const std::string& relativePath)
    {
        std::filesystem::path engineAssetDir = GetEngineAssetDir();
        engineAssetDir.append(relativePath);
        return engineAssetDir;
    }

    std::filesystem::path Filesystem::GetAppAssetPath(const std::string& relativePath)
    {
        std::filesystem::path appAssetDir = GetAppAssetDir();
        appAssetDir.append(relativePath);
        return appAssetDir;
    }

    std::string Filesystem::ReadFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ifstream::binary | std::ifstream::ate);

        ENGINE_CORE_ASSERT(file.is_open(), "Could not open file");
        if (!file.is_open())
            ENGINE_CORE_ERROR("Could not open file");

        size_t fileSize = file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return std::string(buffer.data(), fileSize);
    }
}

#pragma warning(pop)
