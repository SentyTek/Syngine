// ╒═══════════════════════ BasicInput.h ═╕
// │ Syngine                              │
// │ Created 2025-08-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

#include "SDL3/SDL_events.h"
#include <functional>
#include <unordered_map>
#include <SDL3/SDL.h>

namespace Syngine {

/// @brief Manages user input for the application.
/// @section TempInput
class BasicInput {
  public:
    enum class MouseMove {
        MOTION,
        SCROLL,
    };
    enum class MouseButton {
        BUTTON_UP,
        BUTTON_DOWN,
    };
    using KeyCallback = std::function<void()>;
    using MouseButtonCallback = std::function<void(int)>;
    using MouseMoveCallback = std::function<void(float, float)>;

    /// @brief Registers a callback for a specific key.
    /// @param key The SDL keycode to register the callback for
    /// @param callback The function to call when the key is pressed
    static void RegisterKey(SDL_Keycode key, KeyCallback callback);

    /// @brief Registers a callback for mouse events
    /// @param event The mouse event to register the callback for
    /// @param callback The function to call when the mouse event occurs
    static void RegisterMouseButtonEvent(MouseButton         event,
                                         MouseButtonCallback callback);

    /// @brief Registers a callback for mouse scroll events
    /// @param callback The function to call when the mouse scroll event occurs
    static void RegisterMouseMoveEvent(MouseMove         event,
                                       MouseMoveCallback callback);

    /// @brief Handles SDL events and triggers the appropriate callbacks.
    /// @param event The SDL event to handle
    /// @internal
    static void _HandleEvent(const SDL_Event& event);

    /// @brief Gets the current state of the keyboard.
    static const bool* GetKeyboardState();

  private:
    static std::unordered_map<SDL_Keycode, KeyCallback> keyCallbacks;
    static std::unordered_map<MouseButton, MouseButtonCallback> mouseEventCallbacks;
    static std::unordered_map<MouseMove, MouseMoveCallback> mouseMoveCallbacks;
};

} // namespace Syngine
