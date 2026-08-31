// ╒═════════════ GameObjectRegistry.cpp ═╕
// │ Syngine                              │
// │ Created 2025-07-24                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Scene/GameObjectRegistry.h>
#include <Syngine/Core/Core.h>
#include <Syngine/GameObjects/Component.h>
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/GameObjects/Components/RigidbodyComponent.h>
#include <Syngine/GameObjects/Components/ZoneComponent.h>
#include <Syngine/GameObjects/Components/TransformComponent.h>
#include <Syngine/Scene/ZoneSystem.h>
#include <utility>

namespace Syngine {

// Static member initialization
std::unordered_map<int, GameObject> GameObjectRegistry::m_AllObjects;
std::vector<GameObject*>            GameObjectRegistry::m_PhysicsObjects;
std::vector<GameObject*>            GameObjectRegistry::m_RenderableObjects;
std::vector<GameObject*>            GameObjectRegistry::m_ScriptedObjects;
std::vector<GameObject*>            GameObjectRegistry::m_Gizmos;
std::vector<GameObject*>            GameObjectRegistry::m_queueToRemove;
std::vector<std::pair<GameObject*, Syngine::ComponentTypeID>>
    GameObjectRegistry::m_queueToRemoveComponents;

int GameObjectRegistry::nextID = 0; // Next ID to assign to a new GameObject
std::vector<DirectionalLightComponent*>
    GameObjectRegistry::m_DirectionalLights; // Directional lights for the
                                             // renderer

// Add/remove functions
GameObject&
GameObjectRegistry::CreateGameObject(std::string              name,
                                     std::string              type,
                                     std::vector<std::string> tags) noexcept {
    int id = GameObjectRegistry::nextID++;

    auto [it, inserted] = m_AllObjects.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(id),
        std::forward_as_tuple(
            std::move(name), std::move(type), std::move(tags)));

    GameObject& gameObject = (it->second);
    gameObject._SetID(id);

    return gameObject;
}

GameObject&
GameObjectRegistry::Instantiate(const Serializer::Prefab& prefab) noexcept {
    int id = GameObjectRegistry::nextID++;
    auto [it, inserted] =
        m_AllObjects.emplace(std::piecewise_construct,
                             std::forward_as_tuple(id),
                             std::forward_as_tuple(prefab.rootGameObjectData));
    GameObject& gameObject = (it->second);
    gameObject._SetID(id);
    return gameObject;
}

GameObject&
GameObjectRegistry::CloneGameObject(const GameObject& original) noexcept {
    GameObject& clone = CreateGameObject(
        original.name + "_clone", original.type, original.tags);
    clone.isActive = original.isActive;
    clone.gizmo    = original.gizmo;

    for (const auto& [type, component] : original.components) {
        auto clonedComponent     = component->Clone();
        clonedComponent->m_owner = &clone;
        clone.components[type]   = std::move(clonedComponent);
    }

    return clone;
}

void GameObjectRegistry::RemoveGameObject(GameObject* gameObject) noexcept {
    if (!gameObject) {
        return; // Null pointer, cannot remove
    }

    auto it = m_AllObjects.find(gameObject->GetID());
    if (it == m_AllObjects.end()) {
        return; // GameObject not found
    }

    // Tell children to delete themselves
    for (GameObject* child : gameObject->GetChildren()) {
        if (child) {
            RemoveGameObject(child);
        }
    }

    m_queueToRemove.push_back(
        gameObject); // Queue for removal at the end of the frame
}

void GameObjectRegistry::RemoveGameObjectById(int id) noexcept {
    auto it = m_AllObjects.find(id);
    if (it == m_AllObjects.end()) {
        return; // GameObject not found
    }

    // Tell children to delete themselves
    for (GameObject* child : it->second.GetChildren()) {
        if (child) {
            RemoveGameObject(child);
        }
    }

    m_queueToRemove.push_back(
        &it->second); // Queue for removal at the end of the frame
}

