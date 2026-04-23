// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-28                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#ifndef SynInput_h
#define SynInput_h

// pretty ugly include statement but it's necessary
#include "../src/Syngine/Core/InputHelpers.h"

#include "SDL3/SDL_events.h"

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <sys/types.h>
#include <variant>
#include <string>
#include <vector>
#include <list>
#include <functional>
#include <unordered_set>

namespace Syngine {

/// @brief A tag enum representing each of the keybindable types
/// @since 0.0.1
/// @section Input
enum class KeybindType : size_t {
    UNBOUND  = 0, //* A keybind that is not bound to any key
    KEYCODE  = 1, //* A keybind that is bound to a specific keycode
    SCANCODE = 2, //* A keybind that is bound to a specific scancode
    SHORTCUT = 3, //* A keybind that is bound to a keycode or scancode and a set
                  // of modifiers
    SEQUENCE = 4, //* A keybind that is bound to a sequence of keycodes,
                  // scancodes, and/or shortcuts
    MOUSE_BUTTON = 5 //* A keybind that is bound to a mouse button
};

/// @brief A type representing an unbound action, or the lack of a key binding
/// @since 0.0.1
/// @section Input
using KeyUnbound = std::monostate;

// forward declarations for `AnyKeybind`
struct KeyShortcut;
struct KeySequence;

/// @brief A type representing any keybind
/// @since 0.0.1
/// @section Input
using AnyKeybind = std::variant<KeyUnbound,
                                Keycode,
                                Scancode,
                                KeyShortcut,
                                KeySequence,
                                MouseButton>;

/// @brief A type representing a keyboard shortcut; a single keycode or scancode
/// and any number of modifiers
/// @since 0.0.1
/// @section Input
struct KeyShortcut {
  private:
    /// @brief The stored key
    /// @since 0.0.1
    /// @internal

    std::variant<Keycode, Scancode> key;
    /// @brief The stored modifiers
    /// @since 0.0.1
    /// @internal
    Keymod modifiers;

  public:
    /// @brief `KeyShortcut`s aren't default-constructable
    /// @since 0.0.1
    /// @note Use a ``KeyBinding`` holding a ``KeyUnknown`` (the default
    /// constructor of `KeyBinding`) instead
    KeyShortcut() = delete;

    /// @brief Create a ``KeyShortcut`` with no modifiers
    /// @param key The key to bind
    /// @since 0.0.1
    KeyShortcut(std::variant<Keycode, Scancode> key)
        : key(key), modifiers() {}

    /// @brief Create a ``KeyShortcut`` with the given modifiers
    /// @param key The key to bind
    /// @param modifiers The modifiers to bind
    /// @since 0.0.1
    KeyShortcut(std::variant<Keycode, Scancode> key, Keymod modifiers)
        : key(key), modifiers(modifiers) {}

    /// @brief Returns the ``KeybindType`` corresponding to the type this object
    /// contains, if any
    /// @returns The contained subtype as a ``KeybindType``
    /// @since 0.0.1
    KeybindType subType() const {
        return static_cast<KeybindType>(key.index() + 1);
    }

    /// @brief Equality operator for ``KeyShortcut``
    /// @param other The ``KeyShortcut`` to compare against
    /// @returns `true` if the objects are equal, `false` otherwise
    /// @since 0.0.1
    bool operator==(const KeyShortcut& other) const {
        return key == other.key && modifiers == other.modifiers;
    }

    /// @brief Checks if the keyboard event triggers this binding
    /// @param event The event to check against
    /// @returns `true` if the event triggers the binding, `false` otherwise
    /// @since 0.0.1
    /// @section Input
    /// @internal
    bool _isTriggeredByEvent(SDL_KeyboardEvent event) const;
};

/// @brief An array of key bindings which is triggered when pressed in sequence
/// @since 0.0.1
/// @section Input
struct KeySequence {
  private:
    /// @brief The stored bindings
    /// @since 0.0.1
    /// @internal
    std::vector<std::variant<Keycode, Scancode, KeyShortcut>> bound;

    /// @brief The index of the next binding that hasn't been triggered
    /// @since 0.0.1
    /// @internal
    uint32_t nextIndex;

  public:
    /// @brief `KeySequence`s aren't default-constructable and must hold at
    /// least one binding
    /// @since 0.0.1
    /// @note Use a ``KeyBinding`` holding a ``KeyUnknown`` (the default
    /// constructor of `KeyBinding`) instead
    KeySequence() = delete;

    /// @brief Create a ``KeySequence`` with a single binding. This isn't very
    /// useful on it's own and may be removed
    /// @param key The binding to bind
    /// @since 0.0.1
    KeySequence(std::variant<Keycode, Scancode, KeyShortcut> key)
        : bound{ key }, nextIndex(0) {
        bound.shrink_to_fit();
    }

