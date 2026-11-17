// ╒════════════════════════════ Scene.h ═╕
// │ Syngine Studio                       │
// │ Created 2026-11-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include "Syngine/Scene/GameObjectRegistry.h"
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/Utils/UUID.h>

#include <vector>
#include <string>
#include <span>

namespace Syngine {

/// @brief Defines the mode in which a scene should be loaded.
/// @since v0.0.3
enum class SceneLoadMode {
    Single,  //* Load as the only active scene (persistent scenes will remain)
    Additive //* Load the scene additively alongside existing scenes
};

/// @brief Represents an instance of a scene in the engine.
/// @since v0.0.3
class SceneInstance {
    std::string   m_name;
    std::string   m_path;
    std::string   m_bundle;
    Syngine::UUID m_uuid;
    bool          m_persistent;

    std::vector<Syngine::UUID> m_gameObjects;

    SceneInstance() = delete;

    friend class SceneManager;
    friend class Serializer;

  public:
    /// @brief Construct a new Scene Instance with a given name.
    /// @param name The name of the scene.
    /// @since v0.0.3
    SceneInstance(const std::string& name)
        : m_name(name), m_path(""), m_persistent(false),
          m_uuid(Syngine::GenerateUUID()), m_bundle("") {}

    /// @brief Construct a new Scene Instance with an existing serialized Scene
    /// @param scene The serialized scene data
    /// @param bundle The bundle the scene belongs to
    /// @param path The path to the scene file
    /// @note This constructor will instantiate the game objects contained in
    /// the serialized scene.
    /// @since v0.0.3
    SceneInstance(Serializer::Scene  scene,
                  const std::string& bundle,
                  const std::string& path)
        : m_name(scene.name), m_path(path), m_bundle(bundle),
          m_uuid(scene.uuid), m_persistent(scene.persistent) {

        // Scene gameObjects is an Array node, not an Object map
        if (scene.gameObjects.IsArray()) {
            const size_t count = scene.gameObjects.Size();
            m_gameObjects.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                m_gameObjects.push_back(
                    GameObjectRegistry::Instantiate(scene.gameObjects.At(i))
                        .GetID());
            }
        }
    }

    SceneInstance(const SceneInstance&)            = delete;
    SceneInstance& operator=(const SceneInstance&) = delete;

    SceneInstance(SceneInstance&&) noexcept            = default;
    SceneInstance& operator=(SceneInstance&&) noexcept = default;

    /// @brief Get the name of the scene.
    /// @return The name of the scene.
    /// @since v0.0.3
    std::string_view GetName() const { return m_name; }

    /// @brief Get the path to the scene file.
    /// @return The path to the scene file.
    /// @since v0.0.3
    std::string_view GetPath() const { return m_path; }

    /// @brief Get the bundle the scene belongs to.
    /// @return The bundle name.
    /// @since v0.0.3
    std::string_view GetBundle() const { return m_bundle; }

    /// @brief Get the UUID of the scene.
    /// @return The UUID of the scene.
    /// @since v0.0.3
    const Syngine::UUID& GetUUID() const { return m_uuid; }

    /// @brief Set the name of the scene.
    /// @param name The new name of the scene.
    /// @since v0.0.3
    void SetName(const std::string_view& name) { m_name = name; }

    /// @brief Check if the scene is persistent.
    /// @return True if the scene is persistent, false otherwise.
    /// @since v0.0.3
    bool IsPersistent() const { return m_persistent; }

    /// @brief Set the persistence of the scene.
    /// @param persistent True to make the scene persistent, false otherwise.
    /// @since v0.0.3
    void SetPersistent(bool persistent) { m_persistent = persistent; }

    /// @brief Get the list of game objects in the scene.
    /// @return A vector of UUIDs representing the game objects.
    /// @since v0.0.3
    const std::vector<Syngine::UUID>& GetGameObjects() const {
        return m_gameObjects;
    }

