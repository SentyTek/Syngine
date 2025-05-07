#include <string>
#include <SDL3/SDL.h>

// This function resolves the full relative path to resourced files based on platform.
// macOS uses a different path structure than other because of the app bundle structure.
static std::string resolveOSPath(const char* path)
{
#ifdef __APPLE__
    // SDL_GetBasePath() returns ".../Contents/MacOS/"
    const char* base = SDL_GetBasePath();
    std::string p(base);

    // swap "MacOS/" -> "Resources/"
    auto pos = p.rfind("MacOS/");
    if (pos != std::string::npos) {
        p = p.substr(0, pos) + "Resources/";
    }
    p += path; // e.g. "shaders/vs_simple.sc.bin"
    return p;
#else
    return std::string(path);
#endif
}