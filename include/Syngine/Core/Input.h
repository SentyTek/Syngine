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
#include <unordered_set>
#include <vector>

namespace Syngine {

/// @brief An actively processed keybind grouped with a label
/// @section Input
class InputBinding {
  public:
    /// @brief A type representing the lack of a binding
    struct Unbound {};

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

        // Control keys
        INSERT,
        HOME,
        PAGE_UP,
        PAGE_DOWN,
        END,
        PRINT_SCREEN,
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
    using KeyboardBindable = std::
        variant<KeyboardKey, KeyboardShortcut, ModifierKey, SidedModifierKey, Unbound>;

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

    constexpr InputBinding operator==(const InputBinding& rhs);

    /// @brief Constructs an empty InputBinding
    InputBinding();

    /// @brief Constructs an InputBinding from a KeyboardKey
    InputBinding(KeyboardKey key);

    /// @brief Constructs an InputBinding from a KeyboardShortcut
    InputBinding(KeyboardShortcut shortcut);

    /// @brief Constructs an InputBinding from an InputChord
    InputBinding(InputChord chord);

    /// @brief Returns true if the binding is currently being pressed
    bool isPressed();

    /// @brief Returns true if the binding was pressed since the previous frame
    bool wasPressed();

    /// @brief Returns true if the binding was released since the last frame
    bool wasReleased();

    /// @brief Returns true if the binding's state has changed since the last
    /// frame
    bool changedState();

  private:
    // Intentionally not using KeyboardBindable so we don't have nested
    // `std::variant`s
    /// @brief This object's binding
    std::variant<KeyboardKey,
                 ModifierKey,
                 SidedModifierKey,
                 KeyboardShortcut,
                 InputChord,
                 Unbound>
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
    /// @brief The binding for this input action
    InputBinding binding;

    /// @brief The display name for this input action
    std::string name;

    /// @brief The display category for this input action
    std::string category;

    /// @brief The identifier for this input action
    const std::string identifier;

    /// @brief Make an empty InputAction
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @note Returns `false` if the identifier isn't unique
    InputAction(const std::string& identifier, const std::string& name);

    /// @brief Make an empty InputAction
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param category The category for the input action. This is used for
    /// organizing actions in the bindings menu
    /// @note Returns `false` if the identifier isn't unique
    InputAction(const std::string& identifier,
                const std::string& name,
                const std::string& category);

    /// @brief Make an InputAction with a binding
    /// @param identifier The identifier for the input action. This is required
    /// and *must* be unique
    /// @param name The name for the input action. This is the display name that
    /// is shown in the bindings menu
    /// @param binding The binding for the input action
    /// @note Returns `false` if the identifier isn't unique
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
    /// @note Returns `false` if the identifier isn't unique
    InputAction(const std::string& identifier,
                const std::string& name,
                const std::string& category,
                InputBinding       binding);

    /// @brief Returns true if the binding is currently being pressed
    bool isPressed() { return binding.isPressed(); };

    /// @brief Returns true if the binding was pressed since the previous frame
    bool wasPressed() { return binding.wasPressed(); };

    /// @brief Returns true if the binding was released since the last frame
    bool wasReleased() { return binding.wasReleased(); };

    /// @brief Returns true if the binding's state has changed since the last
    /// frame
    bool changedState() { return binding.changedState(); };

    /// @brief Equality operator for InputAction
    constexpr InputAction operator==(const InputAction& rhs);

  private:
    /// @brief The set of all input actions, for tracking and displaying
    static std::unordered_set<InputAction> bindings;
};

}; // namespace Syngine

namespace std {

template <> struct hash<Syngine::InputAction> {
    /// @brief Hash function for InputAction
    size_t operator()(const Syngine::InputAction& obj) const;
};

template <> struct hash<Syngine::InputBinding> {
    /// @brief Hash function for InputBinding
    size_t operator()(const Syngine::InputBinding& obj) const;
};

}; // namespace std

#endif /* SynInput_h */
