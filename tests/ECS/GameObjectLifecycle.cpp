// ╒════════════ GameObjectLifecycle.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <algorithm>
#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"

#include <Syngine/Core/Registry.h>
#include <Syngine/ECS/GameObject.h>
#include <Syngine/ECS/AllComponents.h>

using namespace Syngine;
using namespace Catch::Matchers;

// Collection of tests for GameObject lifecycle management

// Tests for component lifecycle management, including adding, removing, and
// copying components on GameObjects. Also tests that the Registry correctly
// reflects component membership in its queries.
TEST_CASE("ECS component lifecycle", "[ECS]") {
    const auto baselineCount = Registry::GetGameObjectCount();

    auto* go = new GameObject("LifecycleObject", "default", "spawned");
    REQUIRE(Registry::GetGameObjectCount() == baselineCount + 1);

    auto* transform = go->AddComponent<TransformComponent>();
    REQUIRE(transform != nullptr);
    REQUIRE(go->GetComponentCount() == 1);
    REQUIRE(go->HasComponent(SYN_COMPONENT_TRANSFORM));
    REQUIRE(go->GetComponent<TransformComponent>() == transform);
    REQUIRE(transform->m_owner == go);

    REQUIRE(go->AddComponent<TransformComponent>() == nullptr);
    REQUIRE(go->GetComponentCount() == 1);

    REQUIRE(go->RemoveComponent(SYN_COMPONENT_TRANSFORM));
    REQUIRE(!go->HasComponent(SYN_COMPONENT_TRANSFORM));
    REQUIRE(go->GetComponent<TransformComponent>() == nullptr);
    REQUIRE(go->GetComponentCount() == 0);
    REQUIRE(!go->RemoveComponent(SYN_COMPONENT_TRANSFORM));
    delete go;
}

// Tests for copy construction and copy assignment of GameObjects, ensuring that
// components are properly cloned and that the new GameObject has its own copies
// of components rather than sharing them with the original.
TEST_CASE("ECS registry queries reflect component membership", "[ECS]") {
    const auto baselineCount = Registry::GetGameObjectCount();

    {
        auto* transformObject = new GameObject("TransformObject", "npc", "ai");
        auto* plainObject     = new GameObject("PlainObject", "npc", "ai");

        auto* transform = transformObject->AddComponent<TransformComponent>();
        REQUIRE(transform != nullptr);

        REQUIRE(Registry::GetGameObjectByName("TransformObject") ==
                transformObject);
        REQUIRE(Registry::GetGameObjectById(transformObject->GetID()) ==
                transformObject);

        const auto byType = Registry::GetGameObjectsByType("npc");
        REQUIRE(std::find(byType.begin(), byType.end(), transformObject) !=
                byType.end());
        REQUIRE(std::find(byType.begin(), byType.end(), plainObject) !=
                byType.end());

        const auto withTransform =
            Registry::GetGameObjectsWithComponent(SYN_COMPONENT_TRANSFORM);
        REQUIRE(std::find(withTransform.begin(),
                          withTransform.end(),
                          transformObject) != withTransform.end());
        REQUIRE(std::find(withTransform.begin(),
                          withTransform.end(),
                          plainObject) == withTransform.end());
        delete transformObject;
        delete plainObject;
    }

    REQUIRE(Registry::GetGameObjectCount() == baselineCount);
}

// Tests for copy construction and copy assignment of GameObjects, ensuring that
// components are properly cloned and that the new GameObject has its own copies
// of components rather than sharing them with the original.
TEST_CASE("ECS copy construction clones components", "[ECS]") {
    auto* original = new GameObject("Original", "default", "root");
    original->AddTag("gameplay");
    original->SetActive(false);

    auto* originalTransform = original->AddComponent<TransformComponent>();
    REQUIRE(originalTransform != nullptr);
    originalTransform->SetPosition(4.0f, 5.0f, 6.0f);

    const auto originalId = original->GetID();

    GameObject copy(*original);

    REQUIRE(copy.GetID() != originalId);
    REQUIRE(copy.name == original->name);
    REQUIRE(copy.type == original->type);
    REQUIRE(copy.GetTags() == original->GetTags());
    REQUIRE(copy.IsActive() == original->IsActive());
    REQUIRE(copy.GetComponentCount() == original->GetComponentCount());

    auto* copyTransform = copy.GetComponent<TransformComponent>();
    REQUIRE(copyTransform != nullptr);
    REQUIRE(copyTransform != originalTransform);
    REQUIRE(copyTransform->m_owner == &copy);

    float* originalPosition = originalTransform->GetLocalPosition();
    float* copyPosition     = copyTransform->GetLocalPosition();
    REQUIRE_THAT(copyPosition[0], WithinAbs(originalPosition[0], FLOAT_MARGIN));
    REQUIRE_THAT(copyPosition[1], WithinAbs(originalPosition[1], FLOAT_MARGIN));
    REQUIRE_THAT(copyPosition[2], WithinAbs(originalPosition[2], FLOAT_MARGIN));

    copyTransform->SetPosition(9.0f, 8.0f, 7.0f);
    REQUIRE_THAT(originalTransform->GetLocalPosition()[0],
                 WithinAbs(4.0f, FLOAT_MARGIN));
    REQUIRE_THAT(originalTransform->GetLocalPosition()[1],
                 WithinAbs(5.0f, FLOAT_MARGIN));
    REQUIRE_THAT(originalTransform->GetLocalPosition()[2],
                 WithinAbs(6.0f, FLOAT_MARGIN));
}

