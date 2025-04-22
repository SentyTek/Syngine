#include <SyngineGraphics.h>

SyngineGraphics::SyngineGraphics(const char* title, int width, int height) {
    SyngineWindow* app = new SyngineWindow{
        .win = nullptr,
        .rendr = nullptr,
        .width = width,
        .height = height,
        .title = title ? title : "Syngine Window"
    };

    this->app = app;
}

int SyngineGraphics::CreateWindow() {
    //init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    SyngineWindow& app = *this->app;

    //create window
    app.win = SDL_CreateWindow(
        app.title.c_str(),
        app.width, app.height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    if (!app.win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* win = app.win; //for convenience
    
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    //create renderer
    app.rendr = SDL_CreateRenderer(win, NULL);
    if (!app.rendr) {
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

void SyngineGraphics::DestroyWindow() {
    //cleanup
    SyngineWindow& app = *this->app;
    SDL_Log("app shutting down");
    SDL_DestroyRenderer(app.rendr);
    SDL_DestroyWindow(app.win);
    delete this->app;
    SDL_Log("goodbye world");
}
