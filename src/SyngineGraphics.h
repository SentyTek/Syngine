#pragma once
#include <string>
#include <cmath>

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_hints.h>

class SyngineGraphics {
    int width;
    int height;
    std::string title;
    
    public:
    SDL_Window* win;
    SDL_Renderer* rendr;

    SyngineGraphics(const char* title, int width, int height);
    int CreateWindow();
    void DestroyWindow();
    
    int SynDrawClear();
};