#pragma once

#include <filesystem>

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
        static void SetBaseDirectory();
        static void SetBaseDirectory(const std::filesystem::path& engineDir, const std::filesystem::path& appDir);

        static std::filesystem::path GetBaseEngineDirectory() { return baseEngineDir; }
        static std::filesystem::path GetBaseAppDirectory() { return baseAppDir; }

        static std::filesystem::path ResolveToEngineDir(const std::filesystem::path& path);
        static std::filesystem::path ResolveToAppDir(const std::filesystem::path& path);

        static std::filesystem::path GetEnvironmentPath(const std::string& envVariable);


        /// Converts a relative path to an absolute application-specific path.
        ///
        /// This function transforms the provided relative `path` into an absolute path
        /// based on the application's configuration directory. The resulting path will
        /// be relative to the user-specific directory:
        /// `C:\Users\<User>\AppData\Roaming\FastEngine\Engine` on Windows.
        ///
        /// @param path The relative path to be converted.
        /// @return The absolute application-specific path as a `std::filesystem::path`.

        static std::filesystem::path ToEnginePath(const std::filesystem::path& path);


        /// Converts a relative path to an absolute application-specific path.
        ///
        /// This function transforms the provided relative `path` into an absolute path
        /// based on the application's configuration directory. The resulting path will
        /// be relative to the user-specific directory:
        /// `C:\Users\<User>\AppData\Roaming\FastEngine\<Application>` on Windows.
        ///
        /// @param path The relative path to be converted.
        /// @return The absolute application-specific path as a `std::filesystem::path`.

        static std::filesystem::path ToAppPath(const std::filesystem::path& path);

        static std::filesystem::path GetWorkingDirectory();
        static void SetWorkingDirectory(std::filesystem::path value);
        static bool CreateDir(const std::filesystem::path& dir);
        static bool CreateDir(const std::string& dir);
        static bool Exists(const std::filesystem::path& filePath);
        static bool Exists(const std::string& filePath);

        static bool DeleteFile(const std::filesystem::path& filePath);
        static bool MoveFile(const std::filesystem::path& src, const std::filesystem::path& dest);
        static bool CopyFile(const std::filesystem::path& from, const std::filesystem::path& to);

        static bool IsDir(const std::filesystem::path& filePath);

        static std::string ReadFile(const std::filesystem::path& filePath);
        static std::string ReadFile(const std::string& filePath);

        static std::filesystem::path baseDir;
        static std::filesystem::path baseEngineDir;
        static std::filesystem::path baseAppDir;
    
    };
}
