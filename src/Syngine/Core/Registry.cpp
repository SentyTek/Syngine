// ╒═══════════════════════ Registry.cpp ═╕
// │ Syngine                              │
// │ Created 2025-07-24                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Core.h"
#include "Syngine/Core/Registry.h"
#include "Syngine/Core/ZoneManager.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"

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
    gameObject->_SetID(id);

    // Update indexed sublists
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_MESH) &&
        gameObject->HasComponent(Syngine::SYN_COMPONENT_TRANSFORM)) {
        _NotifyComponentAdded(gameObject, Syngine::SYN_COMPONENT_MESH);
        _NotifyComponentAdded(gameObject, Syngine::SYN_COMPONENT_TRANSFORM);
    }
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_RIGIDBODY)) {
        _NotifyComponentAdded(gameObject, Syngine::SYN_COMPONENT_RIGIDBODY);
    }
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_SCRIPT)) {
        _NotifyComponentAdded(gameObject, Syngine::SYN_COMPONENT_SCRIPT);
    }
    if (gameObject->HasComponent(Syngine::SYN_COMPONENT_CAMERA)) {
        _NotifyComponentAdded(gameObject, Syngine::SYN_COMPONENT_CAMERA);
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
void Registry::_NotifyComponentAdded(GameObject*         gameobject,
                                    Syngine::Components type) noexcept {
    if (!gameobject) return;

    switch (type) {
        case Syngine::SYN_COMPONENT_MESH:
        case Syngine::SYN_COMPONENT_TRANSFORM:
            // If both Mesh and Transform components are present, add to renderable objects
            if (gameobject->HasComponent(Syngine::SYN_COMPONENT_MESH) &&
                gameobject->HasComponent(Syngine::SYN_COMPONENT_TRANSFORM)) {
                // Check if already in the list
                if (std::find(m_RenderableObjects.begin(), m_RenderableObjects.end(), gameobject) == m_RenderableObjects.end()) {
                    m_RenderableObjects.push_back(gameobject);
                }
            }
            break;
        case Syngine::SYN_COMPONENT_RIGIDBODY:
            if (std::find(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), gameobject) == m_PhysicsObjects.end()) {
                m_PhysicsObjects.push_back(gameobject);
            }
            break;
        case Syngine::SYN_COMPONENT_SCRIPT:
            if (std::find(m_ScriptedObjects.begin(), m_ScriptedObjects.end(), gameobject) == m_ScriptedObjects.end()) {
                m_ScriptedObjects.push_back(gameobject);
            }
            break;
        case Syngine::SYN_COMPONENT_CAMERA:
            if (std::find(m_Gizmos.begin(), m_Gizmos.end(), gameobject) == m_Gizmos.end()) {
                m_Gizmos.push_back(gameobject);
                Syngine::Core::_GetApp()->renderer->_RegisterGizmo("camera_render");
            }
            break;
        case Syngine::SYN_COMPONENT_ZONE:
            Core::_GetApp()->zoneManager->_RegisterZone(
                gameobject->GetComponent<ZoneComponent>());
            break;
        default:
            break; // No action for other component types
    }
}

void Registry::_NotifyComponentRemoved(GameObject*         gameobject,
                                      Syngine::Components type) noexcept {
    if (!gameobject) return;

    auto removeFrom = [&](std::vector<GameObject*>& vec) {
        vec.erase(std::remove(vec.begin(), vec.end(), gameobject), vec.end());
    };

    switch (type) {
        case Syngine::SYN_COMPONENT_MESH:
        case Syngine::SYN_COMPONENT_TRANSFORM: // Trigger if either are removed, fully remove when both are gone
            if (!gameobject->HasComponent(Syngine::SYN_COMPONENT_MESH) &&
                !gameobject->HasComponent(Syngine::SYN_COMPONENT_TRANSFORM)) {
                removeFrom(m_RenderableObjects);
            }
            break;
        case Syngine::SYN_COMPONENT_RIGIDBODY:
            removeFrom(m_PhysicsObjects);
            break;
        case Syngine::SYN_COMPONENT_SCRIPT:
            removeFrom(m_ScriptedObjects);
            break;
        case Syngine::SYN_COMPONENT_CAMERA:
            removeFrom(m_Gizmos);
            break;
        case Syngine::SYN_COMPONENT_ZONE:
            Core::_GetApp()->zoneManager->_UnregisterZone(
                gameobject->GetComponent<ZoneComponent>());
            break;
        default:
            break; // No action for other component types
    }
}

} // namespace Syngine