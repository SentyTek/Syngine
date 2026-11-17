// ╒══════════════════════ SceneData.cpp ═╕
// │ Syngine                              │
// │ Created 2026-11-07                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include "../defines.h"

#include <Syngine/GameObjects/Components/TransformComponent.h>
#include <Syngine/GameObjects/Components/RigidbodyComponent.h>
#include <Syngine/GameObjects/ComponentRegistry.h>
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/Scene/Scene.h>
#include <Syngine/Utils/Serializer.h>
#include <Syngine/Utils/UUID.h>

#include <filesystem>
#include <unordered_set>

using namespace Syngine;

TEST_CASE("UUIDs are valid version 4 identifiers", "[UUID]") {
    std::unordered_set<std::string> values;
    for (int i = 0; i < 1000; ++i) {
        std::string id = GenerateUUID().ToString();
        REQUIRE(id.size() == 36);
        REQUIRE(id[8] == '-');
        REQUIRE(id[13] == '-');
        REQUIRE(id[18] == '-');
        REQUIRE(id[23] == '-');
        REQUIRE(id[14] == '4');
        REQUIRE(
            (id[19] == '8' || id[19] == '9' || id[19] == 'a' || id[19] == 'b'));
        REQUIRE(values.insert(id).second);
    }
}

TEST_CASE("DataNode quaternion round trips", "[Serializer]") {
    Serializer::DataNode node;
    node       = Math::Quaternion(0.1f, 0.2f, 0.3f, 0.4f);
    auto value = node.As<Math::Quaternion>();
    REQUIRE(value.x() == 0.1f);
    REQUIRE(value.y() == 0.2f);
    REQUIRE(value.z() == 0.3f);
    REQUIRE(value.w() == 0.4f);
    Serializer::DataNode empty;
    empty.Set(Serializer::DataNode::NodeArray{});
    const Math::Quaternion fallback(1.0f, 0.0f, 0.0f, 0.0f);
    REQUIRE(empty.As<Math::Quaternion>(fallback).x() == 1.0f);
}

TEST_CASE("Object serialization retains authored identity and enabled state",
          "[Serializer]") {
    GameObject original("Object", "default", {});
    auto*      transform = original.AddComponent<TransformComponent>();
    transform->SetEnabled(false);
    const auto node = original.Serialize();
    GameObject restored(node);
    REQUIRE(restored.GetPersistentID() == original.GetPersistentID());
    REQUIRE(restored.GetID() != original.GetID());
    REQUIRE_FALSE(restored.GetComponent<TransformComponent>()->IsEnabled());
}

TEST_CASE("Rigidbody serialization retains authored compound parameters",
          "[Serializer]") {
    GameObject          owner("Object", "default", {});
    RigidbodyParameters parameters;
    parameters.shape      = PhysicsShapes::COMPOUND;
    parameters.motionType = JPH::EMotionType::Kinematic;
    parameters.layer      = Layers::NON_MOVING;
    parameters.mass       = 4.0f;
    parameters.compoundParts.push_back(
        { .shape           = PhysicsShapes::BOX,
          .shapeParameters = Math::Vector3(1.0f, 2.0f, 3.0f),
          .position        = Math::Vector3(4.0f, 5.0f, 6.0f),
          .rotation        = Math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f) });
    RigidbodyComponent original(&owner, parameters);
    auto               restored = ComponentRegistry::Instantiate(
        SYN_COMPONENT_RIGIDBODY, &owner, original.Serialize());
    REQUIRE(restored);
    const auto* rigidbody = static_cast<RigidbodyComponent*>(restored.get());
    auto        result    = rigidbody->GetCurrentParameters();
    REQUIRE(result.motionType == parameters.motionType);
    REQUIRE(result.layer == parameters.layer);
    REQUIRE(result.mass == parameters.mass);
    REQUIRE(result.compoundParts.size() == 1);
    REQUIRE(result.compoundParts[0].shape == PhysicsShapes::BOX);
    REQUIRE(result.compoundParts[0].shapeParameters.y() == 2.0f);
    REQUIRE(result.compoundParts[0].position.z() == 6.0f);
    REQUIRE(result.compoundParts[0].rotation.w() == 1.0f);
    parameters.mass = 8.0f;
    original.SetCurrentParameters(parameters);
    REQUIRE(original.Serialize()["mass"].As<float>() == 8.0f);
}

