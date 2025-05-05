#pragma once
#include <bgfx/bgfx.h>
#include <ios>
#include <vector>
#include <fstream>

inline bgfx::ShaderHandle LoadShader(const char* shaderPath) {
    //load shader
    std::ifstream file(shaderPath, std::ios::binary | std::ios::ate);
    if (!file) return BGFX_INVALID_HANDLE;
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) return BGFX_INVALID_HANDLE;
    const bgfx::Memory* mem = bgfx::copy(buffer.data(), size);
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