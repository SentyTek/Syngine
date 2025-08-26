// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Input.h"
#include "Syngine/Core/Logger.h"

#include "InputHelpers.h"

#include "SDL3/SDL_events.h"

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
                                 LogLevel::FATAL);
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

// MARK: InputAction convenience constructors

Syngine::InputAction::InputAction(const std::string&  identifier,
                                  const std::string&  name,
                                  const std::string&  category,
                                  Syngine::KeyBinding binding)
    : InputAction(identifier, name, category, binding, Callbacks()) {}

Syngine::InputAction::InputAction(const std::string& identifier,
                                  const std::string& name)
    : InputAction(identifier, name, "", Syngine::KeyBinding(), Callbacks()) {}

Syngine::InputAction::InputAction(const std::string& identifier,
                                  const std::string& name,
                                  const std::string& category)
    : InputAction(
          identifier, name, category, Syngine::KeyBinding(), Callbacks()) {}

Syngine::InputAction::InputAction(const std::string&  identifier,
                                  const std::string&  name,
                                  Syngine::KeyBinding binding)
    : InputAction(identifier, name, "", binding, Callbacks()) {}

Syngine::InputAction::~InputAction() {
    Logger::Warn("Removing InputAction '" + this->identifier + "'");
    auto it = std::find(_Registry.begin(), _Registry.end(), this);
    if (it != _Registry.end()) {
        _Registry.erase(it);
    }
}

// MARK: InputAction registration implementation

void Syngine::InputAction::RegisterAction(const std::string&  identifier,
                                          const std::string&  name,
                                          const std::string&  category,
                                          Syngine::KeyBinding binding,
                                          Callbacks           callbacks) {
    Syngine::InputAction::_HomelessShelter.emplace_back(
        identifier, name, category, binding, callbacks);
    Syngine::InputAction::_Registry.back() =
        &Syngine::InputAction::_HomelessShelter.back();
}

// MARK: InputAction convenience registration

void Syngine::InputAction::RegisterAction(const std::string&  identifier,
                                          const std::string&  name,
                                          Syngine::KeyBinding binding,
                                          Callbacks           callbacks) {
    RegisterAction(identifier, name, "", binding, callbacks);
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

constexpr bool
Syngine::KeyShortcut::_isTriggeredByEvent(SDL_KeyboardEvent event) {
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

constexpr bool
Syngine::KeySequence::_isTriggeredByEvent(SDL_KeyboardEvent event) {
    bool triggered;

    switch (this->subType(nextIndex)) {
    case KeybindType::UNBOUND: return false;
    case KeybindType::KEYCODE:
        triggered = _SDLToSyn(event.key) == std::get<Keycode>(next());
        break;
    case KeybindType::SCANCODE:
        triggered = _SDLToSyn(event.scancode) == std::get<Scancode>(next());
        break;
    case KeybindType::SHORTCUT:
        triggered = std::get<KeyShortcut>(next())._isTriggeredByEvent(event);
        break;
    default: triggered = false;
    }

    if (!triggered) this->reset();
    return triggered;
}

constexpr bool
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
}

constexpr bool
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
                Logger::Log("InputAction '" + action->identifier +
                            "' triggered");
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
                Logger::Log("InputAction '" + action->identifier +
                            "' triggered");
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

// MARK: Temporary mouse movement and scroll event handling

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
