// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-28                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#ifndef SynInput_h
#define SynInput_h

#include "../src/Syngine/Core/InputHelpers.h"

#include "SDL3/SDL_events.h"

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <sys/types.h>
#include <variant>
#include <string>
#include <vector>
#include <functional>

namespace Syngine {

enum class KeybindType : size_t {
    UNBOUND  = 0,
    KEYCODE  = 1,
    SCANCODE = 2,
    SHORTCUT = 3,
    SEQUENCE = 4
};

using KeyUnbound = std::monostate;

struct KeyShortcut {
  private:
    std::variant<Keycode, Scancode> key;
    Keymod                          modifiers;

  public:
    KeyShortcut() = delete;

    constexpr KeyShortcut(std::variant<Keycode, Scancode> key)
        : key(key), modifiers() {}

    constexpr KeyShortcut(std::variant<Keycode, Scancode> key, Keymod modifiers)
        : key(key), modifiers(modifiers) {}

    constexpr KeybindType type() const { return KeybindType::SHORTCUT; }

    constexpr KeybindType subType() const {
        return static_cast<KeybindType>(key.index() + 1);
    }

    constexpr bool operator==(const KeyShortcut& other) const {
        return key == other.key && modifiers == other.modifiers;
    }

    constexpr bool _isTriggeredByEvent(SDL_KeyboardEvent event);
};

struct KeySequence {
  private:
    std::vector<std::variant<Keycode, Scancode, KeyShortcut>> bound;
    uint32_t                                                  nextIndex;

  public:
    KeySequence() = delete;

    constexpr KeySequence(std::variant<Keycode, Scancode, KeyShortcut> key)
        : bound{ key }, nextIndex(0) {
        bound.shrink_to_fit();
    }

    constexpr KeySequence(
        std::initializer_list<std::variant<Keycode, Scancode, KeyShortcut>>
            list)
        : bound(list), nextIndex(0) {
        bound.shrink_to_fit();
    }

    constexpr KeybindType subType(uint32_t index) const {
        if (index < bound.size()) {
            return static_cast<KeybindType>(bound[index].index() + 1);
        } else {
            return KeybindType::UNBOUND;
        }
    }

    constexpr void reset() { nextIndex = 0; }

    constexpr std::variant<KeyUnbound, Keycode, Scancode, KeyShortcut> next() {
        if (bound.empty()) Logger::Fatal("KeySequence cannot be empty");

        std::variant<Keycode, Scancode, KeyShortcut> nextKey = bound[nextIndex];

        nextIndex = (nextIndex + 1) % bound.size();

        switch (subType(nextIndex)) {
        case KeybindType::UNBOUND: return KeyUnbound(); break;
        case KeybindType::KEYCODE: return std::get<Keycode>(nextKey); break;
        case KeybindType::SCANCODE: return std::get<Scancode>(nextKey); break;
        case KeybindType::SHORTCUT:
            return std::get<KeyShortcut>(nextKey);
            break;
        default: return KeyUnbound(); // Will probably never be called
        }
    }

    constexpr bool _isTriggeredByEvent(SDL_KeyboardEvent event);

    constexpr bool operator==(const KeySequence& other) const {
        return bound == other.bound;
    }
};

/// @brief Any key binding
/// @section Input
struct KeyBinding {
  private:
    std::variant<KeyUnbound, Keycode, Scancode, KeyShortcut, KeySequence>
        binding;

  public:
    constexpr KeyBinding() : binding(KeyUnbound()) {}

    constexpr KeyBinding(Keycode keycode) : binding(keycode) {}

    constexpr KeyBinding(Scancode scancode) : binding(scancode) {}

    constexpr KeyBinding(KeyShortcut shortcut) : binding(shortcut) {}

    constexpr KeyBinding(KeySequence sequence) : binding(sequence) {}

    constexpr KeybindType subType() const {
        return static_cast<KeybindType>(binding.index());
    }

    constexpr bool operator==(const KeyBinding& other) const {
        return binding == other.binding;
    }

    constexpr bool _isTriggeredByEvent(SDL_KeyboardEvent event);
};

/// @brief A bound input action
/// @note InputActions are *not* anonymous and *must* have a unique identifier,
/// which cannot be changed
/// @section Input
class InputAction {
  public:
    /// @brief A container for callbacks
    struct Callbacks {

