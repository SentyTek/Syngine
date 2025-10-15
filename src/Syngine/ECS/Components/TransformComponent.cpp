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

#include <algorithm>

namespace Syngine {
TransformComponent::TransformComponent(GameObject* owner) {
    m_position[0] = m_position[1] = m_position[2] = 0.0f;
    m_rotation[0] = m_rotation[1] = m_rotation[2] = 0.0f; m_rotation[3] = 1.0f; // Quaternion identity
    m_scale[0] = m_scale[1] = m_scale[2] = 0.5f;
    this->m_owner                        = owner;

    // Cache defaults
    std::fill(std::begin(m_localMtx), std::end(m_localMtx), 0.0f);
    std::fill(std::begin(m_worldMtx), std::end(m_worldMtx), 0.0f);
    m_worldMtx[0] = m_worldMtx[5] = m_worldMtx[10] = m_worldMtx[15] = 1.0f;
    m_worldPosition[0] = m_worldPosition[1] = m_worldPosition[2] = 0.0f;
    m_worldRotation[0] = m_worldRotation[1] = m_worldRotation[2] = 0.0f;
    m_worldRotation[3]                                           = 1.0f;
}

void TransformComponent::Init(std::vector<float> position,
                              std::vector<float> rotation,
                              std::vector<float> scale) {
    // Everything is assumed local by default
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

    _MarkLocalDirty();
}

Syngine::Components TransformComponent::GetComponentType() {
    return SYN_COMPONENT_TRANSFORM;
}

// --- Math helpers
void TransformComponent::_QuatNormalize(float* quat) {
    float len = std::sqrt(quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);
    if (len > 0.0f) {
        float inv = 1.0f / len;
        quat[0] *= inv;
        quat[1] *= inv;
        quat[2] *= inv;
        quat[3] *= inv;
    } else {
        quat[0] = quat[1] = quat[2] = 0.0f;
        quat[3]                     = 1.0f;
    }
}
void TransformComponent::_QuatFromEulerDeg(float x, float y, float z, float* outQuat) {
    // Convert degrees to radians
    x = bx::toRad(x);
    y = bx::toRad(y);
    z = bx::toRad(z);

    float cr = cosf(x * 0.5f);
    float sr = sinf(x * 0.5f);
    float cp = cosf(y * 0.5f);
    float sp = sinf(y * 0.5f);
    float cy = cosf(z * 0.5f);
    float sy = sinf(z * 0.5f);

    outQuat[0] = sr * cp * cy - cr * sp * sy; // x
    outQuat[1] = cr * sp * cy + sr * cp * sy; // y
    outQuat[2] = cr * cp * sy - sr * sp * cy; // z
    outQuat[3] = cr * cp * cy + sr * sp * sy; // w

    _QuatNormalize(outQuat);
}
void TransformComponent::_QuatMultiply(const float* q1, const float* q2, float* outQuat) {
    outQuat[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];
    outQuat[1] = q1[3] * q2[1] - q1[0] * q2[2] + q1[1] * q2[3] + q1[2] * q2[0];
    outQuat[2] = q1[3] * q2[2] + q1[0] * q2[1] - q1[1] * q2[0] + q1[2] * q2[3];
    outQuat[3] = q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2];
}

// --- Dirty flag helpers
void TransformComponent::_MarkLocalDirty() {
    m_dirtyLocal = true;
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

    // Build from local TRS (column-major?)
    float rx = m_rotation[0], ry = m_rotation[1], rz = m_rotation[2],
          rw = m_rotation[3];
    float sx = m_scale[0], sy = m_scale[1], sz = m_scale[2];

    float xx = rx * rx, xy = rx * ry, xz = rx * rz;
    float yy = ry * ry, yz = ry * rz, zz = rz * rz;
    float wx = rw * rx, wy = rw * ry, wz = rw * rz;

    float m[16];
    m[0] = (1.0f - 2.0f * (yy + zz)) * sx;
    m[1] = (2.0f * (xy + wz)) * sx;
    m[2] = (2.0f * (xz - wy)) * sx;
    m[3] = 0.0f;

    m[4] = (2.0f * (xy - wz)) * sy;
    m[5] = (1.0f - 2.0f * (xx + zz)) * sy;
    m[6] = (2.0f * (yz + wx)) * sy;
    m[7] = 0.0f;

    m[8]  = (2.0f * (xz + wy)) * sz;
    m[9]  = (2.0f * (yz - wx)) * sz;
    m[10] = (1.0f - 2.0f * (xx + yy)) * sz;
    m[11] = 0.0f;

    m[12] = m_position[0];
    m[13] = m_position[1];
    m[14] = m_position[2];
    m[15] = 1.0f;

    std::copy(std::begin(m), std::end(m), std::begin(m_localMtx));
    m_dirtyLocal = false;
}
void TransformComponent::_UpdateWorldMatrix() const {
    if (!m_dirtyWorld) return;
    _UpdateLocalMatrix();

    if (m_parent) {
        m_parent->_UpdateWorldMatrix();

        // World = Parent * local
        bx::mtxMul(m_worldMtx, m_parent->m_worldMtx, m_localMtx);
        float tmp[4];
        _QuatMultiply(m_parent->m_worldRotation, m_rotation, tmp);
        std::copy(std::begin(tmp), std::end(tmp), std::begin(m_worldRotation));
        _QuatNormalize(m_worldRotation);
    } else { // No parent, world = local
        std::copy(std::begin(m_localMtx),
                  std::end(m_localMtx),
                  std::begin(m_worldMtx));
    }

    // Extract world position from matrix
    m_worldPosition[0] = m_worldMtx[12];
    m_worldPosition[1] = m_worldMtx[13];
    m_worldPosition[2] = m_worldMtx[14];

    m_dirtyWorld = false;
}

