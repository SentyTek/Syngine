// ╒═════════════════════ InputHelpers.h ═╕
// │ Syngine                              │
// │ Created 2025-08-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯
// behold the horrors

#ifndef SynInputHelpers_h
#define SynInputHelpers_h

#include "Syngine/Core/Logger.h"

#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <array>
#include <utility>

namespace Syngine {

/// @brief A physical keyboard key. This is mapped to the physical position of
/// the key, so SynScancode::W will correspond to the W key on an ANSI keyboard
/// but Z on a French keyboard
/// @note The names of scancodes are based on their position on an EN-US ANSI
/// keyboard
/// @since 0.0.1
enum class Scancode : uint32_t {
    // This is a mirror of SDL_Scancode with keys omitted as needed and
    // convenience scancodes included

    _UNKNOWN = SDL_SCANCODE_UNKNOWN, // Internal, should not be used directly

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

    NUM_1 = SDL_SCANCODE_1,
    NUM_2 = SDL_SCANCODE_2,
    NUM_3 = SDL_SCANCODE_3,
    NUM_4 = SDL_SCANCODE_4,
    NUM_5 = SDL_SCANCODE_5,
    NUM_6 = SDL_SCANCODE_6,
    NUM_7 = SDL_SCANCODE_7,
    NUM_8 = SDL_SCANCODE_8,
    NUM_9 = SDL_SCANCODE_9,
    NUM_0 = SDL_SCANCODE_0,

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

    PRINT_SCREEN = SDL_SCANCODE_PRINTSCREEN,
    SCROLL_LOCK  = SDL_SCANCODE_SCROLLLOCK,
    PAUSE        = SDL_SCANCODE_PAUSE,
    INSERT       = SDL_SCANCODE_INSERT, //* Can be generated when `Help`
                                        //* is pressed on some Mac keyboards
    HOME       = SDL_SCANCODE_HOME,
    PAGE_UP    = SDL_SCANCODE_PAGEUP,
    DELETE_KEY = SDL_SCANCODE_DELETE, // MSVC trips out if it's `DELETE`
    END        = SDL_SCANCODE_END,
    PAGE_DOWN  = SDL_SCANCODE_PAGEDOWN,
    RIGHT      = SDL_SCANCODE_RIGHT,
    LEFT       = SDL_SCANCODE_LEFT,
    DOWN       = SDL_SCANCODE_DOWN,
    UP         = SDL_SCANCODE_UP,

    NUM_LOCK = SDL_SCANCODE_NUMLOCKCLEAR, //* Equivalent to `CLEAR`
    CLEAR    = SDL_SCANCODE_NUMLOCKCLEAR, //* Equivalent to `NUM_LOCK`

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
    KEYPAD_0        = SDL_SCANCODE_KP_0,
    KEYPAD_PERIOD   = SDL_SCANCODE_KP_PERIOD,

    // POWER (after compose) intentionally omitted
    NON_US_BACKSLASH = SDL_SCANCODE_NONUSBACKSLASH,
    APPLICATION      = SDL_SCANCODE_APPLICATION, //* Equivalent to `COMPOSE`
    COMPOSE          = SDL_SCANCODE_APPLICATION, //* Equivalent to `APPLICATION`

    // F13 through VOLUME_DOWN and KEYPAD_EQUALS_AS_400 intentionally omitted
    KEYPAD_EQUALS = SDL_SCANCODE_KP_EQUALS,
    KEYPAD_COMMA  = SDL_SCANCODE_KP_COMMA,

    // INTERNATIONAL_1 through KEYPAD_HEXADECIMAL intentionally omitted

    LEFT_CONTROL = SDL_SCANCODE_LCTRL,
    LEFT_SHIFT   = SDL_SCANCODE_LSHIFT,
    LEFT_ALT     = SDL_SCANCODE_LALT, //* Equivalent to `LEFT_OPTION`
    LEFT_OPTION  = SDL_SCANCODE_LALT, //* Equivalent to `LEFT_ALT`
    LEFT_GUI =
        SDL_SCANCODE_LGUI, //* Equivalent to `LEFT_COMMAND` and `LEFT_WINDOWS`
    LEFT_COMMAND =
        SDL_SCANCODE_LGUI, //* Equivalent to `LEFT_GUI` and `LEFT_WINDOWS`
    LEFT_WINDOWS =
        SDL_SCANCODE_LGUI, //* Equivalent to `LEFT_GUI` and `LEFT_COMMAND`

