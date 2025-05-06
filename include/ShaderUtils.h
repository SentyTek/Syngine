#pragma once
#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>

static std::string resolveShaderPath(const char* shaderPath)
{
#ifdef __APPLE__
    // SDL_GetBasePath() returns ".../Contents/MacOS/"
    const char* base = SDL_GetBasePath();
    std::string p(base);

    // swap "MacOS/" -> "Resources/"
    auto pos = p.rfind("MacOS/");
    if (pos != std::string::npos) {
        p = p.substr(0, pos) + "Resources/";
    }
    p += shaderPath; // e.g. "shaders/vs_simple.sc.bin"
    return p;
#else
    return std::string(shaderPath);
#endif
}

inline bgfx::ShaderHandle LoadShader(const char* shaderPath)
{
    namespace fs = std::filesystem;

    std::string path = resolveShaderPath(shaderPath);
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

inline bgfx::ProgramHandle CreateProgram(const char* vsPath, const char* fsPath) {
    bgfx::ShaderHandle vs = LoadShader(vsPath);
    if (bgfx::isValid(vs)) {
        bgfx::ShaderHandle fs = LoadShader(fsPath);
        if (bgfx::isValid(fs)) {
            return bgfx::createProgram(vs, fs, true);
        }
        bgfx::destroy(vs);
    }
    return BGFX_INVALID_HANDLE;
}