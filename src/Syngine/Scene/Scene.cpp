// ╒══════════════════════════ Scene.cpp ═╕
// │ Syngine Studio                       │
// │ Created 2026-11-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Scene/Scene.h>
#include <Syngine/Utils/Serializer.h>

#include <utility>

namespace Syngine {

using namespace Syngine;

SceneManager*              SceneManager::m_instance = nullptr;
std::vector<SceneInstance> SceneManager::m_scenes;

SceneManager::SceneManager() {
    if (m_instance) {
        Logger::Error(
            "Attempting to create SceneManager when one already exists");
    }
    m_instance = this;
}

void SceneManager::_Shutdown() {
    for (auto& scene : m_scenes) {
        UnloadScene(scene.m_uuid);
    }
}

bool SceneManager::LoadScene(const std::string& bundle,
                             const std::string& sceneName,
                             SceneLoadMode      mode) {
    auto scene = Serializer::Scene::LoadFromFile(bundle, sceneName);

    SceneInstance sceneInstance(scene, bundle, sceneName);

    if (mode == SceneLoadMode::Single) {
        for (auto& scene : m_scenes) {
            if (!scene.IsPersistent()) {
                UnloadScene(scene.m_uuid);
            }
        }
    }

    if (GetScene(scene.uuid)) return false;

    m_scenes.emplace_back(std::move(sceneInstance));

    return true;
}

bool SceneManager::UnloadScene(const UUID& uuid) {
    auto it = std::find_if(
        m_scenes.begin(), m_scenes.end(), [&](const SceneInstance& scene) {
            return scene.m_uuid == uuid;
        });
    if (it == m_scenes.end()) return false;

    m_scenes.erase(it);
    return true;
}

SceneInstance* SceneManager::GetScene(const UUID& uuid) {
    auto it = std::find_if(
        m_scenes.begin(), m_scenes.end(), [&](const SceneInstance& scene) {
            return scene.m_uuid == uuid;
        });
    if (it == m_scenes.end()) return nullptr;
    return &(*it);
}

SceneManager& SceneManager::Get() { return *m_instance; }

} // namespace Syngine
