// ╒════════════════════ PhysicsSync.cpp ═╕
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

#include <Jolt/Jolt.h>
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Math/Real.h"

using namespace Syngine;
using namespace Catch::Matchers;

// Test to ensure that physics and transform components stay in sync when forces
// are applied.

// Create a rigidbody, apply a force, and check that the transform component's
// position changes accordingly after an update.

// Overall it's far from perfect, but it should be good enough to catch any
// major issues with the physics engine and transform component synchronization.
TEST_CASE("Rigidbody and Transform sync", "[Physics]") {
    // This requires a whole engine instance to run the update loop
    SYN_STARTENGINE;
    auto* go = CreateRigidbodyObject();

    engine.SetSimulationState(true);
    SimulateFrames(engine, 10);

    auto* transform = go->GetComponent<TransformComponent>();
    auto* rigidbody = go->GetComponent<RigidbodyComponent>();

    // Get the positions of both the transform and the rigidbody's physics body
    JPH::BodyInterface& bodyInterface =
        engine.GetPhysicsManager()->_GetBodyInterface();
    JPH::RVec3 rbPos =
        bodyInterface.GetCenterOfMassPosition(rigidbody->_GetBodyID());

    float* tpos = transform->GetPosition();

    // Check that the transform's position is approximately equal to the rigidbody's position
    REQUIRE_THAT(tpos[0], WithinAbs(rbPos.GetX(), FLOAT_MARGIN));
    REQUIRE_THAT(tpos[1], WithinAbs(-0.076166, FLOAT_MARGIN * 10)); // Y position is approximately -0.076166 after running test a bunch of times
    REQUIRE_THAT(tpos[2], WithinAbs(rbPos.GetZ(), FLOAT_MARGIN));


    // Cleanup
    delete go;
}

TEST_CASE("Rigidbody falls onto static floor and settles", "[Physics]") {
    SYN_STARTENGINE;

    auto* floor = new GameObject("Floor", "default");
    auto* floorTransform = floor->AddComponent<TransformComponent>();
    floorTransform->SetPosition(0.0f, -1.0f, 0.0f);

    RigidbodyParameters floorParams = {
        .shape           = PhysicsShapes::BOX,
        .mass            = 0.0f,
        .friction        = 0.8f,
        .restitution     = 0.0f,
        .shapeParameters = { 10.0f, 1.0f, 10.0f },
        .motionType      = JPH::EMotionType::Static,
        .layer           = Layers::NON_MOVING
    };
    auto* floorBody = floor->AddComponent<RigidbodyComponent>(floorParams);
    REQUIRE(floorBody != nullptr);

    auto* cube = new GameObject("FallingCube", "default");
    auto* cubeTransform = cube->AddComponent<TransformComponent>();
    cubeTransform->SetPosition(0.0f, 5.0f, 0.0f);

    RigidbodyParameters cubeParams = {
        .shape           = PhysicsShapes::BOX,
        .mass            = 1.0f,
        .friction        = 0.5f,
        .restitution     = 0.0f,
        .shapeParameters = { 1.0f, 1.0f, 1.0f },
        .motionType      = JPH::EMotionType::Dynamic,
        .layer           = Layers::MOVING
    };
    auto* cubeBody = cube->AddComponent<RigidbodyComponent>(cubeParams);
    REQUIRE(cubeBody != nullptr);

    engine.SetSimulationState(true);
    SimulateFrames(engine, 120);

    JPH::BodyInterface& bodyInterface =
        engine.GetPhysicsManager()->_GetBodyInterface();
    JPH::RVec3 rbPos =
        bodyInterface.GetCenterOfMassPosition(cubeBody->_GetBodyID());
    float* tpos = cubeTransform->GetPosition();

    REQUIRE_THAT(static_cast<float>(rbPos.GetY()), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(tpos[0], WithinAbs(static_cast<float>(rbPos.GetX()), FLOAT_MARGIN));
    REQUIRE_THAT(tpos[1], WithinAbs(static_cast<float>(rbPos.GetY()), FLOAT_MARGIN));
    REQUIRE_THAT(tpos[2], WithinAbs(static_cast<float>(rbPos.GetZ()), FLOAT_MARGIN));

    delete cube;
    delete floor;
}
