// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Input.h"
#include "Syngine/Core/Logger.h"

Syngine::InputBinding::InputBinding() { this->binding = Unbound{}; }

Syngine::InputAction::InputAction(const std::string& identifier,
                                  const std::string& name)
    : name(name), category(""),
      binding() {
        if (identifier.empty()) {
            Logger::LogF(Syngine::LogLevel::ERR,
                         "InputAction identifier must be unique");
            this = false;
        }
      };
