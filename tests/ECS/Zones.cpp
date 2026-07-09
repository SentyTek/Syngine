// ╒══════════════════════════ Zones.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-17                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"

#include <Syngine/Syngine.h>

using namespace Syngine;
using namespace Syngine::Math;
using namespace Catch::Matchers;

// Collection of tests for zone management in the ECS, ensuring that we can
// create zones and their callbacks/queries work as expected.

TEST_CASE("Test creating a zone and triggering callbacks", "[ECS][Zones]") {
    SYN_STARTENGINE

    // Create a zone
    const Math::Vector3 pos;
    const Math::Vector3 size(5.0f);
    auto* zonego = new GameObject("TestZone");
    auto* testZone = zonego->AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size, true);

    bool entered = false;
    bool exited  = false;

    testZone->OnEnter = [&](GameObject* obj) {
        entered = true;
        REQUIRE(obj->name == "TestObject");
    };

    testZone->OnExit = [&](GameObject* obj) {
        exited = true;
        REQUIRE(obj->name == "TestObject");
    };

    // Create an object that will enter and exit the zone
    auto* objgo = new GameObject("TestObject");
    auto* transform = objgo->AddComponent<TransformComponent>();
    transform->SetPosition(SVec3(10.0f, 0.0f, 0.0f));

    int framesToSimulate = 10;
    engine.SetSimulationState(true);
    for (int i = 0; i < framesToSimulate; ++i) {
        // Move the object towards the zone
        float newX = 10.0f - (i + 1) * (20.0f / framesToSimulate); // Move from 10 to -10 over the frames
        transform->SetPosition(SVec3(newX, 0.0f, 0.0f));

        // Update the engine to process the zone triggers
        engine.HandleEvents();
        engine.Update();
    }

    // Ensure that the object entered and exited the zone
    REQUIRE(entered);
    REQUIRE(exited);

    // Clean up
    delete zonego;
    delete objgo;
}

TEST_CASE("Test querying objects in a zone", "[ECS][Zones]") {
    SYN_STARTENGINE

    // Create a zone
    const SVec3 pos;
    const SVec3 size(5.0f);
    auto* zonego = new GameObject("TestZone");
    auto* testZone = zonego->AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size);

    // Create objects inside and outside the zone
    auto* insideObj1 = new GameObject("InsideObject1");
    auto* insideObj2 = new GameObject("InsideObject2");
    auto* outsideObj = new GameObject("OutsideObject");

    auto* transform1 = insideObj1->AddComponent<TransformComponent>();
    auto* transform2 = insideObj2->AddComponent<TransformComponent>();
    auto* transform3 = outsideObj->AddComponent<TransformComponent>();

    transform1->SetPosition(SVec3(1.0f, 0.0f, 0.0f)); // Inside
    transform2->SetPosition(SVec3(-1.0f, 0.0f, 0.0f)); // Inside
    transform3->SetPosition(SVec3(10.0f, 0.0f, 0.0f)); // Outside

    // Update the engine to process the zone triggers
    engine.HandleEvents();
    engine.Update();

    // Query objects in the zone
    auto objectsInZone = testZone->GetObjectsInZone();

    REQUIRE(objectsInZone.size() == 2);
    REQUIRE((objectsInZone[0]->name == "InsideObject1" || objectsInZone[1]->name == "InsideObject1"));
    REQUIRE((objectsInZone[0]->name == "InsideObject2" || objectsInZone[1]->name == "InsideObject2"));

    // Clean up
    delete zonego;
    delete insideObj1;
    delete insideObj2;
    delete outsideObj;
}

TEST_CASE("Test zone tags", "[ECS][Zones]") {
    SYN_STARTENGINE

    // Create a zone
    const Math::Vector3 pos;
    const Math::Vector3 size(5.0f);
    auto* zonego = new GameObject("TestZone");
    auto* testZone = zonego->AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size);

    // Add tags to the zone
    testZone->AddTag("Danger");
    testZone->AddTag("Lava");

    REQUIRE(testZone->HasTag("Danger"));
    REQUIRE(testZone->HasTag("Lava"));
    REQUIRE_FALSE(testZone->HasTag("Safe"));

    // Remove a tag and check again
    testZone->RemoveTag("Danger");
    REQUIRE_FALSE(testZone->HasTag("Danger"));
    REQUIRE(testZone->HasTag("Lava"));

    // Clean up
    delete zonego;
}
