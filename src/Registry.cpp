#include "Registry.h"
#include "Components.h"
#include <string>

namespace Syngine {

// Static member initialization
std::unordered_map<int, GameObject*> Registry::m_AllObjects;
std::vector<GameObject*>             Registry::m_PhysicsObjects;
std::vector<GameObject*>             Registry::m_RenderableObjects;
std::vector<GameObject*>             Registry::m_ScriptedObjects;
std::vector<GameObject*>             Registry::m_Gizmos;

// Add/remove functions
int Registry::AddGameObject(GameObject* gameObject) noexcept {
    if (!gameObject) {
        return -1; // Invalid GameObject
    }

    static int nextID = 0;
    int id = nextID++;

    // Add to the main map & assign ID
    m_AllObjects.insert({ id, gameObject });
    gameObject->SetID(id);

    // Update indexed sublists
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_RENDERABLE)) {
        m_RenderableObjects.push_back(gameObject);
    }
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_RIGIDBODY)) {
        m_PhysicsObjects.push_back(gameObject);
    }
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_SCRIPT)) {
        m_ScriptedObjects.push_back(gameObject);
    }
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_CAMERA)) {
        m_Gizmos.push_back(gameObject);
    }

    return id;
}

int Registry::RemoveGameObject(GameObject* gameObject) noexcept {
    if (!gameObject) {
        return 1; // GameObject not found
    }

    auto it = m_AllObjects.find(gameObject->GetID());
    if (it == m_AllObjects.end()) {
        return 1; // GameObject not found
    }

    // Lambda to remove from indexed sublists. No, I do not like this either
    auto removeFrom = [&](std::vector<GameObject*>& vec) {
        vec.erase(std::remove(vec.begin(), vec.end(), gameObject), vec.end());
    };

    // Remove from lists
    m_AllObjects.erase(it);
    removeFrom(m_RenderableObjects);
    removeFrom(m_PhysicsObjects);
    removeFrom(m_ScriptedObjects);
    removeFrom(m_Gizmos);

    return 0; // Success
}

void Registry::Clear() noexcept {
    // Clear all GameObjects and indexed sublists
    m_AllObjects.clear();
    m_PhysicsObjects.clear();
    m_RenderableObjects.clear();
    m_ScriptedObjects.clear();
    m_Gizmos.clear();
}

int Registry::RemoveGameObjectById(int id) noexcept {
    auto it = m_AllObjects.find(id);
    if (it == m_AllObjects.end()) {
        return 1; // GameObject not found
    }

    GameObject* gameObject = it->second;
    return RemoveGameObject(gameObject);
}

// Getters
GameObject*
Registry::GetGameObjectByName(const std::string_view& name) noexcept {
    // Iterate through all GameObjects to find the first match by name
    for (const auto& pair : m_AllObjects) {
        if (pair.second->name == name) {
            return pair.second;
        }
    }
    return nullptr; // Not found
}

std::vector<GameObject*>
Registry::GetGameObjectsByType(const std::string_view& type) noexcept {
    std::vector<GameObject*> result;
    for (const auto& pair : m_AllObjects) {
        if (pair.second->type == type) {
            result.push_back(pair.second);
        }
    }
    return result; // Return all GameObjects of the specified type
}

GameObject* Registry::GetGameObjectById(int id) noexcept {
    auto it = m_AllObjects.find(id);
    if (it != m_AllObjects.end()) {
        return it->second; // Return the GameObject with the given ID
    }
    return nullptr; // Not found
}

std::vector<GameObject*>
Registry::GetGameObjectsWithComponent(Syngine::Components type) noexcept {
    std::vector<GameObject*> result;
    for (const auto& pair : m_AllObjects) {
        if (pair.second->HasComponent(type)) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// Internal notifications
void Registry::NotifyComponentAdded(GameObject*         gameobject,
                                    Syngine::Components type) noexcept {
    if (!gameobject) return;

    switch (type) {
        case Syngine::SYN_COMPONENT_RENDERABLE:
            m_RenderableObjects.push_back(gameobject);
            break;
        case Syngine::SYN_COMPONENT_RIGIDBODY:
            m_PhysicsObjects.push_back(gameobject);
            break;
        case Syngine::SYN_COMPONENT_SCRIPT:
            m_ScriptedObjects.push_back(gameobject);
            break;
        case Syngine::SYN_COMPONENT_CAMERA:
            m_Gizmos.push_back(gameobject);
        default:
            break; // No action for other component types
    }
}

void Registry::NotifyComponentRemoved(GameObject*         gameobject,
                                      Syngine::Components type) noexcept {
    if (!gameobject) return;

    auto removeFrom = [&](std::vector<GameObject*>& vec) {
        vec.erase(std::remove(vec.begin(), vec.end(), gameobject), vec.end());
    };

    switch (type) {
        case Syngine::SYN_COMPONENT_RENDERABLE:
            removeFrom(m_RenderableObjects);
            break;
        case Syngine::SYN_COMPONENT_RIGIDBODY:
            removeFrom(m_PhysicsObjects);
            break;
        case Syngine::SYN_COMPONENT_SCRIPT:
            removeFrom(m_ScriptedObjects);
            break;
        case Syngine::SYN_COMPONENT_CAMERA:
            removeFrom(m_Gizmos);
        default:
            break; // No action for other component types
    }
}

} // namespace Syngine