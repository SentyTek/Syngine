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
    position[0] = position[1] = position[2] = 0.0f;
    rotation[0] = rotation[1] = rotation[2] = 0.0f; rotation[3] = 1.0f; // Quaternion identity
    scale[0] = scale[1] = scale[2] = 0.5f;
    this->m_owner = owner;
}

void TransformComponent::Init(std::vector<float> position,
                              std::vector<float> rotation,
                              std::vector<float> scale) {
    this->position[0] = position[0];
    this->position[1] = position[1];
    this->position[2] = position[2];

    this->rotation[0] = rotation[0];
    this->rotation[1] = rotation[1];
    this->rotation[2] = rotation[2];
    this->rotation[3] = rotation[3];

    this->scale[0] = scale[0];
    this->scale[1] = scale[1];
    this->scale[2] = scale[2];
}

Syngine::Components TransformComponent::GetComponentType() {
    return SYN_COMPONENT_TRANSFORM;
}

void TransformComponent::GetRotationAsEuler(float& x, float& y, float& z) const {
    const float qx = rotation[0];
    const float qy = rotation[1];
    const float qz = rotation[2];
    const float qw = rotation[3];

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

void TransformComponent::SetPosition(float x, float y, float z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
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

    rotation[0] = sr * cp * cy - cr * sp * sy; // x
    rotation[1] = cr * sp * cy + sr * cp * sy; // y
    rotation[2] = cr * cp * sy - sr * sp * cy; // z
    rotation[3] = cr * cp * cy + sr * sp * sy; // w
    //this will probably break
}

void TransformComponent::SetRotation(float x, float y, float z, float w) {
    rotation[0] = x;
    rotation[1] = y;
    rotation[2] = z;
    rotation[3] = w;
}

// Sets the scale of the transform component.
// Note that the params are FULL EXTENT, not half extents.
void TransformComponent::SetScale(float x, float y, float z) {
    scale[0] = x/2;
    scale[1] = y/2;
    scale[2] = z/2;
}

void TransformComponent::GetModelMatrix(float* result) {
    // Calculate the model matrix based on position, rotation, and scale
    // Probably implement a proper matrix library for this

    float rx = rotation[0], ry = rotation[1], rz = rotation[2], rw = rotation[3];
    float sx = scale[0], sy = scale[1], sz = scale[2];

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

    result[12] = position[0];
    result[13] = position[1];
    result[14] = position[2];
    result[15] = 1.0f;
}

const float* TransformComponent::GetPosition() {
    return position;
}

} // namespace Syngine