    RIGHT_CONTROL = SDL_SCANCODE_RCTRL,
    RIGHT_SHIFT   = SDL_SCANCODE_RSHIFT,
    RIGHT_ALT     = SDL_SCANCODE_RALT, //* Equivalent to `RIGHT_OPTION`
    RIGHT_OPTION  = SDL_SCANCODE_RALT, //* Equivalent to `RIGHT_ALT`
    RIGHT_GUI =
        SDL_SCANCODE_RGUI, //* Equivalent to `RIGHT_COMMAND` and `RIGHT_WINDOWS`
    RIGHT_COMMAND =
        SDL_SCANCODE_RGUI, //* Equivalent to `RIGHT_GUI` and `RIGHT_WINDOWS`
    RIGHT_WINDOWS =
        SDL_SCANCODE_RGUI, //* Equivalent to `RIGHT_GUI` and `RIGHT_COMMAND`

    // MODE through END_CALL intentionally omitted

    // No need for an equivalent to SDL_SCANCODE_RESERVED or SDL_SCANCODE_COUNT
};

/// @brief Convert a ``SDL_Scancode`` to a ``Scancode``
/// @param scancode The ``SDL_Scancode`` to convert
/// @return The converted ``Scancode``
/// @since 0.0.1
/// @internal
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

        Logger::Warn("Scancode could not be translated: " +
                     static_cast<std::string>(SDL_GetScancodeName(scancode)));
        return Scancode::_UNKNOWN;

    default: return static_cast<Scancode>(scancode);
    }
}

/// @brief Convert a ``Scancode`` to a ``SDL_Scancode``
/// @param scancode The ``Scancode`` to convert
/// @return The converted ``SDL_Scancode``
/// @since 0.0.1
/// @internal
inline constexpr SDL_Scancode _SynToSDL(const Scancode& scancode) {
    // SynScancode is a strict subset of SDL_Scancode so we don't have to do any
    // checks
    return static_cast<SDL_Scancode>(scancode);
}

/// @brief A virtual keyboard key. This is mapped to the meaning of the key, so
/// SynKeycode::I will correspond to I on any keyboard
/// @since 0.0.1
enum class Keycode : uint32_t {
    // This is a mirror of SDL_Keycode, with keys omitted as needed and
    // convenience scancodes included

    // No need for an equivalent to SDLK_EXTENDED_MASK, SDLK_SCANCODE_MASK, or
    // SDLK_SCANCODE_TO_KEYCODE(X)

    _UNKNOWN = SDLK_UNKNOWN, /// Internal, should not be used directly

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
    LEFT_PARENTHESIS  = SDLK_LEFTPAREN,
    RIGHT_PARENTHESIS = SDLK_RIGHTPAREN,
    ASTERISK          = SDLK_ASTERISK,
    PLUS              = SDLK_PLUS,
    COMMA             = SDLK_COMMA,
    MINUS             = SDLK_MINUS,
    PERIOD            = SDLK_PERIOD,
    SLASH             = SDLK_SLASH,

    NUM_0 = SDLK_0,
    NUM_1 = SDLK_1,
    NUM_2 = SDLK_2,
    NUM_3 = SDLK_3,
    NUM_4 = SDLK_4,
    NUM_5 = SDLK_5,
    NUM_6 = SDLK_6,
    NUM_7 = SDLK_7,
    NUM_8 = SDLK_8,
    NUM_9 = SDLK_9,

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
    DELETE_KEY          = SDLK_DELETE, // MSVC trips out if it's `DELETE`
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

    NUM_LOCK = SDLK_NUMLOCKCLEAR, /// Equivalent to `CLEAR`
    CLEAR    = SDLK_NUMLOCKCLEAR, /// Equivalent to `NUM_LOCK`

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
    LEFT_ALT     = SDLK_LALT, /// Equivalent to `LEFT_OPTION`
    LEFT_OPTION  = SDLK_LALT, /// Equivalent to `LEFT_ALT`
    LEFT_GUI = SDLK_LGUI, /// Equivalent to `LEFT_COMMAND` and `LEFT_WINDOWS`
    LEFT_COMMAND = SDLK_LGUI, /// Equivalent to `LEFT_GUI` and `LEFT_WINDOWS`
    LEFT_WINDOWS = SDLK_LGUI, /// Equivalent to `LEFT_GUI` and `LEFT_COMMAND`

