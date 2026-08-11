// ╒════════════ GameObjectLifecycle.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <algorithm>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"
#include "Syngine/Scene/GameObjectRegistry.h"

#include <Syngine/Syngine.h>

using namespace Syngine;
using namespace Syngine::Math;
using namespace Catch::Matchers;

// Collection of tests for GameObject lifecycle management

// Tests for component lifecycle management, including adding, removing, and
// copying components on GameObjects. Also tests that the Registry correctly
// reflects component membership in its queries.
TEST_CASE("ECS component lifecycle", "[ECS]") {
    const auto baselineCount = GameObjectRegistry::GetGameObjectCount();

    GameObject& go = GameObjectRegistry::CreateGameObject(
        "LifecycleObject", "default", std::vector<std::string>{ "spawned" });
    REQUIRE(GameObjectRegistry::GetGameObjectCount() == baselineCount + 1);

    auto* transform = go.AddComponent<TransformComponent>();
    REQUIRE(transform != nullptr);
    REQUIRE(go.GetComponentCount() == 1);
    REQUIRE(go.HasComponent(SYN_COMPONENT_TRANSFORM));
    REQUIRE(go.GetComponent<TransformComponent>() == transform);
    REQUIRE(transform->_GetOwner() == &go);

    REQUIRE(go.AddComponent<TransformComponent>() == nullptr);
    REQUIRE(go.GetComponentCount() == 1);

    REQUIRE(go.RemoveComponent(SYN_COMPONENT_TRANSFORM));
    REQUIRE(!go.HasComponent(SYN_COMPONENT_TRANSFORM));
    REQUIRE(go.GetComponent<TransformComponent>() == nullptr);
    REQUIRE(go.GetComponentCount() == 0);
    REQUIRE(!go.RemoveComponent(SYN_COMPONENT_TRANSFORM));
    // GameObjectRegistry handles deletion
}

// Tests for copy construction and copy assignment of GameObjects, ensuring that
// components are properly cloned and that the new GameObject has its own copies
// of components rather than sharing them with the original.
TEST_CASE("ECS registry queries reflect component membership", "[ECS]") {
    SYN_STARTENGINE

    const auto baselineCount = GameObjectRegistry::GetGameObjectCount();

    {
        auto& transformObject = GameObjectRegistry::CreateGameObject(
            "TransformObject", "npc", std::vector<std::string>{ "ai" });
        auto& plainObject = GameObjectRegistry::CreateGameObject(
            "PlainObject", "npc", std::vector<std::string>{ "ai" });

        auto* transform = transformObject.AddComponent<TransformComponent>();
        REQUIRE(transform != nullptr);

        REQUIRE(GameObjectRegistry::GetGameObjectByName("TransformObject") ==
                &transformObject);
        REQUIRE(GameObjectRegistry::GetGameObjectById(
                    transformObject.GetID()) == &transformObject);

        const auto byType = GameObjectRegistry::GetGameObjectsByType("npc");
        REQUIRE(std::find(byType.begin(), byType.end(), &transformObject) !=
                byType.end());
        REQUIRE(std::find(byType.begin(), byType.end(), &plainObject) !=
                byType.end());

        const auto withTransform =
            GameObjectRegistry::GetGameObjectsWithComponent(
                SYN_COMPONENT_TRANSFORM);
        REQUIRE(std::find(withTransform.begin(),
                          withTransform.end(),
                          &transformObject) != withTransform.end());
        REQUIRE(std::find(withTransform.begin(),
                          withTransform.end(),
                          &plainObject) == withTransform.end());

        GameObjectRegistry::RemoveGameObject(&transformObject);
        GameObjectRegistry::RemoveGameObject(&plainObject);
        engine.Update();
    }

    REQUIRE(GameObjectRegistry::GetGameObjectCount() == baselineCount);
}

