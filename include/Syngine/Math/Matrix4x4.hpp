// ╒══════════════════════ Matrix4x4.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-04                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

#include <stdexcept>

#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Math/Vector4.hpp"
#include "Syngine/Math/Quaternion.hpp"

#include <bx/math.h>

namespace Syngine::Math {

/// @brief 4x4 matrix class for mathematical operations in 3D space
class Matrix4x4 {
	DirectX::XMFLOAT4X4 m_storage;

	inline bool IsValidPosition(int row, int col) const {
		return row >= 0 && row < 4 && col >= 0 && col < 4;
	}

  public:
	// MARK: Constructors

	/// @brief Default constructor, initializes the matrix to the identity matrix
	/// @since v0.0.2
	inline Matrix4x4()
		: m_storage(1.0f,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					1.0f,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					1.0f,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					1.0f) {}

	/// @brief Construct a matrix from sixteen float components
	/// @param m00 Element at row 0, column 0
	/// @param m01 Element at row 0, column 1
	/// @param m02 Element at row 0, column 2
	/// @param m03 Element at row 0, column 3
	/// @param m10 Element at row 1, column 0
	/// @param m11 Element at row 1, column 1
	/// @param m12 Element at row 1, column 2
	/// @param m13 Element at row 1, column 3
	/// @param m20 Element at row 2, column 0
	/// @param m21 Element at row 2, column 1
	/// @param m22 Element at row 2, column 2
	/// @param m23 Element at row 2, column 3
	/// @param m30 Element at row 3, column 0
	/// @param m31 Element at row 3, column 1
	/// @param m32 Element at row 3, column 2
	/// @param m33 Element at row 3, column 3
	/// @since v0.0.2
	inline Matrix4x4(float m00,
					 float m01,
					 float m02,
					 float m03,
					 float m10,
					 float m11,
					 float m12,
					 float m13,
					 float m20,
					 float m21,
					 float m22,
					 float m23,
					 float m30,
					 float m31,
					 float m32,
					 float m33)
		: m_storage(m00,
					m01,
					m02,
					m03,
					m10,
					m11,
					m12,
					m13,
					m20,
					m21,
					m22,
					m23,
					m30,
					m31,
					m32,
					m33) {}

	/// @brief Construct a matrix from a float array of 16 elements in row-major order
	/// @param elements Pointer to an array of 16 float elements
	/// @since v0.0.2
	inline Matrix4x4(const float* elements) {
		m_storage.m[0][0] = elements[0];
		m_storage.m[0][1] = elements[1];
		m_storage.m[0][2] = elements[2];
		m_storage.m[0][3] = elements[3];
		m_storage.m[1][0] = elements[4];
		m_storage.m[1][1] = elements[5];
		m_storage.m[1][2] = elements[6];
		m_storage.m[1][3] = elements[7];
		m_storage.m[2][0] = elements[8];
		m_storage.m[2][1] = elements[9];
		m_storage.m[2][2] = elements[10];
		m_storage.m[2][3] = elements[11];
		m_storage.m[3][0] = elements[12];
		m_storage.m[3][1] = elements[13];
		m_storage.m[3][2] = elements[14];
		m_storage.m[3][3] = elements[15];
    }

    /// @brief Construct a matrix from standard Rotation, Scale, and Translation
    /// (RST) components
    /// @param rot Rotation quaternion
    /// @param scale Scale vector
    /// @param translation Translation vector
    /// @since v0.0.2
    inline Matrix4x4(const Quaternion& rot,
                     const Vector3&    scale,
                     const Vector3&    translation) {
		DirectX::XMVECTOR quat = DirectX::XMLoadFloat4(&rot.m_storage);
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(quat);
        DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(scale.x(), scale.y(), scale.z());
        DirectX::XMMATRIX translationMtx = DirectX::XMMatrixTranslation(translation.x(), translation.y(), translation.z());

		DirectX::XMMATRIX result = scaling * rotation * translationMtx;
		DirectX::XMStoreFloat4x4(&m_storage, result);
	}

