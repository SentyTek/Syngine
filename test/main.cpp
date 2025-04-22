#include <Syngine.h>
#include <SyngineGraphics.h>

#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    //create window
    SyngineGraphics graphics("bakerman", 800, 600);
    if (graphics.CreateWindow() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window");
        return 1;
    }

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
        SDL_SetRenderDrawColor(graphics.app->rendr, red, green, blue, 255);
        SDL_RenderClear(graphics.app->rendr);

        //present to screen
        SDL_RenderPresent(graphics.app->rendr);
    }

    //cleanup
    graphics.DestroyWindow();
    return 0;
}