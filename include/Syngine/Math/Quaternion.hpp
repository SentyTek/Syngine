// ╒═════════════════════ Quaternion.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-06                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

#include <cmath>
#include <stdexcept>
#include <vector>

#include "Syngine/Math/Matrix3x3.hpp"
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Math/Vector4.hpp"

#include <bx/math.h>
#include <Jolt/Jolt.h>
#include "Jolt/Math/Quat.h"

namespace Syngine::Math {

/// @brief Quaternion class for representing rotations in 3D space
class Quaternion {
    DirectX::XMFLOAT4 m_storage;

    friend class Matrix4x4; // for matrix-quaternion conversion
  public:
    // MARK: Constructors

    /// @brief Default constructor, initializes the quaternion to (0.0, 0.0, 0.0, 1.0)
    /// @since v0.0.2
    inline Quaternion() : m_storage(0.0f, 0.0f, 0.0f, 1.0f) {}

    /// @brief Construct a quaternion from four float components
    /// @param x X component of the quaternion
    /// @param y Y component of the quaternion
    /// @param z Z component of the quaternion
    /// @param w W component of the quaternion
    /// @since v0.0.2
    inline Quaternion(float x, float y, float z, float w)
        : m_storage(x, y, z, w) {}

    /// @brief Construct a quaternion from a rotation axis and angle
    /// @param axis Rotation axis (normalized internally)
    /// @param angle Rotation angle in radians
    /// @since v0.0.2
    inline Quaternion(const Vector3& axis, float angle) {
        DirectX::XMVECTOR axisVec = DirectX::XMVector3Normalize(
            DirectX::XMVectorSet(axis.x(), axis.y(), axis.z(), 0.0f));
        DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(axisVec, angle);
        DirectX::XMStoreFloat4(&m_storage, q);
    }

    /// @brief Construct a quaternion from Euler angles (pitch, yaw, roll)
    /// @param eulerAngles Euler angles in radians (pitch, yaw, roll)
    /// @since v0.0.2
    inline Quaternion(const Vector3& eulerAngles) {
        DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(
            eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
        DirectX::XMStoreFloat4(&m_storage, q);
    }

    /// @brief Constructor from std::vector of floats, expects exactly 4 elements
    /// @param values std::vector containing four float values
    /// @throws std::invalid_argument if the vector does not contain exactly 4 elements
    /// @since v0.0.2
    inline Quaternion(const std::vector<float>& values) {
        if (values.size() != 4) {
            throw std::invalid_argument("Quaternion constructor requires exactly 4 float values.");
        }
        m_storage.x = values[0];
        m_storage.y = values[1];
        m_storage.z = values[2];
        m_storage.w = values[3];
    }

    /// @brief Construct a quaternion from a Jolt::Quat
    /// @param quat Jolt::Quat to convert from
    /// @since v0.0.2
    inline Quaternion(const JPH::Quat& quat)
        : m_storage(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()) {}

    // MARK: Conversions

    /// @brief Convert this quaternion to a bx::Quaternion
    /// @return bx::Quaternion representation of this quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline bx::Quaternion toBxQuat() const {
        return bx::Quaternion(m_storage.x, m_storage.y, m_storage.z, m_storage.w);
    }

    /// @brief Convert this quaternion to a Jolt::Quat
    /// @return Jolt::Quat representation of this quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline JPH::Quat toJoltQuat() const {
        return JPH::Quat(m_storage.x, m_storage.y, m_storage.z, m_storage.w);
    }

