// ╒════════════════ ObjectRoundTrip.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"

#include <Syngine/Syngine.h>
#include <Syngine/GameObjects/ComponentRegistry.h>
#include <Syngine/Scene/Scene.h>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iterator>
using namespace Syngine;
using namespace Syngine::Math;
using namespace Catch::Matchers;

TEST_CASE("Scene XML loads root GameObjects", "[Serializer][Scene]") {
    const auto base       = std::filesystem::temp_directory_path() /
                            ("syngine-scene-" + GenerateUUID().ToString());
    const auto xmlPath    = base.string() + ".xml";
    const auto bundlePath = base.string() + ".spk";
    {
        std::ofstream file(xmlPath);
        REQUIRE(file);
        file
            << R"(<Scene Version="1.0">)"
               R"(<uuid>0d7fea47-e7f1-4c04-8eac-384b00c19ffa</uuid>)"
               R"(<persistent>false</persistent><gameObjects>)"
               R"(<gameObject name="First" type="default" gizmo="none" isActive="true"/>)"
               R"(<gameObject name="Second" type="default" gizmo="none" isActive="false"/>)"
               R"(</gameObjects></Scene>)";
    }

    scl::pack::Packager pack;
    REQUIRE(pack.open(scl::path(bundlePath.c_str())));
    auto* entry = pack.openFile(scl::path(xmlPath.c_str()));
    REQUIRE(entry != nullptr);
    const std::string assetPath = entry->filepath().cstr();
    entry->submit();
    REQUIRE(pack.write());
    pack.close();

    const auto scene = Serializer::Scene::LoadFromFile(assetPath, bundlePath);
    std::filesystem::remove(bundlePath);
    std::filesystem::remove(xmlPath);
    REQUIRE(scene.isValid);
    REQUIRE(scene.gameObjects.IsArray());
    REQUIRE(scene.gameObjects.Size() == 2);
    REQUIRE(scene.gameObjects.At(0).IsObject());
    REQUIRE(scene.gameObjects.At(0)["name"].As<std::string>() == "First");
    REQUIRE(scene.gameObjects.At(1).IsObject());
    REQUIRE(scene.gameObjects.At(1)["name"].As<std::string>() == "Second");

    SceneInstance instance(scene, bundlePath, assetPath);
    REQUIRE(instance.GetGameObjects().size() == 2);
    const auto* first  = instance.GetGameObject(instance.GetGameObjects()[0]);
    const auto* second = instance.GetGameObject(instance.GetGameObjects()[1]);
    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);
    REQUIRE(first->name == "First");
    REQUIRE(second->name == "Second");
    REQUIRE(first->IsActive());
    REQUIRE_FALSE(second->IsActive());
    GameObjectRegistry::Clear();
}

