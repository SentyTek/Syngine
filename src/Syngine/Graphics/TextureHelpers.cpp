// ╒═════════════════ TextureHelpers.cpp ═╕
// │ Syngine                              │
// │ Created 2025-08-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Graphics/TextureHelpers.h"
#include "Syngine/Core/Logger.h"

#include "bgfx/bgfx.h"
#include <SDL3/SDL.h>

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

// This is a private function exclusively used in this file
static void _BoxDownsample2x2(const uint8_t* src, int srcW, int srcH, std::vector<uint8_t>& dst) {
    int dstW = std::max(1, srcW / 2);
    int dstH = std::max(1, srcH / 2);
    dst.resize(dstW * dstH * 4);

    for(int y = 0; y < dstH; y++){
        for(int x = 0; x < dstW; x++){
            int r=0,g=0,b=0,a=0, cnt=0;
            for(int oy=0; oy<2; oy++){
                for(int ox=0; ox<2; ox++){
                    int sx = x*2 + ox;
                    int sy = y*2 + oy;
                    if(sx < srcW && sy < srcH){
                        int i = (sy*srcW + sx)*4;
                        r += src[i+0];
                        g += src[i+1];
                        b += src[i+2];
                        a += src[i+3];
                        cnt++;
                    }
                }
            }
            int di = (y*dstW + x)*4;
            dst[di+0] = r/cnt;
            dst[di+1] = g/cnt;
            dst[di+2] = b/cnt;
            dst[di+3] = a/cnt;
        }
    }
}

bgfx::TextureHandle Syngine::LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name) {
    int w, h, channels;
    stbi_uc* pixels = stbi_load_from_memory(data, size, &w, &h, &channels, 4);
    if (!pixels) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               "Failed to load embedded texture");
        return BGFX_INVALID_HANDLE;
    }

    int mipCount = 1 + (int)floor(log2(std::max(w, h)));
    auto tex = bgfx::createTexture2D(
        (uint16_t)w,
        (uint16_t)h,
        true,               //has mips
        1,                  //num layers
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_NONE   //we'll use updateTexture2D
    );

    //upload levels
    std::vector<uint8_t> src;
    src.assign(pixels, pixels + w*h*4);
    int levelW = w;
    int levelH = h;

    for (int level = 0; level < mipCount; level++) {
        //copy level data into bgfx
        const bgfx::Memory* mem = bgfx::copy(src.data(), src.size());
        bgfx::updateTexture2D(tex, 0, level, 0, 0, (uint16_t)levelW, (uint16_t)levelH, mem);

        //generate next mip in our src buffer
        if(level+1 < mipCount) {
            std::vector<uint8_t> next;
            _BoxDownsample2x2(src.data(), levelW, levelH, next);
            src.swap(next);
            levelW = std::max(1, levelW / 2);
            levelH = std::max(1, levelH / 2);
            src.resize(levelW * levelH * 4);
        }
    }

    stbi_image_free(pixels);
    return tex;
}

bgfx::TextureHandle Syngine::LoadTextureFromFile(const char* path) {
    SDL_IOStream* rw = SDL_IOFromFile(path, "rb");
    if (!rw) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               "Failed to open file %s",
                               path);
        return BGFX_INVALID_HANDLE;
    }

    Sint64 size = SDL_GetIOSize(rw);
    std::vector<Sint64> data(size);
    SDL_ReadIO(rw, data.data(), size);
    SDL_CloseIO(rw);

    return Syngine::LoadTextureFromMemory((uint8_t*)data.data(), size, path);
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