    /// @brief Create a ``KeySequence`` with a list of bindings
    /// @param list The list of bindings to bind
    /// @since 0.0.1
    KeySequence(
        std::initializer_list<std::variant<Keycode, Scancode, KeyShortcut>>
            list)
        : bound(list), nextIndex(0) {
        bound.shrink_to_fit();
    }

    /// @brief Returns the ``KeybindType`` corresponding to the type this object
    /// contains at the given index, or ``KeybindType::UNBOUND`` if the index is
    /// out of bounds
    /// @param index The index to find the type of
    /// @returns The contained subtype at the given index as a ``KeybindType``
    /// @since 0.0.1
    KeybindType subType(uint32_t index) const {
        if (index < bound.size()) {
            return static_cast<KeybindType>(bound[index].index() + 1);
        } else {
            return KeybindType::UNBOUND;
        }
    }

    /// @brief Resets the trigger progress of the sequence
    /// @note You probably shouldn't call this unless you really need to, and it
    /// might be removed
    /// @since 0.0.1
    void reset() { nextIndex = 0; }

    /// @brief Gets the next key to be triggered
    /// @returns The key that must be triggered for the sequence to progress
    /// @note You probably shouldn't call this unless you really need to, and it
    /// might be removed
    /// @since 0.0.1
    AnyKeybind next() {
        const std::variant<Keycode, Scancode, KeyShortcut> nextKey =
            bound[this->nextIndex];

        switch (subType(this->nextIndex)) {
        case KeybindType::KEYCODE: return std::get<Keycode>(nextKey);
        case KeybindType::SCANCODE: return std::get<Scancode>(nextKey);
        case KeybindType::SHORTCUT:
            return std::get<KeyShortcut>(nextKey);
        default:
            if (bound.empty()) Logger::Error("KeySequence cannot be empty");
            Logger::Fatal("KeySequence::subType() failed");
            return Keycode(); // will never be called; program terminates on
                              // line above
        }
    }

    /// @brief Increments the trigger progress of the sequence, as if the
    /// previous key had been triggered
    /// @note You probably shouldn't call this unless you really need to, and it
    /// might be removed
    /// @since 0.0.1
    void increment() {
        this->nextIndex = (nextIndex + 1) % bound.size();
    }

    /// @brief Gets the reset state of the sequence
    /// @returns `true` if the sequence was complete or reset (if the next index
    /// is 0), `false` otherwise
    /// @since 0.0.1
    bool wasReset() const { return nextIndex == 0; }

    /// @brief Determines if the keybind is triggered, increments the progress
    /// if it was, and resets the sequence if it wasn't
    /// @param event The ``SDL_KeyboardEvent`` to check the sequence against
    /// @returns `true` if the sequence was just triggered *and* was also reset,
    /// `false` otherwise (even if the sequence was triggered)
    /// @since 0.0.1
    /// @section Input
    /// @internal
    bool _isTriggeredByEvent(SDL_KeyboardEvent event);

    /// @brief Equality operator for ``KeySequence``
    /// @param other The ``KeySequence`` to compare against
    /// @returns `true` if the objects are equal, `false` otherwise
    /// @since 0.0.1
    bool operator==(const KeySequence& other) const {
        return bound == other.bound;
    }
};

/// @brief A container for holding any type of keybind
/// @since 0.0.1
/// @section Input
struct KeyBinding {
  private:
    /// @brief The stored binding
    /// @since 0.0.1
    /// @internal
    AnyKeybind binding;

  public:
    /// @brief Construct a `KeyBinding` containing a ``KeyUnbound``
    /// @since 0.0.1
    KeyBinding() : binding(KeyUnbound()) {};

    /// @brief Construct a `KeyBinding` from ``AnyKeybind``
    /// @param keycode The keycode to bind
    /// @since 0.0.1
    KeyBinding(AnyKeybind bind) : binding(bind) {}

    /// @brief Returns the ``KeybindType`` corresponding to the type this object
    /// contains
    /// @returns The contained subtype as a ``KeybindType``
    /// @since 0.0.1
    KeybindType subType() const {
        return static_cast<KeybindType>(binding.index());
    }

    /// @brief Equality operator for ``KeyBinding``
    /// @param other The ``KeyBinding`` to compare against
    /// @returns `true` if the objects are equal, `false` otherwise
    /// @since 0.0.1
    bool operator==(const KeyBinding& other) const {
        return binding == other.binding;
    }

