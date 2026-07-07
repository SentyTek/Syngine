// ╒══════════════════════ Matrix3x3.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-04                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

#include "Syngine/Math/Vector3.hpp"

#include <bx/math.h>

namespace Syngine::Math {

/// @brief 3x3 matrix class for mathematical operations in 3D space
class Matrix3x3 {
    DirectX::XMFLOAT3X3 m_storage;

    inline bool IsValidPosition(int row, int col) const {
        return row >= 0 && row < 3 && col >= 0 && col < 3;
    }

    friend class Quaternion; // for quaternion-matrix conversion
  public:
    // MARK: Constructors

    /// @brief Default constructor, initializes the matrix to the identity matrix
    /// @since v0.0.2
    inline Matrix3x3() : m_storage(1.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f) {}

    /// @brief Construct a matrix from nine float components
    /// @param m00 Element at row 0, column 0
    /// @param m01 Element at row 0, column 1
    /// @param m02 Element at row 0, column 2
    /// @param m10 Element at row 1, column 0
    /// @param m11 Element at row 1, column 1
    /// @param m12 Element at row 1, column 2
    /// @param m20 Element at row 2, column 0
    /// @param m21 Element at row 2, column 1
    /// @param m22 Element at row 2, column 2
    /// @since v0.0.2
    inline Matrix3x3(float m00,
                     float m01,
                     float m02,
                     float m10,
                     float m11,
                     float m12,
                     float m20,
                     float m21,
                     float m22)
        : m_storage(m00, m01, m02, m10, m11, m12, m20, m21, m22) {}

    /// @brief Construct a matrix from a float array of 9 elements in row-major order
    /// @param elements Pointer to an array of 9 float elements
    /// @since v0.0.2
    inline Matrix3x3(const float* elements) {
        m_storage.m[0][0] = elements[0];
        m_storage.m[0][1] = elements[1];
        m_storage.m[0][2] = elements[2];
        m_storage.m[1][0] = elements[3];
        m_storage.m[1][1] = elements[4];
        m_storage.m[1][2] = elements[5];
        m_storage.m[2][0] = elements[6];
        m_storage.m[2][1] = elements[7];
        m_storage.m[2][2] = elements[8];
    }

    /// @brief Copy constructor
    /// @param other Matrix to copy
    /// @since v0.0.2
    inline Matrix3x3(const Matrix3x3& other) : m_storage(other.m_storage) {}

    /// @brief Assignment operator
    /// @param other Matrix to assign from
    /// @return Reference to this matrix after assignment
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Matrix3x3& operator=(const Matrix3x3& other) {
        m_storage = other.m_storage;
        return *this;
    }

    // MARK: Accessors

    /// @brief Get a matrix element by row and column
    /// @param row Row index in range [0, 2]
    /// @param col Column index in range [0, 2]
    /// @return Element value at the given row and column
    /// @threadsafety safe
    /// @since v0.0.2
    inline float m(int row, int col) const {
        if (!IsValidPosition(row, col)) {
            throw std::out_of_range("Row and column indices must be between 0 and 2.");
        }
        return m_storage.m[row][col];
    }

    /// @brief Set a matrix element by row and column
    /// @param row Row index in range [0, 2]
    /// @param col Column index in range [0, 2]
    /// @param value New element value
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setM(int row, int col, float value) {
        if (!IsValidPosition(row, col)) {
            throw std::out_of_range("Row and column indices must be between 0 and 2.");
        }
        m_storage.m[row][col] = value;
    }

    /// @brief Get a row as a Vector3
    /// @param row Row index in range [0, 2]
    /// @return Row vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 row(int row) const {
        if (row < 0 || row > 2) {
            throw std::out_of_range("Row index must be between 0 and 2.");
        }
        return Vector3(m_storage.m[row][0], m_storage.m[row][1], m_storage.m[row][2]);
    }

