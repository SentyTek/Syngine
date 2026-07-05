// ╒════════════════════════ Vector2.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-03                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

namespace Syngine::Math {

/// @brief 2D vector class for mathematical operations in 2D space
class Vector2 {
    DirectX::XMFLOAT2 m_storage;

  public:
    /// @brief Default constructor, initializes vector to (0.0, 0.0)
    /// @since v0.0.2
    inline Vector2() : m_storage(0.0f, 0.0f) {}

    /// @brief Constructor with x and y values
    /// @param x X component of the vector
    /// @param y Y component of the vector
    /// @since v0.0.2
    inline Vector2(float x, float y) : m_storage(x, y) {}

    /// @brief Get the X component of the vector
    /// @return X component
    /// @threadsafety safe
    /// @since v0.0.2
    inline float x() const { return m_storage.x; }

    /// @brief Get the Y component of the vector
    /// @return Y component
    /// @threadsafety safe
    /// @since v0.0.2
    inline float y() const { return m_storage.y; }

    /// @brief Set the X component of the vector
    /// @param x New X component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void  setX(float x) { m_storage.x = x; }

    /// @brief Set the Y component of the vector
    /// @param y New Y component value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void  setY(float y) { m_storage.y = y; }

    // MARK: Math operations

    /// @brief Add two vectors
    /// @param other Vector to add
    /// @return New vector representing the sum
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 operator+(const Vector2& other) const {
        // Load unaligned storage into XMVECTOR
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);

