// ╒══════════════════ ZoneComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-10-03                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/ZoneComponent.h"
#include "Syngine/Core/Registry.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/GameObject.h"

namespace Syngine {

// constructor
ZoneComponent::ZoneComponent(GameObject* owner,
                             ZoneShape   shape,
                             const float pos[3],
                             const float size[3],
                             bool        oneShot) {
    this->m_owner = owner;
    Init(shape, pos, size, oneShot);
}

ZoneComponent::ZoneComponent(const ZoneComponent& other) {
    for (int i = 0; i < 3; ++i) {
        m_size[i] = other.m_size[i];
        m_pos[i] = other.m_pos[i];
        m_rot[i] = other.m_rot[i];
    }

    m_shape            = other.m_shape;
    m_active           = other.m_active;
    m_oneShot          = other.m_oneShot;
    m_triggeredObjects = other.m_triggeredObjects;
    m_tags             = other.m_tags;
    m_owner            = other.m_owner;
}

ZoneComponent& ZoneComponent::operator=(const ZoneComponent& other) {
    if (this != &other) {
        for (int i = 0; i < 3; ++i) {
            m_size[i] = other.m_size[i];
            m_pos[i]  = other.m_pos[i];
            m_rot[i]  = other.m_rot[i];
        }

        m_shape            = other.m_shape;
        m_active           = other.m_active;
        m_oneShot          = other.m_oneShot;
        m_triggeredObjects = other.m_triggeredObjects;
        m_tags             = other.m_tags;
        m_owner            = other.m_owner;
    }
    return *this;
}

ZoneComponent::~ZoneComponent() {}
Components ZoneComponent::GetComponentType() { return componentType; }

void ZoneComponent::Init(ZoneShape   shape,
                         const float pos[3],
                         const float size[3],
                         bool        oneShot) {
    m_shape   = shape;
    m_oneShot = oneShot;

    SetPosition(pos);
    SetSize(size);
}

// --- Getters and shit

ZoneShape ZoneComponent::GetShape() const { return m_shape; }

void ZoneComponent::GetPosition(float outPos[3]) const {
    outPos[0] = m_pos[0];
    outPos[1] = m_pos[1];
    outPos[2] = m_pos[2];
}

void ZoneComponent::SetPosition(const float pos[3]) {
    m_pos[0] = pos[0];
    m_pos[1] = pos[1];
    m_pos[2] = pos[2];
}

void ZoneComponent::GetSize(float outSize[3]) const {
    outSize[0] = m_size[0];
    outSize[1] = m_size[1];
    outSize[2] = m_size[2];
}

void ZoneComponent::SetSize(const float size[3]) {
    m_size[0] = size[0] < 0.0f ? 0.0f : size[0];
    m_size[1] = size[1] < 0.0f ? 0.0f : size[1];
    m_size[2] = size[2] < 0.0f ? 0.0f : size[2];
}

void ZoneComponent::GetRotation(float outRot[3]) const {
    outRot[0] = m_rot[0];
    outRot[1] = m_rot[1];
    outRot[2] = m_rot[2];
}

void ZoneComponent::SetRotation(const float rot[3]) {
    m_rot[0] = rot[0];
    m_rot[1] = rot[1];
    m_rot[2] = rot[2];
}

bool ZoneComponent::IsActive() const { return m_active; }
void ZoneComponent::SetActive(bool active) { m_active = active; }
bool ZoneComponent::IsOneShot() const { return m_oneShot; }

void ZoneComponent::_AddTriggeredObject(GameObject* object) {
    if (object && std::find(m_triggeredObjects.begin(), m_triggeredObjects.end(),
                            object) == m_triggeredObjects.end()) {
        m_triggeredObjects.emplace(object);
    }
}

bool ZoneComponent::_HasTriggeredObject(GameObject* object) {
    return object &&
           (m_triggeredObjects.find(object) != m_triggeredObjects.end());
}

void ZoneComponent::_RemoveTriggeredObject(GameObject* object) {
    if (object) {
        m_triggeredObjects.erase(object);
    }
}

void ZoneComponent::AddTag(const std::string& tag) {
    if (std::find(m_tags.begin(), m_tags.end(), tag) == m_tags.end()) {
        m_tags.emplace(tag);
    }
}

void ZoneComponent::RemoveTag(const std::string& tag) {
    auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    if (it != m_tags.end()) {
        m_tags.erase(it);
    }
}

bool ZoneComponent::HasTag(const std::string& tag) const {
    return std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end();
}

std::vector<std::string> ZoneComponent::GetTags() const {
    return std::vector<std::string>(m_tags.begin(), m_tags.end());
}

void ZoneComponent::SetTags(const std::vector<std::string>& tags) {
    m_tags.clear();
    for (const auto& tag : tags) {
        AddTag(tag);
    }
}

GameObject* ZoneComponent::_GetOwner() const { return m_owner; }

// --- Actual zone logic

// TODO: These will ABSOLUTELY need to be changed not only for rotation support
// but also for scene optimization like quadtrees or actrees.
bool ZoneComponent::IsInZone(const float point[3]) const {
    switch (m_shape) {
        case ZoneShape::BOX: {
            return (point[0] >= m_pos[0] - m_size[0] / 2.0f &&
                    point[0] <= m_pos[0] + m_size[0] / 2.0f) &&
                   (point[1] >= m_pos[1] - m_size[1] / 2.0f &&
                    point[1] <= m_pos[1] + m_size[1] / 2.0f) &&
                   (point[2] >= m_pos[2] - m_size[2] / 2.0f &&
                    point[2] <= m_pos[2] + m_size[2] / 2.0f);
        }
        case ZoneShape::SPHERE: {
            float dx = point[0] - m_pos[0];
            float dy = point[1] - m_pos[1];
            float dz = point[2] - m_pos[2];
            float distanceSquared = dx * dx + dy * dy + dz * dz;
            float radius = m_size[0]; // Use size[0] as radius for sphere
            return distanceSquared <= radius * radius;
        }
        default:
            return false;
    }
}

bool ZoneComponent::IsInZone(const GameObject* object) const {
    if (!object) return false;

    TransformComponent* transform = object->GetComponent<TransformComponent>();
    if (!transform) return false;

    float objPos[3];
    transform->GetPosition();
    return IsInZone(objPos);
}

std::vector<GameObject*> ZoneComponent::GetObjectsInZone() const {
    std::vector<GameObject*> objectsInZone;

    for (const auto& pair : Registry::GetAllGameObjects()) {
        GameObject* obj = pair.second;
        if (IsInZone(obj)) {
            objectsInZone.push_back(obj);
        }
    }

    return objectsInZone;
}

std::vector<GameObject*>
ZoneComponent::GetObjectsInZoneByTag(const std::string& tag) const {
    std::vector<GameObject*> objectsInZone;

    for (const auto& pair : Registry::GetAllGameObjects()) {
        GameObject* obj = pair.second;
        if (IsInZone(obj) && obj->HasTag(tag)) {
            objectsInZone.push_back(obj);
        }
    }

    return objectsInZone;
}

} // namespace Syngine