    /// @brief Get a column as a Vector3
    /// @param col Column index in range [0, 2]
    /// @return Column vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 column(int col) const {
        if (col < 0 || col > 2) {
            throw std::out_of_range("Column index must be between 0 and 2.");
        }
        return Vector3(m_storage.m[0][col], m_storage.m[1][col], m_storage.m[2][col]);
    }

    /// @brief Set a full row from a Vector3
    /// @param row Row index in range [0, 2]
    /// @param vec Source row vector
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setRow(int row, const Vector3& vec) {
        if (row < 0 || row > 2) {
            throw std::out_of_range("Row index must be between 0 and 2.");
        }
        m_storage.m[row][0] = vec.x();
        m_storage.m[row][1] = vec.y();
        m_storage.m[row][2] = vec.z();
    }

    /// @brief Set a full column from a Vector3
    /// @param col Column index in range [0, 2]
    /// @param vec Source column vector
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void setColumn(int col, const Vector3& vec) {
        if (col < 0 || col > 2) {
            throw std::out_of_range("Column index must be between 0 and 2.");
        }
        m_storage.m[0][col] = vec.x();
        m_storage.m[1][col] = vec.y();
        m_storage.m[2][col] = vec.z();
    }

    /// @brief Get a row vector by index using bracket syntax
    /// @param row Row index in range [0, 2]
    /// @return Row vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator[](int row) const {
        return this->row(row);
    }

    // MARK: Arithmetic operations

    /// @brief Multiply two matrices
    /// @param other Matrix to multiply with
    /// @return Product matrix
    /// @threadsafety safe
    /// @since v0.0.2
    inline Matrix3x3 operator*(const Matrix3x3& other) const {
        DirectX::XMMATRIX m1 = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMMATRIX m2 = DirectX::XMLoadFloat3x3(&other.m_storage);
        DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m1, m2);
        Matrix3x3 res;
        DirectX::XMStoreFloat3x3(&res.m_storage, result);
        return res;
    }

    /// @brief Multiply and assign matrix in-place
    /// @param other Matrix to multiply with
    /// @return Reference to this matrix after multiplication
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Matrix3x3& operator*=(const Matrix3x3& other) {
        *this = *this * other;
        return *this;
    }

    /// @brief Add two matrices
    /// @param other Matrix to add
    /// @return Sum matrix
    /// @threadsafety safe
    /// @since v0.0.2
    inline Matrix3x3 operator+(const Matrix3x3& other) const {
        DirectX::XMMATRIX m1 = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMMATRIX m2 = DirectX::XMLoadFloat3x3(&other.m_storage);
        DirectX::XMMATRIX result = m1 + m2;
        Matrix3x3 res;
        DirectX::XMStoreFloat3x3(&res.m_storage, result);
        return res;
    }

    /// @brief Add and assign matrix in-place
    /// @param other Matrix to add
    /// @return Reference to this matrix after addition
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Matrix3x3& operator+=(const Matrix3x3& other) {
        *this = *this + other;
        return *this;
    }

    /// @brief Subtract two matrices
    /// @param other Matrix to subtract
    /// @return Difference matrix
    /// @threadsafety safe
    /// @since v0.0.2
    inline Matrix3x3 operator-(const Matrix3x3& other) const {
        DirectX::XMMATRIX m1 = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMMATRIX m2 = DirectX::XMLoadFloat3x3(&other.m_storage);
        DirectX::XMMATRIX result = m1 - m2;
        Matrix3x3 res;
        DirectX::XMStoreFloat3x3(&res.m_storage, result);
        return res;
    }

    /// @brief Subtract and assign matrix in-place
    /// @param other Matrix to subtract
    /// @return Reference to this matrix after subtraction
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline Matrix3x3& operator-=(const Matrix3x3& other) {
        *this = *this - other;
        return *this;
    }

    // Vector multiplication

    /// @brief Transform a vector by this matrix
    /// @param vec Vector to transform
    /// @return Transformed vector
    /// @threadsafety safe
    /// @since v0.0.2
    inline Vector3 operator*(const Vector3& vec) {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&vec.m_storage);
        DirectX::XMVECTOR result = DirectX::XMVector3Transform(v, m);
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, result);
        return res;
    }

    // MARK: Normal matrix operations

    /// @brief Transpose this matrix in-place
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void transpose() {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(m);
        DirectX::XMStoreFloat3x3(&m_storage, result);
    }

    /// @brief Invert this matrix in-place
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void invert() {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMVECTOR det;
        DirectX::XMMATRIX result = DirectX::XMMatrixInverse(&det, m);
        DirectX::XMStoreFloat3x3(&m_storage, result);
    }

    /// @brief Compute the determinant of this matrix
    /// @return Determinant value
    /// @threadsafety safe
    /// @since v0.0.2
    inline float determinant() const {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMVECTOR det;
        det = DirectX::XMMatrixDeterminant(m);
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
               m_storage.m[1][0] == 0.0f && m_storage.m[1][1] == 1.0f && m_storage.m[1][2] == 0.0f &&
               m_storage.m[2][0] == 0.0f && m_storage.m[2][1] == 0.0f && m_storage.m[2][2] == 1.0f;
    }

    /// @brief Apply translation offsets to matrix translation elements
    /// @param translation Translation vector
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void translate(const Vector3& translation) {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMFLOAT3X3 t_storage(1.0f,
                                      0.0f,
                                      0.0f,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      translation.x(),
                                      translation.y(),
                                      translation.z());
        DirectX::XMMATRIX t = DirectX::XMLoadFloat3x3(&t_storage);
        DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, t);
        DirectX::XMStoreFloat3x3(&m_storage, result);
    }

    /// @brief Apply component-wise scale to diagonal elements
    /// @param scale Scale vector
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void scale(const Vector3& scale) {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMFLOAT3X3 s_storage(scale.x(),
                                      0.0f,
                                      0.0f,
                                      0.0f,
                                      scale.y(),
                                      0.0f,
                                      0.0f,
                                      0.0f,
                                      scale.z());
        DirectX::XMMATRIX s = DirectX::XMLoadFloat3x3(&s_storage);
        DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, s);
        DirectX::XMStoreFloat3x3(&m_storage, result);
    }

    /// @brief Apply axis-angle rotation to this matrix in-place
    /// @param angle Rotation angle in radians
    /// @param axis Rotation axis
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void rotate(float angle, const Vector3& axis) {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat3x3(&m_storage);
        DirectX::XMVECTOR axisVec = DirectX::XMVector3Normalize(
            DirectX::XMVectorSet(axis.x(), axis.y(), axis.z(), 0.0f));
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(axisVec, angle);
        DirectX::XMMATRIX result = DirectX::XMMatrixMultiply(m, rotation);
        DirectX::XMStoreFloat3x3(&m_storage, result);
    }
};

using Mat3 = Matrix3x3; // Alias mat3 for Matrix3x3
} // namespace Syngine::Math