    /// @brief Determines if the KeyBinding is triggered by a keyboard event
    /// @param event The ``SDL_KeyboardEvent`` to check against
    /// @returns `true` if the contained subtype is triggered by the event,
    /// `false` otherwise
    /// @since 0.0.1
    /// @section Input
    /// @internal
    bool _isTriggeredByEvent(SDL_KeyboardEvent event);

    /// @brief Determines if the KeyBinding is triggered by a keyboard event
    /// @param event The ``SDL_KeyboardEvent`` to check against
    /// @returns `true` if the contained subtype is triggered by the event,
    /// `false` otherwise
    /// @since 0.0.1
    /// @section Input
    /// @internal
    bool _isTriggeredByEvent(SDL_MouseButtonEvent event);
};

/// @brief A bound input action
/// @note InputActions are *not* anonymous and *must* have a unique identifier,
/// which cannot be changed
/// @section Input
class InputAction {
  public:
    /// @brief A container for callbacks
    /// @since 0.0.1
    struct Callbacks {

        /// @brief The callback called when the action is triggered
        /// @since 0.0.1
        std::function<void()> onPressed = []() -> void {};

        /// @brief The callback called when the action is released
        /// @since 0.0.1
        std::function<void()> onReleased = []() -> void {};

        /// @brief The callback called whenever the trigger state of the action
        /// changes
        /// @since 0.0.1
        std::function<void()> onStateChanged = []() -> void {};
    };

    /// @brief The identifier for this input action
    /// @since 0.0.1
    const std::string identifier;

    /// @brief The display name for this input action
    /// @since 0.0.1
    std::string name;

    /// @brief The display category for this input action
    /// @since 0.0.1
    std::string category;

    /// @brief The binding for this input action
    /// @since 0.0.1
    Syngine::KeyBinding binding;

    /// @brief Make an InputAction with a binding and callbacks
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param category The category for the input action. This is used for
    /// organizing actions in the bindings menu
    /// @param binding The binding for the input action
    /// @param callbacks The callbacks to be called when the input action is
    /// triggered
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    InputAction(const std::string&  identifier,
                const std::string&  name,
                const std::string&  category  = "",
                Syngine::KeyBinding binding   = KeyBinding(),
                Callbacks           callbacks = { .onPressed  = []() -> void {},
                                                  .onReleased = []() -> void {},
                                                  .onStateChanged = []() -> void {} });

    /// @brief InputActions are non-copyable because of their unique identifiers
    /// @since 0.0.1
    InputAction(const InputAction& other) = delete;

    /// @brief InputActions are non-copyable because of their unique identifiers
    /// @since 0.0.1
    InputAction& operator=(const InputAction& other) = delete;

    /// @brief For now, InputActions are non-movable to keep the registry system
    /// intact
    /// @since 0.0.1
    InputAction(InputAction&& other) = delete;

    /// @brief For now, InputActions are non-movable to keep the registry system
    /// intact
    /// @since 0.0.1
    InputAction& operator=(InputAction&& other) = delete;

    /// @brief Deconstructs and cleans up the InputAction
    /// @since 0.0.1
    ~InputAction();

    /// @brief Returns true if the binding is currently being pressed
    /// @returns `true` for every frame the binding is triggered, `false`
    /// otherwise
    /// @since 0.0.1
    bool isPressed();

    /// @brief Returns true if the binding was pressed since the previous frame
    /// @returns `true` the first frame the binding is triggered, `false`
    /// otherwise
    /// @since 0.0.1
    bool wasPressed();

    /// @brief Returns true if the binding was released since the last frame
    /// @returns `true` the first frame the binding is released, `false`
    /// otherwise
    /// @since 0.0.1
    bool wasReleased();

    /// @brief Returns true if the binding's state has changed since the last
    /// frame
    /// @returns `true` on every frame where the state is different from the
    /// last frame, `false` otherwise
    /// @since 0.0.1
    bool stateChanged();

    /// @brief Sets the `onPressed` callback
    /// @param callback The callback to set
    /// @since 0.0.1
    void setOnPressedCallback(std::function<void()> callback) {
        callbacks.onPressed = callback;
    }

    /// @brief Sets the `onReleased` callback
    /// @param callback The callback to set
    /// @since 0.0.1
    void setOnReleasedCallback(std::function<void()> callback) {
        callbacks.onReleased = callback;
    }

    /// @brief Sets the `onStateChanged` callback
    /// @param callback The callback to set
    /// @since 0.0.1
    void setOnStateChangedCallback(std::function<void()> callback) {
        callbacks.onStateChanged = callback;
    }

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
    /// @returns A pointer to the registered action
    /// @note Logs a fatal error and crashes the program if the identifier is
    /// not unique
    /// @since 0.0.1
    static InputAction* RegisterAction(const std::string& identifier,
                               const std::string& name,
                               const std::string& category,
                               KeyBinding         binding,
                               Callbacks          callbacks);

