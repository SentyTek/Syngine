// ╒═════════════════════ InputHelpers.h ═╕
// │ Syngine                              │
// │ Created 2025-08-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯
// Helper functions for input handling. You probably shouldn't use this directly

#ifndef SynInputHelpers_h
#define SynInputHelpers_h

#include "Syngine/Core/Input.h"

#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_events.h"

#include <cstdint>
#include <stdexcept>

namespace Syngine {

/// @brief Contains private helper functions for input handling
/// @internal
/// @section Input
struct _InputHelpers {

    // you should not be able to make an instance of this
    _InputHelpers() = delete;

    /// @brief Converts SDL_Keycode to KeyBinding::KeyboardKey
    /// @internal
    /// @param key The SDL_Keycode to convert
    /// @return The corresponding KeyBinding::KeyboardKey
    /// @throws std::invalid_argument if the keycode cannot be converted
    static inline Syngine::KeyBinding::KeyboardKey
    _KeycodeToKeyboardKey(const SDL_Keycode& keycode) {
        switch (keycode) {
        case SDLK_A: return Syngine::KeyBinding::KeyboardKey::A;
        case SDLK_B: return Syngine::KeyBinding::KeyboardKey::B;
        case SDLK_C: return Syngine::KeyBinding::KeyboardKey::C;
        case SDLK_D: return Syngine::KeyBinding::KeyboardKey::D;
        case SDLK_E: return Syngine::KeyBinding::KeyboardKey::E;
        case SDLK_F: return Syngine::KeyBinding::KeyboardKey::F;
        case SDLK_G: return Syngine::KeyBinding::KeyboardKey::G;
        case SDLK_H: return Syngine::KeyBinding::KeyboardKey::H;
        case SDLK_I: return Syngine::KeyBinding::KeyboardKey::I;
        case SDLK_J: return Syngine::KeyBinding::KeyboardKey::J;
        case SDLK_K: return Syngine::KeyBinding::KeyboardKey::K;
        case SDLK_L: return Syngine::KeyBinding::KeyboardKey::L;
        case SDLK_M: return Syngine::KeyBinding::KeyboardKey::M;
        case SDLK_N: return Syngine::KeyBinding::KeyboardKey::N;
        case SDLK_O: return Syngine::KeyBinding::KeyboardKey::O;
        case SDLK_P: return Syngine::KeyBinding::KeyboardKey::P;
        case SDLK_Q: return Syngine::KeyBinding::KeyboardKey::Q;
        case SDLK_R: return Syngine::KeyBinding::KeyboardKey::R;
        case SDLK_S: return Syngine::KeyBinding::KeyboardKey::S;
        case SDLK_T: return Syngine::KeyBinding::KeyboardKey::T;
        case SDLK_U: return Syngine::KeyBinding::KeyboardKey::U;
        case SDLK_V: return Syngine::KeyBinding::KeyboardKey::V;
        case SDLK_W: return Syngine::KeyBinding::KeyboardKey::W;
        case SDLK_X: return Syngine::KeyBinding::KeyboardKey::X;
        case SDLK_Y: return Syngine::KeyBinding::KeyboardKey::Y;
        case SDLK_Z: return Syngine::KeyBinding::KeyboardKey::Z;

        case SDLK_0: return Syngine::KeyBinding::KeyboardKey::_0;
        case SDLK_1: return Syngine::KeyBinding::KeyboardKey::_1;
        case SDLK_2: return Syngine::KeyBinding::KeyboardKey::_2;
        case SDLK_3: return Syngine::KeyBinding::KeyboardKey::_3;
        case SDLK_4: return Syngine::KeyBinding::KeyboardKey::_4;
        case SDLK_5: return Syngine::KeyBinding::KeyboardKey::_5;
        case SDLK_6: return Syngine::KeyBinding::KeyboardKey::_6;
        case SDLK_7: return Syngine::KeyBinding::KeyboardKey::_7;
        case SDLK_8: return Syngine::KeyBinding::KeyboardKey::_8;
        case SDLK_9: return Syngine::KeyBinding::KeyboardKey::_9;

        case SDLK_F1: return Syngine::KeyBinding::KeyboardKey::F1;
        case SDLK_F2: return Syngine::KeyBinding::KeyboardKey::F2;
        case SDLK_F3: return Syngine::KeyBinding::KeyboardKey::F3;
        case SDLK_F4: return Syngine::KeyBinding::KeyboardKey::F4;
        case SDLK_F5: return Syngine::KeyBinding::KeyboardKey::F5;
        case SDLK_F6: return Syngine::KeyBinding::KeyboardKey::F6;
        case SDLK_F7: return Syngine::KeyBinding::KeyboardKey::F7;
        case SDLK_F8: return Syngine::KeyBinding::KeyboardKey::F8;
        case SDLK_F9: return Syngine::KeyBinding::KeyboardKey::F9;
        case SDLK_F10: return Syngine::KeyBinding::KeyboardKey::F10;
        case SDLK_F11: return Syngine::KeyBinding::KeyboardKey::F11;
        case SDLK_F12: return Syngine::KeyBinding::KeyboardKey::F12;

        case SDLK_COMMA: return Syngine::KeyBinding::KeyboardKey::COMMA;
        case SDLK_PERIOD: return Syngine::KeyBinding::KeyboardKey::PERIOD;
        case SDLK_SEMICOLON: return Syngine::KeyBinding::KeyboardKey::SEMICOLON;
        case SDLK_APOSTROPHE:
            return Syngine::KeyBinding::KeyboardKey::APOSTROPHE;

        case SDLK_SPACE: return Syngine::KeyBinding::KeyboardKey::SPACE;
        case SDLK_TAB: return Syngine::KeyBinding::KeyboardKey::TAB;
        case SDLK_RETURN: return Syngine::KeyBinding::KeyboardKey::ENTER;
        case SDLK_BACKSPACE: return Syngine::KeyBinding::KeyboardKey::BACKSPACE;
        case SDLK_DELETE: return Syngine::KeyBinding::KeyboardKey::DELETE;

        case SDLK_MINUS: return Syngine::KeyBinding::KeyboardKey::MINUS;
        case SDLK_EQUALS: return Syngine::KeyBinding::KeyboardKey::EQUALS;
        case SDLK_TILDE: return Syngine::KeyBinding::KeyboardKey::TILDE;
        case SDLK_LEFTBRACKET:
            return Syngine::KeyBinding::KeyboardKey::LEFT_BRACKET;
        case SDLK_RIGHTBRACKET:
            return Syngine::KeyBinding::KeyboardKey::RIGHT_BRACKET;
        case SDLK_SLASH: return Syngine::KeyBinding::KeyboardKey::FORWARD_SLASH;
        case SDLK_BACKSLASH: return Syngine::KeyBinding::KeyboardKey::BACKSLASH;

        case SDLK_LEFT: return Syngine::KeyBinding::KeyboardKey::LEFT;
        case SDLK_RIGHT: return Syngine::KeyBinding::KeyboardKey::RIGHT;
        case SDLK_UP: return Syngine::KeyBinding::KeyboardKey::UP;
        case SDLK_DOWN: return Syngine::KeyBinding::KeyboardKey::DOWN;

        case SDLK_INSERT: return Syngine::KeyBinding::KeyboardKey::INSERT;
        case SDLK_HOME: return Syngine::KeyBinding::KeyboardKey::HOME;
        case SDLK_PAGEUP: return Syngine::KeyBinding::KeyboardKey::PAGE_UP;
        case SDLK_PAGEDOWN: return Syngine::KeyBinding::KeyboardKey::PAGE_DOWN;
        case SDLK_END: return Syngine::KeyBinding::KeyboardKey::END;
        case SDLK_PRINTSCREEN:
            return Syngine::KeyBinding::KeyboardKey::PRINT_SCREEN;
        case SDLK_ESCAPE: return Syngine::KeyBinding::KeyboardKey::ESCAPE;

        default: throw std::invalid_argument("Inconvertable SDL_Keycode");
        }
    }

