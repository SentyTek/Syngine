#include <cwchar>
#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

#include <SDL3/SDL.h>
#include "SyngineCore.h"
#include "SyngineGraphics.h"
#include "SyngineLogger.h"
#include "Registry.h"
#include <string>

#if defined(_WIN32)
void EnableConsole() {
    AllocConsole();

    // Redirect standard output to the console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    std::ios::sync_with_stdio();
}
#endif

void ErrorAndExit(const std::string& message, int code) {
    Syngine::Logger::LogF(Syngine::LogLevel::FATAL, "%s", message.c_str());
    SDL_Quit();
    exit(code);
}

int AppMain(int argc, char* argv[]) {
    const char* appName = "Bakerman";

    Syngine::Logger::Init(std::string(appName));
    Syngine::Logger::Log("Starting " + std::string(appName));

    // Create window
    Syngine::Core syngine(appName);
    Syngine::Graphics graphics(appName, 1600, 900);

    if (graphics.CreateGameWindow() != 0) {
        ErrorAndExit("Failed to create window! Check the log for details.", 1);
    }

    // Create renderer
    if (graphics.CreateRenderer() != 0) {
        graphics.DestroyWindow();
        ErrorAndExit("Failed to create renderer! Check the log for details.", 1);
    }

    // Attach graphics to core
    if (syngine.AttachGraphics(&graphics) != 0) {
        graphics.DestroyWindow();
        ErrorAndExit("Failed to attach graphics to core. Check the log for details.", 1);
    }

    Syngine::Logger::LogHardwareInfo();

    // Shader programs can be created arbitrarily, being supplied an optional
    // ViewID to match the rendering view (e.g. VIEW_FOWARD, VIEW_SKY, VIEW_DEBUG, etc.)
    if (graphics.AddProgram("shaders/sky", "sky", Syngine::VIEW_SKY) == -1) {
        graphics.DestroyRenderer();
        graphics.DestroyWindow();
        ErrorAndExit("Failed to create program. Check the log for details.", 1);
    }
    
    if (graphics.AddProgram("shaders/terrain", "terrain") == -1) {
        graphics.DestroyRenderer();
        graphics.DestroyWindow();
        ErrorAndExit("Failed to create program. Check the log for details.", 1);
    }

    // Run event loop
    syngine.SyngineEventLoop(); // Note that this is a blocking call, it will run until the window is closed or quit event is triggered

    // Cleanup
    for (auto& [id, gameObject] : Syngine::Registry::GetAllGameObjects()) {
        syngine.DeleteGameobject(gameObject);
    }
    Syngine::Registry::Clear();
    syngine.DetachGraphics();
    graphics.RemoveAllPrograms();
    graphics.DestroyRenderer();
    graphics.DestroyWindow();
    Syngine::Logger::Shutdown();
    SDL_Quit();
    return 0;
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmd,
                   int       nShowCmd) {
    // Convert command line to argc/argv
    int argc;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
    char** argv = new char*[argc];
    for (int i = 0; i < argc; ++i) {
        int len = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
        argv[i] = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], len, nullptr, nullptr);
    }

    for (int i = 0; i < argc; ++i) {
        if (wcscmp(argvW[i], L"--console") == 0) {
            EnableConsole();
            break;
        }
    }

    // Call the main application function
    int result = AppMain(argc, argv);

    // Cleanup allocated memory
    for (int i = 0; i < argc; ++i) {
        delete[] argv[i];
    }
    delete[] argv;
    LocalFree(argvW);
    return result;
}

#else
int main(int argc, char* argv[]) {
    return AppMain(argc, argv);
}
#endif