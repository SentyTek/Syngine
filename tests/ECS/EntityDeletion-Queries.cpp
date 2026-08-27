// ╒════════════════════════════ EntityDeletion.cpp ═╕
// │ Syngine                              │
// │ Created 2026-06-16                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../defines.h"

#include <vector>
#include <algorithm>

#include <Syngine/Syngine.h>
using namespace Syngine;
using namespace Catch::Matchers;

// Collection of tests for entity deletion in the Syngine ECS, ensuring that
// entities are properly removed from the registry and that associated resources
// are cleaned up

TEST_CASE("Entity deletion removes entity from registry", "[ECS]") {
    SYN_STARTENGINE

    auto& go = GameObjectRegistry::CreateGameObject(
        "TestEntity", "default", std::vector<std::string>{});
    go.AddComponent<TransformComponent>();
    REQUIRE(go.HasComponent(TransformComponent::componentType));

    // Testing all the registry functions
    REQUIRE(GameObjectRegistry::GetGameObjectByName("TestEntity") == &go);
    REQUIRE(GameObjectRegistry::GetGameObjectsByType("default").front() == &go);
    REQUIRE(GameObjectRegistry::GetGameObjectsWithComponent(
                TransformComponent::componentType)
                .front() == &go);
    REQUIRE(GameObjectRegistry::GetAllGameObjects().size() == 1);
    REQUIRE(&GameObjectRegistry::GetAllGameObjects().begin()->second == &go);
    REQUIRE(GameObjectRegistry::GetGameObjectCount() == 1);

    GameObjectRegistry::RemoveGameObject(&go);
    engine.Update();

    // The entity should no longer exist in the registry
    REQUIRE(!GameObjectRegistry::GetGameObjectByName("TestEntity"));
}

TEST_CASE("Vector of GameObjects creation/deletion", "[ECS]") {
    SYN_STARTENGINE

    constexpr int            numEntities = 10;
    std::vector<GameObject*> entities;
    for (int i = 0; i < numEntities; ++i) {
        auto& go =
            GameObjectRegistry::CreateGameObject("Entity" + std::to_string(i),
                                                 "default",
                                                 std::vector<std::string>{});
        go.AddComponent<TransformComponent>();
        entities.push_back(&go);
    }
    REQUIRE(GameObjectRegistry::GetGameObjectCount() == numEntities);

    // Delete all entities
    for (auto* go : entities) {
        GameObjectRegistry::RemoveGameObject(go);
    }
    engine.Update();

    // Registry should be empty
    REQUIRE(GameObjectRegistry::GetGameObjectCount() == 0);
}

TEST_CASE("Querying the Registry of various types of GameObjects", "[ECS]") {
    SYN_STARTENGINE

    auto& go1 = GameObjectRegistry::CreateGameObject(
        "Entity1", "default", std::vector<std::string>{});
    go1.AddComponent<TransformComponent>();

    auto& go2 = GameObjectRegistry::CreateGameObject(
        "Entity2", "notdefault", std::vector<std::string>{});
    go2.AddComponent<TransformComponent>();

    auto& go3 = GameObjectRegistry::CreateGameObject(
        "Entity3", "default", std::vector<std::string>{});
    go3.AddComponent<TransformComponent>();

    REQUIRE(GameObjectRegistry::GetGameObjectByName("Entity1") == &go1);
    REQUIRE(GameObjectRegistry::GetGameObjectByName("Entity2") == &go2);
    REQUIRE(GameObjectRegistry::GetGameObjectByName("Entity3") == &go3);

    REQUIRE(GameObjectRegistry::GetGameObjectsByType("default").size() == 2);
    REQUIRE(GameObjectRegistry::GetGameObjectsByType("notdefault").size() == 1);
    REQUIRE(GameObjectRegistry::GetAllGameObjects().size() == 3);
    REQUIRE(GameObjectRegistry::GetGameObjectsWithComponent(
                TransformComponent::componentType)
                .size() == 3);

    const auto defaultGameObjects =
        GameObjectRegistry::GetGameObjectsByType("default");
    REQUIRE(std::find(defaultGameObjects.begin(),
                      defaultGameObjects.end(),
                      &go1) != defaultGameObjects.end());

    const auto notDefaultGameObjects =
        GameObjectRegistry::GetGameObjectsByType("notdefault");
    REQUIRE(std::find(notDefaultGameObjects.begin(),
                      notDefaultGameObjects.end(),
                      &go2) != notDefaultGameObjects.end());

    const auto gameObjectsWithTransform =
        GameObjectRegistry::GetGameObjectsWithComponent(
            TransformComponent::componentType);
    REQUIRE(std::find(gameObjectsWithTransform.begin(),
                      gameObjectsWithTransform.end(),
                      &go1) != gameObjectsWithTransform.end());
    REQUIRE(std::find(gameObjectsWithTransform.begin(),
                      gameObjectsWithTransform.end(),
                      &go2) != gameObjectsWithTransform.end());
    REQUIRE(std::find(gameObjectsWithTransform.begin(),
                      gameObjectsWithTransform.end(),
                      &go3) != gameObjectsWithTransform.end());

    GameObjectRegistry::Clear();
}
