//
//  SynInput.h
//  bakerman
//
//  Created by programmer137 on 7/28/25.
//

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
		A, B, C // etc
	};
	
	/// @brief A single non-directional modifier key
	/// @section Input
	/// @see ModifierKeys
	enum class ModifierKey: uint8_t {
		command = 1 << 0, gui = 1 << 0, windows = 1 << 0,
		shift   = 1 << 1,
		option  = 1 << 2, alt = 1 << 2,
		control = 1 << 3
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
		
		/// @brief Returns the ModifierKeys instance equivilant to the right ModifierKey instance added to the left ModifierKeys instance
		/// @param rhs The ModifierKey instance to combine
		/// @return A ModifierKeys instance containing the logical OR of all the contained modifiers of both sides
		constexpr InputBinding::ModifierKeys operator+(const InputBinding::ModifierKey& rhs);
		
		/// @brief Returns the ModifierKeys instance equivilant to the combination of both ModifierKeys instances
		/// @param rhs The ModifierKeys instance to combine
		/// @return A ModifierKeys instance containing the logical OR of all the contained modifiers of both sides
		constexpr InputBinding::ModifierKeys operator+(const InputBinding::ModifierKeys& rhs);
	};
	
	// TODO: Fill in KeyboardShortcut
	/// @brief A keyboard shortcut containing of a ``KeyboardKey`` and a set of ``ModifierKeys``
	/// @section Input
	class KeyboardShortcut {
	private:
		/// @brief The bound key
		const KeyboardKey key;
		
		/// @brief The bound modifiers
		const ModifierKeys modifiers;
		
	public:
		/// @brief Initialize a KeyboardShortcut from a key and a set of modifiers
		KeyboardShortcut(KeyboardKey key, ModifierKeys modifiers);
		
		/// @brief Initialize a KeyboardShortcut from a key, with no modifiers set
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
	std::variant<KeyboardKey, ModifierKeys, KeyboardShortcut, InputChord> binding;
};

/// @brief Returns the ModifierKeys instance equivilant to the combination of both ModifierKey instances
/// @param lhs A ModifierKey instance to combine
/// @param rhs A ModifierKey instance to combine
/// @return A ModifierKeys instance containing the logical OR of all the contained modifiers of both sides
/// @section Input
constexpr InputBinding::ModifierKeys operator+(const InputBinding::ModifierKey& lhs, const InputBinding::ModifierKey& rhs);

class InputAction {
public:
	InputBinding binding;
	std::string name;
	
	bool isPressed();
	bool wasPressed();
	bool wasReleased();
	
	constexpr InputAction operator==(const InputAction& rhs);
	
private:
	static std::unordered_set<InputAction> bindings;
};

};

namespace std {

template<>
struct hash<Syngine::InputAction> {
	size_t operator()(const Syngine::InputAction& obj) const;
};

template<>
struct hash<Syngine::InputBinding> {
	size_t operator()(const Syngine::InputBinding& obj) const;
};

};

#endif /* SynInput_h */


