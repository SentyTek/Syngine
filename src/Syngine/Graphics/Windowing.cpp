// ╒══════════════════════ Windowing.cpp ═╕
// │ Syngine                              │
// │ Created 2025-08-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Graphics/Windowing.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include "Syngine/Core/Logger.h"

namespace Syngine {

std::string Window::m_title = "Syngine Game";
bool        Window::m_contextCreated = false;
bool        Window::m_shouldClose    = false;
SDL_Window* Window::m_window         = nullptr;

Window::Window(const EngineConfig& config) {
    m_title = config.windowTitle;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Syngine::Logger::LogF(Syngine::LogLevel::FATAL,
                              "Failed to initialize SDL: %s",
                              SDL_GetError());
    }

    m_window = SDL_CreateWindow(m_title.c_str(),
                                 config.windowWidth,
                                 config.windowHeight,
                                 SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);

    if (!m_window) {
        Syngine::Logger::LogF(Syngine::LogLevel::FATAL,
                              "Failed to create SDL window: %s",
                              SDL_GetError());
        SDL_Quit();
    }

#if BX_PLATFORM_OSX
    // macOS requires some kind of renderer to be created before Metal can be
    // initialized
    SDL_Renderer* renderer = SDL_CreateRenderer(m_window, NULL);
    SDL_DestroyRenderer(renderer);
#endif
}

Window::~Window() {
    if (!m_window) {
        Syngine::Logger::Error("No app to destroy");
    }

    // Cleanup
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    Syngine::Logger::Info("Window destroyed successfully");
}

bool Window::ShouldClose() const noexcept { return m_shouldClose; }
void Window::_SetShouldClose(bool enabled) noexcept { m_shouldClose = enabled; }
void Window::_SetContextCreated(bool enabled) noexcept {
    m_contextCreated = enabled;
}
SDL_Window* Window::_GetSDLWindow() noexcept { return m_window; }


int Window::GetWidth() const noexcept {
    if (m_window) {
        int width;
        SDL_GetWindowSize(m_window, &width, nullptr);
        return width;
    }
    return 0;
}

int Window::GetHeight() const noexcept {
    if (m_window) {
        int height;
        SDL_GetWindowSize(m_window, nullptr, &height);
        return height;
    }
    return 0;
}

float Window::GetAspectRatio() const noexcept {
    int width, height;
    SDL_GetWindowSize(m_window, &width, &height);
    return height > 0 ? static_cast<float>(width) / height : 0.0f;
}

void Window::SetTitle(const std::string_view& title) {
    m_title = title;
    if (m_window) {
        SDL_SetWindowTitle(m_window, m_title.c_str());
    }
}

void Window::SetVSync(bool enabled) {
    if (m_window && m_contextCreated) {
        
    }
}

void Window::SetWindowMode(int mode) {
    if (m_window && m_contextCreated && mode >= 0 && mode <= 2) {
        SDL_DisplayID displayID;
        SDL_DisplayMode** dm;
        SDL_DisplayMode* displayMode;

        switch (mode) {
            case 0: // Bordered
                SDL_SetWindowFullscreen(m_window, false);
                break;
            case 1: // Fullscreen Borderless
                SDL_SetWindowFullscreen(m_window, true);
                break;
            case 2: // Exclusive Fullscreen
                // Get display information
                int count;
                displayID = SDL_GetDisplayForWindow(m_window);

                // Available display modes
                dm = SDL_GetFullscreenDisplayModes(displayID, &count);

                if (count <= 0 || !dm) {
                    Syngine::Logger::Error("No displays found or failed to get fullscreen display modes");
                    break;
                }

                // Get the preferred display mode
                displayMode = nullptr;
                if (count > 0) {
                    displayMode = dm[0];
                }

                // Set the window to fullscreen
                SDL_SetWindowFullscreen(m_window, true);
                SDL_SetWindowFullscreenMode(m_window, displayMode);

                SDL_free(&displayID);
                SDL_free(dm);
                SDL_free(displayMode);
                break;
            default:
                break;
        }
    }
}

} // namespace Syngine
