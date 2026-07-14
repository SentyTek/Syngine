// ╒════════════════════════════ Vector4.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-04                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

#include "Syngine/Math/Vector2.hpp"
#include "Syngine/Math/Vector3.hpp"

#include <bx/math.h>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Math.h>
#include "Jolt/Math/Real.h"

#include <stdexcept>
#include <vector>
namespace Syngine::Math {

class Matrix4x4; // Forward declaration

/// @brief 4D vector class for mathematical operations in 4D space
class Vector4 {
	DirectX::XMFLOAT4 m_storage;

    friend class Matrix4x4; // for matrix-vector multiplication
    friend class Quaternion; // for quaternion-vector multiplication
  public:
	// MARK: Constructors

	/// @brief Default constructor, initializes the vector to (0.0, 0.0, 0.0, 0.0)
	/// @since v0.0.2
    inline Vector4() : m_storage(0.0f, 0.0f, 0.0f, 0.0f) {}

    /// @brief Construct a vector from a single float value, initializes all
    /// components to the same value
    /// @param value Value to set all components
	/// @since v0.0.2
	inline Vector4(float value) : m_storage(value, value, value, value) {}

	/// @brief Construct a vector from four float components
	/// @param x X component of the vector
	/// @param y Y component of the vector
	/// @param z Z component of the vector
	/// @param w W component of the vector
	/// @since v0.0.2
	inline Vector4(float x, float y, float z, float w) : m_storage(x, y, z, w) {}

	/// @brief Construct a vector from a 3D vector and a W component
	/// @param v Source 3D vector
	/// @param w W component of the vector
	/// @since v0.0.2
	inline Vector4(const Vector3& v, float w = 1.0f) : m_storage(v.x(), v.y(), v.z(), w) {}

	/// @brief Construct a vector from a 2D vector and Z/W components
	/// @param v Source 2D vector
	/// @param z Z component of the vector
	/// @param w W component of the vector
	/// @since v0.0.2
	inline Vector4(const Vector2& v, float z, float w) : m_storage(v.x(), v.y(), z, w) {}

	/// @brief Construct a vector from a bgfx vector and W component
	/// @param v Source bgfx vector
	/// @param w W component of the vector
	/// @since v0.0.2
	inline Vector4(const bx::Vec3& v, float w = 1.0f) : m_storage(v.x, v.y, v.z, w) {}

	/// @brief Construct a vector from a Jolt Vec3 and W component
	/// @param v Source Jolt Vec3
	/// @param w W component of the vector
	/// @since v0.0.2
	inline Vector4(const JPH::Vec3& v, float w = 1.0f)
		: m_storage(v.GetX(), v.GetY(), v.GetZ(), w) {}

	/// @brief Construct a vector from a Jolt Vec4
	/// @param v Source Jolt Vec4
	/// @since v0.0.2
    inline Vector4(const JPH::Vec4& v)
        : m_storage(v.GetX(), v.GetY(), v.GetZ(), v.GetW()) {}

    /// @brief Constructor from std::vector of floats, expects exactly 4 elements
    /// @param values std::vector containing four float values
    /// @throws std::invalid_argument if the vector does not contain exactly 4 elements
    /// @since v0.0.2
    inline Vector4(const ::std::vector<float>& values) {
        if (values.size() != 4) {
            throw ::std::invalid_argument("Vector4 constructor requires exactly 4 float values.");
        }
        m_storage.x = values[0];
        m_storage.y = values[1];
        m_storage.z = values[2];
        m_storage.w = values[3];
    }

    /// @brief Copy constructor
    /// @param other Vector to copy from
    /// @since v0.0.2
    inline Vector4(const Vector4& other) = default;

    /// @brief Assign from another vector
    /// @param other Vector to assign from
    /// @return Reference to this vector after assignment
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector4& operator=(const Vector4& other) = default;

    /// @brief Move constructor
    /// @param other Vector to move from
    /// @since v0.0.2
    inline Vector4(Vector4&& other) noexcept = default;

    /// @brief Move assignment operator
    /// @param other Vector to move from
    /// @return Reference to this vector after move assignment
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Vector4& operator=(Vector4&& other) noexcept = default;

    // MARK: Accessors

