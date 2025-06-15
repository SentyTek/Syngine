#include "SyngineCore.h"

int main(int argc, char* argv[]) {
    //create window
    Syngine::Core syngine;
    Syngine::Graphics graphics("bakerman", 800, 600);
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
    
    //Sky program is created first, as it is the background
    //This implies that shaders are loaded in the order they should be rendered
    //Ex: sky -> terrain -> models -> post processing
    size_t skyProg = graphics.AddProgram("shaders/sky.vert.sc.bin", "shaders/sky.frag.sc.bin", "sky");
    if (skyProg == (size_t)-1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create program");
        graphics.DestroyRenderer();
        graphics.DestroyWindow();
        return 1;
    }
    size_t terProg = graphics.AddProgram("shaders/terrain.vert.sc.bin", "shaders/terrain.frag.sc.bin", "terrain");
    if (terProg == (size_t)-1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create program");
        graphics.DestroyRenderer();
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
    for(size_t i = 0; i < syngine.app->gameObjects.size(); ++i) {
        Syngine::GameObject* gameObject = syngine.app->gameObjects[i];
        if (gameObject) {
            syngine.DeleteGameobject(gameObject);
        }
    }
    syngine.app->gameObjects.clear();
    syngine.DetachGraphics();
    graphics.RemoveProgram(true);
    graphics.DestroyRenderer();
    graphics.DestroyWindow();
    SDL_Quit();
    return 0;
}