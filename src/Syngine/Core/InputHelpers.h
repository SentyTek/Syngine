// ╒═════════════════════ InputHelpers.h ═╕
// │ Syngine                              │
// │ Created 2025-08-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯
// Keycode and scancode definitions

#ifndef SynInputHelpers_h
#define SynInputHelpers_h

#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_keycode.h"

#include <cstdint>

namespace Syngine {

/// @brief A type representing the lack of a binding
struct Unbound {
    /// @brief Always returns true, since every instance of Unbound is the
    /// same
    constexpr bool operator==(const Unbound& other) const { return true; }
};

/// @brief A physical keyboard key. This is mapped to the physical position of
/// the key, so SynScancode::W will correspond to the W key on an ANSI keyboard
/// but Z on a French keyboard
/// @note The names of scancodes are based on their position on an EN-US ANSI
/// keyboard
enum class SynScancode : uint32_t {
    // This is a mirror of SDL_Scancode with keys ommitted as needed and
    // convenience scancodes included

    UNKNOWN = SDL_SCANCODE_UNKNOWN,

    A = SDL_SCANCODE_A,
    B = SDL_SCANCODE_B,
    C = SDL_SCANCODE_C,
    D = SDL_SCANCODE_D,
    E = SDL_SCANCODE_E,
    F = SDL_SCANCODE_F,
    G = SDL_SCANCODE_G,
    H = SDL_SCANCODE_H,
    I = SDL_SCANCODE_I,
    J = SDL_SCANCODE_J,
    K = SDL_SCANCODE_K,
    L = SDL_SCANCODE_L,
    M = SDL_SCANCODE_M,
    N = SDL_SCANCODE_N,
    O = SDL_SCANCODE_O,
    P = SDL_SCANCODE_P,
    Q = SDL_SCANCODE_Q,
    R = SDL_SCANCODE_R,
    S = SDL_SCANCODE_S,
    T = SDL_SCANCODE_T,
    U = SDL_SCANCODE_U,
    V = SDL_SCANCODE_V,
    W = SDL_SCANCODE_W,
    X = SDL_SCANCODE_X,
    Y = SDL_SCANCODE_Y,
    Z = SDL_SCANCODE_Z,

    _1 = SDL_SCANCODE_1,
    _2 = SDL_SCANCODE_2,
    _3 = SDL_SCANCODE_3,
    _4 = SDL_SCANCODE_4,
    _5 = SDL_SCANCODE_5,
    _6 = SDL_SCANCODE_6,
    _7 = SDL_SCANCODE_7,
    _8 = SDL_SCANCODE_8,
    _9 = SDL_SCANCODE_9,
    _0 = SDL_SCANCODE_0,

    ENTER     = SDL_SCANCODE_RETURN,
    ESCAPE    = SDL_SCANCODE_ESCAPE,
    BACKSPACE = SDL_SCANCODE_BACKSPACE,
    TAB       = SDL_SCANCODE_TAB,
    SPACE     = SDL_SCANCODE_SPACE,

    // NON_US_HASH (between BACKSLASH and SEMICOLON) intentionally
    // omitted
    MINUS         = SDL_SCANCODE_MINUS,
    EQUALS        = SDL_SCANCODE_EQUALS,
    LEFT_BRACKET  = SDL_SCANCODE_LEFTBRACKET,
    RIGHT_BRACKET = SDL_SCANCODE_RIGHTBRACKET,
    BACKSLASH     = SDL_SCANCODE_BACKSLASH,
    SEMICOLON     = SDL_SCANCODE_SEMICOLON,
    APOSTROPHE    = SDL_SCANCODE_APOSTROPHE,
    GRAVE         = SDL_SCANCODE_GRAVE,
    COMMA         = SDL_SCANCODE_COMMA,
    PERIOD        = SDL_SCANCODE_PERIOD,
    SLASH         = SDL_SCANCODE_SLASH,

    CAPSLOCK = SDL_SCANCODE_CAPSLOCK,

