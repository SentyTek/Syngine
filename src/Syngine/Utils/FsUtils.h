// ╒══════════════════════════ FsUtils.h ═╕
// │ Syngine                              │
// │ Created 2025-07-18                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include <filesystem>
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
    
    // swap "MacOS/" -> "Resources/"
    size_t pos = p.find("MacOS/");
    if (pos != std::string::npos)
    {
        p.replace(pos, 6, "Resources/");
    }
    else
    {
        
    }
    p += path; // e.g. "shaders/vs_simple.sc.bin"
    return p;
#else
    return std::string(path);
#endif
}

/// @brief Checks if required folders exist in the game directory
/// @return True if all required folders exist, false otherwise
/// @since v0.0.1
/// @internal
inline bool _CheckRequiredFolders() {
    const char* requiredFolders[] = {
        "shaders",
        "meshes",
        "default",
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

} // namespace Syngine
