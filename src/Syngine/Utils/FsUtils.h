// ╒══════════════════════════ FsUtils.h ═╕
// │ Syngine                              │
// │ Created 2025-07-18                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯
#pragma once

#include <filesystem>
#include <string>
#include "Syngine/Core/Logger.h"

#include "SDL3/SDL_filesystem.h"

namespace Syngine {

/// @brief Get the path to the application data directory for the specified
/// application.
/// @param appName The name of the application
/// @note This function does take into account the platform-specific
/// differences in the application data directory structure.
/// @return The path to the application data directory
/// @since v0.0.1
/// @internal
std::filesystem::path _GetAppdataPath(const std::string& appName);

/// @brief Resolves the full relative path to resourced files based on platform.
/// macOS uses a different path structure than others because of the app bundle
/// structure.
/// @param path The relative path to the resource file
/// @return The full path to the resource file, adjusted for the platform
/// @since v0.0.1
/// @internal
static inline std::string _ResolveOSPath(const char* path)
{
#ifdef __APPLE__
    // SDL_GetBasePath() returns ".../Contents/MacOS/"
    const char* base = SDL_GetBasePath();
    std::string p(base);

    // If path is already absolute, just return it since it's probably got rom.
    if (std::filesystem::path(path).is_absolute()) {
        return path;
    }

    // swap "MacOS/" -> "Resources/"
    size_t pos = p.find("MacOS/");
    if (pos != std::string::npos)
    {
        p.replace(pos, 6, "Resources");
    }
    // Only add "rom/" prefix if it's not already there, to allow for absolute paths in the bundle
    if (p.find("rom/") == std::string::npos) {
        p += "rom/" + std::string(path); // e.g. "shaders/vs_simple.sc.bin"
    } else {
        p += path; // If "rom/" is already in the base path, just append the relative path
    }
    return p;
#else
    // If path is already absolute, just return it since it's probably got rom.
    if (std::filesystem::path(path).is_absolute()) {
        return path;
    }

    // Only add "rom/" prefix if it's not already there, to allow for absolute paths in the game directory
    std::string p(path);
    if (p.find("rom/") == std::string::npos) {
        p = "rom/" + p; // e.g. "shaders/vs_simple.sc.bin"
    }
    return p;
#endif
}

static inline std::string _ResolveOSPath(std::string path) {
    return _ResolveOSPath(path.c_str());
}

/// @brief Checks if required folders exist in the game directory
/// @return True if all required folders exist, false otherwise
/// @since v0.0.1
/// @internal
inline bool _CheckRequiredFolders() {
    const char* requiredFolders[] = {
        "shaders",
        "gizmos", // Should meshes be considered required?
    };

    for (const char* folder : requiredFolders) {
        std::string fullPath = Syngine::_ResolveOSPath(folder);
        if (!SDL_GetPathInfo(fullPath.c_str(), nullptr)) {
            Syngine::Logger::LogF(Syngine::LogLevel::FATAL,
                                 "Required folder '%s' does not exist in game dir: %s",
                                 folder,
                                 fullPath.c_str());
            return false;
        }
    }
    return true;
}

/// @brief Take an absolute path and make it relative to the root of the game
/// directory
/// @param path The absolute path to convert
/// @note Assumes the input path is to something within the game directory. If
/// it's not, the behavior is undefined.
/// @return The path relative to the root of the game directory
/// @since v0.0.1
static inline std::string _MakeRelativeToRoot(std::string path) {
    std::string gameDir = Syngine::_ResolveOSPath("").substr(0, Syngine::_ResolveOSPath("").find_last_of("/\\") + 1);
    if (path.find(gameDir) == 0) {
        return path.substr(gameDir.length());
    }
    return path; // If the path doesn't start with the game directory, return it unchanged
}

/// @brief Check if a file exists at the given path, taking into account
/// platform-specific path resolution
/// @param path The relative path to the file to check
/// @return True if the file exists, false otherwise
/// @since v0.0.1
bool _FileExists(const char* path);

} // namespace Syngine
