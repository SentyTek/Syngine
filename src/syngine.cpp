#include "syngine.h"

using namespace std;
SyngineWindow* syn_createWindow(const char* title, int width, int height) {
    //init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return nullptr;
    }

    //create window struct
    //using designated initializers for clarity and ease of use
    SyngineWindow* app = new SyngineWindow{
        .win = nullptr,
        .rendr = nullptr,
        .width = width,
        .height = height,
        .title = title ? title : "Syngine Window"
    };

    //create window
    app->win = SDL_CreateWindow(
        title,
        width, height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!app->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return nullptr;
    }

    SDL_Window* win = app->win; //for convenience
    
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    //create renderer
    app->rendr = SDL_CreateRenderer(win, NULL);
    if (!app->rendr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return nullptr;
    }

    //print window info
    SDL_ShowWindow(win);
    {
        int w, h, bufw, bufh;
        SDL_GetWindowSize(win, &w, &h);
        SDL_GetWindowSizeInPixels(win, &bufw, &bufh);
        SDL_Log("Window size: %d x %d, Buffer size: %d x %d", w, h, bufw, bufh);
        if(w != bufw) {
            SDL_Log("This is a high DPI display");
        }
    }

    SDL_Log("hello world, this is bakerman coming at you live from CNN");

    //return window
    return app;
}

void syn_destroyWindow(SyngineWindow& app) {
    //cleanup
    SDL_Log("app shutting down");
    SDL_DestroyRenderer(app.rendr);
    SDL_DestroyWindow(app.win);
    SDL_Quit();

    SDL_Log("goodbye world");
}