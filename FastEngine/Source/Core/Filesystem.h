#pragma once

#include <filesystem>

#include "Utils/Log.h"

// Thank you microsoft

#ifdef CreateDirectory
#undef CreateDirectory
#undef DeleteFile
#undef MoveFile
#undef CopyFile
#undef SetEnvironmentVariable
#undef GetEnvironmentVariable
#undef ReadFile
#endif

namespace Engine
{
    class Filesystem
    {
    public:
        static void InitFilesystem();
        
        static std::string ReadFile(const std::filesystem::path& filePath);
        static std::string ReadFile(const std::string& filePath);

        static std::filesystem::path GetEngineDir() { return EngineDir; }
        static std::filesystem::path GetAppDir() { return AppDir; }

        static std::filesystem::path GetEngineAssetDir();
        static std::filesystem::path GetAppAssetDir() ;

        static std::filesystem::path GetEngineAssetPath(const std::string& relativePath);
        static std::filesystem::path GetAppAssetPath(const std::string& relativePath);

    private:

        static std::filesystem::path EngineDir;
        static std::filesystem::path AppDir;


    
    };
}