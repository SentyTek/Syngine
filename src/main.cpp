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

using namespace std;

const int windowStartWidth = 800;
const int windowStartHeight = 600;

int main(int argc, char* argv[]) {
    //init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    //create window
    SDL_Window* win = SDL_CreateWindow(
        "bakerman",
        windowStartWidth, windowStartHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    //enable vsync
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    //create renderer
    SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
    if (!ren) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
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

    //main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        //event handling
        while (SDL_PollEvent(&event)) { //where inputs can also be handled
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                int w, h;
                SDL_Window* resizedWindow = SDL_GetWindowFromID(event.window.windowID);
                if (resizedWindow) {
                    SDL_GetWindowSize(resizedWindow, &w, &h);
                }
                SDL_Log("Window resized to %d x %d", w, h);
            }
        }

        //sure, let's draw a color or something
        auto time = SDL_GetTicks() / 1000.0f; // get time in seconds
        auto red = static_cast<Uint8>((sin(time) + 1.0f) / 2.0f * 255);
        auto green = static_cast<Uint8>((sin(time / 2.0f) + 1.0f) / 2.0f * 255);
        auto blue = static_cast<Uint8>((sin(time * 2.0f) + 1.0f) / 2.0f * 255);

        //clear screen with color
        SDL_SetRenderDrawColor(ren, red, green, blue, 255);
        SDL_RenderClear(ren);

        //present to screen
        SDL_RenderPresent(ren);
    }

    //cleanup
    SDL_Log("app shutting down");
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    SDL_Log("goodbye world");
    return 0;
}