// --- Public API

void TransformComponent::GetRotationEuler(float& x, float& y, float& z) const {
    _UpdateWorldMatrix();

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

void TransformComponent::GetRotationQuaternion(float& x,
                                               float& y,
                                               float& z,
                                               float& w) const {
    _UpdateWorldMatrix();
    x = m_rotation[0];
    y = m_rotation[1];
    z = m_rotation[2];
    w = m_rotation[3];
}

void TransformComponent::SetPosition(float x, float y, float z) {
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
    _MarkLocalDirty();
}

void TransformComponent::SetRotationEuler(float x, float y, float z) {
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
    // this will probably break
   _MarkLocalDirty();
}

void TransformComponent::SetRotationQuat(float x, float y, float z, float w) {
    float worldQ[4] = { x,y,z,w };
    _QuatNormalize(worldQ);
    if (m_parent) {
        m_parent->_UpdateWorldMatrix();
        float pw[4] = { m_parent->m_worldRotation[0], m_parent->m_worldRotation[1], m_parent->m_worldRotation[2], m_parent->m_worldRotation[3] };
        float pinv[4] = { -pw[0], -pw[1], -pw[2], pw[3] };
        float localQ[4];
        _QuatMultiply(pinv, worldQ, localQ);
        m_rotation[0]=localQ[0]; m_rotation[1]=localQ[1]; m_rotation[2]=localQ[2]; m_rotation[3]=localQ[3];
    } else {
        m_rotation[0]=worldQ[0]; m_rotation[1]=worldQ[1]; m_rotation[2]=worldQ[2]; m_rotation[3]=worldQ[3];
    }
    _MarkLocalDirty();
}

// Sets the scale of the transform component.
// Note that the params are FULL EXTENT, not half extents.
void TransformComponent::SetScale(float x, float y, float z) {
    m_scale[0] = x;
    m_scale[1] = y;
    m_scale[2] = z;
    _MarkLocalDirty();
}

// --- World setters

void TransformComponent::SetWorldPosition(float x, float y, float z) {
    if (m_parent) {
        m_parent->_UpdateWorldMatrix();
        float invParentMtx[16];
        bx::mtxInverse(invParentMtx, m_parent->m_worldMtx);
        float lx = invParentMtx[0] * x + invParentMtx[4] * y +
                   invParentMtx[8] * z + invParentMtx[12];
        float ly = invParentMtx[1] * x + invParentMtx[5] * y +
                   invParentMtx[9] * z + invParentMtx[13];
        float lz = invParentMtx[2] * x + invParentMtx[6] * y +
                   invParentMtx[10] * z + invParentMtx[14];
        m_position[0] = lx;
        m_position[1] = ly;
        m_position[2] = lz;
    } else {
        m_position[0] = x;
        m_position[1] = y;
        m_position[2] = z;
    }
    _MarkLocalDirty();
}
void TransformComponent::SetWorldRotationEuler(float x, float y, float z) {
    float q[4];
    _QuatFromEulerDeg(x, y, z, q);
    SetWorldRotationQuat(q[0], q[1], q[2], q[3]);
}
void TransformComponent::SetWorldRotationQuat(float x, float y, float z, float w) {
    float worldQ[4] = { x,y,z,w };
    _QuatNormalize(worldQ);
    if (m_parent) {
        m_parent->_UpdateWorldMatrix();
        float pw[4]   = { m_parent->m_worldRotation[0], m_parent->m_worldRotation[1], m_parent->m_worldRotation[2], m_parent->m_worldRotation[3] };
        float pinv[4] = { -pw[0], -pw[1], -pw[2], pw[3] };
        float localQ[4];
        _QuatMultiply(pinv, worldQ, localQ);
        m_rotation[0]=localQ[0]; m_rotation[1]=localQ[1]; m_rotation[2]=localQ[2]; m_rotation[3]=localQ[3];
    } else {
        m_rotation[0]=worldQ[0]; m_rotation[1]=worldQ[1]; m_rotation[2]=worldQ[2]; m_rotation[3]=worldQ[3];
    }
    _MarkLocalDirty();
}

void TransformComponent::GetModelMatrix(float* result) {
    _UpdateWorldMatrix();
    std::copy(std::begin(m_worldMtx), std::end(m_worldMtx), result);
}

// Global getters
float* TransformComponent::GetPosition() {
    _UpdateWorldMatrix();
    return m_worldPosition;
}

float* TransformComponent::GetScale() {
    _UpdateWorldMatrix();
    return m_scale;
}

float* TransformComponent::GetLocalPosition() { return m_position; }
float* TransformComponent::GetLocalRotation() { return m_rotation; }
float* TransformComponent::GetLocalScale() { return m_scale; }
void TransformComponent::GetLocalMatrix(float* result) {
    _UpdateLocalMatrix();
    std::copy(std::begin(m_localMtx), std::end(m_localMtx), result);
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

} // namespace Syngine