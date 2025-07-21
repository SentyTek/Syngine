#pragma once
#include "bgfx/bgfx.h"
#include <string>
#include <SDL3/SDL.h>
#include "SyngineLogger.h"

namespace Syngine {

// This function resolves the full relative path to resourced files based on
// platform. macOS uses a different path structure than other because of the app bundle structure.
inline std::string resolveOSPath(const char* path)
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

// Loads a texture from memory
bgfx::TextureHandle
LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);
// Loads a texture from file
bgfx::TextureHandle LoadTextureFromFile(const char* path);
// Creates a flat texture
bgfx::TextureHandle CreateFlatTexture();

// Checks if required folders exist in the game directory
inline bool CheckRequiredFolders() {
    const char* requiredFolders[] = {
        "shaders",
        "meshes",
        "default",
    };

    for (const char* folder : requiredFolders) {
        std::string fullPath = resolveOSPath(folder);
        if (!SDL_GetPathInfo(fullPath.c_str(), nullptr)) {
            Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                 "Required folder '%s' does not exist in game dir: %s",
                                 folder,
                                 fullPath.c_str());
            return false;
        }
    }
    return true;
}

} // namespace Syngine