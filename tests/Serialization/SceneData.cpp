// ╒══════════════════════ SceneData.cpp ═╕
// │ Syngine                              │
// │ Created 2026-11-07                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>

#include <Syngine/GameObjects/Components/TransformComponent.h>
#include <Syngine/GameObjects/Components/RigidbodyComponent.h>
#include <Syngine/GameObjects/ComponentRegistry.h>
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/Utils/Serializer.h>
#include <Syngine/Utils/UUID.h>

#include <bit>
#include <filesystem>
#include <fstream>
#include <unordered_set>

using namespace Syngine;

TEST_CASE("UUIDs are valid version 4 identifiers", "[UUID]") {
    std::unordered_set<std::string> values;
    for (int i = 0; i < 1000; ++i) {
        std::string id = GenerateUUID();
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
    Serializer::Scene scene;
    scene.name       = "Scene & \"test\"";
    scene.guid       = GenerateUUID();
    scene.persistent = true;
    scene.gameObjects.Set(Serializer::DataNode::NodeArray{});
    Serializer::DataNode object;
    object["name"]     = "One <two>";
    object["empty"]    = "";
    object["number"]   = 17;
    object["unsigned"] = uint64_t{ 18446744073709551615ULL };
    object["float"]    = std::bit_cast<float>(uint32_t{ 0x3f800001 });
    object["enabled"]  = false;
    object["emptyArray"].Set(Serializer::DataNode::NodeArray{});
    object["emptyObject"].Set(Serializer::DataNode::NodeMap{});
    scene.gameObjects.Append(object);
    scene.sceneSettings["lighting"] = "test";

    const auto path = std::filesystem::temp_directory_path() /
                      ("syngine-scene-" + GenerateUUID() + ".scene");
    REQUIRE(Serializer::Scene::SaveToFile(path.string(), scene));
    auto loaded = Serializer::Scene::LoadFromFile(path.string());
    std::filesystem::remove(path);
    REQUIRE(loaded.isValid);
    REQUIRE(loaded.name == scene.name);
    REQUIRE(loaded.guid == scene.guid);
    REQUIRE(loaded.persistent);
    REQUIRE(loaded.gameObjects.Size() == 1);
    const auto& value = loaded.gameObjects.At(0);
    REQUIRE(value["name"].As<std::string>() == "One <two>");
    REQUIRE(value["empty"].As<std::string>() == "");
    REQUIRE(value["number"].As<int>() == 17);
    REQUIRE(value["unsigned"].As<uint64_t>() == UINT64_MAX);
    REQUIRE(std::bit_cast<uint32_t>(value["float"].As<float>()) == 0x3f800001);
    REQUIRE_FALSE(value["enabled"].As<bool>());
    REQUIRE(value["emptyArray"].IsArray());
    REQUIRE(value["emptyObject"].IsObject());
    REQUIRE(loaded.sceneSettings["lighting"].As<std::string>() == "test");
}

TEST_CASE("Invalid scene data is rejected without changing its destination",
          "[Serializer][Scene]") {
    Serializer::Scene scene;
    scene.name = "original";
    Serializer::DataNode invalid;
    REQUIRE_THROWS(scene.Deserialize(invalid));
    REQUIRE(scene.name == "original");

    const auto path = std::filesystem::temp_directory_path() /
                      ("syngine-scene-" + GenerateUUID() + ".scene");
    {
        std::ofstream file(path);
        file << "<Scene Version=\"invalid\"/>";
    }
    REQUIRE_FALSE(Serializer::Scene::LoadFromFile(path.string()).isValid);

    {
        std::ofstream file(path);
        file << "<Scene Version=\"1.0\"><Node type=\"object\">";
        for (int i = 0; i < 200; ++i) {
            file << "<Node key=\"61\" type=\"null\"/>";
        }
        file << "</invalid></Scene>";
    }
    REQUIRE_FALSE(Serializer::Scene::LoadFromFile(path.string()).isValid);
    std::filesystem::remove(path);
}

TEST_CASE("Scene save replaces a file without losing it on validation errors",
          "[Serializer][Scene]") {
    const auto        path = std::filesystem::temp_directory_path() /
                             ("syngine-scene-" + GenerateUUID() + ".scene");
    Serializer::Scene scene;
    scene.name = "first";
    scene.guid = GenerateUUID();
    scene.gameObjects.Set(Serializer::DataNode::NodeArray{});
    REQUIRE(Serializer::Scene::SaveToFile(path.string(), scene));
    scene.name = "second";
    REQUIRE(Serializer::Scene::SaveToFile(path.string(), scene));
    scene.gameObjects = Serializer::DataNode{};
    REQUIRE_FALSE(Serializer::Scene::SaveToFile(path.string(), scene));
    auto loaded = Serializer::Scene::LoadFromFile(path.string());
    std::filesystem::remove(path);
    REQUIRE(loaded.isValid);
    REQUIRE(loaded.name == "second");
}

TEST_CASE("Scene save respects the node nesting limit", "[Serializer][Scene]") {
    const auto        path = std::filesystem::temp_directory_path() /
                             ("syngine-scene-" + GenerateUUID() + ".scene");
    Serializer::Scene scene;
    scene.name = "within limit";
    scene.guid = GenerateUUID();
    scene.gameObjects.Set(Serializer::DataNode::NodeArray{});

    auto* node = &scene.sceneSettings;
    for (size_t i = 0; i < 255; ++i) node = &(*node)["child"];
    *node = 42;
    REQUIRE(Serializer::Scene::SaveToFile(path.string(), scene));
    REQUIRE(Serializer::Scene::LoadFromFile(path.string()).isValid);

    *node            = Serializer::DataNode{};
    (*node)["child"] = 42;
    scene.name       = "exceeds limit";
    REQUIRE_FALSE(Serializer::Scene::SaveToFile(path.string(), scene));
    auto loaded = Serializer::Scene::LoadFromFile(path.string());
    std::filesystem::remove(path);
    REQUIRE(loaded.isValid);
    REQUIRE(loaded.name == "within limit");
}
