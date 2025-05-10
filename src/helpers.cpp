#include "helpers.h"
#include <SDL3/SDL.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb_image.h"

bgfx::TextureHandle SynLoadTextureFromMemory(const uint8_t* data, size_t size, const char* name) {
    int w, h, channels;
    stbi_uc* pixels = stbi_load_from_memory(data, size, &w, &h, &channels, 4);
    if (!pixels) { SDL_Log("Failed to load embedded texture"); return BGFX_INVALID_HANDLE; }

    const bgfx::Memory* mem = bgfx::copy(pixels, w * h * 4);
    stbi_image_free(pixels);

    return bgfx::createTexture2D(
        (uint16_t)w,
        (uint16_t)h,
        false,
        1,
        bgfx::TextureFormat::RGBA8,
        0,
        mem
    );
}