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

class InputBinding {
	// TODO: Fill in KeyboardKey
	enum class KeyboardKey {
		A, B, C // etc
	};
	
	enum class ModifierKey: uint8_t {
		command = 1 << 0, gui = 1 << 0, windows = 1 << 0,
		shift   = 1 << 1,
		option  = 1 << 2, alt = 1 << 2,
		control = 1 << 3
	};
	
	class ModifierKeys {
	private:
		const uint8_t rawValue;
		
	public:
		ModifierKeys(uint8_t rawValue);
		ModifierKeys(ModifierKey key);
		// these will all take constexpr arrays
		ModifierKeys(std::array<ModifierKey, 2>);
		ModifierKeys(std::array<ModifierKey, 3>);
		ModifierKeys(std::array<ModifierKey, 4>);
		
		constexpr ModifierKeys operator==(const ModifierKey& rhs);
	};
	
	// TODO: Fill in KeyShortcut
	class KeyShortcut {
	public:
		KeyShortcut();
	};
	
	// TODO: Fill in InputChord
	class InputChord {
	public:
		InputChord();
	};

	constexpr InputBinding operator==(const InputBinding& rhs);
	
	InputBinding(KeyboardKey key);
	InputBinding(KeyShortcut shortcut);
	InputBinding(InputChord chord);
	
	std::variant<KeyboardKey, ModifierKeys, KeyShortcut, InputChord> binding;
};

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


