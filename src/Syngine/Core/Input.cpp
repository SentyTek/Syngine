// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Input.h"
#include "SDL3/SDL_events.h"
#include "InputHelpers.h"
#include "Syngine/Core/Logger.h"

// KeyBinding constructors

Syngine::KeyBinding::KeyBinding() : binding(Unbound{}) {}

Syngine::KeyBinding::KeyBinding(Syngine::KeyBinding::KeyboardKey key)
    : binding(key) {}

Syngine::KeyBinding::KeyBinding(Syngine::KeyBinding::ModifierKey modifier)
    : binding(modifier) {}

Syngine::KeyBinding::KeyBinding(Syngine::KeyBinding::SidedModifierKey modifier)
    : binding(modifier) {}

Syngine::KeyBinding::KeyBinding(Syngine::KeyBinding::KeyboardShortcut shortcut)
    : binding(shortcut) {}

Syngine::KeyBinding::KeyBinding(Syngine::KeyBinding::InputChord chord)
    : binding(chord) {}

Syngine::KeyBinding::KeyBinding(Syngine::KeyBinding::MouseKey mouseKey)
    : binding(mouseKey) {}

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
    Logger::Info("Removing InputAction '" + this->identifier + "'");
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

void Syngine::InputAction::_HandleEvent(SDL_Event event) {
    Syngine::KeyBinding called = _InputHelpers::_InputEventToKeyBinding(event);
    bool                down;

    if (called == Syngine::KeyBinding()) {
        Logger::Log("Event contains no convertible binding");
        return; // No binding, nothing to do
    }

    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_DOWN: down = true; break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_KEY_UP: down = false; break;
    
    default:
        Logger::Warn("Input event handler called with unimplemented event");
        return; // Not a relevant event
    }

    // TODO: Get this to handle chords properly
    for (InputAction* action : InputAction::_Bindings) {

        if (action->binding == called) {
            action->previousState = action->currentState;
            action->currentState  = down;

            Logger::Info(action->identifier + " has been called");

            if (down) {
                Logger::Info(action->identifier + " has been pressed");
                action->callbacks.onPressed();
            } else {
                Logger::Info(action->identifier + " has been released");
                action->callbacks.onReleased();
            }
            action->callbacks.onStateChanged();
        }
    }
}
