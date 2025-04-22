#include <SyngineCore.h>

SyngineCore::SyngineCore() {
    //initialize app
    this->app = new SyngineApp();
    this->app->graphics = nullptr; // No graphics attached initially
}

SyngineCore::~SyngineCore() {
    //cleanup
    if (this->app) {
        if (this->app->graphics) {
            this->app->graphics->DestroyWindow();
            delete this->app->graphics;
        }
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

        this->app->graphics->SynDrawClear(); //temp draw clear function
    }

    return 0;
}