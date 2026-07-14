// ╒════════════════════════ Vector3.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-04                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

#include "Syngine/Math/Math.hpp"
#include "Syngine/Math/Vector2.hpp"

#include <bx/math.h>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Math.h>
#include "Jolt/Math/Real.h"

#include <stdexcept>
#include <vector>

namespace Syngine::Math {

/// @brief 3D vector class for mathematical operations in 3D space
/// @alias Vec3, SVec3
class Vector3 {
    DirectX::XMFLOAT3 m_storage;

    DirectX::XMFLOAT3 m_getStorage() const { return m_storage; }

    inline Vector3(const DirectX::XMFLOAT3& storage) : m_storage(storage) {}

    friend class Matrix3x3; // for matrix-vector multiplication
    friend class Quaternion; // for quaternion-vector multiplication
    friend class Ray;        // for ray operations
  public:
    // MARK: Constructors

    /// @brief Default constructor, initializes the vector to (0.0, 0.0, 0.0)
    /// @constructor
    /// @since v0.0.2
    inline Vector3() : m_storage(0.0f, 0.0f, 0.0f) {}

    /// @brief Construct a vector from a single float value, initializes all
    /// components to the same value
    /// @param value Value to set all components
    /// @constructor
    /// @priority
    /// @since v0.0.2
    inline Vector3(float value) : m_storage(value, value, value) {}

    /// @brief Construct a vector from three float components
    /// @param x X component of the vector
    /// @param y Y component of the vector
    /// @param z Z component of the vector
    /// @constructor
    /// @priority
    /// @since v0.0.2
    inline Vector3(float x, float y, float z) : m_storage(x, y, z) {}

    /// @brief Construct a vector from a 2D vector and a Z component
    /// @param v Source 2D vector
    /// @param z Z component of the vector
    /// @constructor
    /// @since v0.0.2
    inline Vector3(const Vector2& v, float z) : m_storage(v.x(), v.y(), z) {}

    /// @brief Construct a vector from a bgfx vector
    /// @param v Source bgfx vector
    /// @constructor
    /// @since v0.0.2
    inline Vector3(const bx::Vec3& v) : m_storage(v.x, v.y, v.z) {}

    /// @brief Construct a vector from a Jolt vector
    /// @param v Source Jolt vector
    /// @constructor
    /// @since v0.0.2
    inline Vector3(const JPH::Vec3& v)
        : m_storage(v.GetX(), v.GetY(), v.GetZ()) {}

    /// @brief Constructor from std::vector of floats, expects exactly 3
    /// elements
    /// @param values std::vector containing three float values
    /// @throws std::invalid_argument if the vector does not contain exactly 3
    /// elements
    /// @constructor
    /// @since v0.0.2
    inline Vector3(const ::std::vector<float>& values) {
        if (values.size() != 3) {
            throw ::std::invalid_argument("Vector3 constructor requires exactly 3 float values.");
        }
        m_storage.x = values[0];
        m_storage.y = values[1];
        m_storage.z = values[2];
    }

    /// @brief Copy constructor
    /// @param other Vector to copy
    /// @constructor
    /// @since v0.0.2
    inline Vector3(const Vector3& other) = default;

    /// @brief Assignment operator
    /// @param other Vector to assign from
    /// @return Reference to this vector after assignment
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector3& operator=(const Vector3& other) = default;

    inline Vector3(Vector3&& other) noexcept = default;
    inline Vector3& operator=(Vector3&& other) noexcept = default;

    // MARK: Accessors

    /// @brief Get the raw data as a pointer to float array
    /// @return Pointer to the first element of the float array
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline float* data() { return reinterpret_cast<float*>(&m_storage); }

    /// @brief Get the raw data as a pointer to float array
    /// @return Pointer to the first element of the float array
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline const float* data() const { return reinterpret_cast<const float*>(&m_storage); }

    /// @brief Get the X component of the vector
    /// @return X component
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline float x() const { return m_storage.x; }

