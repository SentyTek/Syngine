// ╒═════════════ TransformComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/GameObject.h"

#include "bx/math.h"
#include <cmath>

namespace Syngine {
TransformComponent::TransformComponent(GameObject* owner) {
    m_position[0] = m_position[1] = m_position[2] = 0.0f;
    m_rotation[0] = m_rotation[1] = m_rotation[2] = 0.0f; m_rotation[3] = 1.0f; // Quaternion identity
    m_scale[0] = m_scale[1] = m_scale[2] = 0.5f;
    this->m_owner = owner;
}

void TransformComponent::Init(std::vector<float> position,
                              std::vector<float> rotation,
                              std::vector<float> scale) {
    this->m_position[0] = position[0];
    this->m_position[1] = position[1];
    this->m_position[2] = position[2];

    this->m_rotation[0] = rotation[0];
    this->m_rotation[1] = rotation[1];
    this->m_rotation[2] = rotation[2];
    this->m_rotation[3] = rotation[3];

    this->m_scale[0] = scale[0];
    this->m_scale[1] = scale[1];
    this->m_scale[2] = scale[2];
}

Syngine::Components TransformComponent::GetComponentType() {
    return SYN_COMPONENT_TRANSFORM;
}

void TransformComponent::GetRotationEuler(float& x, float& y, float& z) const {
    const float qx = m_rotation[0];
    const float qy = m_rotation[1];
    const float qz = m_rotation[2];
    const float qw = m_rotation[3];

    // Quaternion → Euler XYZ (roll, pitch, yaw)
    float sinr_cosp = 2.0f * (qw * qx + qy * qz);
    float cosr_cosp = 1.0f - 2.0f * (qx * qx + qy * qy);
    x = std::atan2(sinr_cosp, cosr_cosp);

    float sinp = 2.0f * (qw * qy - qz * qx);
    if (std::abs(sinp) >= 1.0f)
        y = std::copysign(3.1415926f / 2.0f, sinp);
    else
        y = std::asin(sinp);

    float siny_cosp = 2.0f * (qw * qz + qx * qy);
    float cosy_cosp = 1.0f - 2.0f * (qy * qy + qz * qz);
    z = std::atan2(siny_cosp, cosy_cosp);

    x = bx::toDeg(x);
    y = bx::toDeg(y);
    z = bx::toDeg(z);
}

float* TransformComponent::GetRotationQuat() const {
    return const_cast<float*>(m_rotation);
}

void TransformComponent::SetPosition(float x, float y, float z) {
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void TransformComponent::SetRotation(float x, float y, float z) {
    //Convert Euler angles to quaternion
    x = bx::toRad(x);
    y = bx::toRad(y);
    z = bx::toRad(z);

    float cr = cosf(x * 0.5f);
    float sr = sinf(x * 0.5f);
    float cp = cosf(y * 0.5f);
    float sp = sinf(y * 0.5f);
    float cy = cosf(z * 0.5f);
    float sy = sinf(z * 0.5f);

    m_rotation[0] = sr * cp * cy - cr * sp * sy; // x
    m_rotation[1] = cr * sp * cy + sr * cp * sy; // y
    m_rotation[2] = cr * cp * sy - sr * sp * cy; // z
    m_rotation[3] = cr * cp * cy + sr * sp * sy; // w
    //this will probably break
}

void TransformComponent::GetGlobalRotation(float* outQuat) {
    if (!m_parent) {
        memcpy(outQuat, m_rotation, sizeof(float) * 4);
        return;
    }

    float parentGlobalRot[4];
    m_parent->GetGlobalRotation(parentGlobalRot);

    bx::Quaternion parentQ = { parentGlobalRot[0],
                               parentGlobalRot[1],
                               parentGlobalRot[2],
                               parentGlobalRot[3] };
    bx::Quaternion localQ  = {
        m_rotation[0], m_rotation[1], m_rotation[2], m_rotation[3]
    };

    bx::Quaternion globalQ = bx::mul(localQ, parentQ);
    outQuat[0]             = globalQ.x;
    outQuat[1]             = globalQ.y;
    outQuat[2]             = globalQ.z;
    outQuat[3]             = globalQ.w;
}

void TransformComponent::SetRotation(float x, float y, float z, float w) {
    m_rotation[0] = x;
    m_rotation[1] = y;
    m_rotation[2] = z;
    m_rotation[3] = w;
}

// Sets the scale of the transform component.
// Note that the params are FULL EXTENT, not half extents.
void TransformComponent::SetScale(float x, float y, float z) {
    m_scale[0] = x / 2;
    m_scale[1] = y / 2;
    m_scale[2] = z / 2;
}

void TransformComponent::GetModelMatrix(float* result) {
    // If there's a parent, multiply parent's model matrix with local model matrix
    if (m_parent) {
        float parentMatrix[16];
        m_parent->GetModelMatrix(parentMatrix);

        float localMatrix[16];
        _CalcModelMatrix(localMatrix);

        bx::mtxMul(result, parentMatrix, localMatrix);
        return;
    }

    // No parent, just calculate local model matrix
    _CalcModelMatrix(result);
}

float* TransformComponent::GetPosition(bool global) {
    if (!global || !m_parent) {
        return m_position;
    }

    static float globalPos[3];
    float modelMatrix[16];
    GetModelMatrix(modelMatrix);
    globalPos[0] = modelMatrix[12];
    globalPos[1] = modelMatrix[13];
    globalPos[2] = modelMatrix[14];
    return globalPos;
}

void TransformComponent::SetParent(TransformComponent* parent) {
    if (m_parent) {
        m_parent->_RemoveChild(this);
    }
    m_parent = parent;

    if (m_parent) {
        m_parent->_AddChild(this);
    }
}

TransformComponent* TransformComponent::GetParent() const { return m_parent; }

void TransformComponent::_AddChild(TransformComponent* child) {
    m_children.push_back(child);
}

void TransformComponent::_RemoveChild(TransformComponent* child) {
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child),
                     m_children.end());
}

void TransformComponent::_CalcModelMatrix(float* result) {
    // Calculate the model matrix based on position, rotation, and scale
    // Probably implement a proper matrix library for this

    float rx = m_rotation[0], ry = m_rotation[1], rz = m_rotation[2], rw = m_rotation[3];
    float sx = m_scale[0], sy = m_scale[1], sz = m_scale[2];

    float xx = rx * rx;
    float xy = rx * ry;
    float xz = rx * rz;
    float yy = ry * ry;
    float yz = ry * rz;
    float zz = rz * rz;
    float wx = rw * rx;
    float wy = rw * ry;
    float wz = rw * rz;

    // Column-major order
    result[0]  = (1.0f - 2.0f * (yy + zz)) * sx;
    result[1]  = (2.0f * (xy + wz))        * sx;
    result[2]  = (2.0f * (xz - wy))        * sx;
    result[3]  = 0.0f;

    result[4]  = (2.0f * (xy - wz))        * sy;
    result[5]  = (1.0f - 2.0f * (xx + zz)) * sy;
    result[6]  = (2.0f * (yz + wx))        * sy;
    result[7]  = 0.0f;

    result[8]  = (2.0f * (xz + wy))        * sz;
    result[9]  = (2.0f * (yz - wx))        * sz;
    result[10] = (1.0f - 2.0f * (xx + yy)) * sz;
    result[11] = 0.0f;

    result[12] = m_position[0];
    result[13] = m_position[1];
    result[14] = m_position[2];
    result[15] = 1.0f;
}

} // namespace Syngine