// ╒═════════════════════ Quaternion.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-06                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

#include "Syngine/Math/Matrix3x3.hpp"
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Math/Vector4.hpp"

#include <bx/math.h>

namespace Syngine::Math {

/// @brief Quaternion class for representing rotations in 3D space
class Quaternion {
    DirectX::XMFLOAT4 m_storage;

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
    /// @param axis Rotation axis
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
    /// @return Vector4 where XYZ is the normalized axis and W is the angle in radians
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

    /// @brief Linearly interpolate between this quaternion and another, then normalize the result
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
} // namespace Syngine::Math
