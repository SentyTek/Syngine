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

enum class SceneLoadMode { Single, Additive };

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

    std::string_view GetName() const { return m_name; }
    std::string_view GetPath() const { return m_path; }
    std::string_view GetBundle() const { return m_bundle; }

    const Syngine::UUID& GetUUID() const { return m_uuid; }
    void SetName(const std::string_view& name) { m_name = name; }

    bool IsPersistent() const { return m_persistent; }
    void SetPersistent(bool persistent) { m_persistent = persistent; }

    const std::vector<Syngine::UUID>& GetGameObjects() const {
        return m_gameObjects;
    }

    const inline GameObject* GetGameObject(const Syngine::UUID& id) const {
        for (const auto& gameObjectID : m_gameObjects) {
            if (gameObjectID == id) {
                return GameObjectRegistry::GetGameObjectByID(id);
            }
        }
        return nullptr;
    }

    void AddGameObject(GameObject& gameObject) {
        m_gameObjects.push_back(gameObject.GetID());
    }

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

    static SceneManager& Get();

    static bool LoadScene(const std::string& bundle,
                          const std::string& sceneName,
                          SceneLoadMode      mode = SceneLoadMode::Single);
    static bool UnloadScene(const std::string& bundle,
                            const std::string& sceneName);
    static bool UnloadScene(const Syngine::UUID& uuid);

    static SceneInstance* GetScene(const std::string& sceneName);
    static SceneInstance* GetScene(const Syngine::UUID& uuid);
    static bool           IsSceneLoaded(const std::string& sceneName);
    static bool           IsSceneLoaded(const Syngine::UUID& uuid);

    static std::span<const SceneInstance> GetLoadedScenes();
};

} // namespace Syngine
