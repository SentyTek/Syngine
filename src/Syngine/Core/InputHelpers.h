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

#include "Syngine/Core/Logger.h"

#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"

#include <cstdint>
#include <string>

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
enum class Scancode : uint32_t {
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

inline constexpr Scancode _SDLToSyn(const SDL_Scancode& scancode) {
    switch (scancode) {
    case SDL_SCANCODE_NONUSHASH:

    case SDL_SCANCODE_POWER:

    case SDL_SCANCODE_F13:
    case SDL_SCANCODE_F14:
    case SDL_SCANCODE_F15:
    case SDL_SCANCODE_F16:
    case SDL_SCANCODE_F17:
    case SDL_SCANCODE_F18:
    case SDL_SCANCODE_F19:
    case SDL_SCANCODE_F20:
    case SDL_SCANCODE_F21:
    case SDL_SCANCODE_F22:
    case SDL_SCANCODE_F23:
    case SDL_SCANCODE_F24:
    case SDL_SCANCODE_EXECUTE:
    case SDL_SCANCODE_HELP:
    case SDL_SCANCODE_MENU:
    case SDL_SCANCODE_SELECT:
    case SDL_SCANCODE_STOP:
    case SDL_SCANCODE_AGAIN:
    case SDL_SCANCODE_UNDO:
    case SDL_SCANCODE_CUT:
    case SDL_SCANCODE_COPY:
    case SDL_SCANCODE_PASTE:
    case SDL_SCANCODE_FIND:
    case SDL_SCANCODE_MUTE:
    case SDL_SCANCODE_VOLUMEUP:
    case SDL_SCANCODE_VOLUMEDOWN:

    case SDL_SCANCODE_KP_EQUALSAS400:

    case SDL_SCANCODE_INTERNATIONAL1:
    case SDL_SCANCODE_INTERNATIONAL2:
    case SDL_SCANCODE_INTERNATIONAL3:
    case SDL_SCANCODE_INTERNATIONAL4:
    case SDL_SCANCODE_INTERNATIONAL5:
    case SDL_SCANCODE_INTERNATIONAL6:
    case SDL_SCANCODE_INTERNATIONAL7:
    case SDL_SCANCODE_INTERNATIONAL8:
    case SDL_SCANCODE_INTERNATIONAL9:
    case SDL_SCANCODE_LANG1:
    case SDL_SCANCODE_LANG2:
    case SDL_SCANCODE_LANG3:
    case SDL_SCANCODE_LANG4:
    case SDL_SCANCODE_LANG5:
    case SDL_SCANCODE_LANG6:
    case SDL_SCANCODE_LANG7:
    case SDL_SCANCODE_LANG8:
    case SDL_SCANCODE_LANG9:

    case SDL_SCANCODE_ALTERASE:
    case SDL_SCANCODE_SYSREQ:
    case SDL_SCANCODE_CANCEL:
    case SDL_SCANCODE_CLEAR:
    case SDL_SCANCODE_PRIOR:
    case SDL_SCANCODE_RETURN2:
    case SDL_SCANCODE_SEPARATOR:
    case SDL_SCANCODE_OUT:
    case SDL_SCANCODE_OPER:
    case SDL_SCANCODE_CLEARAGAIN:
    case SDL_SCANCODE_CRSEL:
    case SDL_SCANCODE_EXSEL:

    case SDL_SCANCODE_KP_00:
    case SDL_SCANCODE_KP_000:
    case SDL_SCANCODE_THOUSANDSSEPARATOR:
    case SDL_SCANCODE_DECIMALSEPARATOR:
    case SDL_SCANCODE_CURRENCYUNIT:
    case SDL_SCANCODE_CURRENCYSUBUNIT:
    case SDL_SCANCODE_KP_LEFTPAREN:
    case SDL_SCANCODE_KP_RIGHTPAREN:
    case SDL_SCANCODE_KP_LEFTBRACE:
    case SDL_SCANCODE_KP_RIGHTBRACE:
    case SDL_SCANCODE_KP_TAB:
    case SDL_SCANCODE_KP_BACKSPACE:
    case SDL_SCANCODE_KP_A:
    case SDL_SCANCODE_KP_B:
    case SDL_SCANCODE_KP_C:
    case SDL_SCANCODE_KP_D:
    case SDL_SCANCODE_KP_E:
    case SDL_SCANCODE_KP_F:
    case SDL_SCANCODE_KP_XOR:
    case SDL_SCANCODE_KP_POWER:
    case SDL_SCANCODE_KP_PERCENT:
    case SDL_SCANCODE_KP_LESS:
    case SDL_SCANCODE_KP_GREATER:
    case SDL_SCANCODE_KP_AMPERSAND:
    case SDL_SCANCODE_KP_DBLAMPERSAND:
    case SDL_SCANCODE_KP_VERTICALBAR:
    case SDL_SCANCODE_KP_DBLVERTICALBAR:
    case SDL_SCANCODE_KP_COLON:
    case SDL_SCANCODE_KP_HASH:
    case SDL_SCANCODE_KP_SPACE:
    case SDL_SCANCODE_KP_AT:
    case SDL_SCANCODE_KP_EXCLAM:
    case SDL_SCANCODE_KP_MEMSTORE:
    case SDL_SCANCODE_KP_MEMRECALL:
    case SDL_SCANCODE_KP_MEMCLEAR:
    case SDL_SCANCODE_KP_MEMADD:
    case SDL_SCANCODE_KP_MEMSUBTRACT:
    case SDL_SCANCODE_KP_MEMMULTIPLY:
    case SDL_SCANCODE_KP_MEMDIVIDE:
    case SDL_SCANCODE_KP_PLUSMINUS:
    case SDL_SCANCODE_KP_CLEAR:
    case SDL_SCANCODE_KP_CLEARENTRY:
    case SDL_SCANCODE_KP_BINARY:
    case SDL_SCANCODE_KP_OCTAL:
    case SDL_SCANCODE_KP_DECIMAL:
    case SDL_SCANCODE_KP_HEXADECIMAL:

    case SDL_SCANCODE_SLEEP:
    case SDL_SCANCODE_WAKE:

    case SDL_SCANCODE_CHANNEL_INCREMENT:
    case SDL_SCANCODE_CHANNEL_DECREMENT:

    case SDL_SCANCODE_MEDIA_PLAY:
    case SDL_SCANCODE_MEDIA_PAUSE:
    case SDL_SCANCODE_MEDIA_RECORD:
    case SDL_SCANCODE_MEDIA_FAST_FORWARD:
    case SDL_SCANCODE_MEDIA_REWIND:
    case SDL_SCANCODE_MEDIA_NEXT_TRACK:
    case SDL_SCANCODE_MEDIA_PREVIOUS_TRACK:
    case SDL_SCANCODE_MEDIA_STOP:
    case SDL_SCANCODE_MEDIA_EJECT:
    case SDL_SCANCODE_MEDIA_PLAY_PAUSE:
    case SDL_SCANCODE_MEDIA_SELECT:

    case SDL_SCANCODE_AC_NEW:
    case SDL_SCANCODE_AC_OPEN:
    case SDL_SCANCODE_AC_CLOSE:
    case SDL_SCANCODE_AC_EXIT:
    case SDL_SCANCODE_AC_SAVE:
    case SDL_SCANCODE_AC_PRINT:
    case SDL_SCANCODE_AC_PROPERTIES:

    case SDL_SCANCODE_AC_SEARCH:
    case SDL_SCANCODE_AC_HOME:
    case SDL_SCANCODE_AC_BACK:
    case SDL_SCANCODE_AC_FORWARD:
    case SDL_SCANCODE_AC_STOP:
    case SDL_SCANCODE_AC_REFRESH:
    case SDL_SCANCODE_AC_BOOKMARKS:

    case SDL_SCANCODE_SOFTLEFT:
    case SDL_SCANCODE_SOFTRIGHT:
    case SDL_SCANCODE_CALL:
    case SDL_SCANCODE_ENDCALL:

    case SDL_SCANCODE_RESERVED:
    case SDL_SCANCODE_COUNT:

        Logger::Warn("Untranslatable scancode: " +
                     static_cast<std::string>(SDL_GetScancodeName(scancode)));
        return Scancode::UNKNOWN;

    default: return static_cast<Scancode>(scancode);
    }
}

inline constexpr SDL_Scancode _SynToSDL(const Scancode& scancode) {
    // SynScancode is a strict subset of SDL_Scancode so we don't have to do any
    // checks
    return static_cast<SDL_Scancode>(scancode);
}

/// @brief A virtual keyboard key. This is mapped to the meaning of the key, so
/// SynKeycode::I will correspond to I on any keyboard
enum class Keycode : uint32_t {
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

inline constexpr Keycode _SDLToSyn(const SDL_Keycode& keycode) {
    switch (keycode) {
    case SDLK_EXTENDED_MASK:
    case SDLK_SCANCODE_MASK:

    case SDLK_POWER:

    case SDLK_EXECUTE:
    case SDLK_HELP:
    case SDLK_MENU:
    case SDLK_SELECT:
    case SDLK_STOP:
    case SDLK_AGAIN:
    case SDLK_UNDO:
    case SDLK_CUT:
    case SDLK_COPY:
    case SDLK_PASTE:
    case SDLK_FIND:
    case SDLK_MUTE:
    case SDLK_VOLUMEUP:
    case SDLK_VOLUMEDOWN:
    case SDLK_KP_COMMA:
    case SDLK_KP_EQUALSAS400:
    case SDLK_ALTERASE:
    case SDLK_SYSREQ:
    case SDLK_CANCEL:
    case SDLK_CLEAR:
    case SDLK_PRIOR:
    case SDLK_RETURN2:
    case SDLK_SEPARATOR:
    case SDLK_OUT:
    case SDLK_OPER:
    case SDLK_CLEARAGAIN:
    case SDLK_CRSEL:
    case SDLK_EXSEL:
    case SDLK_KP_00:
    case SDLK_KP_000:
    case SDLK_THOUSANDSSEPARATOR:
    case SDLK_DECIMALSEPARATOR:
    case SDLK_CURRENCYUNIT:
    case SDLK_CURRENCYSUBUNIT:
    case SDLK_KP_LEFTPAREN:
    case SDLK_KP_RIGHTPAREN:
    case SDLK_KP_LEFTBRACE:
    case SDLK_KP_RIGHTBRACE:
    case SDLK_KP_TAB:
    case SDLK_KP_BACKSPACE:
    case SDLK_KP_A:
    case SDLK_KP_B:
    case SDLK_KP_C:
    case SDLK_KP_D:
    case SDLK_KP_E:
    case SDLK_KP_F:
    case SDLK_KP_XOR:
    case SDLK_KP_POWER:
    case SDLK_KP_PERCENT:
    case SDLK_KP_LESS:
    case SDLK_KP_GREATER:
    case SDLK_KP_AMPERSAND:
    case SDLK_KP_DBLAMPERSAND:
    case SDLK_KP_VERTICALBAR:
    case SDLK_KP_DBLVERTICALBAR:
    case SDLK_KP_COLON:
    case SDLK_KP_HASH:
    case SDLK_KP_SPACE:
    case SDLK_KP_AT:
    case SDLK_KP_EXCLAM:
    case SDLK_KP_MEMSTORE:
    case SDLK_KP_MEMRECALL:
    case SDLK_KP_MEMCLEAR:
    case SDLK_KP_MEMADD:
    case SDLK_KP_MEMSUBTRACT:
    case SDLK_KP_MEMMULTIPLY:
    case SDLK_KP_MEMDIVIDE:
    case SDLK_KP_PLUSMINUS:
    case SDLK_KP_CLEAR:
    case SDLK_KP_CLEARENTRY:
    case SDLK_KP_BINARY:
    case SDLK_KP_OCTAL:
    case SDLK_KP_DECIMAL:
    case SDLK_KP_HEXADECIMAL:

    case SDLK_MODE:

    case SDLK_SLEEP:
    case SDLK_WAKE:

    case SDLK_CHANNEL_INCREMENT:
    case SDLK_CHANNEL_DECREMENT:

    case SDLK_MEDIA_PLAY:
    case SDLK_MEDIA_PAUSE:
    case SDLK_MEDIA_RECORD:
    case SDLK_MEDIA_FAST_FORWARD:
    case SDLK_MEDIA_REWIND:
    case SDLK_MEDIA_NEXT_TRACK:
    case SDLK_MEDIA_PREVIOUS_TRACK:
    case SDLK_MEDIA_STOP:
    case SDLK_MEDIA_EJECT:
    case SDLK_MEDIA_PLAY_PAUSE:
    case SDLK_MEDIA_SELECT:

    case SDLK_AC_NEW:
    case SDLK_AC_OPEN:
    case SDLK_AC_CLOSE:
    case SDLK_AC_EXIT:
    case SDLK_AC_SAVE:
    case SDLK_AC_PRINT:
    case SDLK_AC_PROPERTIES:

    case SDLK_AC_SEARCH:
    case SDLK_AC_HOME:
    case SDLK_AC_BACK:
    case SDLK_AC_FORWARD:
    case SDLK_AC_STOP:
    case SDLK_AC_REFRESH:
    case SDLK_AC_BOOKMARKS:

    case SDLK_SOFTLEFT:
    case SDLK_SOFTRIGHT:
    case SDLK_CALL:
    case SDLK_ENDCALL:

    case SDLK_LEFT_TAB:
    case SDLK_LEVEL5_SHIFT:
    case SDLK_MULTI_KEY_COMPOSE:
    case SDLK_LMETA:
    case SDLK_RMETA:
    case SDLK_LHYPER:
    case SDLK_RHYPER:

        Logger::Warn("Untranslatable keycode: " +
                     static_cast<std::string>(SDL_GetKeyName(keycode)));
        return Keycode::UNKNOWN;

    default: return static_cast<Keycode>(keycode);
    }
}

inline constexpr SDL_Keycode _SynToSDL(const Keycode& keycode) {
    // SynKeycode is a strict subset of SDL_Keycode so we don't have to do any
    // checks
    return static_cast<SDL_Keycode>(keycode);
}

struct Keymod; // Forward declaration for friend functions below

constexpr Keymod     _SDLToSyn(const SDL_Keymod& keymod);
constexpr SDL_Keymod _SynToSDL(const Keymod& keymod);

/// @brief A virtual modifier key
struct Keymod {
    // This is a mirror of SDL_Keymod, with modifiers ommitted as needed and
    // convenience modifiers included