    RIGHT_CONTROL = SDLK_RCTRL,
    RIGHT_SHIFT   = SDLK_RSHIFT,
    RIGHT_ALT     = SDLK_RALT, /// Equivalent to `RIGHT_OPTION`
    RIGHT_OPTION  = SDLK_RALT, /// Equivalent to `RIGHT_ALT`
    RIGHT_GUI = SDLK_RGUI, /// Equivalent to `RIGHT_COMMAND` and `RIGHT_WINDOWS`
    RIGHT_COMMAND = SDLK_RGUI, /// Equivalent to `RIGHT_GUI` and `RIGHT_WINDOWS`
    RIGHT_WINDOWS = SDLK_RGUI, /// Equivalent to `RIGHT_GUI` and `RIGHT_COMMAND`

    // MODE through RIGHT_HYPER intentionally omitted
};

/// @brief Convert a ``SDL_Keycode`` to a ``Keycode``
/// @param keycode The ``SDL_Keycode`` to convert
/// @return The converted ``Keycode``
/// @since 0.0.1
/// @internal
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

        Logger::Warn("Keycode could not be translated: " +
                     static_cast<std::string>(SDL_GetKeyName(keycode)));
        return Keycode::_UNKNOWN;

    default: return static_cast<Keycode>(keycode);
    }
}

/// @brief Convert a ``Keycode`` to a ``SDL_Keycode``
/// @param keycode The ``Keycode`` to convert
/// @return The converted ``SDL_Keycode``
/// @since 0.0.1
/// @internal
inline constexpr SDL_Keycode _SynToSDL(const Keycode& keycode) {
    // SynKeycode is a strict subset of SDL_Keycode so we don't have to do any
    // checks
    return static_cast<SDL_Keycode>(keycode);
}

// Forward declaration for friend functions below
struct Keymod;

// Friend functions for `Keymod`
constexpr Keymod     _SDLToSyn(const SDL_Keymod& keymod);
constexpr SDL_Keymod _SynToSDL(const Keymod& keymod);

/// @brief A virtual modifier key
/// @example The recommend method of constructing a `Keymod` is to use the `+`
/// operator with the static constants in the class, optionally calling the
/// `Keymod(uint16_t)` constructor explicitly;
/// ```cpp
/// Keymod someKeymod = Keymod::LEFT_SHIFT + Keymod::RIGHT_CONTROL;
/// someFunctionThatTakesAModifier(Keymod(Keymod::MODE + Keymod::LEFT_ALT));
/// ```
/// All involved functions are marked `constexpr`, so there won't be any
/// performance downsides from doing this
/// @since 0.0.1
struct Keymod {
    // This is a mirror of SDL_Keymod, with modifiers omitted as needed and
    // convenience modifiers included

  private:
    /// @brief The raw value of the bitset
    /// @since 0.0.1
    /// @internal
    uint16_t _rawValue;

  public:
    // LEVEL_5_SHIFT, NUM_LOCK, CAPS_LOCK, and SCROLL_LOCK intentionally omitted
    static constexpr uint16_t NONE          = SDL_KMOD_NONE;
    static constexpr uint16_t LEFT_SHIFT    = SDL_KMOD_LSHIFT;
    static constexpr uint16_t RIGHT_SHIFT   = SDL_KMOD_RSHIFT;
    static constexpr uint16_t LEFT_CONTROL  = SDL_KMOD_LCTRL;
    static constexpr uint16_t RIGHT_CONTROL = SDL_KMOD_RCTRL;

    /// @note Equivalent to `LEFT_OPTION`
    static constexpr uint16_t LEFT_ALT = SDL_KMOD_LALT;
    /// @note Equivalent to `LEFT_ALT`
    static constexpr uint16_t LEFT_OPTION = SDL_KMOD_LALT;

    /// @note Equivalent to `RIGHT_OPTION`
    static constexpr uint16_t RIGHT_ALT = SDL_KMOD_RALT;
    /// @note Equivalent to `RIGHT_ALT`
    static constexpr uint16_t RIGHT_OPTION = SDL_KMOD_RALT;

    /// @note Equivalent to `LEFT_COMMAND` and `LEFT_WINDOWS`
    static constexpr uint16_t LEFT_GUI = SDL_KMOD_LGUI;
    /// @note Equivalent to `LEFT_GUI` and `LEFT_WINDOWS`
    static constexpr uint16_t LEFT_COMMAND = SDL_KMOD_LGUI;
    /// @note Equivalent to `LEFT_GUI` and `LEFT_COMMAND`
    static constexpr uint16_t LEFT_WINDOWS = SDL_KMOD_LGUI;