// Test that a GameObject can be serialized and deserialized correctly
TEST_CASE("GameObject serialization round-trip", "[Serializer]") {
    // Create a GameObject with some components
    auto& go = GameObjectRegistry::CreateGameObject(
        "TestObject", "default", std::vector<std::string>{ "test_tag" });
    go.AddTag("2");
    auto* t = go.AddComponent<TransformComponent>();
    t->SetPosition(SVec3(1.0f, 2.0f, 3.0f));
    t->SetRotationEuler(SVec3(45.0f, 80.0f, 0.0f).toRads());
    t->SetScale(SVec3(1.0f, 2.0f, 3.0f));

    // Serialize
    Serializer::Prefab prefab(&go);

    GameObjectRegistry::Clear(); // Delete original object to ensure we're not
                                 // just comparing the same instance

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

    SVec3 localRotationEuler =
        deserializedTransform->GetRotationEuler().toDegs();
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
    auto& parent = GameObjectRegistry::CreateGameObject(
        "Parent", "default", std::vector<std::string>{});
    auto* parentTransform = parent.AddComponent<TransformComponent>();
    parentTransform->SetPosition(SVec3(0.0f, 1.0f, 0.0f));

    // Create child GameObject
    auto& child = GameObjectRegistry::CreateGameObject(
        "Child", "default", std::vector<std::string>{});
    auto* childTransform = child.AddComponent<TransformComponent>();
    childTransform->SetPosition(SVec3(1.0f, 0.0f, 0.0f));

    // Set up hierarchy
    parent.AddChild(&child);

    // Serialize
    Serializer::Prefab prefab(&parent);

    GameObjectRegistry::Clear(); // Delete original objects

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
    // Pretty much the same as the first test, but we save to disk and load from
    // disk
    auto& go = GameObjectRegistry::CreateGameObject(
        "DiskTestObject", "default", std::vector<std::string>{ "disk_tag" });
    go.AddTag("disk_tag_2");
    auto* t = go.AddComponent<TransformComponent>();
    t->SetPosition(SVec3(4.0f, 5.0f, 6.0f));

    auto& child = GameObjectRegistry::CreateGameObject(
        "ChildOfDiskTestObject", "default", std::vector<std::string>{});
    child.AddComponent<TransformComponent>()->SetPosition(
        SVec3(1.0f, 2.0f, 3.0f));
    go.AddChild(&child);

    // Serialize to file
    Serializer::Prefab prefab(&go);
    prefab.SaveToFile("syn_serial_test.xml");
    GameObjectRegistry::Clear();

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
    auto* loadedChildTransform =
        loadedChild->GetComponent<TransformComponent>();
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

TEST_CASE("Compound rigidbody prefab XML round trip", "[Serializer][Physics]") {
    RigidbodyParameters params;
    params.shape    = PhysicsShapes::COMPOUND;
    params.friction = 0.12345678f;
    params.compoundParts.push_back(
        { .shape           = PhysicsShapes::BOX,
          .shapeParameters = SVec3(1.00000012f, 2.0f, 3.0f),
          .position        = SVec3(-4.0f, 5.0f, 6.0f),
          .rotation        = SQuat(0.1f, 0.2f, 0.3f, 0.9f) });
    SECTION("one part") {}
    SECTION("multiple parts") {
        params.compoundParts.push_back(
            { .shape           = PhysicsShapes::SPHERE,
              .shapeParameters = SVec3(0.5f, 0.0f, 0.0f),
              .position        = SVec3(7.0f, -8.0f, 9.0f),
              .rotation        = SQuat(-0.2f, 0.4f, -0.5f, 0.7f) });
    }

    GameObject original("Compound", "default", {});
    original.AddComponent<TransformComponent>()->SetPosition(
        SVec3(0.12345678f, 0.0f, 0.0f));
    original.AddComponent<RigidbodyComponent>(params);
    Serializer::Prefab prefab(&original);
    const auto         path =
        std::filesystem::temp_directory_path() /
        ("syngine-compound-" + GenerateUUID().ToString() + ".xml");
    REQUIRE(prefab.SaveToFile(path.string()));
    std::ifstream     file(path);
    const std::string xml((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    file.close();
    REQUIRE(xml.find("compoundParts=\"1:") != std::string::npos);
    REQUIRE((xml.find('|') != std::string::npos) ==
            (params.compoundParts.size() == 2));
#if SYN_SERIALIZER_FLOAT_DECIMALS == 6
    REQUIRE(xml.find("friction=\"0.123457\"") != std::string::npos);
    REQUIRE(xml.find("position=\"0.123457,0,0\"") != std::string::npos);
    REQUIRE(xml.find("compoundParts=\"1:1,2,3:") != std::string::npos);
#elif SYN_SERIALIZER_FLOAT_DECIMALS == 2
    REQUIRE(xml.find("friction=\"0.12\"") != std::string::npos);
    REQUIRE(xml.find("position=\"0.12,0,0\"") != std::string::npos);
    REQUIRE(xml.find("compoundParts=\"1:1,2,3:") != std::string::npos);
#elif SYN_SERIALIZER_FLOAT_DECIMALS == 0
    REQUIRE(xml.find("friction=\"0\"") != std::string::npos);
    REQUIRE(xml.find("position=\"0,0,0\"") != std::string::npos);
    REQUIRE(xml.find("compoundParts=\"1:1,2,3:") != std::string::npos);
#endif

    Serializer::Prefab loaded(path.string());
    std::filesystem::remove(path);
    REQUIRE(loaded.isValid);
    REQUIRE(loaded.rootGameObject != nullptr);
    auto* body = loaded.rootGameObject->GetComponent<RigidbodyComponent>();
    REQUIRE(body != nullptr);
    const auto result = body->GetCurrentParameters();
    REQUIRE(result.shape == PhysicsShapes::COMPOUND);
    REQUIRE(result.compoundParts.size() == params.compoundParts.size());
    const double margin =
        std::pow(10.0, -SYN_SERIALIZER_FLOAT_DECIMALS) / 2.0 + 1e-7;
    for (size_t i = 0; i < params.compoundParts.size(); ++i) {
        const auto& expected = params.compoundParts[i];
        const auto& actual   = result.compoundParts[i];
        REQUIRE(actual.shape == expected.shape);
        REQUIRE_THAT(actual.shapeParameters.x(),
                     WithinAbs(expected.shapeParameters.x(), margin));
        REQUIRE_THAT(actual.shapeParameters.y(),
                     WithinAbs(expected.shapeParameters.y(), margin));
        REQUIRE_THAT(actual.shapeParameters.z(),
                     WithinAbs(expected.shapeParameters.z(), margin));
        REQUIRE_THAT(actual.position.x(),
                     WithinAbs(expected.position.x(), margin));
        REQUIRE_THAT(actual.position.y(),
                     WithinAbs(expected.position.y(), margin));
        REQUIRE_THAT(actual.position.z(),
                     WithinAbs(expected.position.z(), margin));
        REQUIRE_THAT(actual.rotation.x(),
                     WithinAbs(expected.rotation.x(), margin));
        REQUIRE_THAT(actual.rotation.y(),
                     WithinAbs(expected.rotation.y(), margin));
        REQUIRE_THAT(actual.rotation.z(),
                     WithinAbs(expected.rotation.z(), margin));
        REQUIRE_THAT(actual.rotation.w(),
                     WithinAbs(expected.rotation.w(), margin));
    }
    GameObjectRegistry::Clear();
}

TEST_CASE("Prefab save rejects unsupported nested component data",
          "[Serializer][Physics]") {
    RigidbodyParameters params;
    params.shape = PhysicsShapes::COMPOUND;
    params.compoundParts.push_back(
        { .shape           = PhysicsShapes::BOX,
          .shapeParameters = SVec3(1.0f, 2.0f, 3.0f),
          .position        = SVec3(0.0f, 0.0f, 0.0f),
          .rotation        = SQuat(0.0f, 0.0f, 0.0f, 1.0f) });
    GameObject original("Compound", "default", {});
    original.AddComponent<RigidbodyComponent>(params);
    Serializer::Prefab prefab(&original);
    const auto         path =
        std::filesystem::temp_directory_path() /
        ("syngine-compound-invalid-" + GenerateUUID().ToString() + ".xml");

    REQUIRE(prefab.SaveToFile(path.string()));
    std::ifstream     originalFile(path);
    const std::string originalXml(
        (std::istreambuf_iterator<char>(originalFile)),
        std::istreambuf_iterator<char>());
    originalFile.close();

    auto& component =
        prefab.rootGameObjectData["components"]
                                 [std::to_string(SYN_COMPONENT_RIGIDBODY)];
    SECTION("malformed compound part") {
        Serializer::DataNode part = component["compoundParts"].At(0);
        part["rotation"]          = std::vector<float>{ 1.0f };
        component["compoundParts"].Set(Serializer::DataNode::NodeArray{});
        component["compoundParts"].Append(part);
    }
    SECTION("other nested field") { component["extra"]["nested"] = 1; }
    REQUIRE_FALSE(prefab.SaveToFile(path.string()));
    std::ifstream     savedFile(path);
    const std::string savedXml((std::istreambuf_iterator<char>(savedFile)),
                               std::istreambuf_iterator<char>());
    savedFile.close();
    REQUIRE(savedXml == originalXml);
    std::filesystem::remove(path);
}

TEST_CASE("Settings float serialization uses configured decimal places",
          "[Serializer]") {
    Serializer::CoreSettings settings;
    settings.video.brightness = 0.12345678f;
    const auto path =
        std::filesystem::temp_directory_path() /
        ("syngine-settings-" + GenerateUUID().ToString() + ".xml");
    REQUIRE(Serializer::CoreSettings::SaveToFile(path.string(), settings));
    std::ifstream     file(path);
    const std::string xml((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    file.close();
    std::filesystem::remove(path);
#if SYN_SERIALIZER_FLOAT_DECIMALS == 6
    REQUIRE(xml.find("brightness=\"0.123457\"") != std::string::npos);
#elif SYN_SERIALIZER_FLOAT_DECIMALS == 2
    REQUIRE(xml.find("brightness=\"0.12\"") != std::string::npos);
#elif SYN_SERIALIZER_FLOAT_DECIMALS == 0
    REQUIRE(xml.find("brightness=\"0\"") != std::string::npos);
#else
    REQUIRE(xml.find("brightness=\"") != std::string::npos);
#endif
}

TEST_CASE("Non-compound rigidbody retains shape parameters",
          "[Serializer][Physics]") {
    GameObject          owner("Box", "default", {});
    RigidbodyParameters params;
    params.shape           = PhysicsShapes::BOX;
    params.shapeParameters = SVec3(1.0f, 2.0f, 3.0f);
    RigidbodyComponent original(&owner, params);
    const auto         node = original.Serialize();
    REQUIRE_FALSE(node.Has("compoundParts"));
    auto restored =
        ComponentRegistry::Instantiate(SYN_COMPONENT_RIGIDBODY, &owner, node);
    REQUIRE(restored != nullptr);
    const auto result = static_cast<RigidbodyComponent*>(restored.get())
                            ->GetCurrentParameters();
    REQUIRE(result.shapeParameters.x() == 1.0f);
    REQUIRE(result.shapeParameters.y() == 2.0f);
    REQUIRE(result.shapeParameters.z() == 3.0f);
}