    F1  = SDL_SCANCODE_F1,
    F2  = SDL_SCANCODE_F2,
    F3  = SDL_SCANCODE_F3,
    F4  = SDL_SCANCODE_F4,
    F5  = SDL_SCANCODE_F5,
    F6  = SDL_SCANCODE_F6,
    F7  = SDL_SCANCODE_F7,
    F8  = SDL_SCANCODE_F8,
    F9  = SDL_SCANCODE_F9,
    F10 = SDL_SCANCODE_F10,
    F11 = SDL_SCANCODE_F11,
    F12 = SDL_SCANCODE_F12,

    // INSERT can be generated when Help is pressed on some Mac keyboards
    PRINT_SCREEN = SDL_SCANCODE_PRINTSCREEN,
    SCROLL_LOCK  = SDL_SCANCODE_SCROLLLOCK,
    PAUSE        = SDL_SCANCODE_PAUSE,
    INSERT       = SDL_SCANCODE_INSERT,

    HOME      = SDL_SCANCODE_HOME,
    PAGE_UP   = SDL_SCANCODE_PAGEUP,
    DELETE    = SDL_SCANCODE_DELETE,
    END       = SDL_SCANCODE_END,
    PAGE_DOWN = SDL_SCANCODE_PAGEDOWN,
    RIGHT     = SDL_SCANCODE_RIGHT,
    LEFT      = SDL_SCANCODE_LEFT,
    DOWN      = SDL_SCANCODE_DOWN,
    UP        = SDL_SCANCODE_UP,

    NUM_LOCK = SDL_SCANCODE_NUMLOCKCLEAR,
    CLEAR    = SDL_SCANCODE_NUMLOCKCLEAR, // Convenience scancode

    KEYPAD_DIVIDE   = SDL_SCANCODE_KP_DIVIDE,
    KEYPAD_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY,
    KEYPAD_MINUS    = SDL_SCANCODE_KP_MINUS,
    KEYPAD_PLUS     = SDL_SCANCODE_KP_PLUS,
    KEYPAD_ENTER    = SDL_SCANCODE_KP_ENTER,
    KEYPAD_1        = SDL_SCANCODE_KP_1,
    KEYPAD_2        = SDL_SCANCODE_KP_2,
    KEYPAD_3        = SDL_SCANCODE_KP_3,
    KEYPAD_4        = SDL_SCANCODE_KP_4,
    KEYPAD_5        = SDL_SCANCODE_KP_5,
    KEYPAD_6        = SDL_SCANCODE_KP_6,
    KEYPAD_7        = SDL_SCANCODE_KP_7,
    KEYPAD_8        = SDL_SCANCODE_KP_8,
    KEYPAD_9        = SDL_SCANCODE_KP_9,
    KEYPAD_PERIOD   = SDL_SCANCODE_KP_PERIOD,

    // POWER (after compose) intentionally ommitted
    NON_US_BACKSLASH = SDL_SCANCODE_NONUSBACKSLASH,
    APPLICATION      = SDL_SCANCODE_APPLICATION,
    COMPOSE          = SDL_SCANCODE_APPLICATION, // Convenience scancode

    // F13 through VOLUME_DOWN and KEYPAD_EQUALS_AS_400 intentionally ommitted
    KEYPAD_EQUALS = SDL_SCANCODE_KP_EQUALS,
    KEYPAD_COMMA  = SDL_SCANCODE_KP_COMMA,

    // INTERNATIONAL_1 through KEYPAD_HEXADECIMAL intentionally ommitted

    LEFT_CONTROL = SDL_SCANCODE_LCTRL,
    LEFT_SHIFT   = SDL_SCANCODE_LSHIFT,
    LEFT_ALT     = SDL_SCANCODE_LALT,
    LEFT_OPTION  = SDL_SCANCODE_LALT, // Convenience scancode
    LEFT_GUI     = SDL_SCANCODE_LGUI,
    LEFT_COMMAND = SDL_SCANCODE_LGUI, // Convenience scancode
    LEFT_WINDOWS = SDL_SCANCODE_LGUI, // Convenience scancode

