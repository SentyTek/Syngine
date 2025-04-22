#pragma once
#include <Syngine.h>
#include <string>

struct SyngineWindow {
    SDL_Window* win;
    SDL_Renderer* rendr;
    int width;
    int height;
    std::string title;
};

class SyngineGraphics {
    public:
    SyngineWindow* app;
    SyngineGraphics(const char* title, int width, int height);
    int CreateWindow();
    void DestroyWindow();
};