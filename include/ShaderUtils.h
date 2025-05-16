#pragma once
#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>
#include "helpers.h"

inline bgfx::ShaderHandle LoadShader(const char* shaderPath)
{
    namespace fs = std::filesystem;

    std::string path = resolveOSPath(shaderPath);
    if (!fs::exists(path))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "LoadShader: file not found: %s", path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "LoadShader: unable to open: %s", path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    auto size = file.tellg();
    if (size <= 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "LoadShader: bad size: %s", path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer((size_t)size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "LoadShader: read failed: %s", path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    const bgfx::Memory* mem = bgfx::copy(buffer.data(), (uint32_t)buffer.size());
    return bgfx::createShader(mem);
}