// Tests for copy assignment of GameObjects, ensuring that components are
// properly cloned and that the new GameObject has its own copies of components
// rather than sharing them with the original.
TEST_CASE("ECS copy assignment clones components", "[ECS]") {
    auto* source = new GameObject("Source", "default", "root");
    source->AddTag("shared");
    source->SetActive(false);

    auto* sourceTransform = source->AddComponent<TransformComponent>();
    REQUIRE(sourceTransform != nullptr);
    sourceTransform->SetPosition(2.0f, 3.0f, 4.0f);

    GameObject target("Target", "default", "temporary");
    auto* targetTransformBefore = target.AddComponent<TransformComponent>();
    REQUIRE(targetTransformBefore != nullptr);
    targetTransformBefore->SetPosition(-1.0f, -2.0f, -3.0f);

    target = *source;

    REQUIRE(target.name == source->name);
    REQUIRE(target.type == source->type);
    REQUIRE(target.GetTags() == source->GetTags());
    REQUIRE(target.IsActive() == source->IsActive());

    auto* targetTransform = target.GetComponent<TransformComponent>();
    REQUIRE(targetTransform != nullptr);
    REQUIRE(targetTransform != sourceTransform);
    REQUIRE(targetTransform->m_owner == &target);
    REQUIRE_THAT(targetTransform->GetLocalPosition()[0],
                 WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(targetTransform->GetLocalPosition()[1],
                 WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(targetTransform->GetLocalPosition()[2],
                 WithinAbs(4.0f, FLOAT_MARGIN));
}

// Tests adding and removing tags from GameObjects, as well as setting the
// active state, and ensuring that the tags and active state are reflected
// correctly in the GameObject's properties
TEST_CASE("ECS tags and activation state behave predictably", "[ECS]") {
    GameObject go("TaggedObject", "default", "player");

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
    auto parent = std::make_unique<GameObject>("Parent", "scene", "root");
    auto child  = std::make_unique<GameObject>("Child", "scene", "leaf");

    auto* parentTransform = parent->AddComponent<TransformComponent>();
    auto* childTransform  = child->AddComponent<TransformComponent>();
    REQUIRE(parentTransform != nullptr);
    REQUIRE(childTransform != nullptr);

    parentTransform->SetPosition(1.0f, 2.0f, 3.0f);
    childTransform->SetPosition(4.0f, 5.0f, 6.0f);

    child->SetParent(parent.get());

    REQUIRE(child->GetParent() == parent.get());
    REQUIRE(std::find(parentTransform->GetChildren().begin(),
                      parentTransform->GetChildren().end(),
                      childTransform) != parentTransform->GetChildren().end());

    const auto serialized = parent->Serialize();

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

    child->SetParent(nullptr);
    REQUIRE(child->GetParent() == nullptr);
    REQUIRE(std::find(parentTransform->GetChildren().begin(),
                      parentTransform->GetChildren().end(),
                      childTransform) == parentTransform->GetChildren().end());
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

    auto* zoneGO = new GameObject("ZoneGO", "default");
    zoneGO->SetActive(false);
    float pos[3] = { 0.0f, -5.0f, 0.0f };
    float size[3] = { 3.0f, 3.0f, 3.0f };
    auto* zone = zoneGO->AddComponent<ZoneComponent>(ZoneShape::BOX, pos, size);
    zone->OnEnter = [&](GameObject* obj) {
        FAIL("Zone should not be entered when inactive");
    };

    auto* rbGO = CreateRigidbodyObject();
    rbGO->SetActive(false);

    auto* transformGO = new GameObject("TransformGO", "default");
    auto* transform   = transformGO->AddComponent<TransformComponent>();

    // Act: Move the transform GO into the zone over several frames and check
    // that the zone is not entered and the RB GO does not move
    engine.SetSimulationState(true);
    for (int i = 0; i < 60; ++i) {
        transform->SetPosition(0.0f, -5.0f + i * 1.0f, 0.0f);
        engine.HandleEvents();
        engine.Update();
    }

    // Check that the RB GO has not moved from its initial position
    auto* rbTransform = rbGO->GetComponent<TransformComponent>();
    REQUIRE_THAT(rbTransform->GetLocalPosition()[1],
                 WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(transform->GetPosition()[1], WithinAbs(54.0f, FLOAT_MARGIN));

    // Clean up
    delete zoneGO;
    delete rbGO;
    delete transformGO;
}