    /// @note Equivalent to `RIGHT_COMMAND` and `RIGHT_WINDOWS`
    static constexpr uint16_t RIGHT_GUI = SDL_KMOD_RGUI;
    /// @note Equivalent to `RIGHT_GUI` and `RIGHT_WINDOWS`
    static constexpr uint16_t RIGHT_COMMAND = SDL_KMOD_RGUI;
    /// @note Equivalent to `RIGHT_GUI` and `RIGHT_COMMAND`
    static constexpr uint16_t RIGHT_WINDOWS = SDL_KMOD_RGUI;
    
    static constexpr uint16_t CONTROL = SDL_KMOD_CTRL;
    static constexpr uint16_t SHIFT   = SDL_KMOD_SHIFT;

    /// @note Equivalent to `OPTION`
    static constexpr uint16_t ALT = SDL_KMOD_ALT;
    /// @note Equivalent to `ALT`
    static constexpr uint16_t OPTION = SDL_KMOD_ALT;
    /// @note Equivalent to `COMMAND` and `WINDOWS`
    static constexpr uint16_t GUI = SDL_KMOD_GUI;
    /// @note Equivalent to `GUI` and `WINDOWS`
    static constexpr uint16_t COMMAND = SDL_KMOD_GUI;
    /// @note Equivalent to `GUI` and `COMMAND`
    static constexpr uint16_t WINDOWS = SDL_KMOD_GUI;

    /// @brief Construct an empty ``Keymod``
    /// @since 0.0.1
    constexpr Keymod() : _rawValue(NONE) {}

    /// @brief Construct a ``Keymod`` from a raw value
    /// @param rawValue The raw integer representation of the ``Keymod``
    /// @note This may be removed from the public API at some point
    /// @since 0.0.1
    constexpr Keymod(uint16_t rawValue) : _rawValue(rawValue) {}

    /// @brief Merge two ``Keymod``s together
    /// @param other The ``Keymod`` to combine
    /// @return A keymod equivalent to the union of the two given keymods
    /// @since 0.0.1
    constexpr Keymod operator+(const Keymod& other) const {
        return Keymod(this->_rawValue | other._rawValue);
    }

    /// @brief Subtract a ``Keymod`` from another `Keymod`
    /// @param other The ``Keymod`` to subtract
    /// @return A keymod equivalent to the first ``Keymod`` with every modifier
    /// from the second `Keymod` removed
    /// @since 0.0.1
    constexpr Keymod operator-(const Keymod& other) const {
        return Keymod(this->_rawValue & ~other._rawValue);
    }

    /// @brief Merge a ``Keymod`` into an existing `Keymod`
    /// @param other The ``Keymod`` to combine
    /// @return A keymod equivalent to the union of the two given keymods
    /// @post The keymod this is called on will be modified to the value of the
    /// returned keymod
    /// @since 0.0.1
    constexpr Keymod operator+=(const Keymod& other) {
        this->_rawValue |= other._rawValue;
        return *this;
    }

    /// @brief Subtract a ``Keymod`` out of an existing `Keymod`
    /// @param other The ``Keymod`` to subtract
    /// @return A keymod equivalent to the first ``Keymod`` with every modifier
    /// from the second `Keymod` removed
    /// @post The keymod this is called on will be modified to the value of the
    /// returned keymod
    /// @since 0.0.1
    constexpr Keymod operator-=(const Keymod& other) {
        this->_rawValue &= ~other._rawValue;
        return *this;
    }

    /// @brief Equality operator for ``Keymod``
    /// @param other The ``Keymod`` to compare against
    /// @returns `true` if the objects are equal, `false` otherwise
    /// @since 0.0.1
    constexpr bool operator==(const Keymod& other) const {
        return this->_rawValue == other._rawValue;
    }

    constexpr Keymod operator&&(const Keymod& other) const {
        return this->_rawValue & other._rawValue;
    }

    constexpr bool contains(const Keymod& other) const {
        return (*this && other) != NONE;
    }

    /// @brief Convert a ``SDL_Keymod`` to a ``Keymod``
    /// @param keymod The ``SDL_Keymod`` to convert
    /// @return The converted ``Keymod``
    /// @since 0.0.1
    /// @internal
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
            Logger::Warn("Modifiers could not be translated: '" + omittedMods +
                         "'. All other modifiers will be translated.");
        }

        // mask out the omitted modifiers
        const SDL_Keymod masked = keymod & ~omitted;
        return Keymod(static_cast<uint16_t>(masked));
    }

    /// @brief Convert a ``Keymod`` to a ``SDL_Keymod``
    /// @param keymod The ``Keymod`` to convert
    /// @return The converted ``SDL_Keymod``
    /// @since 0.0.1
    /// @internal
    friend constexpr SDL_Keymod _SynToSDL(const Keymod& keymod) {
        return static_cast<SDL_Keymod>(keymod._rawValue);
    }
};

