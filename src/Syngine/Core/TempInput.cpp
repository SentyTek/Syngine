// ╒══════════════════════ TempInput.cpp ═╕
// │ Syngine                              │
// │ Created 2025-08-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/TempInput.h"

namespace Syngine {

std::unordered_map<SDL_Keycode, InputManager::Callback> InputManager::keyCallbacks;

void InputManager::RegisterKey(SDL_Keycode key, Callback callback) {
    keyCallbacks[key] = callback;
}

void InputManager::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        auto it = keyCallbacks.find(event.key.key);
        if (it != keyCallbacks.end()) {
            it->second(); // Call the registered callback
        }
    }
}

} // namespace Syngine