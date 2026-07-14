// ╒════════════════ ObjectRoundTrip.cpp ═╕
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
using namespace Syngine::Math;
using namespace Catch::Matchers;

// Test that a GameObject can be serialized and deserialized correctly
TEST_CASE("GameObject serialization round-trip", "[Serializer]") {
    // Create a GameObject with some components
    auto* go = new GameObject("TestObject", "default", "test_tag");
    go->AddTag("2");
    auto* t = go->AddComponent<TransformComponent>();
    t->SetPosition(SVec3(1.0f, 2.0f, 3.0f));
    t->SetRotationEuler(SVec3(45.0f, 80.0f, 0.0f).toRads());
    t->SetScale(SVec3(1.0f, 2.0f, 3.0f));

    // Serialize
    Serializer::Prefab prefab(go);

    delete go; // Delete original object to ensure we're not just comparing the
               // same instance

    // Deserialize
    auto* deserializedGo = new GameObject(prefab.rootGameObjectData);

    // Check that the deserialized GameObject has the same properties
    REQUIRE(deserializedGo->name == "TestObject");
    REQUIRE(deserializedGo->tags[0] == "test_tag");
    REQUIRE(deserializedGo->tags[1] == "2");

    // Check TransformComponent values
    auto* deserializedTransform =
        deserializedGo->GetComponent<TransformComponent>();
    REQUIRE(deserializedTransform != nullptr);
    REQUIRE_THAT(deserializedTransform->GetPosition().x(),
                 WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(deserializedTransform->GetPosition().y(),
                 WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(deserializedTransform->GetPosition().z(),
                 WithinAbs(3.0f, FLOAT_MARGIN));

    SVec3 localRotationEuler = deserializedTransform->GetRotationEuler().toDegs();
    REQUIRE_THAT(localRotationEuler.x(), WithinAbs(45.0f, FLOAT_MARGIN));
    REQUIRE_THAT(localRotationEuler.y(), WithinAbs(80.0f, FLOAT_MARGIN));
    REQUIRE_THAT(localRotationEuler.z(), WithinAbs(0.0f, FLOAT_MARGIN));

    REQUIRE_THAT(deserializedTransform->GetScale().x(),
                 WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(deserializedTransform->GetScale().y(),
                 WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(deserializedTransform->GetScale().z(),
                 WithinAbs(3.0f, FLOAT_MARGIN));

    // Clean up
    delete deserializedGo;
}

TEST_CASE("GameObject serialization with children", "[Serializer]") {
    // Create parent GameObject
    auto* parent          = new GameObject("Parent", "default");
    auto* parentTransform = parent->AddComponent<TransformComponent>();
    parentTransform->SetPosition(SVec3(0.0f, 1.0f, 0.0f));

    // Create child GameObject
    auto* child          = new GameObject("Child", "default");
    auto* childTransform = child->AddComponent<TransformComponent>();
    childTransform->SetPosition(SVec3(1.0f, 0.0f, 0.0f));

    // Set up hierarchy
    parent->AddChild(child);

    // Serialize
    Serializer::Prefab prefab(parent);

    delete parent; // Delete original objects

    // Deserialize
    auto* deserializedParent = new GameObject(prefab.rootGameObjectData);

    // Check parent properties
    REQUIRE(deserializedParent->name == "Parent");
    auto* deserializedParentTransform =
        deserializedParent->GetComponent<TransformComponent>();
    REQUIRE(deserializedParentTransform != nullptr);
    REQUIRE_THAT(deserializedParentTransform->GetPosition().y(),
                 WithinAbs(1.0f, FLOAT_MARGIN));

    // Check child properties
    REQUIRE(deserializedParent->GetChildren().size() == 1);
    auto* deserializedChild = deserializedParent->GetChildren().front();

    REQUIRE(deserializedChild->name == "Child");
    auto* deserializedChildTransform =
        deserializedChild->GetComponent<TransformComponent>();
    REQUIRE(deserializedChildTransform != nullptr);
    REQUIRE_THAT(deserializedChildTransform->GetPosition().x(),
                 WithinAbs(1.0f, FLOAT_MARGIN));

    // Clean up
    delete deserializedParent;
}

TEST_CASE("Saving and loading a GameObject tree from disk", "[Serializer]") {
    // Pretty much the same as the first test, but we save to disk and load from disk
    auto* go = new GameObject("DiskTestObject", "default", "disk_tag");
    go->AddTag("disk_tag_2");
    auto* t = go->AddComponent<TransformComponent>();
    t->SetPosition(SVec3(4.0f, 5.0f, 6.0f));

    auto* child = new GameObject("ChildOfDiskTestObject");
    child->AddComponent<TransformComponent>()->SetPosition(SVec3(1.0f, 2.0f, 3.0f));
    go->AddChild(child);

    // Serialize to file
    Serializer::Prefab prefab(go);
    prefab.SaveToFile("syn_serial_test.xml");
    delete child;
    delete go;

    // Load from file
    Serializer::Prefab loadedPrefab("syn_serial_test.xml");
    auto* loadedGo = new GameObject(loadedPrefab.rootGameObjectData);

    REQUIRE(loadedGo->name == "DiskTestObject");
    REQUIRE(loadedGo->tags[0] == "disk_tag");
    REQUIRE(loadedGo->tags[1] == "disk_tag_2");

    // Check transform
    auto* loadedTransform = loadedGo->GetComponent<TransformComponent>();
    REQUIRE(loadedTransform != nullptr);
    REQUIRE_THAT(loadedTransform->GetWorldPosition().x(),
                 WithinAbs(4.0f, FLOAT_MARGIN));
    REQUIRE_THAT(loadedTransform->GetWorldPosition().y(),
                 WithinAbs(5.0f, FLOAT_MARGIN));
    REQUIRE_THAT(loadedTransform->GetWorldPosition().z(),
                 WithinAbs(6.0f, FLOAT_MARGIN));

    // Check child
    REQUIRE(loadedGo->GetChildren().size() == 1);
    auto* loadedChild = loadedGo->GetChildren().front();
    REQUIRE(loadedChild->name == "ChildOfDiskTestObject");
    auto* loadedChildTransform = loadedChild->GetComponent<TransformComponent>();
    REQUIRE(loadedChildTransform != nullptr);
    REQUIRE_THAT(loadedChildTransform->GetPosition().x(),
                 WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(loadedChildTransform->GetPosition().y(),
                 WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(loadedChildTransform->GetPosition().z(),
                 WithinAbs(3.0f, FLOAT_MARGIN));

    // Clean up
    delete loadedGo;
    delete loadedChild;
}
