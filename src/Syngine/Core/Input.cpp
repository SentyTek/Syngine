// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Input.h"
#include "Syngine/Core/Logger.h"

#include "InputHelpers.h"

#include "SDL3/SDL_events.h"

#include <algorithm>

// MARK: Initialize InputAction's static members

std::vector<Syngine::InputAction*> Syngine::InputAction::_Registry;
std::list<Syngine::InputAction>    Syngine::InputAction::_HomelessShelter;

// MARK: InputAction private helpers and operators

void Syngine::InputAction::EnsureUniqueIdentifier(
    const std::string& identifier) {
    for (Syngine::InputAction* binding : Syngine::InputAction::_Registry) {
        if (binding->identifier == identifier) {
            Syngine::Logger::Log("InputAction identifier '" + identifier +
                                     "' is not unique",
                                 LogLevel::ERR);
        }
    }
}

// MARK: InputAction constructor implementation

Syngine::InputAction::InputAction(const std::string&  identifier,
                                  const std::string&  name,
                                  const std::string&  category,
                                  Syngine::KeyBinding binding,
                                  Callbacks           callbacks)
    : identifier(identifier), name(name), category(category), binding(binding),
      currentState(false), previousState(false), callbacks(callbacks) {
    Logger::Log("Creating InputAction '" + identifier + "'");
    Syngine::InputAction::EnsureUniqueIdentifier(identifier);
    Syngine::InputAction::_Registry.push_back(this);
}

Syngine::InputAction::~InputAction() {
    auto it = std::find(_Registry.begin(), _Registry.end(), this);
    if (it != _Registry.end()) {
        _Registry.erase(it);
    }
}

// MARK: InputAction registration implementation

Syngine::InputAction* Syngine::InputAction::RegisterAction(const std::string&  identifier,
                                          const std::string&  name,
                                          const std::string&  category,
                                          Syngine::KeyBinding binding,
                                          Callbacks           callbacks) {
    Syngine::InputAction::_HomelessShelter.emplace_back(
        identifier, name, category, binding, callbacks);
    Syngine::InputAction::_Registry.back() =
        &Syngine::InputAction::_HomelessShelter.back();
    return &_HomelessShelter.back();
}

// MARK: InputAction state getters

bool Syngine::InputAction::isPressed() { return this->currentState; }

bool Syngine::InputAction::wasPressed() {
    return this->currentState && !this->previousState;
}

bool Syngine::InputAction::wasReleased() {
    return !this->currentState && this->previousState;
}

bool Syngine::InputAction::stateChanged() {
    return this->currentState != this->previousState;
}

// MARK: Input event handler private helpers

bool
Syngine::KeyShortcut::_isTriggeredByEvent(SDL_KeyboardEvent event) const {
    switch (this->subType()) {
    case KeybindType::KEYCODE:
        return _SDLToSyn(event.key) == std::get<Keycode>(this->key) &&
               (_SDLToSyn(event.mod) == this->modifiers);
    case KeybindType::SCANCODE:
        return _SDLToSyn(event.scancode) == std::get<Scancode>(this->key) &&
               (_SDLToSyn(event.mod) == this->modifiers);
    default: return false;
    }
}

bool
Syngine::KeySequence::_isTriggeredByEvent(SDL_KeyboardEvent event) {
    const auto next = this->next();
    bool       triggered;

    switch (static_cast<KeybindType>(next.index())) {
    case KeybindType::KEYCODE:
        triggered = _SDLToSyn(event.key) == std::get<Keycode>(next);
        break;
    case KeybindType::SCANCODE:
        triggered = _SDLToSyn(event.scancode) == std::get<Scancode>(next);
        break;
    case KeybindType::SHORTCUT:
        triggered = std::get<KeyShortcut>(next)._isTriggeredByEvent(event);
        break;
    default: triggered = false; break;
    }

    if (triggered) {
        this->increment();
        return this->wasReset();
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
        this->reset();
    }
    return false;
}

bool
Syngine::KeyBinding::_isTriggeredByEvent(SDL_KeyboardEvent event) {
    switch (this->subType()) {
    case KeybindType::UNBOUND: return false;
    case KeybindType::KEYCODE:
        return _SDLToSyn(event.key) == std::get<Keycode>(this->binding);
    case KeybindType::SCANCODE:
        return _SDLToSyn(event.scancode) == std::get<Scancode>(this->binding);
    case KeybindType::MOUSE_BUTTON:
        return false; // Mouse buttons are not triggered by keyboard events
    case KeybindType::SHORTCUT:
        return std::get<KeyShortcut>(this->binding)._isTriggeredByEvent(event);
    case KeybindType::SEQUENCE:
        return std::get<KeySequence>(this->binding)._isTriggeredByEvent(event);
    }

    return false;
}

bool
Syngine::KeyBinding::_isTriggeredByEvent(SDL_MouseButtonEvent event) {
    if (this->subType() == KeybindType::MOUSE_BUTTON) {
        return _SDLToSyn(event.button) == std::get<MouseButton>(this->binding);
    } else {
        return false;
    }
}

// MARK: Input event handler