    /// @brief Converts KeyBinding::KeyboardKey to SDL_Keycode
    /// @internal
    /// @param key The KeyBinding::KeyboardKey to convert
    /// @return The corresponding SDL_Keycode
    static inline SDL_Keycode
    _KeyboardKeyToKeycode(const Syngine::KeyBinding::KeyboardKey& key) {
        switch (key) {
        case Syngine::KeyBinding::KeyboardKey::A: return SDLK_A;
        case Syngine::KeyBinding::KeyboardKey::B: return SDLK_B;
        case Syngine::KeyBinding::KeyboardKey::C: return SDLK_C;
        case Syngine::KeyBinding::KeyboardKey::D: return SDLK_D;
        case Syngine::KeyBinding::KeyboardKey::E: return SDLK_E;
        case Syngine::KeyBinding::KeyboardKey::F: return SDLK_F;
        case Syngine::KeyBinding::KeyboardKey::G: return SDLK_G;
        case Syngine::KeyBinding::KeyboardKey::H: return SDLK_H;
        case Syngine::KeyBinding::KeyboardKey::I: return SDLK_I;
        case Syngine::KeyBinding::KeyboardKey::J: return SDLK_J;
        case Syngine::KeyBinding::KeyboardKey::K: return SDLK_K;
        case Syngine::KeyBinding::KeyboardKey::L: return SDLK_L;
        case Syngine::KeyBinding::KeyboardKey::M: return SDLK_M;
        case Syngine::KeyBinding::KeyboardKey::N: return SDLK_N;
        case Syngine::KeyBinding::KeyboardKey::O: return SDLK_O;
        case Syngine::KeyBinding::KeyboardKey::P: return SDLK_P;
        case Syngine::KeyBinding::KeyboardKey::Q: return SDLK_Q;
        case Syngine::KeyBinding::KeyboardKey::R: return SDLK_R;
        case Syngine::KeyBinding::KeyboardKey::S: return SDLK_S;
        case Syngine::KeyBinding::KeyboardKey::T: return SDLK_T;
        case Syngine::KeyBinding::KeyboardKey::U: return SDLK_U;
        case Syngine::KeyBinding::KeyboardKey::V: return SDLK_V;
        case Syngine::KeyBinding::KeyboardKey::W: return SDLK_W;
        case Syngine::KeyBinding::KeyboardKey::X: return SDLK_X;
        case Syngine::KeyBinding::KeyboardKey::Y: return SDLK_Y;
        case Syngine::KeyBinding::KeyboardKey::Z: return SDLK_Z;

        case Syngine::KeyBinding::KeyboardKey::_0: return SDLK_0;
        case Syngine::KeyBinding::KeyboardKey::_1: return SDLK_1;
        case Syngine::KeyBinding::KeyboardKey::_2: return SDLK_2;
        case Syngine::KeyBinding::KeyboardKey::_3: return SDLK_3;
        case Syngine::KeyBinding::KeyboardKey::_4: return SDLK_4;
        case Syngine::KeyBinding::KeyboardKey::_5: return SDLK_5;
        case Syngine::KeyBinding::KeyboardKey::_6: return SDLK_6;
        case Syngine::KeyBinding::KeyboardKey::_7: return SDLK_7;
        case Syngine::KeyBinding::KeyboardKey::_8: return SDLK_8;
        case Syngine::KeyBinding::KeyboardKey::_9: return SDLK_9;

        case Syngine::KeyBinding::KeyboardKey::F1: return SDLK_F1;
        case Syngine::KeyBinding::KeyboardKey::F2: return SDLK_F2;
        case Syngine::KeyBinding::KeyboardKey::F3: return SDLK_F3;
        case Syngine::KeyBinding::KeyboardKey::F4: return SDLK_F4;
        case Syngine::KeyBinding::KeyboardKey::F5: return SDLK_F5;
        case Syngine::KeyBinding::KeyboardKey::F6: return SDLK_F6;
        case Syngine::KeyBinding::KeyboardKey::F7: return SDLK_F7;
        case Syngine::KeyBinding::KeyboardKey::F8: return SDLK_F8;
        case Syngine::KeyBinding::KeyboardKey::F9: return SDLK_F9;
        case Syngine::KeyBinding::KeyboardKey::F10: return SDLK_F10;
        case Syngine::KeyBinding::KeyboardKey::F11: return SDLK_F11;
        case Syngine::KeyBinding::KeyboardKey::F12: return SDLK_F12;

        case Syngine::KeyBinding::KeyboardKey::COMMA: return SDLK_COMMA;
        case Syngine::KeyBinding::KeyboardKey::PERIOD: return SDLK_PERIOD;
        case Syngine::KeyBinding::KeyboardKey::SEMICOLON: return SDLK_SEMICOLON;
        case Syngine::KeyBinding::KeyboardKey::APOSTROPHE:
            return SDLK_APOSTROPHE;

        case Syngine::KeyBinding::KeyboardKey::SPACE: return SDLK_SPACE;
        case Syngine::KeyBinding::KeyboardKey::TAB: return SDLK_TAB;
        case Syngine::KeyBinding::KeyboardKey::ENTER: return SDLK_RETURN;
        case Syngine::KeyBinding::KeyboardKey::BACKSPACE: return SDLK_BACKSPACE;
        case Syngine::KeyBinding::KeyboardKey::DELETE: return SDLK_DELETE;

        case Syngine::KeyBinding::KeyboardKey::MINUS: return SDLK_MINUS;
        case Syngine::KeyBinding::KeyboardKey::EQUALS: return SDLK_EQUALS;
        case Syngine::KeyBinding::KeyboardKey::TILDE: return SDLK_TILDE;
        case Syngine::KeyBinding::KeyboardKey::LEFT_BRACKET:
            return SDLK_LEFTBRACKET;
        case Syngine::KeyBinding::KeyboardKey::RIGHT_BRACKET:
            return SDLK_RIGHTBRACKET;
        case Syngine::KeyBinding::KeyboardKey::FORWARD_SLASH: return SDLK_SLASH;
        case Syngine::KeyBinding::KeyboardKey::BACKSLASH: return SDLK_BACKSLASH;

        case Syngine::KeyBinding::KeyboardKey::LEFT: return SDLK_LEFT;
        case Syngine::KeyBinding::KeyboardKey::RIGHT: return SDLK_RIGHT;
        case Syngine::KeyBinding::KeyboardKey::UP: return SDLK_UP;
        case Syngine::KeyBinding::KeyboardKey::DOWN: return SDLK_DOWN;

        case Syngine::KeyBinding::KeyboardKey::INSERT: return SDLK_INSERT;
        case Syngine::KeyBinding::KeyboardKey::HOME: return SDLK_HOME;
        case Syngine::KeyBinding::KeyboardKey::PAGE_UP: return SDLK_PAGEUP;
        case Syngine::KeyBinding::KeyboardKey::PAGE_DOWN: return SDLK_PAGEDOWN;
        case Syngine::KeyBinding::KeyboardKey::END: return SDLK_END;
        case Syngine::KeyBinding::KeyboardKey::PRINT_SCREEN:
            return SDLK_PRINTSCREEN;
        case Syngine::KeyBinding::KeyboardKey::ESCAPE: return SDLK_ESCAPE;
        }
    }