    /// @brief Get the raw data as a pointer to float array
    /// @return Pointer to the first element of the float array
    /// @threadsafety safe
    /// @since v0.0.2
    inline float* data() { return reinterpret_cast<float*>(&m_storage); }

    /// @brief Get the raw data as a pointer to float array
    /// @return Pointer to the first element of the float array
    /// @threadsafety safe
    /// @since v0.0.2
    inline const float* data() const { return reinterpret_cast<const float*>(&m_storage); }

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

	/// @brief Get the Z component of the vector
	/// @return Z component
	/// @threadsafety safe
	/// @since v0.0.2
	inline float z() const { return m_storage.z; }

	/// @brief Get the W component of the vector
	/// @return W component
	/// @threadsafety safe
	/// @since v0.0.2
	inline float w() const { return m_storage.w; }

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

	/// @brief Get the ZW components as a 2D vector
	/// @return Vector2 containing the Z and W components
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector2 zw() const { return Vector2(m_storage.z, m_storage.w); }

	/// @brief Get the XYZ components as a 3D vector
	/// @return Vector3 containing the X, Y, and Z components
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector3 xyz() const { return Vector3(m_storage.x, m_storage.y, m_storage.z); }

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

	/// @brief Set the W component of the vector
	/// @param w New W component value
	/// @threadsafety not-safe
	/// @since v0.0.2
    inline void setW(float w) { m_storage.w = w; }

    /// @brief Set a component of the vector by index (0=X, 1=Y, 2=Z, 3=W)
    /// @param index Index of the component to set
    /// @param value New value for the component
    /// @throws std::out_of_range if index is not in [0, 3]
    /// @threadsafety not-safe
    /// @since v0.0.2
	inline void set(size_t index, float value) {
		if (index > 3) {
			throw ::std::out_of_range("Index must be in the range [0, 3]");
		}
		data()[index] = value;
	}

	/// @brief Convert this vector to a bgfx vector by dropping W
	/// @return Equivalent bgfx Vec3 from XYZ components
	/// @threadsafety safe
	/// @since v0.0.2
	inline bx::Vec3 toBxVec3() const {
		return bx::Vec3(m_storage.x, m_storage.y, m_storage.z);
	}

	/// @brief Convert this vector to a Jolt Vec3 by dropping W
	/// @return Equivalent Jolt Vec3 from XYZ components
	/// @threadsafety safe
	/// @since v0.0.2
	inline JPH::Vec3 toJoltVec3() const {
		return JPH::Vec3(m_storage.x, m_storage.y, m_storage.z);
	}

	/// @brief Convert this vector to a Jolt RVec3 by dropping W
	/// @return Equivalent Jolt RVec3 from XYZ components
	/// @threadsafety safe
	/// @since v0.0.2
	inline JPH::RVec3 toJoltRVec3() const {
		return JPH::RVec3(m_storage.x, m_storage.y, m_storage.z);
	}

	/// @brief Convert this vector to a Jolt Vec4
	/// @return Equivalent Jolt Vec4
	/// @threadsafety safe
	/// @since v0.0.2
	inline JPH::Vec4 toJoltVec4() const {
		return JPH::Vec4(m_storage.x, m_storage.y, m_storage.z, m_storage.w);
    }

    /// @brief Check if the vector is a zero vector (all components are zero)
	/// @return true if all components are zero, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool isZero() const {
        return m_storage.x == 0.0f && m_storage.y == 0.0f &&
               m_storage.z == 0.0f && m_storage.w == 0.0f;
    }

    /// @brief Convert this vector to a std::vector of floats
    /// @return std::vector containing the X, Y, Z, and W components
    /// @threadsafety safe
	/// @since v0.0.2
	inline operator ::std::vector<float>() const {
		return ::std::vector<float>{m_storage.x, m_storage.y, m_storage.z, m_storage.w};
	}

	// MARK: Math operations

