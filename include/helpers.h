#pragma once
#include "bgfx/bgfx.h"
#include <string>
#include <SDL3/SDL.h>
#include "SyngineLogger.h"
#include "FsUtils.h"

namespace Syngine {

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
        std::string fullPath = Syngine::ResolveOSPath(folder);
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