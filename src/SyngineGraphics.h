#pragma once
#include <string>
#include <cmath>

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_hints.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

class SyngineGraphics {
    std::string title;
    
    public:
    int width;
    int height;
    SDL_Window* win;

    SyngineGraphics(const char* title, int width, int height);
    int CreateWindow();
    int CreateRenderer();
    void DestroyWindow();
    
    int RenderFrame();
};