    /// @brief Convert this quaternion to a std::vector of floats
    /// @return std::vector containing the four components of the quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline operator std::vector<float>() const {
        return {m_storage.x, m_storage.y, m_storage.z, m_storage.w};
    }

    // MARK: Accessors

    /// @brief Get the X component of the quaternion
    /// @return X component
    /// @threadsafety safe
    /// @since v0.0.2
    inline float x() const { return m_storage.x; }

    /// @brief Get the Y component of the quaternion
    /// @return Y component
    /// @threadsafety safe
    /// @since v0.0.2
    inline float y() const { return m_storage.y; }

    /// @brief Get the Z component of the quaternion
    /// @return Z component
    /// @threadsafety safe
    /// @since v0.0.2
    inline float z() const { return m_storage.z; }

    /// @brief Get the W component of the quaternion
    /// @return W component
    /// @threadsafety safe
    /// @since v0.0.2
    inline float w() const { return m_storage.w; }

    /// @brief Set the X component of the quaternion
    /// @param x New X component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setX(float x) { m_storage.x = x; }

    /// @brief Set the Y component of the quaternion
    /// @param y New Y component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setY(float y) { m_storage.y = y; }

    /// @brief Set the Z component of the quaternion
    /// @param z New Z component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setZ(float z) { m_storage.z = z; }

    /// @brief Set the W component of the quaternion
    /// @param w New W component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setW(float w) { m_storage.w = w; }

    // MARK: Arithmetic operations

    /// @brief Compose this quaternion with another quaternion
    /// @param other Quaternion to multiply with
    /// @return Composed quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline Quaternion operator*(const Quaternion& other) const {
        DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&other.m_storage);
        DirectX::XMVECTOR result = DirectX::XMQuaternionMultiply(q1, q2);
        Quaternion res;
        DirectX::XMStoreFloat4(&res.m_storage, result);
        return res;
    }

    /// @brief Rotate a 3D vector by this quaternion
    /// @param vec Vector to rotate
    /// @return Rotated vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator*(const Vector3& vec) const {
        DirectX::XMVECTOR q = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&m_storage));
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&vec.m_storage);
        DirectX::XMVECTOR result = DirectX::XMVector3Rotate(v, q);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, result);
        return res;
    }

    /// @brief Add two quaternions component-wise
    /// @param other Quaternion to add
    /// @return Sum quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline Quaternion operator+(const Quaternion& other) const {
        DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&other.m_storage);
        DirectX::XMVECTOR result = DirectX::XMVectorAdd(q1, q2);
        Quaternion res;
        DirectX::XMStoreFloat4(&res.m_storage, result);
        return res;
    }

    /// @brief Subtract two quaternions component-wise
    /// @param other Quaternion to subtract
    /// @return Difference quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline Quaternion operator-(const Quaternion& other) const {
        DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&other.m_storage);
        DirectX::XMVECTOR result = DirectX::XMVectorSubtract(q1, q2);
        Quaternion res;
        DirectX::XMStoreFloat4(&res.m_storage, result);
        return res;
    }

    // MARK: Normal quaternion operations

    /// @brief Conjugate this quaternion in-place
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void conjugate() {
        DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR result = DirectX::XMQuaternionConjugate(q);
        DirectX::XMStoreFloat4(&m_storage, result);
    }

    /// @brief Normalize this quaternion in-place
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void normalize() {
        DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR result = DirectX::XMQuaternionNormalize(q);
        DirectX::XMStoreFloat4(&m_storage, result);
    }

    /// @brief Invert this quaternion in-place
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void invert() {
        DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR result = DirectX::XMQuaternionInverse(q);
        DirectX::XMStoreFloat4(&m_storage, result);
    }

    /// @brief Calculate the dot product with another quaternion
    /// @param other Quaternion to dot with
    /// @return Dot product result
    /// @threadsafety safe
    /// @since v0.0.2
    inline float dot(const Quaternion& other) const {
        DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&other.m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector4Dot(q1, q2));
    }

    // MARK: Conversions

    /// @brief Convert this quaternion to a 3x3 rotation matrix
    /// @return Equivalent rotation matrix
    /// @threadsafety safe
    /// @since v0.0.2
    inline Matrix3x3 toMatrix3x3() const {
        DirectX::XMVECTOR q = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&m_storage));
        DirectX::XMMATRIX m = DirectX::XMMatrixRotationQuaternion(q);
        Matrix3x3 res;
        DirectX::XMStoreFloat3x3(&res.m_storage, m);
        return res;
    }

    /// @brief Convert this quaternion to axis-angle form
    /// @return Vector4 where XYZ is the normalized axis and W is angle in radians
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector4 toAxisAngle() const {
        DirectX::XMVECTOR q = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&m_storage));
        DirectX::XMVECTOR axis;
        float angle;
        DirectX::XMQuaternionToAxisAngle(&axis, &angle, q);
        Vector4 res;
        DirectX::XMStoreFloat4(&res.m_storage, axis);
        res.m_storage.w = angle;
        return res;
    }

    /// @brief Convert this quaternion to Euler angles (pitch, yaw, roll)
    /// @return Euler angles in radians as (pitch X, yaw Y, roll Z)
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 toEulerAngles() const {
        DirectX::XMVECTOR q = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&m_storage));
        DirectX::XMFLOAT4 qf;
        DirectX::XMStoreFloat4(&qf, q);

        const float x = qf.x;
        const float y = qf.y;
        const float z = qf.z;
        const float w = qf.w;

        // X-axis (pitch)
        const float sinx_cosp = 2.0f * (w * x + y * z);
        const float cosx_cosp = 1.0f - 2.0f * (x * x + y * y);
        const float pitch = std::atan2(sinx_cosp, cosx_cosp);

        // Y-axis (yaw)
        const float siny = 2.0f * (w * y - z * x);
        float yaw;
        if (std::fabs(siny) >= 1.0f) {
            yaw = std::copysign(DirectX::XM_PIDIV2, siny);
        } else {
            yaw = std::asin(siny);
        }

        // Z-axis (roll)
        const float sinz_cosp = 2.0f * (w * z + x * y);
        const float cosz_cosp = 1.0f - 2.0f * (y * y + z * z);
        const float roll = std::atan2(sinz_cosp, cosz_cosp);

        return Vector3(pitch, yaw, roll);
    }

    // MARK: Interpolation

    /// @brief Spherically interpolate between this quaternion and another
    /// @param other Target quaternion
    /// @param t Interpolation factor in [0, 1]
    /// @return Interpolated quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline Quaternion slerp(const Quaternion& other, float t) const {
        DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&other.m_storage);
        DirectX::XMVECTOR result = DirectX::XMQuaternionSlerp(q1, q2, t);
        Quaternion res;
        DirectX::XMStoreFloat4(&res.m_storage, result);
        return res;
    }

    /// @brief Linearly interpolate between this quaternion and another, then normalize
    /// @param other Target quaternion
    /// @param t Interpolation factor in [0, 1]
    /// @return Interpolated unit quaternion
    /// @threadsafety safe
    /// @since v0.0.2
    inline Quaternion lerp(const Quaternion& other, float t) const {
        DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&m_storage);
        DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&other.m_storage);
        DirectX::XMVECTOR result = DirectX::XMQuaternionNormalize(DirectX::XMVectorLerp(q1, q2, t));
        Quaternion res;
        DirectX::XMStoreFloat4(&res.m_storage, result);
        return res;
    }
};

using Quat = Quaternion; // Alias Quat for Quaternion
using SQuat = Quaternion; // Alias SQuat for Quaternion
} // namespace Syngine::Math
