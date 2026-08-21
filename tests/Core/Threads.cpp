// ╒════════════════════════ Threads.cpp ═╕
// │ Syngine                              │
// │ Created 2026-08-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

// Collection of tests for persistent threads and the job system

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"

#include <Syngine/Syngine.h>

#include <atomic>
#include <chrono>
#include <thread>

using namespace Syngine;
using namespace Catch::Matchers;

TEST_CASE("Persistent thread that runs several times with fixed update",
          "[threads]") {
    SYN_STARTENGINE;

    std::atomic<int> counter{ 0 };

    Thread& t = Threads().Create(
        "test thread", [&counter](std::stop_token stop, Thread& thread) {
            while (!stop.stop_requested()) {
                counter.fetch_add(1);
                thread.Wait(stop);
            }
        });
    t.Start();

    // Wake the persistent thread every fixed update, like the game loop does.
    Core::AddFixedUpdateCallback([&t](float) { t.Wake(); });

    engine.SetSimulationState(true);
    SimulateFrames(engine, 10);

    // Give the thread a chance to process the last wake before stopping it.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    Threads().StopAll();
    Core::ClearUpdateCallbacks();

    REQUIRE(counter.load() > 1);
}

TEST_CASE("Job system dispatches a job and waits for the expected result",
          "[jobs]") {
    SYN_STARTENGINE;

    JobResult<int> result =
        Core::Jobs().DispatchWithResult([]() { return 21 * 2; });

    REQUIRE(result.IsValid());
    REQUIRE(result.Get() == 42);
    REQUIRE(result.IsComplete());
}
