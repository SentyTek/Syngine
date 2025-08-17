// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-28                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#ifndef SynInput_h
#define SynInput_h

#include <cstdint>
#include <variant>
#include <string>
#include <vector>
#include <functional>

#include "SDL3/SDL_events.h"

namespace Syngine {

/// @brief Any key binding
/// @section Input
struct KeyBinding {
  public:
    /// @brief A type representing the lack of a binding
    struct Unbound {
        /// @brief Always returns true, since every instance of Unbound is the
        /// same
        constexpr bool operator==(const Unbound& other) const { return true; }
    };

    /// @brief A bindable non-modifier keyboard key
    enum class KeyboardKey {
        UNKNOWN,
        // Letters
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        // Numbers
        _0,
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        _9,

        // Function keys
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,

        // Punctuation keys
        COMMA,
        PERIOD,
        SEMICOLON,
        APOSTROPHE,

        // Whitespace and editing keys
        SPACE,
        TAB,
        ENTER,
        BACKSPACE,
        DELETE,

        // Symbol keys
        MINUS,
        EQUALS,
        TILDE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        FORWARD_SLASH,
        BACKSLASH,

        // Arrow keys
        LEFT,
        RIGHT,
        UP,
        DOWN,

        // Control keys
        INSERT,
        HOME,
        PAGE_UP,
        PAGE_DOWN,
        END,
        PRINT_SCREEN,
        ESCAPE,
    };

    /// @brief A single unsided modifier key
    /// @section Input
    /// @see ModifierKeys
    /// @see SidedModifierKey
    enum class ModifierKey : uint8_t {
        UNKNOWN = 0,

        COMMAND = 1 << 1,
        GUI     = 1 << 1,
        WINDOWS = 1 << 1,

        SHIFT = 1 << 2,

        OPTION = 1 << 3,
        ALT    = 1 << 3,

        CONTROL = 1 << 4
    };

    /// @brief A single sided modifier key
    /// @section Input
    /// @see ModifierKey
    enum class SidedModifierKey : uint8_t {
        UNKNOWN = 0,

        LEFT_COMMAND  = 1,
        LEFT_GUI      = 1,
        LEFT_WINDOWS  = 1,
        RIGHT_COMMAND = 2,
        RIGHT_GUI     = 2,
        RIGHT_WINDOWS = 2,

        LEFT_SHIFT  = 3,
        RIGHT_SHIFT = 4,

        LEFT_OPTION  = 5,
        LEFT_ALT     = 5,
        RIGHT_OPTION = 6,
        RIGHT_ALT    = 6,

        LEFT_CONTROL  = 7,
        RIGHT_CONTROL = 8
    };

    /// @brief A set of ``ModifierKey``s, mainly used in keyboard shortcuts
    /// @section Input
    /// @see ModifierKey
    struct ModifierKeys {
      private:
        const uint8_t rawValue;

      public:
        constexpr ModifierKeys(uint8_t rawValue) : rawValue(rawValue) {};
        constexpr ModifierKeys(const ModifierKey key)
            : rawValue(static_cast<uint8_t>(key)) {};

        /// @brief Returns true if the two ModifierKeys operators are equal
        /// @param rhs The ModifierKeys instance to compare against
        /// @return True when both instances are equal, false otherwise
        constexpr bool operator==(const ModifierKeys& other) const {
            return rawValue == other.rawValue;
        };

        constexpr bool operator==(const ModifierKey& other) const {
            return (rawValue & static_cast<uint8_t>(other)) != 0;
        }

        /// @brief Returns the ModifierKeys instance equivilant to the right
        /// ModifierKey instance added to the left ModifierKeys instance
        /// @param rhs The ModifierKey instance to combine
        /// @return A ModifierKeys instance containing the logical OR of all the
        /// contained modifiers of both sides
        constexpr KeyBinding::ModifierKeys
        operator+(const KeyBinding::ModifierKey& other) const {
            return KeyBinding::ModifierKeys(rawValue |
                                            static_cast<uint8_t>(other));
        }

