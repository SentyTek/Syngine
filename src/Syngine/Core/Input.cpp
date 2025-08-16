// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Input.h"
#include "Syngine/Core/Logger.h"

// InputBinding constructors

Syngine::InputBinding::InputBinding() : binding(Unbound{}) {}

Syngine::InputBinding::InputBinding(Syngine::InputBinding::KeyboardKey key)
    : binding(key) {}

Syngine::InputBinding::InputBinding(Syngine::InputBinding::ModifierKey modifier)
    : binding(modifier) {}

Syngine::InputBinding::InputBinding(
    Syngine::InputBinding::SidedModifierKey modifier)
    : binding(modifier) {}

Syngine::InputBinding::InputBinding(
    Syngine::InputBinding::KeyboardShortcut shortcut)
    : binding(shortcut) {}

Syngine::InputBinding::InputBinding(Syngine::InputBinding::InputChord chord)
    : binding(chord) {}


// InputAction private helpers and operators

constexpr bool
Syngine::InputAction::operator==(const Syngine::InputAction& other) {
    return identifier == other.identifier;
}

void Syngine::InputAction::EnsureUniqueIdentifier(
    const std::string& identifier) {
    for (Syngine::InputAction* binding : Syngine::InputAction::bindings) {
        if (binding->identifier == identifier) {
            Syngine::Logger::Log("InputAction identifier '" + identifier +
                                     "' is not unique",
                                 LogLevel::FATAL);
        }
    }
}


// InputAction constructors

Syngine::InputAction::InputAction(const std::string& identifier,
                                  const std::string& name)
    : identifier(identifier), name(name), category(), binding() {
    Syngine::InputAction::EnsureUniqueIdentifier(identifier);
    Syngine::InputAction::bindings.push_back(this);
}

Syngine::InputAction::InputAction(const std::string& identifier,
                                  const std::string& name,
                                  const std::string& category)
    : identifier(identifier), name(name), category(category), binding() {
    Syngine::InputAction::EnsureUniqueIdentifier(identifier);
    Syngine::InputAction::bindings.push_back(this);
}

Syngine::InputAction::InputAction(const std::string&    identifier,
                                  const std::string&    name,
                                  Syngine::InputBinding binding)
    : identifier(identifier), name(name), category(), binding(binding) {
    Syngine::InputAction::EnsureUniqueIdentifier(identifier);
    Syngine::InputAction::bindings.push_back(this);
}

Syngine::InputAction::InputAction(const std::string&    identifier,
                                  const std::string&    name,
                                  const std::string&    category,
                                  Syngine::InputBinding binding)
    : identifier(identifier), name(name), category(category), binding(binding) {
    Syngine::InputAction::EnsureUniqueIdentifier(identifier);
    Syngine::InputAction::bindings.push_back(this);
}

Syngine::InputAction::~InputAction() {
    Syngine::InputAction::bindings.erase(
        std::remove(Syngine::InputAction::bindings.begin(),
                    Syngine::InputAction::bindings.end(),
                    this),
        Syngine::InputAction::bindings.end());
}