    /// @brief Get the Y component of the vector
    /// @return Y component
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline float y() const { return m_storage.y; }

    /// @brief Get the Z component of the vector
    /// @return Z component
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline float z() const { return m_storage.z; }

    /// @brief Get the XY components as a 2D vector
    /// @return Vector2 containing the X and Y components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 xy() const { return Vector2(m_storage.x, m_storage.y); }

    /// @brief Get the XZ components as a 2D vector
    /// @return Vector2 containing the X and Z components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 xz() const { return Vector2(m_storage.x, m_storage.z); }

    /// @brief Get the YZ components as a 2D vector
    /// @return Vector2 containing the Y and Z components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 yz() const { return Vector2(m_storage.y, m_storage.z); }

    /// @brief Set the X component of the vector
    /// @param x New X component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setX(float x) { m_storage.x = x; }

    /// @brief Set the Y component of the vector
    /// @param y New Y component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setY(float y) { m_storage.y = y; }

    /// @brief Set the Z component of the vector
    /// @param z New Z component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setZ(float z) { m_storage.z = z; }

    /// @brief Convert this vector to a bgfx vector
    /// @return Equivalent bgfx vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline bx::Vec3 toBxVec3() const {
        return bx::Vec3(m_storage.x, m_storage.y, m_storage.z);
    }

    /// @brief Convert this vector to a Jolt Vec3
    /// @return Equivalent Jolt Vec3
    /// @threadsafety safe
    /// @since v0.0.2
    inline JPH::Vec3 toJoltVec3() const {
        return JPH::Vec3(m_storage.x, m_storage.y, m_storage.z);
    }

    /// @brief Convert this vector to a Jolt RVec3
    /// @return Equivalent Jolt RVec3
    /// @threadsafety safe
    /// @since v0.0.2
    inline JPH::RVec3 toJoltRVec3() const {
        return JPH::RVec3(m_storage.x, m_storage.y, m_storage.z);
    }

    /// @brief Convert this vector of euler angles to a vector of radians
    /// @return Vector3 containing the angles in radians
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline Vector3 toRads() const {
        constexpr float DEGRADFACTOR = 3.14159265358979323846f / 180.0f;
        return Vector3(m_storage.x * DEGRADFACTOR,
                       m_storage.y * DEGRADFACTOR,
                       m_storage.z * DEGRADFACTOR);
    }

    /// @brief Convert this vector of radians to a vector of euler angles
    /// @return Vector3 containing the angles in degrees
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 toDegs() const {
        constexpr float RADDEGFACTOR = 180.0f / 3.14159265358979323846f;
        return Vector3(m_storage.x * RADDEGFACTOR,
                       m_storage.y * RADDEGFACTOR,
                       m_storage.z * RADDEGFACTOR);
    }

    /// @brief Convert this vector to a std::vector of floats
    /// @return std::vector containing the X, Y, and Z components
    /// @threadsafety safe
    /// @since v0.0.2
    inline operator std::vector<float>() const {
        return ::std::vector<float>{m_storage.x, m_storage.y, m_storage.z};
    }

    /// @brief Get a component by index using bracket syntax
    /// @param index Component index in range [0, 2]
    /// @return Component value at the given index
    /// @threadsafety safe
    /// @since v0.0.2
    inline float operator[] (int index) const {
        if (index < 0 || index > 2) {
            throw ::std::out_of_range("Index must be between 0 and 2 for Vector3.");
        }
        return reinterpret_cast<const float*>(&m_storage)[index];
    }

