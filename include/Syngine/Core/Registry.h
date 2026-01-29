// ╒═════════════════════════ Registry.h ═╕
// │ Syngine                              │
// │ Created 2025-07-24                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace Syngine {
// Forward declaration of GameObject
class GameObject;

/// @brief Registry class to manage GameObjects. Owns all GameObjects, stores
/// indexed sublists of GameObjects for fast lookups
class Registry {
  public:
    /// @brief Add a GameObject to the registry.
    /// @param GameObject The GameObject to add.
    /// @return 0 on success, -1 if failure.
    /// @threadsafety safe
    /// @since v0.0.1
    static int AddGameObject(GameObject* GameObject) noexcept;

    /// @brief Remove a GameObject from the registry.
    /// @param GameObject The GameObject to remove.
    /// @return 0 on success, 1 if the GameObject was not found.
    /// @threadsafety safe
    /// @since v0.0.1
    static int RemoveGameObject(GameObject* GameObject) noexcept;

    /// @brief Remove a GameObject by ID.
    /// @param id The ID of the GameObject to remove.
    /// @return 0 on success, 1 if the GameObject was not found.
    /// @threadsafety safe
    /// @since v0.0.1
    static int RemoveGameObjectById(int id) noexcept;

    /// @brief Clear all objects
    /// @post All GameObjects are removed from the registry.
    /// @threadsafety safe
    /// @since v0.0.1
    static void Clear() noexcept;

    /// @brief Get a GameObject by name. Only returns the first match. Is slow.
    /// @param name The name of the GameObject to find.
    /// @return GameObject* The GameObject with the given name, nullptr if not
    /// found.
    /// @threadsafety read-only
    /// @since v0.0.1
    static GameObject*
    GetGameObjectByName(const std::string_view& name) noexcept;

    /// @brief Get all GameObjects of a specific type, as defined by the `type`
    /// parameter when creating the GameObject.
    /// @param type The type of GameObject to find.
    /// @return std::vector A vector of GameObjects of the specified type.
    /// @threadsafety read-only
    /// @since v0.0.1
    static std::vector<GameObject*>
    GetGameObjectsByType(const std::string_view& type) noexcept;

    /// @brief Get a GameObject by ID.
    /// @param id The ID of the GameObject to find.
    /// @return GameObject* The GameObject with the given ID, nullptr if not
    /// found.
    /// @threadsafety read-only
    /// @since v0.0.1
    static GameObject* GetGameObjectById(int id) noexcept;

    /// @brief Gets all ``GameObjects`` with a specific component.
    /// @param type The component type to search for.
    /// @return A vector of GameObject pointers with the component.
    /// @threadsafety read-only
    /// @since v0.0.1
    static std::vector<GameObject*>
    GetGameObjectsWithComponent(Syngine::Components type) noexcept;

    /// @brief Get all GameObjects in the registry.
    /// @return std::unordered_map A reference to the unordered map of all
    /// GameObjects. The map is indexed by GameObject ID.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::unordered_map<int, GameObject*>& GetAllGameObjects() noexcept {
        return m_AllObjects;
    }

    /// @brief Get all GameObjects that have physics enabled.
    /// @return std::vector A vector of GameObjects that have physics enabled.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetPhysicsObjects() noexcept {
        return m_PhysicsObjects;
    }

    /// @brief Get all GameObjects that are renderable (have Mesh and Transform
    /// components).
    /// @return std::vector A vector of GameObjects that are renderable.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetRenderableObjects() noexcept {
        return m_RenderableObjects;
    }

    /// @brief Get all GameObjects that have a script attached.
    /// @return std::vector A vector of GameObjects that have a script attached.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetScriptedObjects() noexcept {
        return m_ScriptedObjects;
    }

    /// @brief Get all GameObjects that are gizmos.
    /// @return std::vector A vector of GameObjects that are gizmos.
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline std::vector<GameObject*> GetGizmos() noexcept {
        return m_Gizmos;
    }

    /// @brief Get the total number of GameObjects in the registry.
    /// @return size_t The number of GameObjects in the registry.
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
    static void _NotifyComponentAdded(GameObject*         gameobject,
                                      Syngine::Components type) noexcept;

    /// @brief Internal call to notify the registry that a component has been removed
    /// from a GameObject. This is used to update the indexed sublists.
    /// @param gameobject The GameObject from which the component was removed.
    /// @param type The type of component that was removed.
    /// @threadsafety safe
    /// @since v0.0.1
    /// @internal
    static void _NotifyComponentRemoved(GameObject*         gameobject,
                                        Syngine::Components type) noexcept;

  private:
    // Could be worth using unordered_set for faster removals on the sublists.

    static std::unordered_map<int, GameObject*> m_AllObjects; // All GameObjects
    static std::vector<GameObject*> m_PhysicsObjects; // GameObjects with physics enabled
    static std::vector<GameObject*> m_RenderableObjects; // GameObjects that are renderable
    static std::vector<GameObject*> m_ScriptedObjects; // GameObjects that have a script attached
    static std::vector<GameObject*> m_Gizmos; // GameObjects that are gizmos
};

} // namespace Syngine
