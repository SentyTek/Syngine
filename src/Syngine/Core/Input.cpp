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

// Initialize InputAction's static members

std::vector<Syngine::InputAction*> Syngine::InputAction::_Bindings;
std::vector<Syngine::InputAction>  Syngine::InputAction::_AnonymousActions;

// InputAction private helpers and operators

void Syngine::InputAction::EnsureUniqueIdentifier(
    const std::string& identifier) {
    for (Syngine::InputAction* binding : Syngine::InputAction::_Bindings) {
        if (binding->identifier == identifier) {
            Syngine::Logger::Log("InputAction identifier '" + identifier +
                                     "' is not unique",
                                 LogLevel::FATAL);
        }
    }
}

// InputAction constructors

Syngine::InputAction::InputAction(const std::string&  identifier,
                                  const std::string&  name,
                                  const std::string&  category,
                                  Syngine::KeyBinding binding,
                                  Callbacks           callbacks)
    : identifier(identifier), name(name), category(category), binding(binding),
      currentState(false), previousState(false), callbacks(callbacks) {
    Logger::Log("Creating InputAction '" + identifier + "'");
    Syngine::InputAction::EnsureUniqueIdentifier(identifier);
    Syngine::InputAction::_Bindings.push_back(this);
}

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
    Syngine::InputAction::_Bindings.erase(
        std::remove(Syngine::InputAction::_Bindings.begin(),
                    Syngine::InputAction::_Bindings.end(),
                    this),
        Syngine::InputAction::_Bindings.end());
}

void Syngine::InputAction::RegisterAction(const std::string&  identifier,
                                          const std::string&  name,
                                          Syngine::KeyBinding binding,
                                          Callbacks           callbacks) {
    Syngine::InputAction::_AnonymousActions.emplace_back(
        identifier, name, "", binding, callbacks);
}

void Syngine::InputAction::RegisterAction(const std::string&  identifier,
                                          const std::string&  name,
                                          const std::string&  category,
                                          Syngine::KeyBinding binding,
                                          Callbacks           callbacks) {
    Syngine::InputAction::_AnonymousActions.emplace_back(
        Syngine::InputAction(identifier, name, category, binding, callbacks));
}

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
    case KeybindType::SHORTCUT:
        return std::get<KeyShortcut>(this->binding)._isTriggeredByEvent(event);
    case KeybindType::SEQUENCE:
        return std::get<KeySequence>(this->binding)._isTriggeredByEvent(event);
    }
}

void Syngine::InputAction::_HandleEvent(SDL_Event event) {
    bool down = false;

    switch (event.type) {
    case SDL_EVENT_KEY_DOWN: down = true;
    case SDL_EVENT_KEY_UP:
        for (InputAction* action : InputAction::_Bindings) {
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
    case SDL_EVENT_MOUSE_BUTTON_UP: break;

    default: break;
    }
}