        /// @brief Returns the ModifierKeys instance equivilant to the
        /// combination of both ModifierKeys instances
        /// @param rhs The ModifierKeys instance to combine
        /// @return A ModifierKeys instance containing the logical OR of all the
        /// contained modifiers of both sides
        constexpr KeyBinding::ModifierKeys
        operator+(const KeyBinding::ModifierKeys& other) const {
            return KeyBinding::ModifierKeys(rawValue | other.rawValue);
        }
    };

    /// @brief A keyboard shortcut containing of a ``KeyboardKey`` and a set of
    /// ``ModifierKeys``
    /// @section Input
    struct KeyboardShortcut {
      private:
        /// @brief The bound key
        const KeyboardKey key;

        /// @brief The bound modifiers
        const ModifierKeys modifiers;

      public:
        /// @brief Initialize a KeyboardShortcut from a key and a set of
        /// modifiers
        KeyboardShortcut(KeyboardKey key, ModifierKeys modifiers);

        /// @brief Initialize a KeyboardShortcut from a key, with no modifiers
        /// set
        KeyboardShortcut(KeyboardKey key);

        /// @brief Returns true if the shortcut is unbound
        constexpr bool operator==(const KeyboardShortcut& other) const {
            return (key == other.key) && (modifiers == other.modifiers);
        }
    };

    /// @brief A type that can represent any keyboard input binding
    using KeyboardBindable = std::variant<Unbound,
                                          KeyboardKey,
                                          KeyboardShortcut,
                                          ModifierKey,
                                          SidedModifierKey>;

    /// @brief A chord of multiple keybinds
    struct InputChord {
      private:
        /// @brief The bound keybinds
        std::vector<KeyboardBindable> keys;

      public:
        /// @brief Initialize an empty InputChord
        InputChord() = default;

        /// @brief Initialize an InputChord from a single KeyboardBindable
        /// @param key The key to add
        InputChord(KeyboardBindable key);

        /// @brief Initialize an InputChord from an array of KeyboardBindable
        /// @param keys The keys to add
        InputChord(const std::vector<KeyboardBindable>& keys);

        /// @brief Returns true if the chord is unbound
        constexpr bool operator==(const InputChord& other) const {
            return keys == other.keys;
        }
    };

    enum class MouseKey {
        UNKNOWN,
        LEFT,
        RIGHT,
        MIDDLE,
        BUTTON_4,
        BUTTON_5,
    };

    constexpr bool operator==(const KeyBinding& other) const {
        return binding == other.binding;
    };

    /// @brief Constructs an empty KeyBinding
    KeyBinding();

    /// @brief Constructs a KeyBinding from a KeyboardKey
    KeyBinding(KeyboardKey key);

    /// @brief Constructs a KeyBinding from a ModifierKey
    KeyBinding(ModifierKey modifier);

    /// @brief Constructs a KeyBinding from a SidedModifierKey
    KeyBinding(SidedModifierKey modifier);

    /// @brief Constructs a KeyBinding from a KeyboardShortcut
    KeyBinding(KeyboardShortcut shortcut);

    /// @brief Constructs a KeyBinding from an InputChord
    KeyBinding(InputChord chord);

    /// @brief Constructs a KeyBinding from a MouseKey
    KeyBinding(MouseKey mouseKey);

  private:
    // Intentionally not using KeyboardBindable so we don't have nested
    // `std::variant`s
    /// @brief This object's binding
    std::variant<Unbound,
                 KeyboardKey,
                 ModifierKey,
                 SidedModifierKey,
                 KeyboardShortcut,
                 InputChord,
                 MouseKey>
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
        std::function<void()> onPressed;
        std::function<void()> onReleased;
        std::function<void()> onStateChanged;
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
    /// @note Does not check for full equality, only for equal bindings
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
    static std::vector<InputAction*> bindings;

    /// @brief This is where input actions go if they're not handled as objects
    /// directly
    static std::vector<InputAction> anonymousActions;

    /// @brief Logs a fatal error and halts the program if the provided
    /// identifier is not unique
    static void EnsureUniqueIdentifier(const std::string& identifier);
};

}; // namespace Syngine

#endif /* SynInput_h */
