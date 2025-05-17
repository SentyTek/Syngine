#pragma once
#include <string>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>
#include "SynModelLoader.h"
#include "bx/math.h"

struct Camera {
    float eye[3] = {0.0f, 0.0f, -5.0f};
    float target[3] = {0.0f, 0.0f, 0.0f};
    float up[3] = {0.0f, 1.0f, 0.0f};
    float fov = 70.0f; //deg
    float near = 0.1f;
    float far = 100.0f;
    float view[16];
    float proj[16];

    float yaw = 0.0f;
    float pitch = 0.0f;

    void Update(int viewId, int width, int height);
};

struct SynProgram {
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    std::string name = "empty";
    uint8_t viewId = 0;
};

struct SynHandles {
    std::vector<SynProgram> programs;

    bgfx::UniformHandle u_lightDir = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_normalMatrix = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_floats = BGFX_INVALID_HANDLE;

    bgfx::UniformHandle u_albedoSampler = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_normalMapSampler = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_heightMapSampler = BGFX_INVALID_HANDLE;

    bgfx::UniformHandle u_skyColorDay = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_skyColorNight = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_sunColorDay = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_sunColorRise = BGFX_INVALID_HANDLE;
};

class SyngineGraphics {
    std::string title;
    
    public:
    int width;
    int height;
    SDL_Window* win;
    Camera camera;
    SynHandles handles;
    
    SyngineGraphics(const char* title, int width, int height);
    int CreateWindow();
    int CreateRenderer();
    int DestroyRenderer();
    void DestroyWindow();
    size_t AddProgram(const char* vsPath, const char* fsPath, const char* name);
    SynProgram GetProgram(const char* name) const;
    SynProgram GetProgram(size_t index) const;
    int RemoveProgram(bool all);
    int RemoveProgram(size_t index);
    int RemoveProgram(const char* name);
    
    int RenderFrame(SynModelLoader& modelLoader, bx::Vec3& lightDir);
};