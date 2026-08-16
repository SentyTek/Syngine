// ╒═══════════════════ TextureHelpers.h ═╕
// │ Syngine                              │
// │ Created 2025-08-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <bgfx/bgfx.h>
#include <string>
#include <SDL3/SDL.h>
#include <cstdint>
#include <vector>

namespace Syngine {

struct DecodedTextureData {
    uint16_t               width   = 0;
    uint16_t               height  = 0;
    bool                   hasMips = false;
    std::vector<std::byte> payload;
};

/// @brief Loads a texture from memory
/// @param data Pointer to the texture data in memory
/// @param size Size of the texture data in bytes
/// @param name Name of the texture for debugging purposes
/// @return A bgfx::TextureHandle representing the loaded texture
bgfx::TextureHandle
LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);

/// @brief Decodes texture bytes into RGBA8 and packs mip levels.
/// @param data Pointer to encoded texture data in memory
/// @param size Size of encoded texture data in bytes
/// @param out Decoded output data including packed mip payload
/// @return true if decoding succeeded, false otherwise
bool DecodeTextureFromMemory(const uint8_t*      data,
                             size_t              size,
                             DecodedTextureData& out);

/// @brief Loads a texture from file
/// @param path Path to the texture file
/// @return A bgfx::TextureHandle representing the loaded texture
bgfx::TextureHandle LoadTextureFromFile(const char* path);

/// @brief Load a texture from a bundle
/// @param bundlePath Path to the shader bundle containing the texture
/// @param textureName Name of the texture within the shader bundle
/// @return A bgfx::TextureHandle representing the loaded texture
bgfx::TextureHandle LoadTextureFromBundle(const std::string& bundlePath,
                                          const std::string& textureName);

/// @brief Creates a flat texture, a 1x1 grey pixel texture
/// @return A bgfx::TextureHandle representing a flat texture
bgfx::TextureHandle CreateFlatTexture();

/// @brief Creates a white noise texture
/// @param width Width of the texture
/// @param height Height of the texture
/// @return A bgfx::TextureHandle representing the noise texture
bgfx::TextureHandle CreateNoiseTexture(uint16_t width, uint16_t height);

} // namespace Syngine