    RIGHT_CONTROL = SDL_SCANCODE_RCTRL,
    RIGHT_SHIFT   = SDL_SCANCODE_RSHIFT,
    RIGHT_ALT     = SDL_SCANCODE_RALT,
    RIGHT_OPTION  = SDL_SCANCODE_RALT, // Convenience scancode
    RIGHT_GUI     = SDL_SCANCODE_RGUI,
    RIGHT_COMMAND = SDL_SCANCODE_RGUI, // Convenience scancode
    RIGHT_WINDOWS = SDL_SCANCODE_RGUI, // Convenience scancode

    // MODE through END_CALL intentionally ommitted

    // No need for an equivilant to SDL_SCANCODE_RESERVED or SDL_SCANCODE_COUNT
};

/// @brief A virtual keyboard key. This is mapped to the meaning of the key, so
/// SynKeycode::I will correspond to I on any keyboard
enum class SynKeycode : uint32_t {
    // This is a mirror of SDL_Keycode, with keys ommitted as needed and
    // convenience scancodes included

    // No need for an equivalent to SDLK_EXTENDED_MASK, SDLK_SCANCODE_MASK, or
    // SDLK_SCANCODE_TO_KEYCODE(X)

    UNKNOWN = SDLK_UNKNOWN,

    ENTER     = SDLK_RETURN,
    ESCAPE    = SDLK_ESCAPE,
    BACKSPACE = SDLK_BACKSPACE,
    TAB       = SDLK_TAB,
    SPACE     = SDLK_SPACE,

    EXCLAMATION_MARK  = SDLK_EXCLAIM,
    QUOTATION_MARK    = SDLK_DBLAPOSTROPHE,
    POUND_SIGN        = SDLK_HASH,
    DOLLAR_SIGN       = SDLK_DOLLAR,
    AMPERSAND         = SDLK_AMPERSAND,
    APOSTROPHE        = SDLK_APOSTROPHE,
    LEFT_PARENTHASIS  = SDLK_LEFTPAREN,
    RIGHT_PARENTHASIS = SDLK_RIGHTPAREN,
    ASTERISK          = SDLK_ASTERISK,
    PLUS              = SDLK_PLUS,
    COMMA             = SDLK_COMMA,
    MINUS             = SDLK_MINUS,
    PERIOD            = SDLK_PERIOD,
    SLASH             = SDLK_SLASH,

    _0 = SDLK_0,
    _1 = SDLK_1,
    _2 = SDLK_2,
    _3 = SDLK_3,
    _4 = SDLK_4,
    _5 = SDLK_5,
    _6 = SDLK_6,
    _7 = SDLK_7,
    _8 = SDLK_8,
    _9 = SDLK_9,

    COLON         = SDLK_COLON,
    SEMICOLON     = SDLK_SEMICOLON,
    LESS_THAN     = SDLK_LESS,
    EQUALS        = SDLK_EQUALS,
    GREATER_THAN  = SDLK_GREATER,
    QUESTION_MARK = SDLK_QUESTION,
    AT_SIGN       = SDLK_AT,
    LEFT_BRACKET  = SDLK_LEFTBRACKET,
    BACKSLASH     = SDLK_BACKSLASH,
    RIGHT_BRACKET = SDLK_RIGHTBRACKET,
    CARET         = SDLK_CARET,
    UNDERSCORE    = SDLK_UNDERSCORE,
    GRAVE         = SDLK_GRAVE,

    A = SDLK_A,
    B = SDLK_B,
    C = SDLK_C,
    D = SDLK_D,
    E = SDLK_E,
    F = SDLK_F,
    G = SDLK_G,
    H = SDLK_H,
    I = SDLK_I,
    J = SDLK_J,
    K = SDLK_K,
    L = SDLK_L,
    M = SDLK_M,
    N = SDLK_N,
    O = SDLK_O,
    P = SDLK_P,
    Q = SDLK_Q,
    R = SDLK_R,
    S = SDLK_S,
    T = SDLK_T,
    U = SDLK_U,
    V = SDLK_V,
    W = SDLK_W,
    X = SDLK_X,
    Y = SDLK_Y,
    Z = SDLK_Z,

