#pragma once
#include "Components.h"
#include "SyngineGameObject.h"
#include <unordered_map>
#include <vector>

namespace Syngine {

// Registry class to manage GameObjects.
// Owns all GameObjects, stores indexed sublists of GameObjects for fast lookups
class Registry {
  public:
    // Add a GameObject to the registry.
    // Returns the ID of the added GameObject or -1 on failure.
    static int AddGameObject(GameObject* GameObject) noexcept;
    // Remove a GameObject from the registry.
    // Returns 0 on success, 1 if the GameObject was not found.
    static int RemoveGameObject(GameObject* GameObject) noexcept;
    // Remove a GameObject by ID.
    // Returns 0 on success, 1 if the GameObject was not found.
    static int RemoveGameObjectById(int id) noexcept;
    // Clear all objects
    static void Clear() noexcept;

    // Get a GameObject by name. Returns first found GameObject with the given
    // name, nullptr if not found.
    static GameObject* GetGameObjectByName(const std::string_view& name) noexcept;
    // Get all GameObjects of a specific type.
    // Returns a vector of GameObjects of the specified type.
    static std::vector<GameObject*> 
    GetGameObjectsByType(const std::string_view& type) noexcept;
    // Get a GameObject by ID.
    // Returns the GameObject with the given ID, nullptr if not found.
    static GameObject* GetGameObjectById(int id) noexcept;
    // Gets all GameObjects that have a specific component.
    // Param `type` is the SyngineComponent type to search for.
    // Returns a vector of GameObjects that have the specified component.
    static std::vector<GameObject*>
    GetGameObjectsWithComponent(Syngine::Components type) noexcept;

    // Get all GameObjects in the registry.
    // Returns a reference to the unordered_map of all GameObjects.
    static inline std::unordered_map<int, GameObject*>& GetAllGameObjects() noexcept {
        return m_AllObjects;
    }
    // Get all GameObjects that have physics enabled.
    // Returns a vector of GameObjects that have physics enabled.
    static inline std::vector<GameObject*> GetPhysicsObjects() noexcept {
        return m_PhysicsObjects;
    }
    // Get all GameObjects that are renderable (have Mesh and Transform
    // components). Returns a vector of GameObjects that are renderable.
    static inline std::vector<GameObject*> GetRenderableObjects() noexcept {
        return m_RenderableObjects;
    }
    // Get all GameObjects that have a script attached.
    static inline std::vector<GameObject*> GetScriptedObjects() noexcept {
        return m_ScriptedObjects;
    }
    // Get all GameObjects that are gizmos.
    static inline std::vector<GameObject*> GetGizmos() noexcept {
        return m_Gizmos;
    }

    // Get the total number of GameObjects in the registry.
    static inline size_t GetGameObjectCount() noexcept {
        return m_AllObjects.size();
    }

    // Internal call to notify the registry that a component has been added to a
    // GameObject. This is used to update the indexed sublists.
    static void _NotifyComponentAdded(GameObject*         gameobject,
                                     Syngine::Components type) noexcept;
    // Internal call to notify the registry that a component has been removed
    // from a GameObject. This is used to update the indexed sublists.
    static void _NotifyComponentRemoved(GameObject*         gameobject,
                                       Syngine::Components type) noexcept;
    // Internal call to notify the registry that a GameObject is now active.
    // This is used to update the indexed sublists.
    //static void NotifyGameObjectActivated(GameObject* gameobject) noexcept;
    // Internal call to notify the registry that a GameObject is now inactive.
    // This is used to update the indexed sublists.
    //static void NotifyGameObjectDeactivated(GameObject* gameobject) noexcept;

  private:
    // Could be worth using unordered_set for faster removals on the sublists.

    static std::unordered_map<int, GameObject*> m_AllObjects; // All GameObjects
    static std::vector<GameObject*> m_PhysicsObjects; // GameObjects with physics enabled
    static std::vector<GameObject*> m_RenderableObjects; // GameObjects that are renderable
    static std::vector<GameObject*> m_ScriptedObjects; // GameObjects that have a script attached
    static std::vector<GameObject*> m_Gizmos; // GameObjects that are gizmos
};

} // namespace Syngine