void GameObjectRegistry::Clear() noexcept {
    m_RenderableObjects.clear();
    m_PhysicsObjects.clear();
    m_ScriptedObjects.clear();
    m_Gizmos.clear();
    m_DirectionalLights.clear();
    m_AllObjects.clear();
    nextID = 0;
}

void GameObjectRegistry::_RemoveQueuedObjects() noexcept {
    if (m_queueToRemove.empty()) return; // Nothing to remove
    for (GameObject* obj : m_queueToRemove) {
        // Lambda to remove from indexed sublists. No, I do not like this either
        // reply: what's wrong with it?
        auto removeFrom = [&](std::vector<GameObject*>& vec) {
            vec.erase(std::remove(vec.begin(), vec.end(), obj), vec.end());
        };

        // If it has a zone component, clear it from all zones to prevent
        // dangling pointers in zone tracking
        if (obj->HasComponent(Syngine::SYN_COMPONENT_ZONE)) {
            auto* zoneComp = obj->GetComponent<ZoneComponent>();
            if (zoneComp) {
                // Unregister the zone from the ZoneSystem
                Core::_GetContext()->ZoneSystem->_UnregisterZone(zoneComp);
            }
        }

        // Remove from lists
        removeFrom(m_RenderableObjects);
        removeFrom(m_PhysicsObjects);
        removeFrom(m_ScriptedObjects);
        removeFrom(m_Gizmos);
        m_DirectionalLights.erase(
            std::remove(m_DirectionalLights.begin(),
                        m_DirectionalLights.end(),
                        obj->GetComponent<DirectionalLightComponent>()),
            m_DirectionalLights.end());
        m_AllObjects.erase(obj->GetID());
    }
    m_queueToRemove.clear(); // Clear the queue after processing
}

void GameObjectRegistry::_RemoveQueuedObjectComponents() noexcept {
    for (auto [object, type] : m_queueToRemoveComponents) {
        if (!object) return;

        auto removeFrom = [&](std::vector<GameObject*>& vec) {
            vec.erase(std::remove(vec.begin(), vec.end(), object), vec.end());
        };
        object->components.erase(type);

        switch (type) {
        case Syngine::SYN_COMPONENT_MESH:
        case Syngine::SYN_COMPONENT_TRANSFORM: // Trigger if either are removed
            if (!object->HasComponent(Syngine::SYN_COMPONENT_MESH) ||
                !object->HasComponent(Syngine::SYN_COMPONENT_TRANSFORM)) {
                removeFrom(m_RenderableObjects);
            }
            break;
        case Syngine::SYN_COMPONENT_RIGIDBODY:
            removeFrom(m_PhysicsObjects);
            break;
        case Syngine::SYN_COMPONENT_SCRIPT:
            removeFrom(m_ScriptedObjects);
            break;
        case Syngine::SYN_COMPONENT_CAMERA: removeFrom(m_Gizmos); break;
        case Syngine::SYN_COMPONENT_ZONE:
            Core::_GetContext()->ZoneSystem->_UnregisterZone(
                object->GetComponent<ZoneComponent>());
            break;
        default: break; // No action for other component types
        }
    }
    m_queueToRemoveComponents.clear(); // Clear the queue after processing
}

// Getters
const GameObject*
GameObjectRegistry::GetGameObjectByName(std::string_view name) noexcept {
    // Iterate through all GameObjects to find the first match by name
    for (const auto& pair : m_AllObjects) {
        if (pair.second.name == name) {
            return &pair.second;
        }
    }
    return nullptr; // Return nullptr if not found
}

std::vector<GameObject*>
GameObjectRegistry::GetGameObjectsByType(std::string_view type) noexcept {
    std::vector<GameObject*> result;
    for (auto& pair : m_AllObjects) {
        if (pair.second.type == type) {
            result.push_back(&pair.second);
        }
    }
    return result; // Return all GameObjects of the specified type
}

