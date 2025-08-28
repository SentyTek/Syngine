// ╒═════════════════════ BasicInput.cpp ═╕
// │ Syngine                              │
// │ Created 2025-08-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/BasicInput.h"
#include "SDL3/SDL_events.h"

namespace Syngine {

std::unordered_map<SDL_Keycode, BasicInput::KeyCallback> BasicInput::keyCallbacks;
std::unordered_map<BasicInput::MouseButton, BasicInput::MouseButtonCallback>
    BasicInput::mouseEventCallbacks;
std::unordered_map<BasicInput::MouseMove, BasicInput::MouseMoveCallback>
    BasicInput::mouseMoveCallbacks;

void BasicInput::RegisterKey(SDL_Keycode key, KeyCallback callback) {
    keyCallbacks[key] = callback;
}

void BasicInput::RegisterMouseButtonEvent(MouseButton event, MouseButtonCallback callback) {
    mouseEventCallbacks[event] = callback;
}

void BasicInput::RegisterMouseMoveEvent(MouseMove event, MouseMoveCallback callback) {
    mouseMoveCallbacks[event] = callback;
}

void BasicInput::_HandleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN: {
            auto it = keyCallbacks.find(event.key.key);
            if (it != keyCallbacks.end()) {
                it->second(); // Call the registered callback
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            auto it = mouseEventCallbacks.find(MouseButton::BUTTON_UP);
            if (it != mouseEventCallbacks.end()) {
                it->second(event.button.button); // Call the registered callback
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            auto it = mouseEventCallbacks.find(MouseButton::BUTTON_DOWN);
            if (it != mouseEventCallbacks.end()) {
                it->second(event.button.button); // Call the registered callback
            }
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            auto it = mouseMoveCallbacks.find(MouseMove::SCROLL);
            if (it != mouseMoveCallbacks.end()) {
                it->second(event.wheel.x, event.wheel.y); // Call the registered callback
            }
            break;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            auto it = mouseMoveCallbacks.find(MouseMove::MOTION);
            if (it != mouseMoveCallbacks.end()) {
                it->second(event.motion.xrel, event.motion.yrel); // Call the registered callback
            }
            break;
        }
    }
}

const bool* BasicInput::GetKeyboardState() {
    return SDL_GetKeyboardState(nullptr);
}

} // namespace Syngine