        std::function<void()> onPressed      = []() -> void {};
        std::function<void()> onReleased     = []() -> void {};
        std::function<void()> onStateChanged = []() -> void {};
    };

    /// @brief The identifier for this input action
    const std::string identifier;

    /// @brief The display name for this input action
    std::string name;

    /// @brief The display category for this input action
    std::string category;

    /// @brief The binding for this input action
    Syngine::KeyBinding binding;

    /// @brief Make an empty InputAction
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    InputAction(const std::string& identifier, const std::string& name);

    /// @brief Make an empty InputAction
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param category The category for the input action. This is used for
    /// organizing actions in the bindings menu
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    InputAction(const std::string& identifier,
                const std::string& name,
                const std::string& category);

    /// @brief Make an InputAction with a binding
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param binding The binding for the input action
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    InputAction(const std::string& identifier,
                const std::string& name,
                KeyBinding         binding);

    /// @brief Make an InputAction with a binding
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param category The category for the input action. This is used for
    /// organizing actions in the bindings menu
    /// @param binding The binding for the input action
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    InputAction(const std::string& identifier,
                const std::string& name,
                const std::string& category,
                KeyBinding         binding);

    InputAction(const std::string&  identifier,
                const std::string&  name,
                const std::string&  category,
                Syngine::KeyBinding binding,
                Callbacks           callbacks);

    /// @note InputActions are non-copyable because of their unique identifiers
    InputAction(const InputAction& other) = delete;

    InputAction(InputAction&& other) = default;

    /// @brief Deconstructs and cleans up the InputAction
    ~InputAction();

    /// @brief Returns true if the binding is currently being pressed
    bool isPressed();

    /// @brief Returns true if the binding was pressed since the previous frame
    bool wasPressed();

    /// @brief Returns true if the binding was released since the last frame
    bool wasReleased();

    /// @brief Returns true if the binding's state has changed since the last
    /// frame
    bool stateChanged();

    void setOnPressedCallback(std::function<void()> callback) {
        callbacks.onPressed = callback;
    }

    void setOnReleasedCallback(std::function<void()> callback) {
        callbacks.onReleased = callback;
    }

    void setOnStateChangedCallback(std::function<void()> callback) {
        callbacks.onStateChanged = callback;
    }

    /// @brief Registers an action which calls the provided callback(s) when the
    /// input state changes
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param binding The binding for the input action
    /// @param callbacks The callbacks to call when the action state updates
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    static void RegisterAction(const std::string& identifier,
                               const std::string& name,
                               KeyBinding         binding,
                               Callbacks          callbacks);

    /// @brief Registers an action which calls the provided callback when the
    /// action is pressed
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param category The category for the input action. This is used for
    /// organizing actions in the bindings menu
    /// @param binding The binding for the input action
    /// @param callbacks The callbacks to call when the action state updates
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    static void RegisterAction(const std::string& identifier,
                               const std::string& name,
                               const std::string& category,
                               KeyBinding         binding,
                               Callbacks          callbacks);

    /// @brief Equality operator for InputAction
    /// @note Does not check for full equality, only for equal bindings, as
    /// InputActions are guaranteed to be unique
    constexpr bool operator==(const InputAction& other) const {
        return binding == other.binding;
    };

    /// @brief Handle input events. This should *not* be called outside of
    /// Core::HandleEvents()
    /// @param event The SDL event to handle
    /// @internal
    /// @see Core::HandleEvents()
    static void _HandleEvent(SDL_Event event);

  private:
    /// @brief This object's callbacks, if any
    Callbacks callbacks;

    /// @brief The state of this action in this frame
    bool currentState;
    /// @brief The state of this action in the last frame
    bool previousState;

    /// @brief An array of pointers to every active input action for tracking
    /// and updating. Each pointer is guaranteed to point to an existing object
    /// and be unique
    static std::vector<InputAction*> _Bindings;

    /// @brief This is where input actions go if they're not handled as objects
    /// directly
    static std::vector<InputAction> _AnonymousActions;

    /// @brief Logs a fatal error and halts the program if the provided
    /// identifier is not unique
    static void EnsureUniqueIdentifier(const std::string& identifier);
};

}; // namespace Syngine

#endif /* SynInput_h */