	/// @brief Copy constructor
	/// @param other Matrix to copy
	/// @since v0.0.2
	inline Matrix4x4(const Matrix4x4& other) : m_storage(other.m_storage) {}

	/// @brief Assignment operator
	/// @param other Matrix to assign from
	/// @return Reference to this matrix after assignment
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Matrix4x4& operator=(const Matrix4x4& other) {
		m_storage = other.m_storage;
		return *this;
	}

	// MARK: Accessors

	/// @brief Get a matrix element by row and column
	/// @param row Row index in range [0, 3]
	/// @param col Column index in range [0, 3]
	/// @return Element value at the given row and column
	/// @threadsafety safe
	/// @since v0.0.2
	inline float m(int row, int col) const {
		if (!IsValidPosition(row, col)) {
			throw std::out_of_range("Row and column indices must be between 0 and 3.");
		}
		return m_storage.m[row][col];
	}

	/// @brief Set a matrix element by row and column
	/// @param row Row index in range [0, 3]
	/// @param col Column index in range [0, 3]
	/// @param value New element value
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void setM(int row, int col, float value) {
		if (!IsValidPosition(row, col)) {
			throw std::out_of_range("Row and column indices must be between 0 and 3.");
		}
		m_storage.m[row][col] = value;
	}

	/// @brief Get a row as a Vector4
	/// @param row Row index in range [0, 3]
	/// @return Row vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 row(int row) const {
		if (row < 0 || row > 3) {
			throw std::out_of_range("Row index must be between 0 and 3.");
		}
		return Vector4(m_storage.m[row][0], m_storage.m[row][1], m_storage.m[row][2], m_storage.m[row][3]);
	}