// Tests for copy construction and copy assignment of GameObjects, ensuring that
// components are properly cloned and that the new GameObject has its own copies
// of components rather than sharing them with the original.
TEST_CASE("ECS copy construction clones components", "[ECS]") {
    SYN_STARTENGINE

    auto& original = GameObjectRegistry::CreateGameObject(
        "Original", "default", std::vector<std::string>{ "root" });
    original.AddTag("gameplay");
    original.SetActive(false);

    auto* originalTransform = original.AddComponent<TransformComponent>();
    REQUIRE(originalTransform != nullptr);
    originalTransform->SetPosition(Syngine::Math::Vec3(4.0f, 5.0f, 6.0f));

    const auto originalId = original.GetID();

    GameObject& copy = GameObjectRegistry::CloneGameObject(original);

    REQUIRE(copy.GetID() != originalId);
    REQUIRE(copy.name == original.name + "_clone");
    REQUIRE(copy.type == original.type);
    REQUIRE(copy.GetTags() == original.GetTags());
    REQUIRE(copy.IsActive() == original.IsActive());
    REQUIRE(copy.GetComponentCount() == original.GetComponentCount());

    auto* copyTransform = copy.GetComponent<TransformComponent>();
    REQUIRE(copyTransform != nullptr);
    REQUIRE(copyTransform != originalTransform);
    REQUIRE(copyTransform->_GetOwner() == &copy);

    Math::Vector3 originalPosition = originalTransform->GetPosition();
    Math::Vector3 copyPosition     = copyTransform->GetPosition();
    REQUIRE_THAT(copyPosition.x(),
                 WithinAbs(originalPosition.x(), FLOAT_MARGIN));
    REQUIRE_THAT(copyPosition.y(),
                 WithinAbs(originalPosition.y(), FLOAT_MARGIN));
    REQUIRE_THAT(copyPosition.z(),
                 WithinAbs(originalPosition.z(), FLOAT_MARGIN));

    copyTransform->SetPosition(Vector3(9.0f, 8.0f, 7.0f));
    REQUIRE_THAT(originalTransform->GetPosition().x(),
                 WithinAbs(4.0f, FLOAT_MARGIN));
    REQUIRE_THAT(originalTransform->GetPosition().y(),
                 WithinAbs(5.0f, FLOAT_MARGIN));
    REQUIRE_THAT(originalTransform->GetPosition().z(),
                 WithinAbs(6.0f, FLOAT_MARGIN));

    GameObjectRegistry::RemoveGameObject(&original);
    GameObjectRegistry::RemoveGameObject(&copy);
    engine.Update();
}

// Tests adding and removing tags from GameObjects, as well as setting the
// active state, and ensuring that the tags and active state are reflected
// correctly in the GameObject's properties
TEST_CASE("ECS tags and activation state behave predictably", "[ECS]") {
    GameObject& go = GameObjectRegistry::CreateGameObject(
        "TaggedObject", "default", std::vector<std::string>{ "player" });

    REQUIRE(go.IsActive());
    REQUIRE(go.HasTag("player"));

    go.AddTag("player");
    go.AddTag("boss");

    const auto tags = go.GetTags();
    REQUIRE(std::count(tags.begin(), tags.end(), "player") == 1);
    REQUIRE(std::count(tags.begin(), tags.end(), "boss") == 1);

    go.RemoveTag("player");
    REQUIRE(!go.HasTag("player"));
    REQUIRE(go.HasTag("boss"));

    go.ClearTags();
    REQUIRE(go.GetTags().empty());

    go.SetActive(false);
    REQUIRE(!go.IsActive());
    go.SetActive(true);
    REQUIRE(go.IsActive());
}

