// ╒═════════════════════ ArenaAlloc.cpp ═╕
// │ Syngine                              │
// │ Created 2026-08-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯
// Collection of tests for the arena allocator

#include <catch2/catch_test_macros.hpp>
#include <Syngine/Core/Memory/ArenaAlloc.h>

using namespace Syngine::Memory;

TEST_CASE("ArenaAlloc allocates and stores values within capacity",
          "[memory]") {
    ArenaAlloc arena(1024);

    auto ints = arena.Allocate<int>(4);
    REQUIRE(ints.size() == 4);

    for (int i = 0; i < 4; ++i) {
        ints[i] = i * 10;
    }
    for (int i = 0; i < 4; ++i) {
        REQUIRE(ints[i] == i * 10);
    }
}

TEST_CASE("ArenaAlloc Create constructs objects in place", "[memory]") {
    struct Foo {
        int   a;
        float b;
        Foo(int a_, float b_) : a(a_), b(b_) {}
    };

    ArenaAlloc arena(256);
    Foo*       foo = arena.Create<Foo>(5, 2.5f);

    REQUIRE(foo != nullptr);
    REQUIRE(foo->a == 5);
    REQUIRE(foo->b == 2.5f);
}

TEST_CASE("ArenaAlloc returns an empty span when out of memory", "[memory]") {
    ArenaAlloc arena(16);

    auto fits = arena.Allocate<int>(2); // 8 bytes, fits within capacity
    REQUIRE_FALSE(fits.empty());

    auto tooBig = arena.Allocate<int>(100); // far exceeds remaining capacity
    REQUIRE(tooBig.empty());
}

TEST_CASE("ArenaAlloc Reset allows the arena to be reused", "[memory]") {
    ArenaAlloc arena(64);

    auto first = arena.Allocate<int>(4);
    REQUIRE_FALSE(first.empty());

    arena.Reset();

    auto second = arena.Allocate<int>(4);
    REQUIRE_FALSE(second.empty());
    REQUIRE(second.data() == first.data());
}
