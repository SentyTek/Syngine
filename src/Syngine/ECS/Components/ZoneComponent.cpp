// ╒══════════════════ ZoneComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-10-03                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/ZoneComponent.h"
#include "Syngine/Core/LuaManager.h"
#include "Syngine/Core/Registry.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Math/Quaternion.hpp"

#include <sol/sol.hpp>
#include <sol/types.hpp>
#include <string>

namespace Syngine {

// constructor
ZoneComponent::ZoneComponent(GameObject*          owner,
                             ZoneShape            shape,
                             const Math::Vector3& pos,
                             const Math::Vector3& size,
                             bool                 oneShot) {
    this->m_owner = owner;
    Init(shape, pos, size, oneShot);
}

ZoneComponent::ZoneComponent(const ZoneComponent& other) {
    this->m_owner = other.m_owner;
    m_pos         = other.m_pos;
    m_size        = other.m_size;
    m_rot         = other.m_rot;
    m_shape       = other.m_shape;
    m_active           = other.m_active;
    m_oneShot          = other.m_oneShot;
    m_triggeredObjects = other.m_triggeredObjects;
    m_tags             = other.m_tags;
    m_owner            = other.m_owner;
}

ZoneComponent& ZoneComponent::operator=(const ZoneComponent& other) {
    if (this != &other) {
        m_pos = other.m_pos;
        m_size = other.m_size;
        m_rot  = other.m_rot;
        m_shape = other.m_shape;
        m_active = other.m_active;
        m_oneShot = other.m_oneShot;
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
    node / "position" = std::vector<float>(m_pos);
    node / "size" = std::vector<float>(m_size);
    node / "rotation" = std::vector<float>(m_rot);
    node / "active" = m_active;
    node / "oneShot" = m_oneShot;
    node / "tags" = m_tags;
    return node;
}

void ZoneComponent::Init(ZoneShape            shape,
                         const Math::Vector3& pos,
                         const Math::Vector3& size,
                         bool                 oneShot) {
    m_shape   = shape;
    m_oneShot = oneShot;

    SetPosition(pos);
    SetSize(size);
}

// --- Getters and shit

ZoneShape ZoneComponent::GetShape() const { return m_shape; }

Math::Vector3 ZoneComponent::GetPosition() const { return m_pos; }

void ZoneComponent::SetPosition(const Math::Vector3& pos) { m_pos = pos; }

Math::Vector3 ZoneComponent::GetSize() const { return m_size; }

void ZoneComponent::SetSize(const Math::Vector3& size) {
    if (m_shape == ZoneShape::BOX) {
        m_size = size;
    } else if (m_shape == ZoneShape::SPHERE) {
        m_size.setX(size.x()); // Only use x for radius
    }
}

Math::Quaternion ZoneComponent::GetRotation() const { return m_rot; }

void ZoneComponent::SetRotation(const Math::Quaternion& rot) {
    m_rot = rot;
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
bool ZoneComponent::IsInZone(const Math::Vector3& point) const {
    switch (m_shape) {
        case ZoneShape::BOX: {
            return (point.x() >= m_pos.x() - m_size.x() / 2.0f &&
                    point.x() <= m_pos.x() + m_size.x() / 2.0f &&
                    point.y() >= m_pos.y() - m_size.y() / 2.0f &&
                    point.y() <= m_pos.y() + m_size.y() / 2.0f &&
                    point.z() >= m_pos.z() - m_size.z() / 2.0f &&
                    point.z() <= m_pos.z() + m_size.z() / 2.0f);
        }
        case ZoneShape::SPHERE: {
            return m_pos.distance(point) <= m_size.x(); // size[0] is radius for sphere
        }
        default:
            return false;
    }
}

bool ZoneComponent::IsInZone(const GameObject* object) const {
    if (!object) return false;

    TransformComponent* transform = object->GetComponent<TransformComponent>();
    if (!transform) return false;

    return IsInZone(transform->GetPosition());
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
    [](GameObject*                 owner,
       const Serializer::DataNode& data) -> std::unique_ptr<Component> {
        ZoneShape shape = static_cast<ZoneShape>(data["shape"].As<int>(0));
        std::vector<float> pos = data["position"].As<std::vector<float>>({0.0f, 0.0f, 0.0f});
        std::vector<float> size = data["size"].As<std::vector<float>>({1.0f, 1.0f, 1.0f});
        std::vector<float> rot = data["rotation"].As<std::vector<float>>({0.0f, 0.0f, 0.0f});
        bool oneShot = data["oneShot"].As<bool>(false);
        auto comp = std::make_unique<ZoneComponent>(owner, shape, Math::Vector3(pos), Math::Vector3(size), oneShot);
        comp->SetRotation(Math::Quaternion(rot));
        comp->SetActive(data["active"].As<bool>(true));
        comp->SetTags(data["tags"].As<std::vector<std::string>>({}));
        return comp;
    },

    // Lua bindings
    // clang-format off
    [](sol::state& lua) {
        lua.new_usertype<ZoneComponent>("ZoneComponent",
            // Methods
            "GetShape", [](ZoneComponent& self) {
                return self.GetShape() == ZoneShape::BOX ? "BOX" : "SPHERE";
             },
            "GetPosition", [](ZoneComponent& self) -> std::tuple<float, float, float> {
                Math::Vector3 pos = self.GetPosition();
                return {pos.x(), pos.y(), pos.z()};
            },
            "SetPosition", [](ZoneComponent& self, float x, float y, float z) {
                Math::Vector3 pos(x, y, z);
                self.SetPosition(pos);
            },
            "GetSize", [](ZoneComponent& self) -> std::tuple<float, float, float> {
                Math::Vector3 size = self.GetSize();
                return {size.x(), size.y(), size.z()};
            },
            "SetSize", [](ZoneComponent& self, float x, float y, float z) {
                Math::Vector3 size(x, y, z);
                self.SetSize(size);
            },
            "GetRotation", [](ZoneComponent& self) -> std::tuple<float, float, float> {
                Math::Quaternion rot = self.GetRotation();
                return {rot.x(), rot.y(), rot.z()};
            },
            "SetRotation", [](ZoneComponent& self, float x, float y, float z) {
                Math::Quaternion rot(Math::Vector3(x, y, z));
                self.SetRotation(rot);
            },
            "IsActive", &ZoneComponent::IsActive,
            "SetActive", &ZoneComponent::SetActive,
            "IsOneShot", &ZoneComponent::IsOneShot,
            "AddTag", &ZoneComponent::AddTag,
            "RemoveTag", &ZoneComponent::RemoveTag,
            "HasTag", &ZoneComponent::HasTag,
            "GetTags", [](ZoneComponent& self, sol::this_state ts) {
                std::vector<std::string> tags = self.GetTags();
                sol::state_view lua(ts);
                sol::table tagsTable = lua.create_table();
                for (size_t i = 0; i < tags.size(); ++i) {
                    tagsTable[i + 1] = tags[i];
                }
                return tagsTable;
            },
            "SetTags", [](ZoneComponent& self, sol::table tagsTable) {
                std::vector<std::string> tags;
                for (size_t i = 1; i <= tagsTable.size(); ++i) {
                    if (tagsTable[i].is<std::string>()) {
                        tags.push_back(tagsTable[i]);
                    }
                }
                self.SetTags(tags);
            },
            "IsPointInZone", [](ZoneComponent& self, float x, float y, float z) {
                Math::Vector3 point(x, y, z);
                return self.IsInZone(point);
            },
            "IsObjectInZone", [](ZoneComponent& self, GameObject* obj) {
                return self.IsInZone(obj);
            },
            "GetObjectsInZone", [](ZoneComponent& self, sol::this_state ts) {
                std::vector<GameObject*> objects = self.GetObjectsInZone();
                sol::state_view lua(ts);
                sol::table objTable = lua.create_table();
                for (size_t i = 0; i < objects.size(); ++i) {
                    objTable[i + 1] = objects[i];
                }
                return objTable;
            },
            "GetObjectsInZoneByTag", [](ZoneComponent& self, const std::string& tag, sol::this_state ts) {
                std::vector<GameObject*> objects = self.GetObjectsInZoneByTag(tag);
                sol::state_view lua(ts);
                sol::table objTable = lua.create_table();
                for (size_t i = 0; i < objects.size(); ++i) {
                    objTable[i + 1] = objects[i];
                }
                return objTable;
            },
            "SetEntryCallback", [](ZoneComponent& self, sol::function callback) {
                self.OnEnter = callback;
            },
            "SetExitCallback", [](ZoneComponent& self, sol::function callback) {
                self.OnExit = callback;
            }
        );
    }
    // clang-format on
);

} // namespace Syngine