    /// @brief Get a game object by its UUID.
    /// @param id The UUID of the game object.
    /// @return Pointer to the game object if found, nullptr otherwise.
    /// @since v0.0.3
    const inline GameObject* GetGameObject(const Syngine::UUID& id) const {
        for (const auto& gameObjectID : m_gameObjects) {
            if (gameObjectID == id) {
                return GameObjectRegistry::GetGameObjectByID(id);
            }
        }
        return nullptr;
    }

    /// @brief Add a game object to the scene.
    /// @param gameObject The game object to add.
    /// @since v0.0.3
    void AddGameObject(GameObject& gameObject) {
        m_gameObjects.push_back(gameObject.GetID());
    }

    /// @brief Add a game object to the scene by its UUID.
    /// @param id The UUID of the game object to add.
    /// @since v0.0.3
    void AddGameObject(const UUID& id) { m_gameObjects.push_back(id); }
};

class SceneManager {
    static SceneManager* m_instance;

    static std::vector<SceneInstance> m_scenes;

    static void _Shutdown();
    friend class Core;

  public:
    SceneManager(const SceneManager&)            = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager();

    /// @brief Get the singleton instance of the SceneManager.
    /// @return Reference to the singleton SceneManager instance.
    /// @since v0.0.3
    static SceneManager& Get();

    /// @brief Load a scene from the specified bundle and scene name.
    /// @param bundle The bundle containing the scene.
    /// @param sceneName The name of the scene to load.
    /// @param mode The load mode (Single or Additive).
    /// @return True if the scene was successfully loaded, false otherwise.
    /// @since v0.0.3
    static bool LoadScene(const std::string& bundle,
                          const std::string& sceneName,
                          SceneLoadMode      mode = SceneLoadMode::Single);

    /// @brief Load a scene from the specified path.
    /// @param path The path to the scene file.
    /// @param mode The load mode (Single or Additive).
    /// @return True if the scene was successfully loaded, false otherwise.
    /// @since v0.0.3
    static bool LoadScene(const std::string& path,
                          SceneLoadMode      mode = SceneLoadMode::Single);

    /// @brief Unload a scene from the specified bundle and scene name.
    /// @param bundle The bundle containing the scene.
    /// @param sceneName The name of the scene to unload.
    /// @return True if the scene was successfully unloaded, false otherwise.
    /// @since v0.0.3
    static bool UnloadScene(const std::string& bundle,
                            const std::string& sceneName);

    /// @brief Unload a scene by its UUID.
    /// @param uuid The UUID of the scene to unload.
    /// @return True if the scene was successfully unloaded, false otherwise.
    /// @since v0.0.3
    static bool UnloadScene(const Syngine::UUID& uuid);

    /// @brief Get a scene by its name.
    /// @param sceneName The name of the scene.
    /// @return Pointer to the SceneInstance if found, nullptr otherwise.
    /// @since v0.0.3
    static SceneInstance* GetScene(const std::string& sceneName);

    /// @brief Get a scene by its UUID.
    /// @param uuid The UUID of the scene.
    /// @return Pointer to the SceneInstance if found, nullptr otherwise.
    /// @since v0.0.3
    static SceneInstance* GetScene(const Syngine::UUID& uuid);
    /// @brief Check if a scene is loaded by its name.
    /// @param sceneName The name of the scene.
    /// @return True if the scene is loaded, false otherwise.
    /// @since v0.0.3
    static bool IsSceneLoaded(const std::string& sceneName);

    /// @brief Check if a scene is loaded by its UUID.
    /// @param uuid The UUID of the scene.
    /// @return True if the scene is loaded, false otherwise.
    /// @since v0.0.3
    static bool IsSceneLoaded(const Syngine::UUID& uuid);

    /// @brief Get all loaded scenes.
    /// @return A span containing all loaded SceneInstance objects.
    /// @since v0.0.3
    static std::span<const SceneInstance> GetLoadedScenes();
};

} // namespace Syngine