// Tests for parent-child relationships between GameObjects, ensuring that
// setting a parent correctly updates the child and parent GameObjects, and that
// this relationship is reflected in serialization.
TEST_CASE("ECS parenting is reflected in serialization", "[ECS]") {
    auto& parent = GameObjectRegistry::CreateGameObject(
        "Parent", "scene", std::vector<std::string>{ "root" });
    auto& child = GameObjectRegistry::CreateGameObject(
        "Child", "scene", std::vector<std::string>{ "leaf" });

    auto* parentTransform = parent.AddComponent<TransformComponent>();
    auto* childTransform  = child.AddComponent<TransformComponent>();
    REQUIRE(parentTransform != nullptr);
    REQUIRE(childTransform != nullptr);

    parentTransform->SetPosition(Math::Vector3(1.0f, 2.0f, 3.0f));
    childTransform->SetPosition(Math::Vector3(4.0f, 5.0f, 6.0f));

    child.SetParent(&parent);

    REQUIRE(child.GetParent() == &parent);
    REQUIRE(std::find(parentTransform->GetChildren().begin(),
                      parentTransform->GetChildren().end(),
                      childTransform) != parentTransform->GetChildren().end());

    const auto serialized = parent.Serialize();

    REQUIRE(serialized["name"].As<std::string>() == "Parent");
    REQUIRE(serialized["type"].As<std::string>() == "scene");
    REQUIRE(serialized["tags"].As<std::vector<std::string>>().size() == 1);
    REQUIRE(serialized["components"].Has("2"));
    REQUIRE(serialized["components"]["2"]["type"].As<uint64_t>() ==
            SYN_COMPONENT_TRANSFORM);
    REQUIRE(serialized.Has("children"));
    REQUIRE(serialized["children"].IsArray());
    REQUIRE(serialized["children"].Size() == 1);
    REQUIRE(serialized["children"].At(0)["name"].As<std::string>() == "Child");

    child.SetParent(nullptr);
    REQUIRE(child.GetParent() == nullptr);
    REQUIRE(std::find(parentTransform->GetChildren().begin(),
                      parentTransform->GetChildren().end(),
                      childTransform) == parentTransform->GetChildren().end());
}

void entercb(const GameObject* obj) {
    FAIL("Zone should not be entered when inactive");
}

// Tests that an inactive GameObject does not have its components updated during
// the engine update cycle, and that activating the GameObject allows its
// components to be updated as expected.
TEST_CASE("ECS inactive GameObjects do not update components", "[ECS]") {
    SYN_STARTENGINE

    // This test creates 3 GOs: one with physics, one with a transform, and one
    // with a zone. The zone and RB one are disabled. We then move the transform
    // GO into the zone and check that the zone's OnEnter callback is not called
    // because the zone GO is inactive. We also check that the RB GO does not
    // move because it is inactive.

    auto& zoneGO = GameObjectRegistry::CreateGameObject(
        "ZoneGO", "default", std::vector<std::string>{});
    zoneGO.SetActive(false);
    const Math::Vector3 pos(0.0f, -5.0f, 0.0f);
    const Math::Vector3 size(3.0f, 3.0f, 3.0f);
    auto* zone = zoneGO.AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size);
    zone->OnEnter = entercb;

    auto& rbGO = CreateRigidbodyObject();
    rbGO.SetActive(false);

    auto& transformGO = GameObjectRegistry::CreateGameObject(
        "TransformGO", "default", std::vector<std::string>{});
    auto* transform = transformGO.AddComponent<TransformComponent>();

    // Act: Move the transform GO into the zone over several frames and check
    // that the zone is not entered and the RB GO does not move
    engine.SetSimulationState(true);
    for (int i = 0; i < 60; ++i) {
        transform->SetPosition(Vector3(0.0f, -5.0f + i * 1.0f, 0.0f));
        engine.HandleEvents();
        engine.Update();
    }

    // Check that the RB GO has not moved from its initial position
    auto* rbTransform = rbGO.GetComponent<TransformComponent>();
    REQUIRE_THAT(rbTransform->GetPosition().y(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(transform->GetPosition().y(), WithinAbs(54.0f, FLOAT_MARGIN));
}
