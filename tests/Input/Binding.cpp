// ╒════════════════════════ Binding.cpp ═╕
// │ Syngine                              │
// │ Created 2026-07-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../defines.h"
#include <Syngine/Syngine.h>

#include <SDL3/SDL.h>

// Collection of tests for Input binding system

TEST_CASE("Binding an input action to a key", "[Input]") {
    Syngine::InputAction* action = Syngine::InputAction::RegisterAction(
        "test.action",
        "Test Action",
        "test",
        Syngine::KeyBinding(Syngine::Scancode::A),
        { .onPressed = []() { /* Do something */ } });

    REQUIRE(action != nullptr);
}

TEST_CASE("Unregistering an input action by prefix", "[Input]") {
    Syngine::InputAction* action1 = Syngine::InputAction::RegisterAction(
        "test.action1",
        "Test Action 1",
        "test",
        Syngine::KeyBinding(Syngine::Scancode::B),
        { .onPressed = []() { /* Do something */ } });

    Syngine::InputAction* action2 = Syngine::InputAction::RegisterAction(
        "test.action2",
        "Test Action 2",
        "test",
        Syngine::KeyBinding(Syngine::Scancode::C),
        { .onPressed = []() { /* Do something */ } });

    REQUIRE(action1 != nullptr);
    REQUIRE(action2 != nullptr);

    size_t removedCount = Syngine::InputAction::UnregisterActionsByPrefix("test.");
    REQUIRE(removedCount == 2);
}

static bool actionTriggered = false;
static void onpressed() { actionTriggered = true; }

TEST_CASE("Triggering an input action with a key press", "[Input]") {
    SYN_STARTENGINE

    Syngine::InputAction::RegisterAction(
        "test.trigger",
        "Test Trigger",
        "test",
        Syngine::KeyBinding(Syngine::Scancode::A),
        { .onPressed = onpressed });

    // Simulate press by pushing something to the SDL event queue
    SDL_Event event;
    SDL_zero(event); // SDL3 macro to zero out the structure

    event.type = SDL_EVENT_KEY_DOWN;
    event.key.scancode = SDL_SCANCODE_A;
    event.key.key = SDL_GetKeyFromScancode(SDL_SCANCODE_A, SDL_KMOD_NONE, true);
    bool res      = SDL_PushEvent(&event);
    if (!res) {
        Syngine::Logger::ToConsole("SDL Failed to push event: %s",
                                   SDL_GetError());
        FAIL("SDL Failed to push event");
    }

    // Handle the event
    engine.HandleEvents();

    REQUIRE(actionTriggered == true);
}

static bool comboTriggered = false;
static void onComboPressed() { comboTriggered = true; }

TEST_CASE("Triggering an input action with a key combination", "[Input]") {
    SYN_STARTENGINE

    Syngine::InputAction::RegisterAction(
        "test.combo",
        "Test Combo",
        "test",
        Syngine::KeyBinding(Syngine::KeyShortcut(Syngine::Scancode::A,
                                                 Syngine::Keymod::LEFT_SHIFT)),
        { .onPressed = onComboPressed });

    // Simulate press by pushing something to the SDL event queue
    SDL_Event eventShift;
    SDL_zero(eventShift);
    eventShift.type = SDL_EVENT_KEY_DOWN;
    eventShift.key.scancode = SDL_SCANCODE_LSHIFT;
    eventShift.key.key = SDL_GetKeyFromScancode(SDL_SCANCODE_LSHIFT, SDL_KMOD_LSHIFT, true);
    bool res = SDL_PushEvent(&eventShift);
    if (!res) {
        Syngine::Logger::ToConsole("SDL Failed to push event: %s",
                                   SDL_GetError());
        FAIL("SDL Failed to push event shift");
    }

    SDL_Event eventA;
    SDL_zero(eventA);
    eventA.type = SDL_EVENT_KEY_DOWN;
    eventA.key.scancode = SDL_SCANCODE_A;
    eventA.key.mod = SDL_KMOD_LSHIFT;
    eventA.key.key = SDL_GetKeyFromScancode(SDL_SCANCODE_A, SDL_KMOD_LSHIFT, true);
    res            = SDL_PushEvent(&eventA);
    if (!res) {
        Syngine::Logger::ToConsole("SDL Failed to push event: %s",
                                   SDL_GetError());
        FAIL("SDL Failed to push event A");
    }

    // Handle the event
    engine.HandleEvents();

    REQUIRE(comboTriggered == true);
}
