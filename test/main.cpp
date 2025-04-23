#include <SyngineCore.h>
#include <SyngineGraphics.h>

#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    //create window
    SyngineCore syngine;
    SyngineGraphics graphics("bakerman", 800, 600);
    if (graphics.CreateWindow() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window");
        return 1;
    }
    //create renderer
    if (graphics.CreateRenderer() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer");
        graphics.DestroyWindow();
        return 1;
    }

    //attach graphics to core
    if (syngine.AttachGraphics(&graphics) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to attach graphics to core");
        graphics.DestroyWindow();
        return 1;
    }

    //run event loop
    syngine.SyngineEventLoop(); //note that this is a blocking call, it will run until the window is closed or quit event is triggered

    //cleanup
    syngine.DetachGraphics();
    graphics.DestroyWindow();
    SDL_Quit(); //cleanup SDL
    return 0;
}