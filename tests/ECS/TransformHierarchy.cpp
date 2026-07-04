// ╒═════════════ TransformHierarchy.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-13                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../defines.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include <Syngine/Core/Registry.h>
#include <Syngine/ECS/GameObject.h>
#include <Syngine/ECS/AllComponents.h>

using namespace Syngine;
using namespace Catch::Matchers;

// Collection of tests for Transform component hierarchy management

// Tests that movement of a parent GameObject propagates correctly to its children
TEST_CASE("Parent Movement Propagation in children", "[ECS]") {
    auto* parent = new GameObject("Parent");
    auto* child  = new GameObject("Child");

    auto* parentTransform = parent->AddComponent<TransformComponent>();
    auto* childTransform  = child->AddComponent<TransformComponent>();
    child->SetParent(parent);

    REQUIRE(parentTransform != nullptr);
    REQUIRE(childTransform != nullptr);

    parentTransform->SetPosition(10.0f, 0.0f, 0.0f);
    REQUIRE_THAT(childTransform->GetPosition()[0], WithinAbs(10.0f, FLOAT_MARGIN));

    delete parent;
    delete child;
}

// Tests that local and world getters work for child GameObjects
TEST_CASE("Local and World Getters in children", "[ECS]") {
    // Logic behind this is if a child is spawned at (0, 5, 0) (parent at
    // origin), and parent is moved down 5 meters, child should be at origin
    auto* parent = new GameObject("Parent");
    auto* child  = new GameObject("Child");

    parent->AddComponent<TransformComponent>();
    auto* childTransform = child->AddComponent<TransformComponent>();
    childTransform->SetPosition(0.f, 5.f, 0.f);

    child->SetParent(parent);

    REQUIRE_THAT(childTransform->GetPosition()[1],
                 WithinAbs(5.0f, FLOAT_MARGIN));
    REQUIRE_THAT(childTransform->GetLocalPosition()[1],
                 WithinAbs(5.0f, FLOAT_MARGIN));

    parent->GetComponent<TransformComponent>()->SetPosition(0.f, -5.f, 0.f);

    REQUIRE_THAT(childTransform->GetPosition()[1],
                 WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(childTransform->GetLocalPosition()[1],
                 WithinAbs(5.0f, FLOAT_MARGIN));

    delete parent;
    delete child;
}