/// @brief A mouse button
/// @since 0.0.1
/// @section Input
/// @internal
enum class MouseButton : uint8_t {
    _UNKNOWN  = 0,                 /// Internal, should not be used directly
    LEFT      = SDL_BUTTON_LEFT,   /// Equivalent to `PRIMARY`
    PRIMARY   = SDL_BUTTON_LEFT,   /// Equivalent to `LEFT`
    RIGHT     = SDL_BUTTON_RIGHT,  /// Equivalent to `SECONDARY`
    SECONDARY = SDL_BUTTON_RIGHT,  /// Equivalent to `RIGHT`
    MIDDLE    = SDL_BUTTON_MIDDLE, /// Equivalent to `TERTIARY`
    TERTIARY  = SDL_BUTTON_MIDDLE, /// Equivalent to `MIDDLE`
    FOUR      = SDL_BUTTON_X1,
    FIVE      = SDL_BUTTON_X2
};

/// @brief Convert a ``SDL_MouseButtonFlags`` as a `uint8_t` to a
/// ``MouseButton``
/// @param button The ``uint8_t`` to convert
/// @return The converted ``MouseButton``
/// @note Cannot accept `SDL_MouseButtonFlags` directly as the overload is
/// ambiguous due to `SDL_MouseButtonFlags` being a macro typedef of uint32_t
/// @since 0.0.1
/// @internal
constexpr MouseButton _SDLToSyn(const uint8_t& button) {
    if (button > 0 && button <= SDL_BUTTON_X2) {
        return static_cast<MouseButton>(button);
    } else {
        Logger::Warn("Untranslatable mouse button: " +
                     std::to_string(static_cast<uint32_t>(button)));
        return MouseButton::_UNKNOWN;
    }
}

/// @brief Convert a ``MouseButton`` to a ``SDL_MouseButtonFlags``
/// @param button The ``MouseButton`` to convert
/// @return The converted ``SDL_MouseButtonFlags``
/// @since 0.0.1
/// @internal
constexpr SDL_MouseButtonFlags _SynToSDL(const MouseButton& button) {
    return static_cast<SDL_MouseButtonFlags>(button);
}

