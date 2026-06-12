// ╒════════════ AddRemoveComponents.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>

#include <Syngine/ECS/GameObject.h>
#include <Syngine/ECS/AllComponents.h>
using namespace Syngine;

TEST_CASE("Add and Remove Components", "[ECS]") {
    auto* go = new GameObject("TestObject");
    auto* transform = go->AddComponent<TransformComponent>();
    REQUIRE(transform != nullptr);
    REQUIRE(go->HasComponent(SYN_COMPONENT_TRANSFORM));
    REQUIRE(go->GetComponent<TransformComponent>() == transform);

    go->RemoveComponent(SYN_COMPONENT_TRANSFORM);
    REQUIRE(!go->HasComponent(SYN_COMPONENT_TRANSFORM));
    delete go;
}
