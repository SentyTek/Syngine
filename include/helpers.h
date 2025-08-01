#pragma once
#include "bgfx/bgfx.h"
#include <string>
#include <SDL3/SDL.h>
#include "SyngineLogger.h"
#include "FsUtils.h"

namespace Syngine {

enum class Platform {
	DARWIN, WINDOWS, LINUX,
};

#if defined(_WIN32)
	constexpr Platform CurrentPlatform = Platform::WINDOWS;
#elif defined(__APPLE__)
	constexpr Platform CurrentPlatform = Platform::DARWIN;
#elif defined(__linux__)
	constexpr Platform CurrentPlatform = Platform::LINUX;
#else
	static_assert(false, "Unsupported patform: not defined in SynPlatform");
#endif

// Loads a texture from memory
bgfx::TextureHandle
LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);
// Loads a texture from file
bgfx::TextureHandle LoadTextureFromFile(const char* path);
// Creates a flat texture
bgfx::TextureHandle CreateFlatTexture();

} // namespace Syngine
