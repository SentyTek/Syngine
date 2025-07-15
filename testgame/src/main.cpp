#include "SyngineCore.h"
#include "SyngineGraphics.h"
#include "SyngineLogger.h"
#include <string>

int main(int argc, char* argv[]) {
    const char* appName = "bakerman";

    Syngine::Logger::Init(std::string(appName));
    Syngine::Logger::Log("good morning vietnam");

    // Create window
    Syngine::Core syngine;
    Syngine::Graphics graphics(appName, 1600, 900);

    if (graphics.CreateGameWindow() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window! If any errors are listed above, these will likely fix it!");
        return 1;
    }

    // Create renderer
    if (graphics.CreateRenderer() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer! If any errors are listed above, these will likely fix it!");
        graphics.DestroyWindow();
        return 1;
    }

    // Attach graphics to core
    if (syngine.AttachGraphics(&graphics) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to attach graphics to core");
        graphics.DestroyWindow();
        return 1;
    }

    Syngine::Logger::LogHardwareInfo(syngine);

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

    // Run event loop
    syngine.SyngineEventLoop(); // Note that this is a blocking call, it will run until the window is closed or quit event is triggered

    // Cleanup and quit
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
    Syngine::Logger::Shutdown();
    SDL_Quit();
    return 0;
}