  private:
    uint16_t _rawValue;

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

    constexpr Keymod() : _rawValue(NONE) {}
    constexpr Keymod(uint16_t rawValue) : _rawValue(rawValue) {}

    constexpr Keymod operator+(const Keymod& other) const {
        return Keymod(this->_rawValue | other._rawValue);
    }

    constexpr Keymod operator-(const Keymod& other) const {
        return Keymod(this->_rawValue & ~other._rawValue);
    }

    constexpr Keymod operator+=(const Keymod& other) {
        this->_rawValue |= other._rawValue;
        return *this;
    }

    constexpr Keymod operator-=(const Keymod& other) {
        this->_rawValue &= ~other._rawValue;
        return *this;
    }

    constexpr bool operator==(const Keymod& other) const {
        return this->_rawValue == other._rawValue;
    }

    friend constexpr Keymod _SDLToSyn(const SDL_Keymod& keymod) {
        // modifiers SDL vends that we don't support
        const SDL_Keymod omitted = static_cast<SDL_Keymod>(0xF000u);

        if (keymod & omitted) {
            std::string omittedMods{};
            bool        first{ true };
            if (keymod & SDL_KMOD_NUM) {
                omittedMods += "Num Lock";
                first = false;
            }
            if (keymod & SDL_KMOD_CAPS) {
                if (!first) omittedMods += ", ";
                omittedMods += "Caps Lock";
                first = false;
            }
            if (keymod & SDL_KMOD_MODE) {
                if (!first) omittedMods += ", ";
                omittedMods += "Mode";
                first = false;
            }
            if (keymod & SDL_KMOD_SCROLL) {
                if (!first) omittedMods += ", ";
                omittedMods += "Scroll Lock";
            }
            Logger::Warn("Untranslatable modifiers: '" + omittedMods +
                         "'. All other modifiers will be translated.");
        }

        // mask out the omitted modifiers
        const SDL_Keymod masked = keymod & ~omitted;
        return Keymod(static_cast<uint16_t>(masked));
    }

