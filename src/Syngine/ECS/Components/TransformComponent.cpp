// ╒═════════════ TransformComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Math/Math.hpp"
#include "Syngine/Math/Quaternion.hpp"
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Utils/Serializer.h"

#include <algorithm>
#include <vector>

#include <sol/sol.hpp>

namespace Syngine {
TransformComponent::TransformComponent(GameObject* owner) {
    m_position = Vector3();
    m_rotation = Quaternion();
    m_scale = Vector3(1.0f);
    this->m_owner = owner;
}

TransformComponent::TransformComponent(const TransformComponent& other) {
    this->m_owner = other.m_owner;
    this->m_position = other.m_position;
    this->m_rotation = other.m_rotation;
    this->m_scale = other.m_scale;
}

TransformComponent& TransformComponent::operator=(const TransformComponent& other) {
    if (this != &other) {
        this->m_owner = other.m_owner;
        this->m_position = other.m_position;
        this->m_rotation = other.m_rotation;
        this->m_scale = other.m_scale;
    }
    return *this;
}

Serializer::DataNode TransformComponent::Serialize() const {
    Serializer::DataNode transformNode;
    Serializer::Float3   pos{ m_position.x(), m_position.y(), m_position.z() };
    Serializer::Float4   rot{
        m_rotation.x(), m_rotation.y(), m_rotation.z(), m_rotation.w()
    };
    Serializer::Float3 scale{ m_scale.x(), m_scale.y(), m_scale.z() };

    transformNode / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_TRANSFORM);
    transformNode / "position" = pos;
    transformNode / "rotation" = rot;
    transformNode / "scale"    = scale;

    return transformNode;
}

void TransformComponent::Init(Vector3 position,
                              Quaternion rotation,
                              Vector3 scale) {
    // Everything is assumed local by default
    this->m_position = position;
    this->m_rotation = rotation;
    this->m_scale    = scale;

    _MarkLocalDirty();
}

// --- Dirty flag helpers
void TransformComponent::_MarkLocalDirty() {
    m_dirtyLocal = true;
    ++m_version;
    _MarkWorldDirty();
}
void TransformComponent::_MarkWorldDirty() {
    m_dirtyWorld = true;
    // Propagate to children
    for (auto& child : m_children) {
        if (child) {
            child->_MarkWorldDirty();
        }
    }
}

// --- Matrix updates
void TransformComponent::_UpdateLocalMatrix() const {
    if (!m_dirtyLocal) return;

    // Build local matrix as: S * R * T
    m_localMtx = Mat4(m_scale, m_rotation, m_position);
    m_dirtyLocal = false;
}

void TransformComponent::_UpdateWorldMatrix() const {
    if (!m_dirtyWorld) return;
    _UpdateLocalMatrix();

    if (m_parent) {
        m_parent->_UpdateWorldMatrix();

        // World = Parent * local (reversed here for stupid reasons?)
        m_worldMtx = m_parent->m_worldMtx * m_localMtx;
        m_worldRotation = m_parent->m_worldRotation * m_rotation;
        m_worldRotation.normalize();
    } else { // No parent, world = local
        m_worldMtx = m_localMtx;
        m_worldRotation = m_rotation;
    }

    // Extract world position from matrix
    m_worldPosition = m_worldMtx.getTranslation();

    m_dirtyWorld = false;
}

// --- Public API

Vector3 TransformComponent::GetWorldRotationEuler() const {
    _UpdateWorldMatrix();
    Vector3 eulerVec = m_worldRotation.toEulerAngles();
    return eulerVec;
}

Quaternion TransformComponent::GetWorldRotationQuaternion() const {
    _UpdateWorldMatrix();
    return m_worldRotation;
}

void TransformComponent::SetPosition(Vector3 position) {
    m_position = position;
    _MarkLocalDirty();
}

void TransformComponent::SetRotationEuler(Vector3 rotation) {
    // Convert Euler angles to quaternion
    // LOCAL rotation
    // this will probably break
    Quaternion q = Quaternion(rotation);
    q.normalize();
    m_rotation = q;
    _MarkLocalDirty();
}

void TransformComponent::SetRotationQuat(Quaternion rotation) {
    // LOCAL rotation
    m_rotation = rotation;
    m_rotation.normalize();
    _MarkLocalDirty();
}

// Sets the scale of the transform component.
// Note that the params are FULL EXTENT, not half extents.
void TransformComponent::SetScale(Vector3 scale) {
    m_scale = scale;
    _MarkLocalDirty();
}

// --- World setters

void TransformComponent::SetWorldPosition(Vector3 position) {
    if (m_parent) {
        m_parent->_UpdateWorldMatrix();
        Mat4 invParentMtx = m_parent->m_worldMtx;
        if (!invParentMtx.isInvertible()) {
            // Handle non-invertible parent matrix (e.g., scale of zero)
            // For now, we can just set the local position to the world position
            m_position = position;
            _MarkLocalDirty();
            return;
        }

        invParentMtx.invert();
        Vector4 localPos = Math::Vector4(position, 1) * invParentMtx;
        m_position = localPos.xyz();
    } else {
        m_position = position;
    }
    _MarkLocalDirty();
}

void TransformComponent::SetWorldRotationEuler(Vector3 rotation) {
    Quaternion quat(rotation);
    quat.normalize();
    SetWorldRotationQuat(quat);
    _MarkWorldDirty();
}