	/// @brief Get a column as a Vector4
	/// @param col Column index in range [0, 3]
	/// @return Column vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 column(int col) const {
		if (col < 0 || col > 3) {
			throw std::out_of_range("Column index must be between 0 and 3.");
		}
		return Vector4(m_storage.m[0][col], m_storage.m[1][col], m_storage.m[2][col], m_storage.m[3][col]);
	}

	/// @brief Set a full row from a Vector4
	/// @param row Row index in range [0, 3]
	/// @param vec Source row vector
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void setRow(int row, const Vector4& vec) {
		if (row < 0 || row > 3) {
			throw std::out_of_range("Row index must be between 0 and 3.");
		}
		m_storage.m[row][0] = vec.x();
		m_storage.m[row][1] = vec.y();
		m_storage.m[row][2] = vec.z();
		m_storage.m[row][3] = vec.w();
	}

	/// @brief Set a full column from a Vector4
	/// @param col Column index in range [0, 3]
	/// @param vec Source column vector
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void setColumn(int col, const Vector4& vec) {
		if (col < 0 || col > 3) {
			throw std::out_of_range("Column index must be between 0 and 3.");
		}
		m_storage.m[0][col] = vec.x();
		m_storage.m[1][col] = vec.y();
		m_storage.m[2][col] = vec.z();
		m_storage.m[3][col] = vec.w();
	}

	/// @brief Get a row vector by index using bracket syntax
	/// @param row Row index in range [0, 3]
	/// @return Row vector
	/// @threadsafety safe
	/// @since v0.0.2
    inline Vector4 operator[](int row) const { return this->row(row); }

    /// @brief Get the data as a pointer to the underlying float array
    /// (row-major order)
    /// @return Pointer to the float array
    /// @threadsafety safe
	/// @since v0.0.2
    inline float* data() { return reinterpret_cast<float*>(&m_storage); }

	/// @brief Get the data as a pointer to the underlying float array (row-major order)
	/// @return Pointer to the float array
	/// @threadsafety safe
	/// @since v0.0.2
    inline const float* data() const { return reinterpret_cast<const float*>(&m_storage); }

    /// @brief Get the translation component of the matrix as a Vector3
    /// @return Translation vector (last column of the matrix)
    /// @threadsafety safe
	/// @since v0.0.2
    inline Vector3 getTranslation() const {
		return Vector3(m_storage.m[3][0], m_storage.m[3][1], m_storage.m[3][2]);
	}

	// MARK: Arithmetic operations

	/// @brief Multiply two matrices
	/// @param other Matrix to multiply with
	/// @return Product matrix
	/// @threadsafety safe
	/// @since v0.0.2
	inline Matrix4x4 operator*(const Matrix4x4& other) const {
		DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&other.m_storage);
		DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m1, m2);
		Matrix4x4 res;
		DirectX::XMStoreFloat4x4(&res.m_storage, result);
		return res;
	}

	/// @brief Multiply and assign matrix in-place
	/// @param other Matrix to multiply with
	/// @return Reference to this matrix after multiplication
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Matrix4x4& operator*=(const Matrix4x4& other) {
		*this = *this * other;
		return *this;
	}

	/// @brief Add two matrices
	/// @param other Matrix to add
	/// @return Sum matrix
	/// @threadsafety safe
	/// @since v0.0.2
	inline Matrix4x4 operator+(const Matrix4x4& other) const {
		DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&other.m_storage);
		DirectX::XMMATRIX result = m1 + m2;
		Matrix4x4 res;
		DirectX::XMStoreFloat4x4(&res.m_storage, result);
		return res;
	}

	/// @brief Add and assign matrix in-place
	/// @param other Matrix to add
	/// @return Reference to this matrix after addition
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Matrix4x4& operator+=(const Matrix4x4& other) {
		*this = *this + other;
		return *this;
	}

	/// @brief Subtract two matrices
	/// @param other Matrix to subtract
	/// @return Difference matrix
	/// @threadsafety safe
	/// @since v0.0.2
	inline Matrix4x4 operator-(const Matrix4x4& other) const {
		DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&other.m_storage);
		DirectX::XMMATRIX result = m1 - m2;
		Matrix4x4 res;
		DirectX::XMStoreFloat4x4(&res.m_storage, result);
		return res;
	}

	/// @brief Subtract and assign matrix in-place
	/// @param other Matrix to subtract
	/// @return Reference to this matrix after subtraction
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline Matrix4x4& operator-=(const Matrix4x4& other) {
		*this = *this - other;
		return *this;
	}

	// Vector multiplication

	/// @brief Transform a 4D vector by this matrix
	/// @param vec Vector to transform
	/// @return Transformed vector
	/// @threadsafety safe
	/// @since v0.0.2
	inline Vector4 operator*(const Vector4& vec) const {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&vec.m_storage);
		DirectX::XMVECTOR result = DirectX::XMVector4Transform(v, m);
		Vector4 res;
        DirectX::XMStoreFloat4(&res.m_storage, result);
        return res;
    }

	// MARK: Normal matrix operations

	/// @brief Transpose this matrix in-place
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void transpose() {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(m);
		DirectX::XMStoreFloat4x4(&m_storage, result);
	}

    /// @brief Inverse this matrix in-place
    /// @note This function does NOT return a new matrix, it modifies the
    /// current matrix. To get a new inverted matrix, use the `inverse()`
    /// method.
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void invert() {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMVECTOR det;
		DirectX::XMMATRIX result = DirectX::XMMatrixInverse(&det, m);
		DirectX::XMStoreFloat4x4(&m_storage, result);
    }

    /// @brief Inverse this matrix
    /// @return Inverted matrix
    /// @note This function does NOT modify the current matrix, it returns a new
    /// inverted matrix. To invert the current matrix in-place, use the
    /// `invert()` method.
    /// @threadsafety safe
    /// @since v0.0.2
    [[nodiscard("Use the invert() method to invert in-place")]] inline Matrix4x4 inverse() const {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMVECTOR det;
		DirectX::XMMATRIX result = DirectX::XMMatrixInverse(&det, m);
		Matrix4x4 res;
		DirectX::XMStoreFloat4x4(&res.m_storage, result);
		return res;
	}

	/// @brief Compute the determinant of this matrix
	/// @return Determinant value
	/// @threadsafety safe
	/// @since v0.0.2
	inline float determinant() const {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(m);
		return DirectX::XMVectorGetX(det);
	}

	/// @brief Check whether this matrix is invertible
	/// @return true if determinant is non-zero, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool isInvertible() const { return determinant() != 0.0f; }

	/// @brief Check whether this matrix is the identity matrix
	/// @return true if this matrix is identity, false otherwise
	/// @threadsafety safe
	/// @since v0.0.2
	inline bool isIdentity() const {
		return m_storage.m[0][0] == 1.0f && m_storage.m[0][1] == 0.0f && m_storage.m[0][2] == 0.0f &&
			   m_storage.m[0][3] == 0.0f && m_storage.m[1][0] == 0.0f && m_storage.m[1][1] == 1.0f &&
			   m_storage.m[1][2] == 0.0f && m_storage.m[1][3] == 0.0f && m_storage.m[2][0] == 0.0f &&
			   m_storage.m[2][1] == 0.0f && m_storage.m[2][2] == 1.0f && m_storage.m[2][3] == 0.0f &&
			   m_storage.m[3][0] == 0.0f && m_storage.m[3][1] == 0.0f && m_storage.m[3][2] == 0.0f &&
			   m_storage.m[3][3] == 1.0f;
	}

	/// @brief Apply translation offsets to matrix translation elements
	/// @param translation Translation vector
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void translate(const Vector3& translation) {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(
			translation.x(), translation.y(), translation.z());
		DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, t);
		DirectX::XMStoreFloat4x4(&m_storage, result);
	}

	/// @brief Apply component-wise scale to diagonal elements
	/// @param scale Scale vector
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void scale(const Vector3& scale) {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX s = DirectX::XMMatrixScaling(
			scale.x(), scale.y(), scale.z());
		DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, s);
		DirectX::XMStoreFloat4x4(&m_storage, result);
	}

	/// @brief Apply axis-angle rotation to this matrix in-place
	/// @param angle Rotation angle in radians
	/// @param axis Rotation axis
	/// @threadsafety not-safe
	/// @since v0.0.2
	inline void rotate(float angle, const Vector3& axis) {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMVECTOR axisVec = DirectX::XMVector3Normalize(
			DirectX::XMVectorSet(axis.x(), axis.y(), axis.z(), 0.0f));
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(axisVec, angle);
		DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, rotation);
		DirectX::XMStoreFloat4x4(&m_storage, result);
    }

    /// @brief Rotate the matrix along the X axis in-place
    /// @param angle Rotation angle in radians
    /// @threadsafety not-safe
	/// @since v0.0.2
	inline void rotateX(float angle) {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationX(angle);
		DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, rotation);
		DirectX::XMStoreFloat4x4(&m_storage, result);
	}

    /// @brief Rotate the matrix along the Y axis in-place
    /// @param angle Rotation angle in radians
    /// @threadsafety not-safe
	/// @since v0.0.2
	inline void rotateY(float angle) {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(angle);
		DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, rotation);
		DirectX::XMStoreFloat4x4(&m_storage, result);
	}

    /// @brief Rotate the matrix along the Z axis in-place
    /// @param angle Rotation angle in radians
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void rotateZ(float angle) {
		DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&m_storage);
		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(angle);
		DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, rotation);
		DirectX::XMStoreFloat4x4(&m_storage, result);
	}
};

using Mat4 = Matrix4x4; // Alias Mat4 for Matrix4x4

} // namespace Syngine::Math
