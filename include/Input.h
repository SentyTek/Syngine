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
#include <unordered_map>

namespace Syngine {

/// @brief An actively processed keybind grouped with a label
/// @section Input
class InputBinding {
  public:
    // TODO: Fill in KeyboardKey
    ///
    enum class KeyboardKey {
        A,
        B,
        C // etc
    };

    /// @brief A single unsided modifier key
    /// @section Input`@param` and `@return`
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

    // TODO: Fill in KeyboardShortcut
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

        bool isPressed();
        bool wasPressed();
        bool wasReleased();
    };

    // TODO: Fill in InputChord
    class InputChord {
      public:
        InputChord();
    };

    constexpr InputBinding operator==(const InputBinding& rhs);

    InputBinding(KeyboardKey key);
    InputBinding(KeyboardShortcut shortcut);
    InputBinding(InputChord chord);

  private:
    std::variant<KeyboardKey,
                 ModifierKey,
                 SidedModifierKey,
                 KeyboardShortcut,
                 InputChord>
        binding;
};

/// @brief Returns the ModifierKeys instance equivilant to the combination of
/// both ModifierKey instances
/// @param lhs A ModifierKey instance to combine
/// @param rhs A ModifierKey instance to combine
/// @return A ModifierKeys instance containing the logical OR of all the
/// contained modifiers of both sides
/// @section Input
constexpr InputBinding::ModifierKeys
operator+(const InputBinding::ModifierKey& lhs,
          const InputBinding::ModifierKey& rhs);

class InputAction {
  public:
    InputBinding binding;
    std::string  name;

    bool isPressed();
    bool wasPressed();
    bool wasReleased();

    constexpr InputAction operator==(const InputAction& rhs);

  private:
    static std::unordered_set<InputAction> bindings;
};

}; // namespace Syngine

namespace std {

template <> struct hash<Syngine::InputAction> {
    size_t operator()(const Syngine::InputAction& obj) const;
};

template <> struct hash<Syngine::InputBinding> {
    size_t operator()(const Syngine::InputBinding& obj) const;
};

}; // namespace std

#endif /* SynInput_h */
