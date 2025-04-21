#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_hints.h>

#include <iostream>
#include <cmath>
#include <string>

struct SyngineWindow {
    SDL_Window* win;
    SDL_Renderer* rendr;
    int width;
    int height;
    std::string title;
};

SyngineWindow* syn_createWindow(const char*, int, int);
void syn_destroyWindow(SyngineWindow& win);