    /// @brief Converts SDL_Keymod to KeyBinding::ModifierKeys
    /// @internal
    /// @param mod The SDL_Keymod to convert
    /// @return The corresponding KeyBinding::ModifierKeys
    static inline Syngine::KeyBinding::ModifierKeys
    _KeymodToModifierKeys(const SDL_Keymod& mod) {
        Syngine::KeyBinding::ModifierKeys keys;
        if (mod & SDL_KMOD_GUI) {
            keys += Syngine::KeyBinding::ModifierKey::COMMAND;
        }
        if (mod & SDL_KMOD_SHIFT) {
            keys += Syngine::KeyBinding::ModifierKey::SHIFT;
        }
        if (mod & SDL_KMOD_ALT) {
            keys += Syngine::KeyBinding::ModifierKey::OPTION;
        }
        if (mod & SDL_KMOD_CTRL) {
            keys += Syngine::KeyBinding::ModifierKey::CONTROL;
        }
        return keys;
    }

    /// @brief Converts KeyBinding::ModifierKeys to SDL_Keymod
    /// @internal
    /// @param mod The KeyBinding::ModifierKeys to convert
    /// @return The corresponding SDL_Keymod
    static inline SDL_Keymod
    _ModifierKeysToKeymod(const Syngine::KeyBinding::ModifierKeys& keys) {
        SDL_Keymod mod = SDL_KMOD_NONE;
        if (keys == Syngine::KeyBinding::ModifierKey::COMMAND) {
            mod |= SDL_KMOD_GUI;
        }
        if (keys == Syngine::KeyBinding::ModifierKey::SHIFT) {
            mod |= SDL_KMOD_SHIFT;
        }
        if (keys == Syngine::KeyBinding::ModifierKey::OPTION) {
            mod |= SDL_KMOD_ALT;
        }
        if (keys == Syngine::KeyBinding::ModifierKey::CONTROL) {
            mod |= SDL_KMOD_CTRL;
        }
        return mod;
    }