    LEFT_CURLY_BRACKET  = SDLK_LEFTBRACE,
    PIPE                = SDLK_PIPE,
    RIGHT_CURLY_BRACKET = SDLK_RIGHTBRACE,
    TILDE               = SDLK_TILDE,
    DELETE              = SDLK_DELETE,
    PLUS_MINUS          = SDLK_PLUSMINUS,
    CAPS_LOCK           = SDLK_CAPSLOCK,

    F1  = SDLK_F1,
    F2  = SDLK_F2,
    F3  = SDLK_F3,
    F4  = SDLK_F4,
    F5  = SDLK_F5,
    F6  = SDLK_F6,
    F7  = SDLK_F7,
    F8  = SDLK_F8,
    F9  = SDLK_F9,
    F10 = SDLK_F10,
    F11 = SDLK_F11,
    F12 = SDLK_F12,

    PRINT_SCREEN = SDLK_PRINTSCREEN,
    SCROLL_LOCK  = SDLK_SCROLLLOCK,
    PAUSE        = SDLK_PAUSE,
    INSERT       = SDLK_INSERT,
    HOME         = SDLK_HOME,
    PAGE_UP      = SDLK_PAGEUP,
    END          = SDLK_END,
    PAGE_DOWN    = SDLK_PAGEDOWN,

    RIGHT = SDLK_RIGHT,
    LEFT  = SDLK_LEFT,
    DOWN  = SDLK_DOWN,
    UP    = SDLK_UP,

    NUM_LOCK = SDLK_NUMLOCKCLEAR,
    CLEAR    = SDLK_NUMLOCKCLEAR, // Convenience keycode

    // POWER (between KEYPAD_PERIOD and KEYPAD_EQUALS) intentionally omitted
    KEYPAD_DIVIDE   = SDLK_KP_DIVIDE,
    KEYPAD_MULTIPLY = SDLK_KP_MULTIPLY,
    KEYPAD_MINUS    = SDLK_KP_MINUS,
    KEYPAD_PLUS     = SDLK_KP_PLUS,
    KEYPAD_ENTER    = SDLK_KP_ENTER,
    KEYPAD_0        = SDLK_KP_0,
    KEYPAD_1        = SDLK_KP_1,
    KEYPAD_2        = SDLK_KP_2,
    KEYPAD_3        = SDLK_KP_3,
    KEYPAD_4        = SDLK_KP_4,
    KEYPAD_5        = SDLK_KP_5,
    KEYPAD_6        = SDLK_KP_6,
    KEYPAD_7        = SDLK_KP_7,
    KEYPAD_8        = SDLK_KP_8,
    KEYPAD_9        = SDLK_KP_9,
    KEYPAD_PERIOD   = SDLK_KP_PERIOD,
    KEYPAD_EQUALS   = SDLK_KP_EQUALS,

    F13 = SDLK_F13,
    F14 = SDLK_F14,
    F15 = SDLK_F15,
    F16 = SDLK_F16,
    F17 = SDLK_F17,
    F18 = SDLK_F18,
    F19 = SDLK_F19,
    F20 = SDLK_F20,
    F21 = SDLK_F21,
    F22 = SDLK_F22,
    F23 = SDLK_F23,
    F24 = SDLK_F24,

    // EXECUTE through KP_HEXADECIMAL intentionally omitted

    LEFT_CONTROL = SDLK_LCTRL,
    LEFT_SHIFT   = SDLK_LSHIFT,
    LEFT_ALT     = SDLK_LALT,
    LEFT_OPTION  = SDLK_LALT, // Convenience keycode
    LEFT_GUI     = SDLK_LGUI,
    LEFT_COMMAND = SDLK_LGUI, // Convenience keycode
    LEFT_WINDOWS = SDLK_LGUI, // Convenience keycode