    friend constexpr SDL_Keymod _SynToSDL(const Keymod& keymod) {
        return static_cast<SDL_Keymod>(keymod._rawValue);
    }
};

enum class MouseButton : uint8_t {
    _UNKNOWN  = 0, /// Not for public use, used for conversions with SDL
    LEFT      = SDL_BUTTON_LEFT,
    PRIMARY   = SDL_BUTTON_LEFT,
    RIGHT     = SDL_BUTTON_RIGHT,
    SECONDARY = SDL_BUTTON_RIGHT,
    MIDDLE    = SDL_BUTTON_MIDDLE,
    TERTIARY  = SDL_BUTTON_MIDDLE,
    FOUR      = SDL_BUTTON_X1,
    FIVE      = SDL_BUTTON_X2
};

/// @note Cannot accept `SDL_MouseButtonFlags` directly as the overload is
/// ambiguous due to `SDL_MouseButtonFlags` being a macro typedef of uint32_t
constexpr MouseButton _SDLToSyn(const uint8_t& button) {
    if (button > 0 && button <= SDL_BUTTON_X2) {
        return static_cast<MouseButton>(button);
    } else {
        Logger::Warn("Untranslatable mouse button: " +
                     std::to_string(static_cast<uint32_t>(button)));
        return MouseButton::_UNKNOWN;
    }
}

constexpr SDL_MouseButtonFlags _SynToSDL(const MouseButton& button) {
    return static_cast<SDL_MouseButtonFlags>(button);
}

} /* namespace Syngine */

#endif /* SynInputHelpers_h */
