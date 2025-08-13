// ╒══════════════════vvvvvv Windowing.h ═╕
// │ Syngine                              │
// │ Created 2025-08-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/Core/Core.h"
#include "SDL3/SDL_video.h"
#include <string_view>
#include <string>

namespace Syngine {

/// @brief Manages all of the windowing operations
/// @section Window
class Window {
  public:
    Window(const EngineConfig& config);
    ~Window();

    /// @brief Checks if the window should close.
    /// @return True if the window should close, false otherwise
    /// @threadsafety safe
    /// @since v0.0.1
    bool ShouldClose() const noexcept;

    /// @brief Gets the width of the window.
    /// @return The width of the window in pixels.
    /// @threadsafety not-safe
    /// @since v0.0.1
    int GetWidth() const noexcept;

    /// @brief Gets the height of the window.
    /// @return The height of the window in pixels.
    /// @threadsafety not-safe
    /// @since v0.0.1
    int GetHeight() const noexcept;

    /// @brief Gets the aspect ratio of the window.
    /// @return The aspect ratio of the window.
    /// @threadsafety not-safe
    /// @since v0.0.1
    float GetAspectRatio() const noexcept;

    /// @brief Sets the title of the window.
    /// @param title The new title of the window.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetTitle(const std::string_view& title);

    /// @brief Sets the vertical sync state of the window.
    /// @param enabled True to enable vertical sync, false to disable it.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetVSync(bool enabled);

    /// @brief Sets the state of the window (bordered, fullscreen borderless,
    /// exclusive fullscreen)
    /// @param mode The new window mode. 0 = bordered, 1 = fullscreen borderless,
    /// 2 = exclusive fullscreen
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetWindowMode(int mode);

    /// @brief Sets the visibility of the mouse cursor.
    /// @param visible True to show the cursor, false to hide it.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetMouseCursorVisible(bool visible);

    /// @brief Gets the current position of the mouse cursor.
    /// @param x A float reference to be filled in with the X position of the mouse cursor.
    /// @param y A float reference to be filled in with the Y position of the mouse cursor.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void GetMousePosition(float& x, float& y);

    /// @brief Sets the current position of the mouse cursor.
    /// @param x A float reference to be filled in with the X position of the mouse cursor.
    /// @param y A float reference to be filled in with the Y position of the mouse cursor.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetMousePosition(float& x, float& y);

    /// @brief Sets the window's close state.
    /// @param enabled True to close the window, false to keep it open.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static void _SetShouldClose(bool enabled) noexcept;

    /// @brief Sets the window's context creation state.
    /// @param enabled True to create the context, false to destroy it.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static void _SetContextCreated(bool enabled) noexcept;

    /// @brief Gets the SDL window pointer
    /// @return The SDL window pointer.
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    static SDL_Window* _GetSDLWindow() noexcept;

  private:
    static std::string m_title;
    static bool        m_contextCreated;
    static bool        m_shouldClose;
    static SDL_Window* m_window;
};

} // namespace Syngine
