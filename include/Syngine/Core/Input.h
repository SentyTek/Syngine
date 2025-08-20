// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-28                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#ifndef SynInput_h
#define SynInput_h

#include "Syngine/Core/InputHelpers.h"

#include "SDL3/SDL_events.h"

#include <variant>
#include <string>
#include <vector>
#include <functional>

namespace Syngine {

/// @brief Any key binding
/// @section Input
struct KeyBinding {

    constexpr bool operator==(const KeyBinding& other) const {
        return binding == other.binding;
    };

    /// @brief Constructs an empty KeyBinding
    KeyBinding();

    /// @brief Constructs a KeyBinding from a SynKeycode
    KeyBinding(SynKeycode key);

    /// @brief Constructs a KeyBinding from a ModifierKey
    KeyBinding(ModifierKey modifier);

    /// @brief Constructs a KeyBinding from a SidedModifierKey
    KeyBinding(SidedModifierKey modifier);

    /// @brief Constructs a KeyBinding from a KeyboardShortcut
    KeyBinding(KeyboardShortcut shortcut);

    /// @brief Constructs a KeyBinding from an InputChord
    KeyBinding(InputChord chord);

  private:
    // Intentionally not using KeyboardBindable so we don't have nested
    // `std::variant`s
    /// @brief This object's binding
    std::variant<Unbound,
                 SynKeycode,
                 ModifierKey,
                 SidedModifierKey,
                 KeyboardShortcut,
                 InputChord>
        binding;
};

/// @brief A bound input action
/// @note InputActions are *not* anonymous and *must* have a unique identifier,
/// which cannot be changed
/// @section Input
class InputAction {
  public:
    /// @brief A container for callbacks
    struct Callbacks {
        // are these lambda expressions valid C++ code on all compilers?
        std::function<void()> onPressed      = [] {};
        std::function<void()> onReleased     = [] {};
        std::function<void()> onStateChanged = [] {};
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
