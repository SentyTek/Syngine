// ╒══════════════════ ZoneComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-10-03                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/ZoneComponent.h"
#include "Syngine/Core/Registry.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/ComponentRegistry.h"
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
Syngine::ComponentTypeID ZoneComponent::GetComponentType() { return componentType; }

Serializer::DataNode ZoneComponent::Serialize() const {
    Serializer::DataNode node;
    node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_ZONE);
    node / "shape" = static_cast<int>(m_shape);
    node / "position" = std::vector<float>{m_pos[0], m_pos[1], m_pos[2]};
    node / "size" = std::vector<float>{m_size[0], m_size[1], m_size[2]};
    node / "rotation" = std::vector<float>{m_rot[0], m_rot[1], m_rot[2]};
    node / "active" = m_active;
    node / "oneShot" = m_oneShot;
    node / "tags" = m_tags;
    return node;
}

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

void ZoneComponent::_AddObject(GameObject* object) {
    if (object && std::find(m_objectsIn.begin(), m_objectsIn.end(),
                            object) == m_objectsIn.end()) {
        m_objectsIn.emplace_back(object);
    }

    if (m_oneShot && object && std::find(m_triggeredObjects.begin(), m_triggeredObjects.end(),
                            object) == m_triggeredObjects.end()) {
        m_triggeredObjects.emplace_back(object);
    }
}

bool ZoneComponent::_HasOneTimeObject(GameObject* object) {
    return m_oneShot && object && (std::find(m_triggeredObjects.begin(),
                                m_triggeredObjects.end(),
                                object) != m_triggeredObjects.end());
}

bool ZoneComponent::_IsTrackingObject(GameObject* object) const {
    return object && (std::find(m_objectsIn.begin(),
                                m_objectsIn.end(),
                                object) != m_objectsIn.end());
}

void ZoneComponent::_RemoveObject(GameObject* object) {
    if (object) {
        m_objectsIn.erase(std::remove(m_objectsIn.begin(),
                                             m_objectsIn.end(),
                                             object),
                                 m_objectsIn.end());
    }
}

void ZoneComponent::AddTag(const std::string& tag) {
    if (std::find(m_tags.begin(), m_tags.end(), tag) == m_tags.end()) {
        m_tags.emplace_back(tag);
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

    const float* objPos = transform->GetPosition();
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

static Syngine::ComponentRegistrar s_zoneRegistrar(
    Syngine::SYN_COMPONENT_ZONE,

    // ParseXml
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_ZONE);
        for (const auto& attr : elem->attributes()) {
            scl::string key = attr->tag();
            scl::string value = attr->data();

            if (key == "shape") {
                node["shape"] = std::stoi(value.cstr());
            } else if (key == "position") {
                node["position"] = Serializer::_ParseFloatArray(value);
            } else if (key == "size") {
                node["size"] = Serializer::_ParseFloatArray(value);
            } else if (key == "rotation") {
                node["rotation"] = Serializer::_ParseFloatArray(value);
            } else if (key == "active") {
                node["active"] = (value == "true");
            } else if (key == "oneShot") {
                node["oneShot"] = (value == "true");
            } else if (key == "tags") {
                // tags are CSV in XML, convert back to array
                std::vector<std::string> tags = Serializer::_ParseStringArray(value);
                node["tags"] = tags;
            }
        }
        return node;
    },

    // Instantiate
    [](GameObject* owner, const Serializer::DataNode& data) -> std::unique_ptr<Component> {
        ZoneShape shape = static_cast<ZoneShape>(data["shape"].As<int>(0));
        std::vector<float> pos = data["position"].As<std::vector<float>>({0.0f, 0.0f, 0.0f});
        std::vector<float> size = data["size"].As<std::vector<float>>({1.0f, 1.0f, 1.0f});
        std::vector<float> rot = data["rotation"].As<std::vector<float>>({0.0f, 0.0f, 0.0f});
        bool oneShot = data["oneShot"].As<bool>(false);
        auto comp = std::make_unique<ZoneComponent>(owner, shape, pos.data(), size.data(), oneShot);
        comp->SetRotation(rot.data());
        comp->SetActive(data["active"].As<bool>(true));
        comp->SetTags(data["tags"].As<std::vector<std::string>>({}));
        return comp;
    }
);

} // namespace Syngine
