// ╒═══════════════ GameObjectRegistry.h ═╕
// │ Syngine                              │
// │ Created 2025-07-24                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/GameObjects/Component.h"
#include "Syngine/GameObjects/Components/DirectionalLightComponent.h"
#include <optional>
#include <unordered_map>
#include <vector>

namespace Syngine {
// Forward declaration of GameObject
class GameObject;

/// @brief Registry class to manage GameObjects. Owns all GameObjects, stores
/// indexed sublists of GameObjects for fast lookups
class GameObjectRegistry {
  public:
    /// @brief Create a new GameObject and add it to the registry. The
    /// GameObject is owned by the registry and will be deleted when the
    /// registry is cleared or the GameObject is removed.
    /// @param name The name of the GameObject.
    /// @param type The type of the GameObject.
    /// @param tags A vector of tags to assign to the GameObject.
    /// @return A reference to the newly created GameObject.
    /// @threadsafety safe
    /// @since v0.0.2
    static GameObject&
    CreateGameObject(std::string              name,
                     std::string              type = "default",
                     std::vector<std::string> tags = {}) noexcept;

    /// @brief Create a new GameObject from a serialized DataNode and add it to
    /// the registry. The GameObject is owned by the registry and will be
    /// deleted when the registry is cleared or the GameObject is removed.
    /// @param dataNode The DataNode containing the serialized GameObject data.
    /// @return A reference to the newly created GameObject.
    /// @threadsafety safe
    /// @since v0.0.2
    static GameObject& Instantiate(const Serializer::Prefab& prefab) noexcept;

    /// @brief Clone an existing GameObject and add the clone to the registry.
    /// @param original The GameObject to clone.
    /// @return A reference to the newly created clone GameObject.
    /// @threadsafety safe
    /// @since v0.0.2
    static GameObject& CloneGameObject(const GameObject& original) noexcept;

    /// @brief Remove a GameObject from the registry.
    /// @param GameObject The GameObject to remove.
    /// @return true if the GameObject was removed, false if it was not found.
    /// @threadsafety safe
    /// @since v0.0.1
    static bool RemoveGameObject(GameObject* GameObject) noexcept;

    /// @brief Remove a GameObject by ID.
    /// @param id The ID of the GameObject to remove.
    /// @return true if the GameObject was removed, false if it was not found.
    /// @threadsafety safe
    /// @since v0.0.1
    static bool RemoveGameObjectById(int id) noexcept;

    /// @brief Clear all objects
    /// @post All GameObjects are removed from the registry.
    /// @threadsafety safe
    /// @since v0.0.1
    static void Clear() noexcept;

    /// @brief Get a GameObject by name. Only returns the first match. Is slow.
    /// @param name The name of the GameObject to find.
    /// @return The GameObject with the given name, nullptr if not
    /// found.
    /// @threadsafety read-only
    /// @since v0.0.1
    static const GameObject*
    GetGameObjectByName(const std::string_view& name) noexcept;

    /// @brief Get all GameObjects of a specific type, as defined by the `type`
    /// parameter when creating the GameObject.
    /// @param type The type of GameObject to find.
    /// @return A vector of GameObjects of the specified type.
    /// @threadsafety read-only
    /// @since v0.0.1
    static std::vector<GameObject*>
    GetGameObjectsByType(const std::string_view& type) noexcept;

    /// @brief Get a GameObject by ID.
    /// @param id The ID of the GameObject to find.
    /// @return The GameObject with the given ID.
    /// @threadsafety read-only
    /// @since v0.0.1
    static const GameObject* GetGameObjectById(int id) noexcept;

    /// @brief Gets all ``GameObjects`` with a specific component.
    /// @param type The component type to search for.
    /// @return A vector of GameObject pointers with the component.
    /// @threadsafety read-only
    /// @since v0.0.1
    static std::vector<GameObject*>
    GetGameObjectsWithComponent(Syngine::ComponentTypeID type) noexcept;

    /// @brief Get all GameObjects in the registry.
    /// @return A reference to the unordered map of all GameObjects. The map is
    /// indexed by GameObject ID.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline const std::unordered_map<int, GameObject>&
    GetAllGameObjects() noexcept {
        return m_AllObjects;
    }

    /// @brief Get all GameObjects that have physics enabled.
    /// @return A vector of GameObjects that have physics enabled.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetPhysicsObjects() noexcept {
        return m_PhysicsObjects;
    }

    /// @brief Get all GameObjects that are renderable (have Mesh and Transform
    /// components).
    /// @return A vector of GameObjects that are renderable.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetRenderableObjects() noexcept {
        return m_RenderableObjects;
    }

    /// @brief Get all GameObjects that have a script attached.
    /// @return A vector of GameObjects that have a script attached.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetScriptedObjects() noexcept {
        return m_ScriptedObjects;
    }

    /// @brief Get all GameObjects that are gizmos.
    /// @return A vector of GameObjects that are gizmos.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetGizmos() noexcept {
        return m_Gizmos;
    }

    /// @brief Get the total number of GameObjects in the registry.
    /// @return The number of GameObjects in the registry.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline size_t GetGameObjectCount() noexcept {
        return m_AllObjects.size();
    }

    /// @brief Internal call to notify the registry that a component has been
    /// added to a GameObject. This is used to update the indexed sublists.
    /// @param gameobject The GameObject to which the component was added.
    /// @param type The type of component that was added.
    /// @threadsafety safe
    /// @since v0.0.1
    /// @internal
    static void _NotifyComponentAdded(GameObject*              gameobject,
                                      Syngine::ComponentTypeID type) noexcept;

    /// @brief Internal call to notify the registry that a component has been
    /// removed from a GameObject. This is used to update the indexed sublists.
    /// @param gameobject The GameObject from which the component was removed.
    /// @param type The type of component that was removed.
    /// @threadsafety safe
    /// @since v0.0.1
    /// @internal
    static void _NotifyComponentRemoved(GameObject*              gameobject,
                                        Syngine::ComponentTypeID type) noexcept;

    /// @brief Get the first active directional light in the scene.
    /// @return A pointer to the first active DirectionalLightComponent, or
    /// nullptr if none are active.
    /// @threadsafety read-only
    /// @since v0.0.2
    static DirectionalLightComponent*
    GetFirstActiveDirectionalLight() noexcept {
        if (m_DirectionalLights.empty()) return nullptr;
        for (auto* light : m_DirectionalLights) {
            if (light && light->IsEnabled()) {
                return light;
            }
        }
        return nullptr;
    }

  private:
    // Could be worth using unordered_set for faster removals on the sublists.

    static std::unordered_map<int, GameObject> m_AllObjects; // All GameObjects
    static std::vector<GameObject*>
        m_PhysicsObjects; // GameObjects with physics enabled
    static std::vector<GameObject*>
        m_RenderableObjects; // GameObjects that are renderable
    static std::vector<GameObject*>
        m_ScriptedObjects; // GameObjects that have a script attached
    static std::vector<GameObject*> m_Gizmos; // GameObjects that are gizmos

    static std::vector<DirectionalLightComponent*>
        m_DirectionalLights; // Directional lights for the renderer

    static int nextID; // Next ID to assign to a new GameObject

    friend class GameObject;
};

} // namespace Syngine
