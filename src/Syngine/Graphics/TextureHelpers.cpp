// ╒═════════════════ TextureHelpers.cpp ═╕
// │ Syngine                              │
// │ Created 2025-08-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Graphics/TextureHelpers.h"
#include "Syngine/Core/Logger.h"

#include "Syngine/Utils/Serializer.h"
#include "bgfx/bgfx.h"
#include <SDL3/SDL.h>

#include <cstdint>
#include <vector>
#include <algorithm>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

#include "miniscl.hpp"

namespace {

static void _BoxDownsample2x2(const uint8_t* src,
                              int            srcW,
                              int            srcH,
                              std::vector<uint8_t>& dst) {
    const int dstW = std::max(1, srcW / 2);
    const int dstH = std::max(1, srcH / 2);
    dst.resize(static_cast<size_t>(dstW) * static_cast<size_t>(dstH) * 4);

    for (int y = 0; y < dstH; ++y) {
        for (int x = 0; x < dstW; ++x) {
            int r = 0;
            int g = 0;
            int b = 0;
            int a = 0;
            int count = 0;

            for (int oy = 0; oy < 2; ++oy) {
                for (int ox = 0; ox < 2; ++ox) {
                    const int sx = x * 2 + ox;
                    const int sy = y * 2 + oy;
                    if (sx >= srcW || sy >= srcH) {
                        continue;
                    }

                    const int srcIndex = (sy * srcW + sx) * 4;
                    r += src[srcIndex + 0];
                    g += src[srcIndex + 1];
                    b += src[srcIndex + 2];
                    a += src[srcIndex + 3];
                    ++count;
                }
            }

            const int dstIndex = (y * dstW + x) * 4;
            dst[dstIndex + 0] = static_cast<uint8_t>(r / count);
            dst[dstIndex + 1] = static_cast<uint8_t>(g / count);
            dst[dstIndex + 2] = static_cast<uint8_t>(b / count);
            dst[dstIndex + 3] = static_cast<uint8_t>(a / count);
        }
    }
}

} // namespace

bgfx::TextureHandle Syngine::LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name) {
    int w, h, channels;
    stbi_uc* pixels = stbi_load_from_memory(data, static_cast<int>(size), &w, &h, &channels, 4);
    if (!pixels) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               false,
                               "Failed to load embedded texture");
        return BGFX_INVALID_HANDLE;
    }

    int mipCount = 1;
    for (int mipW = w, mipH = h; mipW > 1 || mipH > 1; ) {
        mipW = std::max(1, mipW / 2);
        mipH = std::max(1, mipH / 2);
        ++mipCount;
    }

    size_t totalBytes = 0;
    for (int mipW = w, mipH = h, level = 0; level < mipCount; ++level) {
        totalBytes += static_cast<size_t>(mipW) * static_cast<size_t>(mipH) * 4;
        mipW = std::max(1, mipW / 2);
        mipH = std::max(1, mipH / 2);
    }

    std::vector<uint8_t> packedMips;
    packedMips.reserve(totalBytes);

    std::vector<uint8_t> currentLevel(
        pixels,
        pixels + static_cast<size_t>(w) * static_cast<size_t>(h) * 4);
    int levelW = w;
    int levelH = h;

    for (int level = 0; level < mipCount; ++level) {
        packedMips.insert(packedMips.end(), currentLevel.begin(), currentLevel.end());

        if (level + 1 < mipCount) {
            std::vector<uint8_t> nextLevel;
            _BoxDownsample2x2(currentLevel.data(), levelW, levelH, nextLevel);
            currentLevel.swap(nextLevel);
            levelW = std::max(1, levelW / 2);
            levelH = std::max(1, levelH / 2);
        }
    }

    const bgfx::Memory* mem =
        bgfx::copy(packedMips.data(), static_cast<uint32_t>(packedMips.size()));
    const bgfx::TextureHandle tex = bgfx::createTexture2D(
        static_cast<uint16_t>(w),
        static_cast<uint16_t>(h),
        mipCount > 1,
        1,
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_NONE,
        mem);

    stbi_image_free(pixels);

    if (!bgfx::isValid(tex)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, true,
                              "Failed to create texture %s (%dx%d)",
                              name ? name : "<memory>",
                              w,
                              h);
        return BGFX_INVALID_HANDLE;
    }

    return tex;
}

bgfx::TextureHandle Syngine::LoadTextureFromFile(const char* path) {
    SDL_IOStream* rw = SDL_IOFromFile(path, "rb");
    if (!rw) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, true,
                               "Failed to open file %s",
                               path);
        return BGFX_INVALID_HANDLE;
    }

    Sint64 size = SDL_GetIOSize(rw);
    std::vector<uint8_t> data(size);
    SDL_ReadIO(rw, data.data(), size);
    SDL_CloseIO(rw);

    return Syngine::LoadTextureFromMemory(data.data(), static_cast<size_t>(size), path);
}

bgfx::TextureHandle Syngine::LoadTextureFromBundle(const std::string& bundlePath, const std::string& textureName) {
    scl::stream ms =
        Syngine::Serializer::_ReadFromBundle(bundlePath, textureName);
    if (ms.size() == 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, false,
                               "Failed to load texture %s from bundle %s",
                               textureName.c_str(),
                               bundlePath.c_str());
        return BGFX_INVALID_HANDLE;
    }
    std::vector<uint8_t> data(ms.size());
    ms.read(data.data(), ms.size());
    return Syngine::LoadTextureFromMemory(data.data(), data.size(), textureName.c_str());
}

bgfx::TextureHandle Syngine::CreateFlatTexture() {
    uint8_t data[4] = { 127, 127, 127, 127 }; // white color
    const bgfx::Memory* mem = bgfx::copy(data, sizeof(data));
    bgfx::TextureHandle tex = bgfx::createTexture2D(
        1, 1, false, 1, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_NONE | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
        mem
    );
    return tex;
}

bgfx::TextureHandle Syngine::CreateNoiseTexture(uint16_t width, uint16_t height) {
    std::vector<uint8_t> data(width * height * 4);
    // Random value between 0 and 255 for each channel
    for (size_t i = 0; i < width * height * 4; ++i) {
        data[i] = static_cast<uint8_t>(rand() % 256);
    }
    const bgfx::Memory* mem = bgfx::copy(data.data(), static_cast<uint32_t>(data.size()));
    bgfx::TextureHandle tex = bgfx::createTexture2D(
        width, height, false, 1, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_NONE | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
        mem
    );
    return tex;
}
