// ╒══════════════════════════ Zones.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-17                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
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

namespace {
bool gZoneEntered = false;
bool gZoneExited  = false;

void ZoneEnterCallback(const GameObject* obj) {
    gZoneEntered = true;
    REQUIRE(obj->name == "TestObject");
}

void ZoneExitCallback(const GameObject* obj) {
    gZoneExited = true;
    REQUIRE(obj->name == "TestObject");
}
} // namespace

TEST_CASE("Test creating a zone and triggering callbacks", "[ECS][Zones]") {
    SYN_STARTENGINE

    // Create a zone
    const Math::Vector3 pos;
    const Math::Vector3 size(5.0f);
    auto&               zonego = GameObjectRegistry::CreateGameObject(
        "TestZone", "default", std::vector<std::string>{});
    auto* testZone =
        zonego.AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size, true);

    gZoneEntered = false;
    gZoneExited  = false;

    testZone->OnEnter = ZoneEnterCallback;

    testZone->OnExit = ZoneExitCallback;

    // Create an object that will enter and exit the zone
    auto& objgo = GameObjectRegistry::CreateGameObject(
        "TestObject", "default", std::vector<std::string>{});
    auto* transform = objgo.AddComponent<TransformComponent>();
    transform->SetPosition(SVec3(10.0f, 0.0f, 0.0f));

    int framesToSimulate = 10;
    engine.SetSimulationState(true);
    for (int i = 0; i < framesToSimulate; ++i) {
        // Move the object towards the zone
        float newX =
            10.0f -
            (i + 1) * (20.0f /
                       framesToSimulate); // Move from 10 to -10 over the frames
        transform->SetPosition(SVec3(newX, 0.0f, 0.0f));

        // Update the engine to process the zone triggers
        engine.HandleEvents();
        engine.Update();
    }

    // Ensure that the object entered and exited the zone
    REQUIRE(gZoneEntered);
    REQUIRE(gZoneExited);

    GameObjectRegistry::RemoveGameObject(&zonego);
    GameObjectRegistry::RemoveGameObject(&objgo);
    engine.Update();
}

TEST_CASE("Test querying objects in a zone", "[ECS][Zones]") {
    SYN_STARTENGINE

    // Create a zone
    const SVec3 pos;
    const SVec3 size(5.0f);
    auto&       zonego = GameObjectRegistry::CreateGameObject(
        "TestZone", "default", std::vector<std::string>{});
    auto* testZone =
        zonego.AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size);

    // Create objects inside and outside the zone
    auto& insideObj1 = GameObjectRegistry::CreateGameObject(
        "InsideObject1", "default", std::vector<std::string>{});
    auto& insideObj2 = GameObjectRegistry::CreateGameObject(
        "InsideObject2", "default", std::vector<std::string>{});
    auto& outsideObj = GameObjectRegistry::CreateGameObject(
        "OutsideObject", "default", std::vector<std::string>{});

    auto* transform1 = insideObj1.AddComponent<TransformComponent>();
    auto* transform2 = insideObj2.AddComponent<TransformComponent>();
    auto* transform3 = outsideObj.AddComponent<TransformComponent>();

    transform1->SetPosition(SVec3(1.0f, 0.0f, 0.0f));  // Inside
    transform2->SetPosition(SVec3(-1.0f, 0.0f, 0.0f)); // Inside
    transform3->SetPosition(SVec3(10.0f, 0.0f, 0.0f)); // Outside

    // Update the engine to process the zone triggers
    engine.HandleEvents();
    engine.Update();

    // Query objects in the zone
    auto objectsInZone = testZone->GetObjectsInZone();

    REQUIRE(objectsInZone.size() == 2);
    REQUIRE((objectsInZone[0]->name == "InsideObject1" ||
             objectsInZone[1]->name == "InsideObject1"));
    REQUIRE((objectsInZone[0]->name == "InsideObject2" ||
             objectsInZone[1]->name == "InsideObject2"));

    GameObjectRegistry::RemoveGameObject(&zonego);
    GameObjectRegistry::RemoveGameObject(&insideObj1);
    GameObjectRegistry::RemoveGameObject(&insideObj2);
    GameObjectRegistry::RemoveGameObject(&outsideObj);
    engine.Update();
}

TEST_CASE("Test zone tags", "[ECS][Zones]") {
    SYN_STARTENGINE

    // Create a zone
    const Math::Vector3 pos;
    const Math::Vector3 size(5.0f);
    auto&               zonego = GameObjectRegistry::CreateGameObject(
        "TestZone", "default", std::vector<std::string>{});
    auto* testZone =
        zonego.AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size);

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

    GameObjectRegistry::RemoveGameObject(&zonego);
    engine.Update();
}
