// ╒═══════════════════ TextureHelpers.h ═╕
// │ Syngine                              │
// │ Created 2025-08-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "bgfx/bgfx.h"
#include <string>
#include <SDL3/SDL.h>

namespace Syngine {

/// @brief Loads a texture from memory
/// @param data Pointer to the texture data in memory
/// @param size Size of the texture data in bytes
/// @param name Name of the texture for debugging purposes
/// @return A bgfx::TextureHandle representing the loaded texture
bgfx::TextureHandle
LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);

/// @brief Loads a texture from file
/// @param path Path to the texture file
/// @return A bgfx::TextureHandle representing the loaded texture
bgfx::TextureHandle LoadTextureFromFile(const char* path);

/// @brief Creates a flat texture, a 1x1 grey pixel texture
/// @return A bgfx::TextureHandle representing a flat texture
bgfx::TextureHandle CreateFlatTexture();

} // namespace Syngine