    /// @brief Converts a mouse button index to a MouseKey
    /// @internal
    /// @param index The mouse button index
    /// @return The corresponding MouseKey
    static inline Syngine::KeyBinding::MouseKey
    _MouseButtonIndexToMouseKey(const uint8_t& index) {
        switch (index) {
        case 1: return Syngine::KeyBinding::MouseKey::LEFT;
        case 2: return Syngine::KeyBinding::MouseKey::RIGHT;
        case 3: return Syngine::KeyBinding::MouseKey::MIDDLE;
        case 4: return Syngine::KeyBinding::MouseKey::BUTTON_4;
        case 5: return Syngine::KeyBinding::MouseKey::BUTTON_5;
        default:
            throw std::invalid_argument("Cannot convert mouse button index");
        }
    }

    /// @brief Converts a MouseKey to a mouse button index
    /// @internal
    /// @param key The MouseKey to convert
    /// @return The corresponding mouse button index
    static inline uint8_t
    _MouseKeyToMouseButtonIndex(const Syngine::KeyBinding::MouseKey& key) {
        switch (key) {
        case Syngine::KeyBinding::MouseKey::LEFT: return 1;
        case Syngine::KeyBinding::MouseKey::RIGHT: return 2;
        case Syngine::KeyBinding::MouseKey::MIDDLE: return 3;
        case Syngine::KeyBinding::MouseKey::BUTTON_4: return 4;
        case Syngine::KeyBinding::MouseKey::BUTTON_5: return 5;
        }
    }

    static inline Syngine::KeyBinding
    _InputEventToKeyBinding(const SDL_Event& event) {
        try {
            switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                if (event.key.mod == SDL_KMOD_NONE) {
                    return Syngine::KeyBinding(
                        _InputHelpers::_KeycodeToKeyboardKey(event.key.key));
                } else {
                    return Syngine::KeyBinding(
                        Syngine::KeyBinding::KeyboardShortcut(
                            _InputHelpers::_KeycodeToKeyboardKey(event.key.key),
                            _InputHelpers::_KeymodToModifierKeys(
                                event.key.mod)));
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                return Syngine::KeyBinding::MouseKey(event.button.button);
            }
            default: return Syngine::KeyBinding();
            }
        } catch (std::invalid_argument& error) {
            return Syngine::KeyBinding();
        }
    }
};

} /* namespace Syngine */

#endif /* SynInputHelpers_h */
