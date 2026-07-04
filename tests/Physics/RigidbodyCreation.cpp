// ╒══════════════ RigidbodyCreation.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-16                   │
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

// Collection of tests for creating rigidbodies with various parameters and
// ensuring they are created correctly in the physics engine.

// Generic box
TEST_CASE("Generic box rigidbody", "[Physics]") {
    SYN_STARTENGINE;

    auto* go = CreateRigidbodyObject();
    auto* rbc = go->GetComponent<RigidbodyComponent>();

    REQUIRE(rbc != nullptr);
    REQUIRE_THAT(rbc->GetMass(), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(rbc->GetFriction(), WithinAbs(0.5f, FLOAT_MARGIN));
    REQUIRE_THAT(rbc->GetRestitution(), WithinAbs(0.1f, FLOAT_MARGIN));
    delete go;
}

// Compound shape (box + sphere)
TEST_CASE("Compound shape rigidbody", "[Physics]") {
    std::vector<CompoundShapePart> parts;

    CompoundShapePart boxPart = {
        .shape           = PhysicsShapes::BOX,
        .shapeParameters = { 1.0f, 1.0f, 1.0f }, // half extents
        .position        = { 0.f, 0.f, 0.f },
        .rotation        = { 0.f, 0.f, 0.f, 1.f } // identity
    };
    parts.push_back(boxPart);

    CompoundShapePart spherePart = {
        .shape           = PhysicsShapes::SPHERE,
        .shapeParameters = { 0.5f }, // radius
        .position        = { 1.5f, 0.f, 0.f }, // offset from box
        .rotation        = { 0.f, 0.f, 0.f, 1.f } // identity
    };
    parts.push_back(spherePart);

    RigidbodyParameters params = {
        .shape           = PhysicsShapes::COMPOUND,
        .mass            = 2.0f,
        .friction        = 0.6f,
        .restitution     = 0.05f,
        .shapeParameters = {}, // Not used for compound
        .motionType      = JPH::EMotionType::Dynamic,
        .layer           = Layers::MOVING,
        .compoundParts   = parts
    };

    SYN_STARTENGINE;
    GameObject* obj = new GameObject("test_compound");
    obj->AddComponent<TransformComponent>();
    auto* rbc = obj->AddComponent<RigidbodyComponent>(params);

    REQUIRE(rbc != nullptr);
    REQUIRE_THAT(rbc->GetMass(), WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(rbc->GetFriction(), WithinAbs(0.6f, FLOAT_MARGIN));
    REQUIRE_THAT(rbc->GetRestitution(), WithinAbs(0.05f, FLOAT_MARGIN));
    delete obj;
}