void Syngine::InputAction::_HandleEvent(SDL_Event event) {
    bool down = false;

    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: down = true;
    case SDL_EVENT_KEY_UP:
        for (InputAction* action : InputAction::_Registry) {
            if (action->binding._isTriggeredByEvent(event.key)) {
                action->previousState = action->currentState;
                action->currentState  = down;
                action->callbacks.onStateChanged();
                if (down) {
                    action->callbacks.onPressed();
                } else {
                    action->callbacks.onReleased();
                }
            }
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN: down = true;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        for (InputAction* action : InputAction::_Registry) {
            if (action->binding._isTriggeredByEvent(event.button)) {
                action->previousState = action->currentState;
                action->currentState  = down;
                action->callbacks.onStateChanged();
                if (down) {
                    action->callbacks.onPressed();
                } else {
                    action->callbacks.onReleased();
                }
            }
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        _MouseMoveCallback(event.motion.xrel, event.motion.yrel);
        break;

    case SDL_EVENT_MOUSE_WHEEL:
        _ScrollCallback(event.wheel.x, event.wheel.y);
        break;

    default: break;
    }
}

// MARK: Temporary static member initialization

std::function<void(float, float)> Syngine::InputAction::_MouseMoveCallback =
    [](float, float) -> void {};
std::function<void(float, float)> Syngine::InputAction::_ScrollCallback =
    [](float, float) -> void {};

// MARK: Temporary mouse movement and scroll event callback setters

void Syngine::InputAction::RegisterMouseMoveEvent(
    std::function<void(float, float)> callback) {
    _MouseMoveCallback = callback;
}

void Syngine::InputAction::RegisterScrollEvent(
    std::function<void(float, float)> callback) {
    _ScrollCallback = callback;
}

Syngine::Scancode Syngine::StringToScancode(const std::string& code) {
    for (std::pair item : _StringToScancode) {
        if (item.first == code) {
            return item.second;
        }
    }

    return Scancode::_UNKNOWN;
}

std::string Syngine::ScancodeToString(const Scancode& code) {
    for (std::pair item : _StringToScancode) {
        if (item.second == code) {
            return item.first;
        }
    }

    return "";
}


Syngine::Keycode Syngine::StringToKeycode(const std::string& key) {
    for (std::pair item : _StringToKeycode) {
        if (item.first == key) {
            return item.second;
        }
    }

    return Keycode::_UNKNOWN;
}


std::string Syngine::KeycodeToString(const Keycode& key) {
    for (std::pair item : _StringToKeycode) {
        if (item.second == key) {
            return item.first;
        }
    }

    return "";
}

std::string Syngine::MouseButtonToString(const MouseButton& button) {
    switch (button) {
        case MouseButton::LEFT: return "left";
        case MouseButton::RIGHT: return "right";
        case MouseButton::MIDDLE: return "middle";
        case MouseButton::FOUR: return "button_4";
        case MouseButton::FIVE: return "button_5";
        default: return "";
    }
}

Syngine::MouseButton Syngine::StringToMouseButton(const std::string& button) {
    if (button == "left" || button == "primary") {
        return MouseButton::LEFT;
    }
    if (button == "right" || button == "secondary") {
        return MouseButton::RIGHT;
    }
    if (button == "middle" || button == "tertiary") {
        return MouseButton::MIDDLE;
    }
    if (button == "button_4") return MouseButton::FOUR;
    if (button == "button_5") return MouseButton::FIVE;
    return MouseButton::_UNKNOWN;
}

std::vector<std::string> Syngine::KeymodToString(const Keymod& keymod) {
    std::vector<std::string> result = std::vector<std::string>();

    if (keymod.contains(Keymod::LEFT_SHIFT))
        result.push_back("left_shift");
    if (keymod.contains(Keymod::RIGHT_SHIFT))
        result.push_back("right_shift");
    if (keymod.contains(Keymod::LEFT_CONTROL))
        result.push_back("left_control");
    if (keymod.contains(Keymod::RIGHT_CONTROL))
        result.push_back("right_control");
    if (keymod.contains(Keymod::LEFT_ALT))
        result.push_back("left_alt");
    if (keymod.contains(Keymod::RIGHT_ALT))
        result.push_back("right_alt");
    if (keymod.contains(Keymod::LEFT_GUI))
        result.push_back("left_gui");
    if (keymod.contains(Keymod::RIGHT_GUI))
        result.push_back("right_gui");
    if (keymod.contains(Keymod::CONTROL))
        result.push_back("control");
    if (keymod.contains(Keymod::SHIFT))
        result.push_back("shift");
    if (keymod.contains(Keymod::ALT))
        result.push_back("alt");
    if (keymod.contains(Keymod::GUI))
        result.push_back("gui");

    return result;
}

Syngine::Keymod Syngine::StringToKeymod(const std::vector<std::string>& keymod) {
    Keymod result = Keymod::NONE;

    for (const std::string& mod : keymod) {
        if (mod == "left_shift")
            result += Keymod::LEFT_SHIFT;
        if (mod == "right_shift")
            result += Keymod::RIGHT_SHIFT;
        if (mod == "left_control")
            result += Keymod::LEFT_CONTROL;
        if (mod == "right_control")
            result += Keymod::RIGHT_CONTROL;
        if (mod == "left_alt" || mod == "left_option")
            result += Keymod::LEFT_ALT;
        if (mod == "right_alt" || mod == "right_option")
            result += Keymod::RIGHT_ALT;
        if (mod == "left_gui" || mod == "left_command" || mod == "left_windows")
            result += Keymod::LEFT_GUI;
        if (mod == "right_gui" || mod == "right_command" || mod == "right_windows")
            result += Keymod::RIGHT_GUI;
        if (mod == "control")
            result += Keymod::CONTROL;
        if (mod == "shift")
            result += Keymod::SHIFT;
        if (mod == "alt" || mod == "option")
            result += Keymod::ALT;
        if (mod == "gui" || mod == "command" || mod == "windows")
            result += Keymod::GUI;
    }

    return result;
}
