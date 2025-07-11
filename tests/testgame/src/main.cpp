#include "SyngineCore.h"
#include "SyngineGraphics.h"

int main(int argc, char* argv[]) {
    //create window
    Syngine::Core syngine;
    Syngine::Graphics graphics("bakerman", 1600, 900);
    if (graphics.CreateWindow() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window! If any errors are listed above, these will likely fix it!");
        return 1;
    }
    //create renderer
    if (graphics.CreateRenderer() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer! If any errors are listed above, these will likely fix it!");
        graphics.DestroyWindow();
        return 1;
    }

    // Shader programs can be created arbitrarily, being supplied an optional
    // ViewID to match the rendering view (e.g. VIEW_FOWARD, VIEW_SKY, VIEW_DEBUG, etc.)
    if (graphics.AddProgram("shaders/sky", "sky", Syngine::VIEW_SKY) == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create program");
        graphics.DestroyRenderer();
        graphics.DestroyWindow();
        return 1;
    }
    
    if (graphics.AddProgram("shaders/terrain", "terrain") == -1) {
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
    graphics.RemoveAllPrograms();
    graphics.DestroyRenderer();
    graphics.DestroyWindow();
    SDL_Quit();
    return 0;
}