/// @brief A mapping between ``Keycode`` and a lower_snake_case formatted name
/// @since 0.0.1
/// @internal
inline constexpr auto _StringToKeycode = std::array {
    std::pair { "enter", Keycode::ENTER },
    std::pair { "escape", Keycode::ESCAPE },
    std::pair { "backspace", Keycode::BACKSPACE },
    std::pair { "tab", Keycode::TAB },
    std::pair { "space", Keycode::SPACE },

    std::pair { "exclamation_mark", Keycode::EXCLAMATION_MARK },
    std::pair { "quotation_mark", Keycode::QUOTATION_MARK },
    std::pair { "pound_sign", Keycode::POUND_SIGN },
    std::pair { "dollar_sign", Keycode::DOLLAR_SIGN },
    std::pair { "ampersand", Keycode::AMPERSAND },
    std::pair { "apostrophe", Keycode::APOSTROPHE },
    std::pair { "left_parenthesis", Keycode::LEFT_PARENTHESIS },
    std::pair { "right_parenthesis", Keycode::RIGHT_PARENTHESIS },
    std::pair { "asterisk", Keycode::ASTERISK },
    std::pair { "plus", Keycode::PLUS },
    std::pair { "comma", Keycode::COMMA },
    std::pair { "minus", Keycode::MINUS },
    std::pair { "period", Keycode::PERIOD },
    std::pair { "slash", Keycode::SLASH },

    std::pair { "0", Keycode::NUM_0 },
    std::pair { "1", Keycode::NUM_1 },
    std::pair { "2", Keycode::NUM_2 },
    std::pair { "3", Keycode::NUM_3 },
    std::pair { "4", Keycode::NUM_4 },
    std::pair { "5", Keycode::NUM_5 },
    std::pair { "6", Keycode::NUM_6 },
    std::pair { "7", Keycode::NUM_7 },
    std::pair { "8", Keycode::NUM_8 },
    std::pair { "9", Keycode::NUM_9 },

    std::pair { "colon", Keycode::COLON },
    std::pair { "semicolon", Keycode::SEMICOLON },
    std::pair { "less_than", Keycode::LESS_THAN },
    std::pair { "equals", Keycode::EQUALS },
    std::pair { "greater_than", Keycode::GREATER_THAN },
    std::pair { "question_mark", Keycode::QUESTION_MARK },
    std::pair { "at_sign", Keycode::AT_SIGN },
    std::pair { "left_bracket", Keycode::LEFT_BRACKET },
    std::pair { "backslash", Keycode::BACKSLASH },
    std::pair { "right_bracket", Keycode::RIGHT_BRACKET },
    std::pair { "caret", Keycode::CARET },
    std::pair { "underscore", Keycode::UNDERSCORE },

    std::pair { "a", Keycode::A },
    std::pair { "b", Keycode::B },
    std::pair { "c", Keycode::C },
    std::pair { "d", Keycode::D },
    std::pair { "e", Keycode::E },
    std::pair { "f", Keycode::F },
    std::pair { "g", Keycode::G },
    std::pair { "h", Keycode::H },
    std::pair { "i", Keycode::I },
    std::pair { "j", Keycode::J },
    std::pair { "k", Keycode::K },
    std::pair { "l", Keycode::L },
    std::pair { "m", Keycode::M },
    std::pair { "n", Keycode::N },
    std::pair { "o", Keycode::O },
    std::pair { "p", Keycode::P },
    std::pair { "q", Keycode::Q },
    std::pair { "r", Keycode::R },
    std::pair { "s", Keycode::S },
    std::pair { "t", Keycode::T },
    std::pair { "u", Keycode::U },
    std::pair { "v", Keycode::V },
    std::pair { "w", Keycode::W },
    std::pair { "x", Keycode::X },
    std::pair { "y", Keycode::Y },
    std::pair { "z", Keycode::Z },

    std::pair { "left_curly_bracket", Keycode::LEFT_CURLY_BRACKET },
    std::pair { "pipe", Keycode::PIPE },
    std::pair { "right_curly_bracket", Keycode::RIGHT_CURLY_BRACKET },
    std::pair { "tilde", Keycode::TILDE },
    std::pair { "delete", Keycode::DELETE_KEY },
    std::pair { "plus_minus", Keycode::PLUS_MINUS },
    std::pair { "caps_lock", Keycode::CAPS_LOCK },

    std::pair { "f1", Keycode::F1 },
    std::pair { "f2", Keycode::F2 },
    std::pair { "f3", Keycode::F3 },
    std::pair { "f4", Keycode::F4 },
    std::pair { "f5", Keycode::F5 },
    std::pair { "f6", Keycode::F6 },
    std::pair { "f7", Keycode::F7 },
    std::pair { "f8", Keycode::F8 },
    std::pair { "f9", Keycode::F9 },
    std::pair { "f10", Keycode::F10 },
    std::pair { "f11", Keycode::F11 },
    std::pair { "f12", Keycode::F12 },

    std::pair { "print_screen", Keycode::PRINT_SCREEN },
    std::pair { "scroll_lock", Keycode::SCROLL_LOCK },
    std::pair { "pause", Keycode::PAUSE },
    std::pair { "insert", Keycode::INSERT },
    std::pair { "home", Keycode::HOME },
    std::pair { "page_up", Keycode::PAGE_UP },
    std::pair { "end", Keycode::END },
    std::pair { "page_down", Keycode::PAGE_DOWN },

    std::pair { "right", Keycode::RIGHT },
    std::pair { "left", Keycode::LEFT },
    std::pair { "down", Keycode::DOWN },
    std::pair { "up", Keycode::UP },

    std::pair { "num_lock", Keycode::NUM_LOCK },
    std::pair { "clear", Keycode::CLEAR },

    std::pair { "keypad_divide", Keycode::KEYPAD_DIVIDE },
    std::pair { "keypad_multiply", Keycode::KEYPAD_MULTIPLY },
    std::pair { "keypad_minus", Keycode::KEYPAD_MINUS },
    std::pair { "keypad_plus", Keycode::KEYPAD_PLUS },
    std::pair { "keypad_enter", Keycode::KEYPAD_ENTER },
    std::pair { "keypad_0", Keycode::KEYPAD_0 },
    std::pair { "keypad_1", Keycode::KEYPAD_1 },
    std::pair { "keypad_2", Keycode::KEYPAD_2 },
    std::pair { "keypad_3", Keycode::KEYPAD_3 },
    std::pair { "keypad_4", Keycode::KEYPAD_4 },
    std::pair { "keypad_5", Keycode::KEYPAD_5 },
    std::pair { "keypad_6", Keycode::KEYPAD_6 },
    std::pair { "keypad_7", Keycode::KEYPAD_7 },
    std::pair { "keypad_8", Keycode::KEYPAD_8 },
    std::pair { "keypad_9", Keycode::KEYPAD_9 },
    std::pair { "keypad_period", Keycode::KEYPAD_PERIOD },
    std::pair { "keypad_equals", Keycode::KEYPAD_EQUALS },

    std::pair { "f13", Keycode::F13 },
    std::pair { "f14", Keycode::F14 },
    std::pair { "f15", Keycode::F15 },
    std::pair { "f16", Keycode::F16 },
    std::pair { "f17", Keycode::F17 },
    std::pair { "f18", Keycode::F18 },
    std::pair { "f19", Keycode::F19 },
    std::pair { "f20", Keycode::F20 },
    std::pair { "f21", Keycode::F21 },
    std::pair { "f22", Keycode::F22 },
    std::pair { "f23", Keycode::F23 },
    std::pair { "f24", Keycode::F24 },

    std::pair { "left_control", Keycode::LEFT_CONTROL },
    std::pair { "left_shift", Keycode::LEFT_SHIFT },
    std::pair { "left_alt", Keycode::LEFT_ALT },
    std::pair { "left_option", Keycode::LEFT_OPTION },
    std::pair { "left_gui", Keycode::LEFT_GUI },
    std::pair { "left_command" , Keycode::LEFT_COMMAND },
    std::pair { "left_windows", Keycode::LEFT_WINDOWS },

    std::pair { "right_control", Keycode::RIGHT_CONTROL },
    std::pair { "right_shift", Keycode::RIGHT_SHIFT },
    std::pair { "right_alt", Keycode::RIGHT_ALT },
    std::pair { "right_option", Keycode::RIGHT_OPTION },
    std::pair { "right_gui", Keycode::RIGHT_GUI },
    std::pair { "right_command" , Keycode::RIGHT_COMMAND },
    std::pair { "right_windows", Keycode::RIGHT_WINDOWS },
};

