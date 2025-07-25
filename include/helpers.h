#pragma once
#include "bgfx/bgfx.h"
#include <string>
#include <SDL3/SDL.h>

namespace Syngine {

enum class SynPlatform {
	DARWIN, WINDOWS, LINUX,
};

#if defined(_WIN32)
	constexpr SynPlatform currentPlatform = SynPlatform::WINDOWS;
#elif defined(__APPLE__)
	constexpr SynPlatform currentPlatform = SynPlatform::DARWIN;
#elif defined(__linux__)
	constexpr SynPlatform currentPlatform = SynPlatform::LINUX;
#else
	static_assert(false, "Unsupported patform: not defined in SynPlatform");
#endif
};

// This function resolves the full relative path to resourced files based on platform.
// macOS uses a different path structure than other because of the app bundle structure.
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

bgfx::TextureHandle SynLoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);
bgfx::TextureHandle SynLoadTextureFromFile(const char* path);
bgfx::TextureHandle SynCreateFlatTexture();

int SynCreateQuad(int viewId);

inline bool CheckRequiredFolders() {
    const char* requiredFolders[] = {
        "shaders",
        "meshes",
        "default",
    };

    for (const char* folder : requiredFolders) {
        std::string fullPath = resolveOSPath(folder);
        if (!SDL_GetPathInfo(fullPath.c_str(), nullptr)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Required folder '%s' does not exist in game dir: %s", folder, fullPath.c_str());
            return false;
        }
    }
    return true;
}