    RIGHT_CONTROL = SDLK_RCTRL,
    RIGHT_SHIFT   = SDLK_RSHIFT,
    RIGHT_ALT     = SDLK_RALT,
    RIGHT_OPTION  = SDLK_RALT, // Convenience keycode
    RIGHT_GUI     = SDLK_RGUI,
    RIGHT_COMMAND = SDLK_RGUI, // Convenience keycode
    RIGHT_WINDOWS = SDLK_RGUI, // Convenience keycode

    // MODE through RIGHT_HYPER intentionally omitted
};

/// @brief A virtual modifier key
struct SynKeymod {
    // This is a mirror of SDL_Keymod, with modifiers ommitted as needed and
    // convenience modifiers included

  private:
    uint16_t _rawValue;

    SynKeymod(uint16_t rawValue) : _rawValue(rawValue) {}

  public:
    // LEVEL_5_SHIFT, NUM_LOCK, CAPS_LOCK, and SCROLL_LOCK intentionally omitted
    static constexpr uint16_t NONE          = SDL_KMOD_NONE;
    static constexpr uint16_t LEFT_SHIFT    = SDL_KMOD_LSHIFT;
    static constexpr uint16_t RIGHT_SHIFT   = SDL_KMOD_RSHIFT;
    static constexpr uint16_t LEFT_CONTROL  = SDL_KMOD_LCTRL; // Convenience
    static constexpr uint16_t RIGHT_CONTROL = SDL_KMOD_RCTRL; // Convenience
    static constexpr uint16_t LEFT_ALT      = SDL_KMOD_LALT;
    static constexpr uint16_t LEFT_OPTION   = SDL_KMOD_LALT; // Convenience
    static constexpr uint16_t RIGHT_ALT     = SDL_KMOD_RALT;
    static constexpr uint16_t RIGHT_OPTION  = SDL_KMOD_RALT; // Convenience
    static constexpr uint16_t LEFT_GUI      = SDL_KMOD_LGUI;
    static constexpr uint16_t LEFT_COMMAND  = SDL_KMOD_LGUI; // Convenience
    static constexpr uint16_t LEFT_WINDOWS  = SDL_KMOD_LGUI; // Convenience
    static constexpr uint16_t RIGHT_GUI     = SDL_KMOD_RGUI;
    static constexpr uint16_t RIGHT_COMMAND = SDL_KMOD_RGUI; // Convenience
    static constexpr uint16_t RIGHT_WINDOWS = SDL_KMOD_RGUI; // Convenience
    static constexpr uint16_t MODE          = SDL_KMOD_MODE;
    static constexpr uint16_t CONTROL       = SDL_KMOD_CTRL;
    static constexpr uint16_t SHIFT         = SDL_KMOD_SHIFT;
    static constexpr uint16_t ALT           = SDL_KMOD_ALT;
    static constexpr uint16_t OPTION        = SDL_KMOD_ALT; // Convenience
    static constexpr uint16_t GUI           = SDL_KMOD_GUI;
    static constexpr uint16_t COMMAND       = SDL_KMOD_GUI; // Convenience
    static constexpr uint16_t WINDOWS       = SDL_KMOD_GUI; // Convenience

    SynKeymod operator+(const SynKeymod& other) const {
        return SynKeymod(this->_rawValue | other._rawValue);
    }

    SynKeymod operator-(const SynKeymod& other) const {
        return SynKeymod(this->_rawValue & ~other._rawValue);
    }

    SynKeymod operator+=(const SynKeymod& other) {
        this->_rawValue |= other._rawValue;
        return *this;
    }

    SynKeymod operator-=(const SynKeymod& other) {
        this->_rawValue &= ~other._rawValue;
        return *this;
    }

    bool operator==(const SynKeymod& other) const {
        return this->_rawValue == other._rawValue;
    }
};

} /* namespace Syngine */

#endif /* SynInputHelpers_h */
