// ╒══════════════════════ Windowing.cpp ═╕
// │ Syngine                              │
// │ Created 2025-08-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Graphics/Windowing.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_video.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/Utils/Serializer.h"

namespace Syngine {

std::string Window::m_title = "Syngine Game";
bool        Window::m_contextCreated = false;
bool        Window::m_shouldClose    = false;
SDL_Window* Window::m_window         = nullptr;

Window::Window(const EngineConfig& config) {
    m_title = config.gameName;
    int w, h = 0;
    if (config.windowWidth > 0 && config.windowHeight > 0) {
        w = config.windowWidth;
        h = config.windowHeight;
    } else {
        w = 800;
        h = 600;
    }

    auto* videoSettings = Serializer::_LoadCoreSettingsCategory<
        Serializer::CoreSettings::Video>();
    if (videoSettings) {
        w = videoSettings->width;
        h = videoSettings->height;
        Core::_GetConfig()->windowHeight = videoSettings->height;
        Core::_GetConfig()->windowWidth = videoSettings->width;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Syngine::Logger::LogF(Syngine::LogLevel::FATAL, false,
                              "Failed to initialize SDL: %s",
                              SDL_GetError());
    }

    m_window = SDL_CreateWindow(
        m_title.c_str(),
        w,
        h,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN
#if BX_PLATFORM_OSX
            | SDL_WINDOW_METAL
#endif
    );

    if (!m_window) {
        Syngine::Logger::LogF(Syngine::LogLevel::FATAL, false,
                              "Failed to create SDL window: %s",
                              SDL_GetError());
        SDL_Quit();
    }

    Logger::_SetMainWindow(m_window);

    if (videoSettings->fullscreen) {
        m_contextCreated = true; // Some platforms require the context to be created before setting fullscreen
        SetWindowMode(1); // Borderless fullscreen
        m_contextCreated = false;
    }

#if BX_PLATFORM_OSX
    // macOS requires some kind of renderer to be created before Metal can be
    // initialized
    SDL_ShowWindow(m_window);
    SDL_MetalView mv = SDL_Metal_CreateView(m_window);
    if (!mv) {
        Syngine::Logger::Error("Failed to create Metal view", false);
    }
    SDL_HideWindow(m_window);
#endif
    Logger::Info("Window created successfully", false);
}

Window::~Window() {
    if (!m_window) {
        Syngine::Logger::Error("No app to destroy");
    }

    // Cleanup
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    m_window = nullptr;
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
        switch (mode) {
            case 0: // Bordered
                if (!SDL_SetWindowFullscreen(m_window, false)) {
                    Syngine::Logger::LogF(Syngine::LogLevel::ERR, false,
                                         "Failed to set windowed mode: %s", SDL_GetError());
                }
                break;
            case 1: // Fullscreen Borderless
                if (!SDL_SetWindowFullscreen(m_window, true)) {
                    Syngine::Logger::LogF(Syngine::LogLevel::ERR, false,
                                         "Failed to set borderless fullscreen: %s", SDL_GetError());
                }
                break;
            case 2: // Exclusive Fullscreen
                {
                    SDL_DisplayID displayID = SDL_GetDisplayForWindow(m_window);
                    if (!displayID) {
                        Syngine::Logger::LogF(Syngine::LogLevel::ERR, false,
                                             "Failed to get display for window: %s", SDL_GetError());
                        break;
                    }

                    int count = 0;
                    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayID, &count);

                    if (count <= 0 || !modes) {
                        Syngine::Logger::Error("No fullscreen display modes available", false);
                        break;
                    }

                    // Set exclusive fullscreen with the first available mode
                    if (!SDL_SetWindowFullscreen(m_window, true)) {
                        Syngine::Logger::LogF(Syngine::LogLevel::ERR, false,
                                             "Failed to enable fullscreen: %s", SDL_GetError());
                        SDL_free(modes);
                        break;
                    }

                    if (!SDL_SetWindowFullscreenMode(m_window, modes[0])) {
                        Syngine::Logger::LogF(Syngine::LogLevel::ERR, false,
                                             "Failed to set exclusive fullscreen mode: %s", SDL_GetError());
                    }

                    SDL_free(modes);
                }
                break;
            default:
                break;
        }
    }
}

void Window::SetMouseCursorVisible(bool visible) {
    SDL_SetWindowRelativeMouseMode(m_window, !visible);
}

void Window::GetMousePosition(float& x, float& y) {
    if (m_window) {
        SDL_GetMouseState(&x, &y);
    }
}

void Window::SetMousePosition(float& x, float& y) {
    if (m_window) {
        SDL_WarpMouseInWindow(m_window, x, y);
    }
}

} // namespace Syngine
