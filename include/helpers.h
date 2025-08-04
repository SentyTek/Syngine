#pragma once
#include "bgfx/bgfx.h"
#include <string>
#include <SDL3/SDL.h>
#include "SyngineLogger.h"
#include "FsUtils.h"

namespace Syngine {

/// @brief Platform enum to identify the current platform
enum class Platform {
	DARWIN, WINDOWS, LINUX,
};


#if defined(_WIN32)
constexpr Platform CurrentPlatform = Platform::WINDOWS; //* The current platform is Windows
#elif defined(__APPLE__)
constexpr Platform CurrentPlatform = Platform::DARWIN; //* The current platform is macOS
#elif defined(__linux__)
constexpr Platform CurrentPlatform = Platform::LINUX; //* The current platform is Linux
#else
static_assert(false, "Unsupported platform: not defined in SynPlatform");
#endif

/// Loads a texture from memory
/// @param data Pointer to the texture data in memory
/// @param size Size of the texture data in bytes
/// @param name Name of the texture for debugging purposes
/// @return A bgfx::TextureHandle representing the loaded texture
bgfx::TextureHandle
LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);

/// Loads a texture from file
/// @param path Path to the texture file
/// @return A bgfx::TextureHandle representing the loaded texture
bgfx::TextureHandle LoadTextureFromFile(const char* path);

/// Creates a flat texture, a 1x1 grey pixel texture
/// @return A bgfx::TextureHandle representing a flat texture
bgfx::TextureHandle CreateFlatTexture();

} // namespace Syngine