	/// @brief Add two vectors
	/// @param other Vector to add
	/// @return New vector representing the sum
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 operator+(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorAdd(v1, v2));
		return res;
	}

	/// @brief Subtract two vectors
	/// @param other Vector to subtract
	/// @return New vector representing the difference
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 operator-(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorSubtract(v1, v2));
		return res;
	}

	/// @brief Scale vector by a scalar value
	/// @param scalar Scalar multiplier
	/// @return New scaled vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 operator*(float scalar) const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorScale(v, scalar));
		return res;
    }

	/// @brief Divide vector by a scalar value
	/// @param scalar Scalar divisor
	/// @return New divided vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 operator/(float scalar) const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorScale(v, 1.0f / scalar));
		return res;
	}

	/// @brief Add vector in-place
	/// @param other Vector to add
	/// @return Reference to this vector after addition
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Vector4& operator+=(const Vector4& other) {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		DirectX::XMStoreFloat4(&m_storage, DirectX::XMVectorAdd(v1, v2));
		return *this;
	}

	/// @brief Subtract vector in-place
	/// @param other Vector to subtract
	/// @return Reference to this vector after subtraction
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Vector4& operator-=(const Vector4& other) {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		DirectX::XMStoreFloat4(&m_storage, DirectX::XMVectorSubtract(v1, v2));
		return *this;
	}

	/// @brief Scale vector in-place
	/// @param scalar Scalar multiplier
	/// @return Reference to this vector after scaling
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Vector4& operator*=(float scalar) {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMStoreFloat4(&m_storage, DirectX::XMVectorScale(v, scalar));
		return *this;
	}

	/// @brief Divide vector in-place
	/// @param scalar Scalar divisor
	/// @return Reference to this vector after division
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Vector4& operator/=(float scalar) {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMStoreFloat4(&m_storage, DirectX::XMVectorScale(v, 1.0f / scalar));
		return *this;
    }

    friend Vector4 operator*(const Vector4& vec, const Matrix4x4& mat);

	// MARK: Comparison operators

	/// @brief Check if two vectors are equal
	/// @param other Vector to compare
	/// @return true if vectors are equal, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool operator==(const Vector4& other) const {
		return m_storage.x == other.m_storage.x && m_storage.y == other.m_storage.y &&
			   m_storage.z == other.m_storage.z && m_storage.w == other.m_storage.w;
	}

	/// @brief Check if two vectors are not equal
	/// @param other Vector to compare
	/// @return true if vectors are not equal, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool operator!=(const Vector4& other) const {
		return !(*this == other);
	}

	/// @brief Check if all components are less than another vector
	/// @param other Vector to compare
	/// @return true if all components are less, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool operator<(const Vector4& other) const {
		return m_storage.x < other.m_storage.x && m_storage.y < other.m_storage.y &&
			   m_storage.z < other.m_storage.z && m_storage.w < other.m_storage.w;
	}

	/// @brief Check if all components are less than or equal to another vector
	/// @param other Vector to compare
	/// @return true if all components are less or equal, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool operator<=(const Vector4& other) const {
		return m_storage.x <= other.m_storage.x && m_storage.y <= other.m_storage.y &&
			   m_storage.z <= other.m_storage.z && m_storage.w <= other.m_storage.w;
	}

	/// @brief Check if all components are greater than another vector
	/// @param other Vector to compare
	/// @return true if all components are greater, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool operator>(const Vector4& other) const {
		return m_storage.x > other.m_storage.x && m_storage.y > other.m_storage.y &&
			   m_storage.z > other.m_storage.z && m_storage.w > other.m_storage.w;
	}

	/// @brief Check if all components are greater than or equal to another vector
	/// @param other Vector to compare
	/// @return true if all components are greater or equal, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool operator>=(const Vector4& other) const {
		return m_storage.x >= other.m_storage.x && m_storage.y >= other.m_storage.y &&
			   m_storage.z >= other.m_storage.z && m_storage.w >= other.m_storage.w;
	}

	/// @brief Negate the vector
	/// @return New negated vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 operator-() const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorNegate(v));
		return res;
	}

    // MARK: Normal vector operations

    /// @brief Normalize the vector in-place
    /// @note This function modifies the current vector to have a length of 1.
    /// To get a new normalized vector without modifying the current one, use
    /// the `normalized()` method.
	/// @return Reference to this vector after normalization
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void normalize() {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMStoreFloat4(&m_storage, DirectX::XMVector4Normalize(v));
	}

    /// @brief Normalize the vector and return a new normalized vector
    /// @note This function RETURNS a new normalized vector without modifying
    /// the current one. To normalize in-place, use the `normalize()` method.
    /// @return New normalized vector with length 1
    /// @threadsafety safe
    /// @since v0.0.2
	[[nodiscard("Use the normalize() method to normalize in-place")]] inline Vector4 normalized() const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVector4Normalize(v));
		return res;
	}

	/// @brief Calculate the length (magnitude) of the vector
	/// @return Length of the vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline float length() const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(v));
	}

	/// @brief Calculate the squared length of the vector (more efficient than length)
	/// @return Squared length of the vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline float lengthSquared() const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(v));
	}

	/// @brief Calculate the dot product with another vector
	/// @param other Vector to dot with
	/// @return Dot product result
	/// @threadsafety safe
	/// @since v0.0.2
	inline float dot(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		return DirectX::XMVectorGetX(DirectX::XMVector4Dot(v1, v2));
	}

	/// @brief Compute a 3D cross product using XYZ components, with W set to 0
	/// @param other Vector to cross with
	/// @return Cross product vector in XYZ and W = 0
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 cross(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		DirectX::XMVECTOR c = DirectX::XMVector3Cross(v1, v2);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, c);
		res.m_storage.w = 0.0f;
		return res;
	}

	/// @brief Calculate the distance between this vector and another vector
	/// @param other Target vector
	/// @return Distance between vectors
	/// @threadsafety safe
	/// @since v0.0.2
	inline float distance(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		return DirectX::XMVectorGetX(DirectX::XMVector4Length(DirectX::XMVectorSubtract(v1, v2)));
	}

	/// @brief Calculate the squared distance between this vector and another vector
	/// @param other Target vector
	/// @return Squared distance between vectors
	/// @threadsafety safe
	/// @since v0.0.2
	inline float distanceSquared(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(DirectX::XMVectorSubtract(v1, v2)));
	}

	// MARK: Other operations

	/// @brief Linear interpolation between this vector and another
	/// @param other Target vector to interpolate towards
	/// @param t Interpolation factor (0 = this vector, 1 = other vector)
	/// @return Interpolated vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 lerp(const Vector4& other, float t) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorLerp(v1, v2, t));
		return res;
	}

	/// @brief Clamp vector components to a range defined by min and max vectors
	/// @param min Minimum bounds vector
	/// @param max Maximum bounds vector
	/// @return Clamped vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 clamp(const Vector4& min, const Vector4& max) const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR vmin = DirectX::XMLoadFloat4(&min.m_storage);
		DirectX::XMVECTOR vmax = DirectX::XMLoadFloat4(&max.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorClamp(v, vmin, vmax));
		return res;
	}

	/// @brief Get absolute value of vector components
	/// @return Vector with absolute value components
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 abs() const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorAbs(v));
		return res;
	}

	/// @brief Reflect this vector off a surface with the given normal
	/// @param normal Surface normal vector
	/// @return Reflected vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 reflect(const Vector4& normal) const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR n = DirectX::XMLoadFloat4(&normal.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVector4Reflect(v, n));
		return res;
	}

	/// @brief Refract this vector through a surface with the given normal and eta ratio
	/// @param normal Surface normal vector
	/// @param eta Ratio of refractive indices
	/// @return Refracted vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 refract(const Vector4& normal, float eta) const {
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR n = DirectX::XMLoadFloat4(&normal.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVector4Refract(v, n, eta));
		return res;
	}

	/// @brief Get component-wise minimum with another vector
	/// @param other Vector to compare
	/// @return Vector with minimum components
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 min(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorMin(v1, v2));
		return res;
	}

	/// @brief Get component-wise maximum with another vector
	/// @param other Vector to compare
	/// @return Vector with maximum components
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 max(const Vector4& other) const {
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat4(&m_storage);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat4(&other.m_storage);
		Vector4 res;
		DirectX::XMStoreFloat4(&res.m_storage, DirectX::XMVectorMax(v1, v2));
		return res;
	}
};

using Vec4 = Vector4; // Alias Vec4 for Vector4
using SVec4 = Vector4; // Alias SVec4 for Vector4

} // namespace Syngine::Math