    /// @brief Unregister all actions created through RegisterAction whose
    /// identifiers start with a given prefix.
    /// @param prefix Identifier prefix to match.
    /// @returns Number of removed actions.
    /// @since 0.0.1
    static size_t UnregisterActionsByPrefix(const std::string& prefix);

    /// @brief Equality operator for ``InputAction``
    /// @param other The ``InputAction`` to compare against
    /// @returns `true` if the objects are equal, `false` otherwise
    /// @note Does not check for full equality, only for equal bindings, as
    /// InputActions are guaranteed to be unique
    /// @since 0.0.1
    bool operator==(const InputAction& other) const {
        return binding == other.binding;
    };

    /// @brief Handle input events. This should *not* be called outside of
    /// Core::HandleEvents()
    /// @param event The SDL event to handle
    /// @since 0.0.1
    /// @internal
    /// @see Core::HandleEvents()
    static void _HandleEvent(SDL_Event event);

    /// @brief Temporary mouse move event registration
    /// @param callback The callback to call on mouse move
    /// @note This will be removed in favor of a better API at some point
    /// @since 0.0.1
    static void
    RegisterMouseMoveEvent(std::function<void(float, float)> callback);

    /// @brief Temporary mouse scroll event registration
    /// @param callback The callback to call on mouse move
    /// @note This will be removed in favor of a better API at some point
    /// @since 0.0.1
    static void RegisterScrollEvent(std::function<void(float, float)> callback);

  private:
    /// @brief Temporary storage for the mouse move event callback
    /// @note This will be removed in favor of a better API at some point
    /// @since 0.0.1
    static std::function<void(float, float)> _MouseMoveCallback;

    /// @brief Temporary storage for the scroll event callback
    /// @note This will be removed in favor of a better API at some point
    /// @since 0.0.1
    static std::function<void(float, float)> _ScrollCallback;

    /// @brief This object's callbacks, if any
    /// @since 0.0.1
    Callbacks callbacks;

    /// @brief The state of this action in this frame
    /// @since 0.0.1
    bool currentState;

    /// @brief The state of this action in the last frame
    /// @since 0.0.1
    bool previousState;

    /// @brief An array of pointers to every active input action for tracking
    /// and updating. Each pointer is guaranteed to point to an existing object
    /// and be unique
    /// @since 0.0.1
    static std::vector<InputAction*> _Registry;

    /// @brief This is where input actions go if they're not handled as objects
    /// directly
    /// @note Implemented as std::list instead of std::vector because
    /// InputAction does not have a move or copy constructor, so std::vector
    /// would eventually run out of size and not be able to accommodate new
    /// elements. We're not too worried about performance here since we'll
    /// always access actions from the registry and users cannot access this
    /// list
    /// @note This will be removed in favor of a better API at some point
    /// @since 0.0.1
    static std::list<InputAction> _HomelessShelter;

    /// @brief Logs a fatal error and halts the program if the provided
    /// identifier is not unique
    /// @param identifier The identifier to check
    /// @since 0.0.1
    /// @internal
    static void EnsureUniqueIdentifier(const std::string& identifier);
};

/// @brief Converts an identifier to a ``Scancode``
/// @param code The string to convert from
/// @returns The scancode if it succeeded, or ``Scancode::_UNKNOWN`` if it failed
/// @since 0.0.1
Scancode StringToScancode(const std::string& code);

/// @brief Converts a ``Scancode`` to its unique identifier
/// @param code The ``Scancode`` to convert from
/// @returns The identifier if it succeeded, or ``""`` if it failed
/// @since 0.0.1
std::string ScancodeToString(const Scancode& code);

/// @brief Converts an identifier to a ``Keycode``
/// @param key The string to convert from
/// @returns The keycode if it succeeded, or ``Keycode::_UNKNOWN`` if it failed
/// @since 0.0.1
Keycode StringToKeycode(const std::string& key);

/// @brief Converts a ``Keycode`` to its unique identifier
/// @param key The ``Keycode`` to convert from
/// @returns The identifier if it succeeded, or ``""`` if it failed
/// @since 0.0.1
std::string KeycodeToString(const Keycode& key);

std::string MouseButtonToString(const MouseButton& button);

MouseButton StringToMouseButton(const std::string& button);

/// @note The return value doesn't have any duplicates
std::vector<std::string> KeymodToString(const Keymod& keymod);

/// @note This will automatically de-duplicate names
Keymod StringToKeymod(const std::vector<std::string>& keymod);

}; // namespace Syngine

#endif /* SynInput_h */
