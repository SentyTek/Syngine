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

class InputAction {
public:
	enum class KeyboardKey {
		A, B, C // etc
	};
	
	class ModifierKeys {
	private:
		const uint8_t rawValue;
		
	public:
		ModifierKeys(uint8_t rawValue);
		
		static const ModifierKeys command; // 1 << 0
		static const ModifierKeys shift;   // 1 << 1
		static const ModifierKeys option;  // 1 << 2
		static const ModifierKeys control; // 1 << 3
		
		ModifierKeys operator+();
	};
	
	class KeyShortcut {
	public:
		KeyShortcut();
	};
	
	class InputChord {
	public:
		InputChord();
	};
	
	using InputBinding = std::variant<KeyboardKey, ModifierKeys, KeyShortcut, InputChord>;
	
public:
	InputBinding binding;
	std::string name;
	
	bool isPressed();
	bool wasPressed();
	bool wasReleased();
	
private:
	static std::unordered_set<InputAction> bindings;
};

};

#endif /* SynInput_h */
