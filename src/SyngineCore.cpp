#include "SyngineCore.h"

SyngineCore::SyngineCore() {
    //initialize app
    this->app = new SyngineApp();
    this->app->graphics = nullptr; // No graphics attached initially
}

SyngineCore::~SyngineCore() {
    //cleanup
    if (this->app) {
        delete this->app;
    }
}

int SyngineCore::AttachGraphics(SyngineGraphics* graphics) {
    //attach graphics to app
    if (!graphics) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No graphics object provided to attach");
        return 1;
    }
    
    this->app->graphics = graphics;
    return 0;
}

int SyngineCore::DetachGraphics() {
    //detach graphics from app
    if (!this->app->graphics) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No graphics object to detach");
        return 1;
    }
    
    this->app->graphics = nullptr;
    return 0;
}

int SyngineCore::SyngineEventLoop() {
    //main loop
    SDL_Log("Enetring event loop");
    bool running = true;
    bool once = false;
    int frame = 0;
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

                this->app->graphics->width = w;
                this->app->graphics->height = h;
                bgfx::reset(w, h, BGFX_RESET_VSYNC); // reset bgfx with new window size
                bgfx::setViewRect(0, 0, 0, uint16_t(w), uint16_t(h)); // reset view rect
            }
            if (!once) {
                this->app->graphics->CreateRenderer(); // create renderer
                once = true;
            }
        }
        ++frame;

        if (this->app && this->app->graphics && once) {
            this->app->graphics->RenderFrame(); // render frame
            if (frame % 180 == 0) {
                SDL_Log("render frame");
            }
        }
    }
    
    SDL_Log("Exiting event loop");
    return 0;
}