    /// @brief Set a component by index
    /// @param index Component index in range [0, 2]
    /// @param value New component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void set(int index, float value) {
        if (index < 0 || index > 2) {
            throw ::std::out_of_range("Index must be between 0 and 2 for Vector3.");
        }
        reinterpret_cast<float*>(&m_storage)[index] = value;
    }

    /// @brief Check if the vector is a zero vector (all components are zero)
    /// @return true if all components are zero, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool isZero() const {
        return m_storage.x == 0.0f && m_storage.y == 0.0f && m_storage.z == 0.0f;
    }

    // MARK: Math operations

    /// @brief Add two vectors
    /// @param other Vector to add
    /// @return New vector representing the sum
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator+(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorAdd(v1, v2));
        return res;
    }

    /// @brief Subtract two vectors
    /// @param other Vector to subtract
    /// @return New vector representing the difference
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator-(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorSubtract(v1, v2));
        return res;
    }

    /// @brief Scale vector by a scalar value
    /// @param scalar Scalar multiplier
    /// @return New scaled vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator*(float scalar) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorScale(v, scalar));
        return res;
    }

    /// @brief Divide vector by a scalar value
    /// @param scalar Scalar divisor
    /// @return New divided vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator/(float scalar) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorScale(v, 1.0f / scalar));
        return res;
    }

    /// @brief Add vector in-place
    /// @param other Vector to add
    /// @return Reference to this vector after addition
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector3& operator+=(const Vector3& other) {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        DirectX::XMStoreFloat3(&m_storage, DirectX::XMVectorAdd(v1, v2));
        return *this;
    }

    /// @brief Subtract vector in-place
    /// @param other Vector to subtract
    /// @return Reference to this vector after subtraction
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector3& operator-=(const Vector3& other) {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        DirectX::XMStoreFloat3(&m_storage, DirectX::XMVectorSubtract(v1, v2));
        return *this;
    }

    /// @brief Scale vector in-place
    /// @param scalar Scalar multiplier
    /// @return Reference to this vector after scaling
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector3& operator*=(float scalar) {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMStoreFloat3(&m_storage, DirectX::XMVectorScale(v, scalar));
        return *this;
    }

    /// @brief Divide vector in-place
    /// @param scalar Scalar divisor
    /// @return Reference to this vector after division
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector3& operator/=(float scalar) {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMStoreFloat3(&m_storage, DirectX::XMVectorScale(v, 1.0f / scalar));
        return *this;
    }

    // MARK: Comparison operators

    /// @brief Check if two vectors are equal
    /// @param other Vector to compare
    /// @return true if vectors are equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator==(const Vector3& other) const {
        return m_storage.x == other.m_storage.x && m_storage.y == other.m_storage.y &&
               m_storage.z == other.m_storage.z;
    }

    /// @brief Check if two vectors are not equal
    /// @param other Vector to compare
    /// @return true if vectors are not equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }

    /// @brief Check if all components are less than another vector
    /// @param other Vector to compare
    /// @return true if all components are less, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator<(const Vector3& other) const {
        return m_storage.x < other.m_storage.x && m_storage.y < other.m_storage.y &&
               m_storage.z < other.m_storage.z;
    }

    /// @brief Check if all components are less than or equal to another vector
    /// @param other Vector to compare
    /// @return true if all components are less or equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator<=(const Vector3& other) const {
        return m_storage.x <= other.m_storage.x && m_storage.y <= other.m_storage.y &&
               m_storage.z <= other.m_storage.z;
    }

    /// @brief Check if all components are greater than another vector
    /// @param other Vector to compare
    /// @return true if all components are greater, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator>(const Vector3& other) const {
        return m_storage.x > other.m_storage.x && m_storage.y > other.m_storage.y &&
               m_storage.z > other.m_storage.z;
    }

    /// @brief Check if all components are greater than or equal to another vector
    /// @param other Vector to compare
    /// @return true if all components are greater or equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator>=(const Vector3& other) const {
        return m_storage.x >= other.m_storage.x && m_storage.y >= other.m_storage.y &&
               m_storage.z >= other.m_storage.z;
    }

    /// @brief Negate the vector
    /// @return New negated vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator-() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorNegate(v));
        return res;
    }

    // MARK: Normal vector operations

    /// @brief Calculate the length (magnitude) of the vector
    /// @return Length of the vector
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline float length() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector3Length(v));
    }

    /// @brief Calculate the squared length of the vector (more efficient than length)
    /// @return Squared length of the vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline float lengthSquared() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(v));
    }

    /// @brief Normalize the vector in-place to have a length of 1
    /// @note This function modifies the current vector. To get a new normalized
    /// vector without modifying the current one, use the `normalized()` method.
    /// @threadsafety not-safe
    /// @priority
    /// @since v0.0.2
    inline void normalize() {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMStoreFloat3(&m_storage, DirectX::XMVector3Normalize(v));
    }

    /// @brief Get the normalized (unit) vector in the same direction
    /// @note This function RETURNS a new normalized vector without modifying
    /// the current one. To normalize in-place, use the `normalize()` method.
    /// @return New normalized vector with length 1
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 normalized() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVector3Normalize(v));
        return res;
    }

    /// @brief Calculate the dot product with another vector
    /// @param other Vector to dot with
    /// @return Dot product result
    /// @threadsafety safe
    /// @since v0.0.2
    inline float dot(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1, v2));
    }

    /// @brief Calculate the cross product with another vector
    /// @param other Vector to cross with
    /// @return Cross product vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 cross(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVector3Cross(v1, v2));
        return res;
    }

    /// @brief Calculate the distance between this vector and another vector
    /// @param other Target vector
    /// @return Distance between vectors
    /// @threadsafety safe
    /// @priority
    /// @since v0.0.2
    inline float distance(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(v1, v2)));
    }

    /// @brief Calculate the squared distance between this vector and another vector
    /// @param other Target vector
    /// @return Squared distance between vectors
    /// @threadsafety safe
    /// @since v0.0.2
    inline float distanceSquared(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        return DirectX::XMVectorGetX(
            DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(v1, v2)));
    }

    // Mark: Other operations

    /// @brief Linear interpolation between this vector and another
    /// @param other Target vector to interpolate towards
    /// @param t Interpolation factor (0 = this vector, 1 = other vector)
    /// @return Interpolated vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 lerp(const Vector3& other, float t) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorLerp(v1, v2, t));
        return res;
    }

    /// @brief Clamp vector components to a range defined by min and max vectors
    /// @param min Minimum bounds vector
    /// @param max Maximum bounds vector
    /// @return Clamped vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 clamp(const Vector3& min, const Vector3& max) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR vmin = DirectX::XMLoadFloat3(&min.m_storage);
        DirectX::XMVECTOR vmax = DirectX::XMLoadFloat3(&max.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorClamp(v, vmin, vmax));
        return res;
    }

    /// @brief Get absolute value of vector components
    /// @return Vector with absolute value components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 abs() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorAbs(v));
        return res;
    }

    /// @brief Reflect this vector off a surface with the given normal
    /// @param normal Surface normal vector
    /// @return Reflected vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 reflect(const Vector3& normal) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&normal.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVector3Reflect(v, n));
        return res;
    }

    /// @brief Refract this vector through a surface with the given normal and eta ratio
    /// @param normal Surface normal vector
    /// @param eta Ratio of refractive indices
    /// @return Refracted vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 refract(const Vector3& normal, float eta) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&normal.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVector3Refract(v, n, eta));
        return res;
    }

    /// @brief Get component-wise minimum with another vector
    /// @param other Vector to compare
    /// @return Vector with minimum components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 min(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorMin(v1, v2));
        return res;
    }

    /// @brief Get component-wise maximum with another vector
    /// @param other Vector to compare
    /// @return Vector with maximum components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 max(const Vector3& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&other.m_storage);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, DirectX::XMVectorMax(v1, v2));
        return res;
    }
};

using Vec3 = Vector3; // Alias Vec3 for Vector3
using SVec3 = Vector3; // Alias SVec3 for Vector3

} // namespace Syngine::Math