void TransformComponent::SetWorldRotationQuat(Quaternion rotation) {
    rotation.normalize();
    if (m_parent) {
        m_parent->_UpdateWorldMatrix();
        Quaternion parentInv = m_parent->m_worldRotation;
        parentInv.invert();
        Quaternion localQ = parentInv * rotation;
        m_rotation = localQ;
        m_rotation.normalize();
    } else {
        m_rotation = rotation;
    }
    _MarkWorldDirty();
}

Mat4 TransformComponent::GetModelMatrix() const {
    _UpdateWorldMatrix();
    return m_worldMtx;
}

// Global getters
Vector3 TransformComponent::GetWorldPosition() const {
    _UpdateWorldMatrix();
    return m_worldPosition;
}

Vector3 TransformComponent::GetWorldScale() const {
    _UpdateWorldMatrix();
    return m_scale;
}

// Following 4 are local getters
Vector3 TransformComponent::GetPosition() const {
    _UpdateWorldMatrix();
    return m_position;
}

Quaternion TransformComponent::GetRotationQuaternion() const {
    _UpdateWorldMatrix();
    return m_rotation;
}

Vector3 TransformComponent::GetRotationEuler() const {
    Quaternion worldRot = GetRotationQuaternion();
    Vector3    eulerVec = worldRot.toEulerAngles();
    return eulerVec;
}

Vector3    TransformComponent::GetScale() const { return m_scale; }

Mat4 TransformComponent::GetLocalMatrix() const {
    _UpdateLocalMatrix();
    return m_localMtx;
}

// Parenting
void TransformComponent::SetParent(TransformComponent* parent) {
    if (m_parent == parent) return;

    // Remove from current parent
    if (m_parent) {
        auto& siblings = m_parent->m_children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    m_parent = parent;

    // Add to new parent's children list
    if (m_parent) {
        m_parent->m_children.push_back(this);
    }

    _MarkWorldDirty();
}

TransformComponent* TransformComponent::GetParent() const {
    return m_parent;
}

std::vector<TransformComponent*>& TransformComponent::GetChildren() const {
    return (std::vector<TransformComponent*>&)m_children;
}

static Syngine::ComponentRegistrar s_transformRegistrar(
    SYN_COMPONENT_TRANSFORM,

    // ParseXml
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_TRANSFORM);

        for (const auto& attr : elem->attributes()) {
            scl::string key = attr->tag();
            scl::string value = attr->data();

            if (key == "position") {
                std::vector<float> pos = Serializer::_ParseFloatArray(value);
                node / "position" = pos;
            } else if (key == "rotation") {
                std::vector<float> rot = Serializer::_ParseFloatArray(value);
                node / "rotation" = rot;
            } else if (key == "scale") {
                std::vector<float> scale = Serializer::_ParseFloatArray(value);
                node / "scale" = scale;
            }
        }

        return node;
    },

    // Instantiate
    [](GameObject* owner,
       const Serializer::DataNode& data) -> std::unique_ptr<Component> {
        std::vector<float> pos = data["position"].As<std::vector<float>>({0.0f, 0.0f, 0.0f});
        std::vector<float> rot = data["rotation"].As<std::vector<float>>({0.0f, 0.0f, 0.0f, 1.0f});
        std::vector<float> scale = data["scale"].As<std::vector<float>>({1.0f, 1.0f, 1.0f});
        auto comp = std::make_unique<TransformComponent>(owner);
        comp->Init(pos, rot, scale); // This probably isn't Good Practice
        return comp;
    },

    // Lua bindings
    [](sol::state& lua) {
        lua.new_usertype<TransformComponent>(
            "TransformComponent",
            // Methods
            "SetPosition", [](TransformComponent& self, float x, float y, float z) {
                self.SetPosition(Vector3(x, y, z));
            },
            "SetRotationEuler", [](TransformComponent& self, float x, float y, float z) {
                self.SetRotationEuler(Vector3(x, y, z));
            },
            "SetRotationQuat", [](TransformComponent& self, float x, float y, float z, float w) {
                self.SetRotationQuat(Quaternion(x, y, z, w));
            },
            "SetScale", [](TransformComponent& self, float x, float y, float z) {
                self.SetScale(Vector3(x, y, z));
            },
            "GetPosition", [](TransformComponent& self) -> std::tuple<float, float, float> {
                float x, y, z;
                Vector3 pos = self.GetPosition();
                x = pos.x();
                y = pos.y();
                z = pos.z();
                return { x, y, z };
            },
            "GetScale", [](TransformComponent& self) -> std::tuple<float, float, float> {
                float x, y, z;
                Vector3 scale = self.GetScale();
                x = scale.x();
                y = scale.y();
                z = scale.z();
                return { x, y, z };
            },
            "GetRotationEuler", [](TransformComponent& self) -> std::tuple<float, float, float> {
                float x, y, z;
                Vector3 rot = self.GetWorldRotationEuler();
                x = rot.x();
                y = rot.y();
                z = rot.z();
                return { x, y, z };
            },
            "GetRotationQuat", [](TransformComponent& self)
                -> std::tuple<float, float, float, float> {
                float x, y, z, w;
                Quaternion quat = self.GetWorldRotationQuaternion();
                x = quat.x();
                y = quat.y();
                z = quat.z();
                w = quat.w();
                return { x, y, z, w };
            });
    }
);

} // namespace Syngine