        // Do the thing and store it
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorAdd(v1, v2));
        return res;
    }

    /// @brief Subtract two vectors
    /// @param other Vector to subtract
    /// @return New vector representing the difference
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 operator-(const Vector2& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);

        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorSubtract(v1, v2));
        return res;
    }

    /// @brief Scale vector by a scalar value
    /// @param scalar Scalar multiplier
    /// @return New scaled vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 operator*(float scalar) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorScale(v, scalar));
        return res;
    }

    /// @brief Divide vector by a scalar value
    /// @param scalar Scalar divisor
    /// @return New divided vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 operator/(float scalar) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorScale(v, 1.0f / scalar));
        return res;
    }

    /// @brief Add vector in-place
    /// @param other Vector to add
    /// @return Reference to this vector after addition
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector2& operator+=(const Vector2& other) {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        DirectX::XMStoreFloat2(&m_storage, DirectX::XMVectorAdd(v1, v2));
        return *this;
    }

    /// @brief Subtract vector in-place
    /// @param other Vector to subtract
    /// @return Reference to this vector after subtraction
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector2& operator-=(const Vector2& other) {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        DirectX::XMStoreFloat2(&m_storage, DirectX::XMVectorSubtract(v1, v2));
        return *this;
    }

    /// @brief Scale vector in-place
    /// @param scalar Scalar multiplier
    /// @return Reference to this vector after scaling
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector2& operator*=(float scalar) {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMStoreFloat2(&m_storage, DirectX::XMVectorScale(v, scalar));
        return *this;
    }

    /// @brief Divide vector in-place
    /// @param scalar Scalar divisor
    /// @return Reference to this vector after division
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector2& operator/=(float scalar) {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMStoreFloat2(&m_storage, DirectX::XMVectorScale(v, 1.0f / scalar));
        return *this;
    }

    // MARK: Comparison operators

    /// @brief Check if two vectors are equal
    /// @param other Vector to compare
    /// @return true if vectors are equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator==(const Vector2& other) const {
        return m_storage.x == other.m_storage.x && m_storage.y == other.m_storage.y;
    }

    /// @brief Check if two vectors are not equal
    /// @param other Vector to compare
    /// @return true if vectors are not equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    /// @brief Negate the vector
    /// @return New negated vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 operator-() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorNegate(v));
        return res;
    }

    /// @brief Check if all components are less than another vector
    /// @param other Vector to compare
    /// @return true if all components are less, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator<(const Vector2& other) const {
        return m_storage.x < other.m_storage.x && m_storage.y < other.m_storage.y;
    }

    /// @brief Check if all components are less than or equal to another vector
    /// @param other Vector to compare
    /// @return true if all components are less or equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator<=(const Vector2& other) const {
        return m_storage.x <= other.m_storage.x && m_storage.y <= other.m_storage.y;
    }

    /// @brief Check if all components are greater than another vector
    /// @param other Vector to compare
    /// @return true if all components are greater, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator>(const Vector2& other) const {
        return m_storage.x > other.m_storage.x && m_storage.y > other.m_storage.y;
    }

    /// @brief Check if all components are greater than or equal to another vector
    /// @param other Vector to compare
    /// @return true if all components are greater or equal, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    inline bool operator>=(const Vector2& other) const {
        return m_storage.x >= other.m_storage.x && m_storage.y >= other.m_storage.y;
    }

    // MARK: Normal vector operations

    /// @brief Calculate the length (magnitude) of the vector
    /// @return Length of the vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline float length() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector2Length(v));
    }

    /// @brief Calculate the squared length of the vector (more efficient than length)
    /// @return Squared length of the vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline float lengthSquared() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(v));
    }

    /// @brief Calculate the distance between this vector and another vector
    /// @param other Target vector
    /// @return Distance between vectors
    /// @threadsafety safe
    /// @since v0.0.2
    inline float distance(const Vector2& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(v1, v2)));
    }

    /// @brief Calculate the squared distance between this vector and another vector
    /// @param other Target vector
    /// @return Squared distance between vectors
    /// @threadsafety safe
    /// @since v0.0.2
    inline float distanceSquared(const Vector2& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(DirectX::XMVectorSubtract(v1, v2)));
    }

    /// @brief Get the normalized (unit) vector in the same direction
    /// @return New normalized vector with length 1
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 normalized() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVector2Normalize(v));
        return res;
    }

    /// @brief Calculate the dot product with another vector
    /// @param other Vector to dot with
    /// @return Dot product result
    /// @threadsafety safe
    /// @since v0.0.2
    inline float dot(const Vector2& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector2Dot(v1, v2));
    }

    /// @brief Calculate the cross product with another vector
    /// @param other Vector to cross with
    /// @return Cross product result (in 2D, returns scalar Z component)
    /// @threadsafety safe
    /// @since v0.0.2
    inline float cross(const Vector2& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        return DirectX::XMVectorGetX(DirectX::XMVector2Cross(v1, v2));
    }

    // MARK: other ops

    /// @brief Linear interpolation between this vector and another
    /// @param other Target vector to interpolate towards
    /// @param t Interpolation factor (0 = this vector, 1 = other vector)
    /// @return Interpolated vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 lerp(const Vector2& other, float t) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorLerp(v1, v2, t));
        return res;
    }

    /// @brief Clamp vector components to a range defined by min and max vectors
    /// @param min Minimum bounds vector
    /// @param max Maximum bounds vector
    /// @return Clamped vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 clamp(const Vector2& min, const Vector2& max) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR vmin = DirectX::XMLoadFloat2(&min.m_storage);
        DirectX::XMVECTOR vmax = DirectX::XMLoadFloat2(&max.m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorClamp(v, vmin, vmax));
        return res;
    }

    /// @brief Reflect this vector off a surface with the given normal
    /// @param normal Surface normal vector
    /// @return Reflected vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 reflect(const Vector2& normal) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR n = DirectX::XMLoadFloat2(&normal.m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVector2Reflect(v, n));
        return res;
    }

    /// @brief Refract this vector through a surface with the given normal and eta ratio
    /// @param normal Surface normal vector
    /// @param eta Ratio of refractive indices
    /// @return Refracted vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 refract(const Vector2& normal, float eta) const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR n = DirectX::XMLoadFloat2(&normal.m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVector2Refract(v, n, eta));
        return res;
    }

    /// @brief Get component-wise minimum with another vector
    /// @param other Vector to compare
    /// @return Vector with minimum components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 min(const Vector2& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorMin(v1, v2));
        return res;
    }

    /// @brief Get component-wise maximum with another vector
    /// @param other Vector to compare
    /// @return Vector with maximum components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 max(const Vector2& other) const {
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat2(&m_storage);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat2(&other.m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorMax(v1, v2));
        return res;
    }

    /// @brief Get absolute value of vector components
    /// @return Vector with absolute value components
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector2 abs() const {
        DirectX::XMVECTOR v = DirectX::XMLoadFloat2(&m_storage);
        Vector2 res;
        DirectX::XMStoreFloat2(&res.m_storage, DirectX::XMVectorAbs(v));
        return res;
    }
};

// Alias Vec2 for Vector2
using Vec2 = Vector2;

} // namespace Syngine::Math
