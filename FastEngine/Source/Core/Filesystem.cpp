#include "EnginePCH.h"
#include "Filesystem.h"

#include <fstream>
#define CRT_SECURE_NO_WARNINGS

#pragma warning(push)
#pragma warning(disable : 4996)

#include "Core.h"

namespace Engine
{
    std::filesystem::path Filesystem::baseAppDir = "Null";
    std::filesystem::path Filesystem::baseEngineDir = "Null";
    std::filesystem::path Filesystem::baseDir = "Null";

    void Filesystem::SetBaseDirectory()
    {
        std::filesystem::path appDataPath = GetEnvironmentPath("CSIDL_MYDOCUMENTS");

        baseDir = std::filesystem::absolute(appDataPath / "FastEngine");
        baseEngineDir = std::filesystem::absolute(baseDir / "FastEngine");
        baseAppDir = std::filesystem::absolute(baseDir / "Application");

        CreateDir(baseDir);
        CreateDir(baseEngineDir);
        CreateDir(baseAppDir);
    }

    void Filesystem::SetBaseDirectory(const std::filesystem::path& engineDir, const std::filesystem::path& appDir)
    {
        baseAppDir = std::filesystem::absolute(appDir);
        baseEngineDir = std::filesystem::absolute(engineDir);
    }

    std::filesystem::path Filesystem::ResolveToEngineDir(const std::filesystem::path& path)
    {
        if (path.is_absolute())
            return path;
        return baseEngineDir / path;
    }

    std::filesystem::path Filesystem::ResolveToAppDir(const std::filesystem::path& path)
    {
        if (path.is_absolute())
            return path;
        return baseAppDir / path;
    }

    std::filesystem::path Filesystem::GetEnvironmentPath(const std::string& envVariable)
    {
        if (const char* envPath = std::getenv(envVariable.c_str()))
            return std::filesystem::path(envPath);
        return std::filesystem::current_path();
    }


    std::filesystem::path Filesystem::ToEnginePath(const std::filesystem::path& path)
    {
        return ResolveToEngineDir(path);
    }


    std::filesystem::path Filesystem::ToAppPath(const std::filesystem::path& path)
    {
        return ResolveToAppDir(path);
    }

    std::filesystem::path Filesystem::GetWorkingDirectory()
    {
        return std::filesystem::current_path();
    }

    void Filesystem::SetWorkingDirectory(std::filesystem::path value)
    {
        std::filesystem::current_path(value);
    }

    bool Filesystem::CreateDir(const std::filesystem::path& dir)
    {
        if (Exists(dir) && IsDir(dir))
            return std::filesystem::create_directory(dir);
        return false;
    }

    bool Filesystem::CreateDir(const std::string& dir)
    {
        return CreateDir(std::filesystem::path(dir));
    }

    bool Filesystem::Exists(const std::filesystem::path& filePath)
    {
        return std::filesystem::exists(filePath);
    }

    bool Filesystem::Exists(const std::string& filePath)
    {
        return std::filesystem::exists(filePath);
    }

    bool Filesystem::DeleteFile(const std::filesystem::path& filePath)
    {
        return std::filesystem::remove(filePath);
    }

    bool Filesystem::MoveFile(const std::filesystem::path& src, const std::filesystem::path& dest)
    {
        if (Filesystem::Exists(dest))
            return false;
        std::filesystem::rename(src, dest);
        return true;
    }

    bool Filesystem::CopyFile(const std::filesystem::path& from, const std::filesystem::path& to)
    {
        if (Filesystem::Exists(to))
            return false;
        std::filesystem::copy(from, to);
        return true;
    }

    bool Filesystem::IsDir(const std::filesystem::path& filePath)
    {
        return std::filesystem::is_directory(filePath);
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
