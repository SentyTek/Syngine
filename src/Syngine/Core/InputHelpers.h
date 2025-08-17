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
#include "Syngine/Core/Logger.h"

#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_events.h"
#include <cstdint>

namespace Syngine {

// this is about the ugliest piece of code I've ever written but it's probably
// the best way to do it, which is why it gets it's own file
inline Syngine::KeyBinding::KeyboardKey sdlToKeyboardKey(SDL_Keycode key) {
    switch (key) {
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
    case SDLK_APOSTROPHE: return Syngine::KeyBinding::KeyboardKey::APOSTROPHE;

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

    case SDLK_UNKNOWN:
    default: return Syngine::KeyBinding::KeyboardKey::UNKNOWN;
    }
}

inline Syngine::KeyBinding::SidedModifierKey
sdlToSidedModifierKey(SDL_Keycode key) {
    switch (key) {
    case SDLK_LGUI: return Syngine::KeyBinding::SidedModifierKey::LEFT_COMMAND;
    case SDLK_RGUI: return Syngine::KeyBinding::SidedModifierKey::RIGHT_COMMAND;

    case SDLK_LSHIFT: return Syngine::KeyBinding::SidedModifierKey::LEFT_SHIFT;
    case SDLK_RSHIFT: return Syngine::KeyBinding::SidedModifierKey::RIGHT_SHIFT;

    case SDLK_LALT: return Syngine::KeyBinding::SidedModifierKey::LEFT_OPTION;
    case SDLK_RALT: return Syngine::KeyBinding::SidedModifierKey::RIGHT_OPTION;

    case SDLK_LCTRL: return Syngine::KeyBinding::SidedModifierKey::LEFT_CONTROL;
    case SDLK_RCTRL:
        return Syngine::KeyBinding::SidedModifierKey::RIGHT_CONTROL;

    default: return Syngine::KeyBinding::SidedModifierKey::UNKNOWN;
    }
}

inline Syngine::KeyBinding::ModifierKey sdlToModifierKey(SDL_Keymod mod) {
    switch (mod) {
    case SDL_KMOD_GUI: return Syngine::KeyBinding::ModifierKey::COMMAND;
    case SDL_KMOD_SHIFT: return Syngine::KeyBinding::ModifierKey::SHIFT;
    case SDL_KMOD_ALT: return Syngine::KeyBinding::ModifierKey::OPTION;
    case SDL_KMOD_CTRL: return Syngine::KeyBinding::ModifierKey::CONTROL;
    default:
        Syngine::Logger::Log("Could not convert SDL_Keymod to ModifierKey",
                             LogLevel::WARN);
        return Syngine::KeyBinding::ModifierKey::UNKNOWN;
    }
}

inline Syngine::KeyBinding sdlToKeyBinding(uint8_t key) {
    switch (key) {
    case 1: return Syngine::KeyBinding(Syngine::KeyBinding::MouseKey::LEFT);
    case 2: return Syngine::KeyBinding(Syngine::KeyBinding::MouseKey::RIGHT);
    case 3: return Syngine::KeyBinding(Syngine::KeyBinding::MouseKey::MIDDLE);
    case 4: return Syngine::KeyBinding(Syngine::KeyBinding::MouseKey::BUTTON_4);
    case 5: return Syngine::KeyBinding(Syngine::KeyBinding::MouseKey::BUTTON_5);
    default: return Syngine::KeyBinding();
    }
}

inline Syngine::KeyBinding sdlToKeyBinding(SDL_KeyboardEvent event) {
    if (event.mod == SDL_KMOD_NONE) {
        switch (event.key) {
        case SDLK_LGUI:
        case SDLK_RGUI:
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        case SDLK_LALT:
        case SDLK_RALT:
        case SDLK_LCTRL:
        case SDLK_RCTRL:
            return Syngine::KeyBinding(Syngine::KeyBinding::SidedModifierKey(
                sdlToSidedModifierKey(event.key)));
        default: return Syngine::KeyBinding(sdlToKeyboardKey(event.key));
        }
    } else {
        return Syngine::KeyBinding(Syngine::KeyBinding::KeyboardShortcut(
            sdlToKeyboardKey(event.key), sdlToModifierKey(event.mod)));
    }
}

inline Syngine::KeyBinding sdlToKeyBinding(SDL_Event event) {
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: return sdlToKeyBinding(event.key);

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        return Syngine::KeyBinding(sdlToKeyBinding(event.button.button));

    default: return Syngine::KeyBinding();
    }
}

} /* namespace Syngine */

#endif /* SynInputHelpers_h */