const GameObject* GameObjectRegistry::GetGameObjectById(int id) noexcept {
    auto it = m_AllObjects.find(id);
    if (it != m_AllObjects.end()) {
        return &it->second; // Return the GameObject with the given ID
    }
    return nullptr; // Not found
}

std::vector<GameObject*> GameObjectRegistry::GetGameObjectsWithComponent(
    Syngine::ComponentTypeID type) noexcept {
    std::vector<GameObject*> result;
    for (auto& pair : m_AllObjects) {
        if (pair.second.HasComponent(type)) {
            result.push_back(&pair.second);
        }
    }
    return result;
}

// Internal notifications
void GameObjectRegistry::_NotifyComponentAdded(
    GameObject* gameobject, Syngine::ComponentTypeID type) noexcept {
    if (!gameobject) return;

    switch (type) {
    case Syngine::SYN_COMPONENT_MESH:
    case Syngine::SYN_COMPONENT_TRANSFORM:
        // If both Mesh and Transform components are present, add to renderable
        // objects
        if (gameobject->HasComponent(Syngine::SYN_COMPONENT_MESH) &&
            gameobject->HasComponent(Syngine::SYN_COMPONENT_TRANSFORM)) {
            // Check if already in the list
            if (std::find(m_RenderableObjects.begin(),
                          m_RenderableObjects.end(),
                          gameobject) == m_RenderableObjects.end()) {
                m_RenderableObjects.push_back(gameobject);
            }
        }

        // If a rigidbody was added before transform, retry deferred init now.
        if (type == Syngine::SYN_COMPONENT_TRANSFORM &&
            gameobject->HasComponent(Syngine::SYN_COMPONENT_RIGIDBODY)) {
            auto* rb = gameobject->GetComponent<Syngine::RigidbodyComponent>();
            if (rb) {
                rb->RetryInitIfPending();
            }
        }
        break;
    case Syngine::SYN_COMPONENT_RIGIDBODY:
        if (std::find(m_PhysicsObjects.begin(),
                      m_PhysicsObjects.end(),
                      gameobject) == m_PhysicsObjects.end()) {
            m_PhysicsObjects.push_back(gameobject);
        }
        break;
    case Syngine::SYN_COMPONENT_SCRIPT:
        if (std::find(m_ScriptedObjects.begin(),
                      m_ScriptedObjects.end(),
                      gameobject) == m_ScriptedObjects.end()) {
            m_ScriptedObjects.push_back(gameobject);
        }
        break;
    case Syngine::SYN_COMPONENT_CAMERA:
        if (std::find(m_Gizmos.begin(), m_Gizmos.end(), gameobject) ==
            m_Gizmos.end()) {
            m_Gizmos.push_back(gameobject);
            Syngine::Core::_GetContext()->renderer->_RegisterGizmo(
                "camera_render");
        }
        break;
    case Syngine::SYN_COMPONENT_ZONE:
        Core::_GetContext()->ZoneSystem->_RegisterZone(
            gameobject->GetComponent<ZoneComponent>());
        break;
    case Syngine::SYN_COMPONENT_LIGHT_DIRECTIONAL:
        if (std::find(m_DirectionalLights.begin(),
                      m_DirectionalLights.end(),
                      gameobject->GetComponent<DirectionalLightComponent>()) ==
            m_DirectionalLights.end()) {
            m_DirectionalLights.push_back(
                gameobject->GetComponent<DirectionalLightComponent>());
        }
        if (std::find(m_Gizmos.begin(), m_Gizmos.end(), gameobject) ==
            m_Gizmos.end()) {
            m_Gizmos.push_back(gameobject);
            Syngine::Core::_GetContext()->renderer->_RegisterGizmo("light_sun");
        }
        break;
    default: break; // No action for other component types
    }
}

void GameObjectRegistry::_NotifyComponentRemoved(
    GameObject* gameobject, Syngine::ComponentTypeID type) noexcept {
    if (!gameobject || !gameobject->HasComponent(type)) return;

    m_queueToRemoveComponents.push_back({ gameobject, type });
}

} // namespace Syngine
