// ╒══════════════════════════ Shaders.h ═╕
// │ Syngine                              │
// │ Created 2025-05-06                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/Core/Logger.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/Utils/Serializer.h"

#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>

#include <miniscl.hpp>

namespace Syngine {

/// @brief Load a shader from a file.
/// @param shaderPath The path to the shader file.
/// @return bgfx::ShaderHandle The handle to the loaded shader, or
/// BGFX_INVALID_HANDLE on failure.
/// @threadsafety not-safe
/// @since v0.0.1
/// @internal
inline bgfx::ShaderHandle _LoadShader(const char* shaderPath)
{
    namespace fs = std::filesystem;

    std::string path = Syngine::_ResolveOSPath(shaderPath);
    if (!fs::exists(path))
    {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               "LoadShader: file not found: %s",
                               path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               "LoadShader: unable to open: %s",
                               path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    auto size = file.tellg();
    if (size <= 0)
    {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               "LoadShader: bad size: %s",
                               path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer((size_t)size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               "LoadShader: read failed: %s",
                               path.c_str());
        return BGFX_INVALID_HANDLE;
    }

    const bgfx::Memory* mem = bgfx::copy(buffer.data(), (uint32_t)buffer.size());
    return bgfx::createShader(mem);
}

inline bgfx::ShaderHandle _LoadShaderFromMemory(const void* data, size_t size) {
    if (!data || size == 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                               "LoadShaderFromMemory: invalid data or size");
        return BGFX_INVALID_HANDLE;
    }

    const bgfx::Memory* mem = bgfx::copy(data, (uint32_t)size);
    return bgfx::createShader(mem);
}

inline bgfx::ShaderHandle _LoadShaderFromBundle(const char* bundlePath, const char* shaderName) {
    scl::stream stream = Serializer::_ReadFromBundle(bundlePath, shaderName);
    if (stream.size() == 0) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            "LoadShaderFromBundle: failed to read shader '%s' from bundle '%s'",
            shaderName,
            bundlePath);
        return BGFX_INVALID_HANDLE;
    }
    return _LoadShaderFromMemory(stream.data(), stream.size());
}

} // namespace Syngine
