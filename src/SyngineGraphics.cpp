#include <SyngineGraphics.h>

SyngineGraphics::SyngineGraphics(const char* title, int width, int height) {
    //initialize app
    this->title = title;
    this->width = width;
    this->height = height;
    this->win = nullptr;
    this->rendr = nullptr;
};

int SyngineGraphics::CreateWindow() {
    //init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    //check if app is initialized
    if (this->title == "") {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to create window for");
        return 1;
    }

    //create window
    this->win = SDL_CreateWindow(
        this->title.c_str(),
        this->width, this->height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    //create renderer
    this->rendr = SDL_CreateRenderer(win, NULL);
    if (!this->rendr) {
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

    //return window
    return 0;
}

void SyngineGraphics::DestroyWindow() { //dw this is effectively the destructor
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to destroy");
        return;
    }
    //cleanup
    SDL_Log("app shutting down");
    SDL_DestroyRenderer(this->rendr);
    SDL_DestroyWindow(this->win);
    this->rendr = nullptr;
    this->win = nullptr;
    SDL_Log("goodbye world");
}

int SyngineGraphics::SynDrawClear() {
    if (!this->rendr) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to draw on");
        return 1;
    }

    //sure, let's draw a color or something
    auto time = SDL_GetTicks() / 1000.0f; // get time in seconds
    auto red = static_cast<Uint8>((sin(time) + 1.0f) / 2.0f * 255);
    auto green = static_cast<Uint8>((sin(time / 2.0f) + 1.0f) / 2.0f * 255);
    auto blue = static_cast<Uint8>((sin(time * 2.0f) + 1.0f) / 2.0f * 255);

    //clear screen with color
    SDL_SetRenderDrawColor(this->rendr, red, green, blue, 255);
    SDL_RenderClear(this->rendr);

    //present to screen
    SDL_RenderPresent(this->rendr);

    return 0;
}