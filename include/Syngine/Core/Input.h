// ╒════════════════════════════ Input.h ═╕
// │ Syngine                              │
// │ Created 2025-07-28                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#ifndef SynInput_h
#define SynInput_h

#include "Syngine/Core/Core.h"
#include <cstdint>
#include <variant>
#include <string>
#include <unordered_set>
#include <vector>
#include <functional>

namespace Syngine {

/// @brief An actively processed keybind grouped with a label
/// @section Input
class InputBinding {
  public:
    /// @brief A type representing the lack of a binding
    class Unbound {};

    /// @brief A bindable non-modifier keyboard key
    enum class KeyboardKey {
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
        QUOTE,
        EXCLAMATION,
        QUESTION_MARK,

        // Whitespace and editing keys
        SPACE,
        TAB,
        ENTER,
        BACKSPACE,

        // Miscellaneous symbol keys
        MINUS,
        EQUALS,
        TILDE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
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
        COMMAND = 1 << 0,
        GUI     = 1 << 0,
        WINDOWS = 1 << 0,
        SHIFT   = 1 << 1,
        OPTION  = 1 << 2,
        ALT     = 1 << 2,
        CONTROL = 1 << 3
    };

    /// @brief A single sided modifier key
    /// @section Input
    /// @see ModifierKey
    enum class SidedModifierKey : uint8_t {
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
    class ModifierKeys {
      private:
        const uint8_t rawValue;

      public:
        ModifierKeys(uint8_t rawValue);
        ModifierKeys(const ModifierKey key);

        /// @brief Returns true if the two ModifierKeys operators are equal
        /// @param rhs The ModifierKeys instance to compare against
        /// @return True when both instances are equal, false otherwise
        constexpr bool operator==(const ModifierKey& rhs);

        /// @brief Returns the ModifierKeys instance equivilant to the right
        /// ModifierKey instance added to the left ModifierKeys instance
        /// @param rhs The ModifierKey instance to combine
        /// @return A ModifierKeys instance containing the logical OR of all the
        /// contained modifiers of both sides
        constexpr InputBinding::ModifierKeys
        operator+(const InputBinding::ModifierKey& rhs);

        /// @brief Returns the ModifierKeys instance equivilant to the
        /// combination of both ModifierKeys instances
        /// @param rhs The ModifierKeys instance to combine
        /// @return A ModifierKeys instance containing the logical OR of all the
        /// contained modifiers of both sides
        constexpr InputBinding::ModifierKeys
        operator+(const InputBinding::ModifierKeys& rhs);
    };

    /// @brief A keyboard shortcut containing of a ``KeyboardKey`` and a set of
    /// ``ModifierKeys``
    /// @section Input
    class KeyboardShortcut {
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
    };

    /// @brief A type that can represent any keyboard input binding
    using KeyboardBindable = std::variant<Unbound,
                                          KeyboardKey,
                                          KeyboardShortcut,
                                          ModifierKey,
                                          SidedModifierKey>;

    /// @brief A chord of multiple keybinds
    class InputChord {
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
    };

    enum class MouseKey {
        LEFT,
        RIGHT,
        MIDDLE,
        BUTTON_4,
        BUTTON_5,
    };

    constexpr InputBinding operator==(const InputBinding& rhs);

    /// @brief Constructs an empty InputBinding
    InputBinding();

    /// @brief Constructs an InputBinding from a KeyboardKey
    InputBinding(KeyboardKey key);

    /// @brief Constructs an InputBinding from a ModifierKey
    InputBinding(ModifierKey modifier);

    /// @brief Constructs an InputBinding from a SidedModifierKey
    InputBinding(SidedModifierKey modifier);

    /// @brief Constructs an InputBinding from a KeyboardShortcut
    InputBinding(KeyboardShortcut shortcut);

    /// @brief Constructs an InputBinding from an InputChord
    InputBinding(InputChord chord);

    /// @brief Constructs an InputBinding from a MouseKey
    InputBinding(MouseKey mouseKey);

    /// @brief Returns true if the binding is currently being pressed
    bool isPressed();

    /// @brief Returns true if the binding was pressed since the previous frame
    bool wasPressed();

    /// @brief Returns true if the binding was released since the last frame
    bool wasReleased();

    /// @brief Returns true if the binding's state has changed since the last
    /// frame
    bool stateChanged();

    /// @brief Whether this action is enabled or not
    bool enabled = true;

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

    /// @brief An array of all input bindings, for updating state and handling
    /// events
    static std::vector<InputBinding*> allBindings;

  public:
    /// @brief Handle input events
    static void HandleEvent();
};

/// @brief Returns the ModifierKeys instance equivilant to the combination of
/// both ModifierKey instances
/// @param lhs A ModifierKey instance to combine
/// @param rhs A ModifierKey instance to combine
/// @return A ModifierKeys instance containing the logical OR of all contained
/// modifiers in both sides
/// @section Input
constexpr InputBinding::ModifierKeys
operator+(const InputBinding::ModifierKey& lhs,
          const InputBinding::ModifierKey& rhs);

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
    InputBinding binding;

    /// @brief Make an empty InputAction
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @throws std::invalid_argument if the identifier is not unique
    InputAction(const std::string& identifier, const std::string& name);

    /// @brief Make an empty InputAction
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param category The category for the input action. This is used for
    /// organizing actions in the bindings menu
    /// @throws std::invalid_argument if the identifier is not unique
    InputAction(const std::string& identifier,
                const std::string& name,
                const std::string& category);

    /// @brief Make an InputAction with a binding
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param binding The binding for the input action
    /// @throws std::invalid_argument if the identifier is not unqiue
    InputAction(const std::string& identifier,
                const std::string& name,
                InputBinding       binding);

    /// @brief Make an InputAction with a binding
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param category The category for the input action. This is used for
    /// organizing actions in the bindings menu
    /// @param binding The binding for the input action
    /// @throws std::invalid_argument if the identifier is not unique
    InputAction(const std::string& identifier,
                const std::string& name,
                const std::string& category,
                InputBinding       binding);

    /// @brief Deconstructs and cleans up the InputAction
    ~InputAction();

    /// @brief Returns true if the binding is currently being pressed
    bool isPressed() { return binding.isPressed(); };

    /// @brief Returns true if the binding was pressed since the previous frame
    bool wasPressed() { return binding.wasPressed(); };

    /// @brief Returns true if the binding was released since the last frame
    bool wasReleased() { return binding.wasReleased(); };

    /// @brief Returns true if the binding's state has changed since the last
    /// frame
    bool stateChanged() { return binding.stateChanged(); };

    void onPressed(std::function<void()> callback) {
        callbacks.onPressed = callback;
    }

    void onReleased(std::function<void()> callback) {
        callbacks.onReleased = callback;
    }

    void onStateChanged(std::function<void()> callback) {
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
    static void RegisterAction(const std::string& identifier,
                               const std::string& name,
                               InputBinding       binding,
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
    /// @returns False if the identifier was not unique, true otherwise
    static bool RegisterAction(const std::string& identifier,
                               const std::string& name,
                               const std::string& category,
                               InputBinding       binding,
                               Callbacks          callbacks);

    /// @brief Equality operator for InputAction
    constexpr bool operator==(const InputAction& other);

  private:
    /// @brief This object's callbacks, if any
    Callbacks callbacks;

    /// @brief An array of pointers to every active input action. Each pointer
    /// is guaranteed to point to an existing object and be unique
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