TEST_CASE("Scene data round trips typed nested values", "[Serializer][Scene]") {
    SYN_STARTENGINE;
    GameObjectRegistry::Clear();

    GameObject& object = GameObjectRegistry::CreateGameObject(
        "One <two>", "default", { "tag & value" });
    object.SetActive(false);
    auto* transform = object.AddComponent<TransformComponent>();
    transform->SetPosition(Math::Vector3(1.25f, 2.5f, 3.75f));

    SceneInstance scene("Scene & \"test\"");
    scene.SetPersistent(true);
    scene.AddGameObject(object);

    Serializer::Scene serialized;
    serialized.name       = scene.GetName();
    serialized.uuid       = scene.GetUUID();
    serialized.persistent = scene.IsPersistent();
    serialized.gameObjects.Set(Serializer::DataNode::NodeArray{});
    Serializer::Scene inMemory;
    inMemory.Deserialize(serialized.Serialize());
    REQUIRE(inMemory.isValid);
    REQUIRE(inMemory.uuid == scene.GetUUID());

    const auto path = std::filesystem::temp_directory_path() /
                      ("syngine-scene-" + GenerateUUID().ToString() + ".xml");
    REQUIRE(Serializer::Scene::SaveToFile(path.string(), scene));

    const auto data = Serializer::Scene::LoadFromFile(path.string());
    REQUIRE(data.isValid);
    REQUIRE(data.name == "Scene & \"test\"");
    REQUIRE(data.uuid == scene.GetUUID());
    REQUIRE(data.persistent);
    REQUIRE(data.gameObjects.IsArray());
    REQUIRE(data.gameObjects.Size() == 1);

    SceneInstance loaded(data, "", path.string());
    REQUIRE(loaded.GetName() == scene.GetName());
    REQUIRE(loaded.GetPath() == path.string());
    REQUIRE(loaded.GetBundle().empty());
    REQUIRE(loaded.GetGameObjects().size() == 1);

    const auto* restored = loaded.GetGameObject(loaded.GetGameObjects()[0]);
    REQUIRE(restored != nullptr);
    REQUIRE(restored->name == "One <two>");
    REQUIRE_FALSE(restored->IsActive());
    REQUIRE(restored->HasTag("tag & value"));
    const auto* restoredTransform = restored->GetComponent<TransformComponent>();
    REQUIRE(restoredTransform != nullptr);
    REQUIRE(restoredTransform->GetPosition().x() == 1.25f);
    REQUIRE(restoredTransform->GetPosition().y() == 2.5f);
    REQUIRE(restoredTransform->GetPosition().z() == 3.75f);

    GameObjectRegistry::Clear();
    std::filesystem::remove(path);
}

TEST_CASE("Invalid scene data is rejected without changing its destination",
          "[Serializer][Scene]") {
    Serializer::Scene scene;
    scene.name = "original";
    Serializer::DataNode invalid;

    REQUIRE_NOTHROW(scene.Deserialize(invalid));
    REQUIRE(scene.name == "original");
    REQUIRE_FALSE(scene.isValid);
}

TEST_CASE("Scene save replaces a file without losing it on validation errors",
          "[Serializer][Scene]") {
    SYN_STARTENGINE;
    GameObjectRegistry::Clear();

    GameObject& object =
        GameObjectRegistry::CreateGameObject("Original", "default", {});
    SceneInstance original("first");
    original.AddGameObject(object);

    const auto path = std::filesystem::temp_directory_path() /
                      ("syngine-scene-replace-" +
                       GenerateUUID().ToString() + ".xml");
    REQUIRE(Serializer::Scene::SaveToFile(path.string(), original));

    SceneInstance invalid("empty");
    REQUIRE_FALSE(Serializer::Scene::SaveToFile(path.string(), invalid));

    const auto loaded = Serializer::Scene::LoadFromFile(path.string());
    REQUIRE(loaded.isValid);
    REQUIRE(loaded.name == "first");
    REQUIRE_FALSE(loaded.persistent);

    GameObjectRegistry::Clear();
    std::filesystem::remove(path);
}

TEST_CASE("Scene save round trips nested GameObjects", "[Serializer][Scene]") {
    SYN_STARTENGINE;
    GameObjectRegistry::Clear();

    GameObject& parent =
        GameObjectRegistry::CreateGameObject("Parent", "default", {});
    GameObject& child =
        GameObjectRegistry::CreateGameObject("Child", "default", {});
    parent.AddComponent<TransformComponent>();
    child.AddComponent<TransformComponent>();
    parent.AddChild(&child);

    SceneInstance scene("hierarchy");
    scene.AddGameObject(parent);

    const auto path = std::filesystem::temp_directory_path() /
                      ("syngine-scene-hierarchy-" +
                       GenerateUUID().ToString() + ".xml");
    REQUIRE(Serializer::Scene::SaveToFile(path.string(), scene));

    const auto data = Serializer::Scene::LoadFromFile(path.string());
    REQUIRE(data.isValid);
    SceneInstance loaded(data, "", path.string());
    REQUIRE(loaded.GetGameObjects().size() == 1);
    const auto* restoredParent =
        loaded.GetGameObject(loaded.GetGameObjects()[0]);
    REQUIRE(restoredParent != nullptr);
    REQUIRE(restoredParent->GetChildren().size() == 1);
    REQUIRE(restoredParent->GetChildren()[0]->name == "Child");

    GameObjectRegistry::Clear();
    std::filesystem::remove(path);
}
