#include "SyngineGraphics.h"

#if BX_PLATFORM_OSX
#include "SynOSXMetalBridge.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

SyngineGraphics::SyngineGraphics(const char* title, int width, int height) {
    //initialize app
    this->title = title;
    this->width = width;
    this->height = height;
    this->win = nullptr;
};

int SyngineGraphics::CreateWindow() {
    //bgInit
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

        
    //dummy renderer on mac
    #if BX_PLATFORM_OSX
    SDL_Renderer* renderer = SDL_CreateRenderer(this->win, NULL);
    SDL_DestroyRenderer(renderer);
    #endif
    
    SDL_ShowWindow(win);

    //return window
    return 0;
}

int SyngineGraphics::CreateRenderer() {
    //create renderer
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to create renderer for");
        return 1;
    }

    //initialize bgfx
    bgfx::Init bgInit;
    bgInit.debug = true;
    SDL_PropertiesID sdlProps = SDL_GetWindowProperties(this->win);
    if(sdlProps == 0) { //return 0 on failure
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    
    //set platform data
    #if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    //this assumes X11
    bgInit.platformData.ndt = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
    bgInit.platformData.nwh = (void*)SDL_GetNumberProperty(sdlProps, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    if (bgInit.platformData.nwh == NULL || bgInit.platformData.ndt == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties Linux");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    #elif BX_PLATFORM_OSX
    SDL_Log("macOS detected");
    bgInit.type = bgfx::RendererType::Metal; // set renderer type to Metal
    bgInit.platformData.ndt = NULL; //only needed on x11
    bgInit.platformData.nwh = GetSYNMetalView(this->win); //custom obj-c function to get the metal view
    if (bgInit.platformData.nwh == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties macOSX");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    #elif BX_PLATFORM_WINDOWS
    bgInit.platformData.ndt = NULL; //only needed on x11
    bgInit.platformData.nwh = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (bgInit.platformData.nwh == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties windows");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    #else
    #warning "Platform not supported
    bgInit.platformData.ndt = NULL;
    bgInit.platformData.nwh = NULL;
    #endif
    
    bgInit.resolution.width = this->width;
    bgInit.resolution.height = this->height;
    bgInit.resolution.reset = BGFX_RESET_VSYNC; // enable vsync

    if(!bgfx::init(bgInit)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize bgfx");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    bgfx::frame(); // submit the frame

    //reset view 0 to the dimentions of the window and clear it
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 0, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, uint16_t(this->width), uint16_t(this->height));
    bgfx::touch(0); // touch the view to clear it
    bgfx::frame(); // submit the frame

    //print window info
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

    return 0;
}

void SyngineGraphics::DestroyWindow() { //dw this is effectively the destructor
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to destroy");
        return;
    }
    bgfx::shutdown(); //shut down bgfx BEFORE destroying the window
    //cleanup
    SDL_Log("app shutting down");
    SDL_DestroyWindow(this->win);
    this->win = nullptr;
    SDL_Log("goodbye world");
}

int SyngineGraphics::RenderFrame() {
    //sure, let's draw a color or something
   auto time = SDL_GetTicks() / 1000.0f; // get time in seconds
    auto red = static_cast<Uint8>((sin(time) + 1.0f) / 2.0f * 255);
    auto green = static_cast<Uint8>((sin(time / 2.0f) + 1.0f) / 2.0f * 255);
    auto blue = static_cast<Uint8>((sin(time * 2.0f) + 1.0f) / 2.0f * 255);

    uint32_t packedColor = (red << 16) | (green << 8) | blue; //do not worry about it.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, packedColor, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, uint16_t(this->width), uint16_t(this->height));
    bgfx::touch(0); // touch the view to clear it
    bgfx::frame(); // submit the frame

    return 0;
}