/// @brief A mapping between ``Scancode`` and a lower_snake_case formatted name
/// @since 0.0.1
/// @internal
inline constexpr auto _StringToScancode = std::array {
    std::pair { "a", Scancode::A },
    std::pair { "b", Scancode::B },
    std::pair { "c", Scancode::C },
    std::pair { "d", Scancode::D },
    std::pair { "e", Scancode::E },
    std::pair { "f", Scancode::F },
    std::pair { "g", Scancode::G },
    std::pair { "h", Scancode::H },
    std::pair { "i", Scancode::I },
    std::pair { "j", Scancode::J },
    std::pair { "k", Scancode::K },
    std::pair { "l", Scancode::L },
    std::pair { "m", Scancode::M },
    std::pair { "n", Scancode::N },
    std::pair { "o", Scancode::O },
    std::pair { "p", Scancode::P },
    std::pair { "q", Scancode::Q },
    std::pair { "r", Scancode::R },
    std::pair { "s", Scancode::S },
    std::pair { "t", Scancode::T },
    std::pair { "u", Scancode::U },
    std::pair { "v", Scancode::V },
    std::pair { "w", Scancode::W },
    std::pair { "x", Scancode::X },
    std::pair { "y", Scancode::Y },
    std::pair { "z", Scancode::Z },

    std::pair { "1", Scancode::NUM_1 },
    std::pair { "2", Scancode::NUM_2 },
    std::pair { "3", Scancode::NUM_3 },
    std::pair { "4", Scancode::NUM_4 },
    std::pair { "5", Scancode::NUM_5 },
    std::pair { "6", Scancode::NUM_6 },
    std::pair { "7", Scancode::NUM_7 },
    std::pair { "8", Scancode::NUM_8 },
    std::pair { "9", Scancode::NUM_9 },
    std::pair { "0", Scancode::NUM_0 },

    std::pair { "enter", Scancode::ENTER },
    std::pair { "escape", Scancode::ESCAPE },
    std::pair { "backspace", Scancode::BACKSPACE },
    std::pair { "tab", Scancode::TAB },
    std::pair { "space", Scancode::SPACE },

    std::pair { "minus", Scancode::MINUS },
    std::pair { "equals", Scancode::EQUALS },
    std::pair { "left_bracket", Scancode::LEFT_BRACKET },
    std::pair { "right_bracket", Scancode::RIGHT_BRACKET },
    std::pair { "backslash", Scancode::BACKSLASH },
    std::pair { "semicolon", Scancode::SEMICOLON },
    std::pair { "apostrophe", Scancode::APOSTROPHE },
    std::pair { "grave", Scancode::GRAVE },
    std::pair { "comma", Scancode::COMMA },
    std::pair { "period", Scancode::PERIOD },
    std::pair { "slash", Scancode::SLASH },

    std::pair { "capslock", Scancode::CAPSLOCK },

    std::pair { "f1", Scancode::F1 },
    std::pair { "f2", Scancode::F2 },
    std::pair { "f3", Scancode::F3 },
    std::pair { "f4", Scancode::F4 },
    std::pair { "f5", Scancode::F5 },
    std::pair { "f6", Scancode::F6 },
    std::pair { "f7", Scancode::F7 },
    std::pair { "f8", Scancode::F8 },
    std::pair { "f9", Scancode::F9 },
    std::pair { "f10", Scancode::F10 },
    std::pair { "f11", Scancode::F11 },
    std::pair { "f12", Scancode::F12 },

    std::pair { "print_screen", Scancode::PRINT_SCREEN },
    std::pair { "scroll_lock", Scancode::SCROLL_LOCK },
    std::pair { "pause", Scancode::PAUSE },
    std::pair { "insert", Scancode::INSERT },

    std::pair { "home", Scancode::HOME },
    std::pair { "page_up", Scancode::PAGE_UP },
    std::pair { "delete", Scancode::DELETE_KEY },
    std::pair { "end", Scancode::END },
    std::pair { "page_down", Scancode::PAGE_DOWN },
    std::pair { "right", Scancode::RIGHT },
    std::pair { "left", Scancode::LEFT },
    std::pair { "down", Scancode::DOWN },
    std::pair { "up", Scancode::UP },

    std::pair { "num_lock", Scancode::NUM_LOCK },
    std::pair { "clear", Scancode::CLEAR },

    std::pair { "keypad_divide", Scancode::KEYPAD_DIVIDE },
    std::pair { "keypad_multiply", Scancode::KEYPAD_MULTIPLY },
    std::pair { "keypad_minus", Scancode::KEYPAD_MINUS },
    std::pair { "keypad_plus", Scancode::KEYPAD_PLUS },
    std::pair { "keypad_enter", Scancode::KEYPAD_ENTER },
    std::pair { "keypad_1", Scancode::KEYPAD_1 },
    std::pair { "keypad_2", Scancode::KEYPAD_2 },
    std::pair { "keypad_3", Scancode::KEYPAD_3 },
    std::pair { "keypad_4", Scancode::KEYPAD_4 },
    std::pair { "keypad_5", Scancode::KEYPAD_5 },
    std::pair { "keypad_6", Scancode::KEYPAD_6 },
    std::pair { "keypad_7", Scancode::KEYPAD_7 },
    std::pair { "keypad_8", Scancode::KEYPAD_8 },
    std::pair { "keypad_9", Scancode::KEYPAD_9 },
    std::pair { "keypad_0", Scancode::KEYPAD_0 },
    std::pair { "keypad_period", Scancode::KEYPAD_PERIOD },

    std::pair { "non_us_backslash", Scancode::NON_US_BACKSLASH },
    std::pair { "application", Scancode::APPLICATION },
    std::pair { "compose", Scancode::COMPOSE },

    std::pair { "keypad_equals", Scancode::KEYPAD_EQUALS },
    std::pair { "keypad_comma", Scancode::KEYPAD_COMMA },

    std::pair { "left_control", Scancode::LEFT_CONTROL },
    std::pair { "left_shift", Scancode::LEFT_SHIFT },
    std::pair { "left_alt", Scancode::LEFT_ALT },
    std::pair { "left_option", Scancode::LEFT_OPTION },
    std::pair { "left_gui", Scancode::LEFT_GUI },
    std::pair { "left_command", Scancode::LEFT_COMMAND },
    std::pair { "left_windows", Scancode::LEFT_WINDOWS },

    std::pair { "right_control", Scancode::RIGHT_CONTROL },
    std::pair { "right_shift", Scancode::RIGHT_SHIFT },
    std::pair { "right_alt", Scancode::RIGHT_ALT },
    std::pair { "right_option", Scancode::RIGHT_OPTION },
    std::pair { "right_gui", Scancode::RIGHT_GUI },
    std::pair { "right_command", Scancode::RIGHT_COMMAND },
    std::pair { "right_windows", Scancode::RIGHT_WINDOWS },
};

} /* namespace Syngine */

#endif /* SynInputHelpers_h */
