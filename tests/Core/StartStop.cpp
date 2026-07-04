// ╒══════════════════════ StartStop.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-14                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"

#include <Syngine/Syngine.h>

using namespace Syngine;
using namespace Catch::Matchers;

// Collection of tests for initialization and shutdown of the Syngine engine,
// ensuring that subsystems are properly initialized and cleaned up

// Tests that the engine can start and stop without crashing, and that
// subsystems are initialized correctly (except for renderer of course)
TEST_CASE("Core engine start and stop", "[Core]") {
    std::string gameName = "TestGame";
    EngineConfig config   = { .gameName     = gameName,
                              .windowWidth  = 800,
                              .windowHeight = 600,
                              .usePhysics   = true,
                              .headless     = true };

    Syngine::RendererConfig rConfig  = { .useShadows      = false,
                                         .shadowDist      = 0,
                                         .vsync           = false,
                                         .usePseudoCamera = false };

    {
        Core engine(config);
        engine.Initialize(rConfig);

        REQUIRE(engine.IsInitialized());
        REQUIRE(Core::IsPhysicsEnabled());

        Renderer::RemoveAllPrograms();
    }

    // After the engine goes out of scope, subsystems should be cleaned up
    REQUIRE(!Core::IsInitialized());
}

// Tests that multiple start-stop cycles work correctly without resource leaks or crashes
TEST_CASE("Multiple start-stop cycles", "[Core]") {
    std::string gameName = "TestGame";
    EngineConfig config   = { .gameName     = gameName,
                              .windowWidth  = 800,
                              .windowHeight = 600,
                              .usePhysics   = true,
                              .headless     = true };

    Syngine::RendererConfig rConfig  = { .useShadows      = false,
                                         .shadowDist      = 0,
                                         .vsync           = false,
                                         .usePseudoCamera = false };

    for (int i = 0; i < 3; ++i) {
        Core engine(config);
        engine.Initialize(rConfig);

        REQUIRE(engine.IsInitialized());
        REQUIRE(Core::IsPhysicsEnabled());

        Renderer::RemoveAllPrograms();
    }

    REQUIRE(!Core::IsInitialized());
}

// Tests that its not possible to call initialize twice without shutting down,
// and that the engine handles this gracefully
TEST_CASE("Double initialization/Double constructor", "[Core]") {
    std::string gameName = "TestGame";
    EngineConfig config   = { .gameName     = gameName,
                              .windowWidth  = 800,
                              .windowHeight = 600,
                              .usePhysics   = true,
                              .headless     = true };

    Syngine::RendererConfig rConfig  = { .useShadows      = false,
                                         .shadowDist      = 0,
                                         .vsync           = false,
                                         .usePseudoCamera = false };

    Core engine(config);
    REQUIRE_NOTHROW(engine.Initialize(rConfig));
    REQUIRE_THROWS(engine.Initialize(rConfig));

    REQUIRE_THROWS([&config]() { Core engine2(config); }());

    Renderer::RemoveAllPrograms();
}
