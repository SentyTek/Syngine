#pragma once
#include <string>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>
#include "SynModelLoader.h"

struct Camera {
    float eye[3] = {0.0f, 0.0f, -5.0f};
    float target[3] = {0.0f, 0.0f, 0.0f};
    float up[3] = {0.0f, 1.0f, 0.0f};
    float fov = 70.0f; //deg
    float near = 0.1f;
    float far = 100.0f;
    float view[16];
    float proj[16];

    void Update(float aspect);
};

class SyngineGraphics {
    std::string title;
    
    public:
    int width;
    int height;
    SDL_Window* win;
    Camera camera;
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle u_mvp = BGFX_INVALID_HANDLE;
    
    SyngineGraphics(const char* title, int width, int height);
    int CreateWindow();
    int CreateRenderer();
    int DestroyRenderer();
    void DestroyWindow();
    void SetProgram(bgfx::ProgramHandle program);
    bgfx::ProgramHandle GetProgram() const;
    int DestroyProgram();
    
    int RenderFrame(SynModelLoader& modelLoader);
};