// ╒════════════════════════ TempInput.h ═╕
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
class InputManager {
  public:
    using Callback = std::function<void()>;

    /// @brief Registers a callback for a specific key.
    static void RegisterKey(SDL_Keycode key, Callback callback);

    /// @brief Handles SDL events and triggers the appropriate callbacks.
    static void HandleEvent(const SDL_Event& event);
  private:
    static std::unordered_map<SDL_Keycode, Callback> keyCallbacks;
};

} // namespace Syngine
