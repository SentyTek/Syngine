# Syngine API Documentation


## Matrix4x4.hpp header


[<- Back](../index.md)

[See source](./../../include/Syngine/Math/Matrix4x4.hpp)

---

## Goto: 


- [Constructor](#class-constructor)
- [Matrix4x4](#matrix4x4matrix4x4)
- [Matrix4x4](#matrix4x4matrix4x4)
- [Matrix4x4](#matrix4x4matrix4x4)
- [Matrix4x4](#matrix4x4matrix4x4)
- [Matrix4x4](#matrix4x4matrix4x4)
- [Operator = overload](#matrix4x4operator=overload)
- [Matrix4x4](#matrix4x4matrix4x4)
- [Operator = overload](#matrix4x4operator=overload)
- [m](#matrix4x4m)
- [setM](#matrix4x4setm)
- [row](#matrix4x4row)
- [column](#matrix4x4column)
- [setRow](#matrix4x4setrow)
- [setColumn](#matrix4x4setcolumn)
- [row](#matrix4x4row)
- [data](#matrix4x4data)
- [data](#matrix4x4data)
- [getTranslation](#matrix4x4gettranslation)
- [DirectX::XMLoadFloat4x4](#matrix4x4directx::xmloadfloat4x4)
- [Operator *= overload](#matrix4x4operator*=overload)
- [DirectX::XMLoadFloat4x4](#matrix4x4directx::xmloadfloat4x4)
- [Operator += overload](#matrix4x4operator+=overload)
- [DirectX::XMLoadFloat4x4](#matrix4x4directx::xmloadfloat4x4)
- [Operator -= overload](#matrix4x4operator-=overload)
- [Operator * overload](#matrix4x4operator*overload)
- [transpose](#matrix4x4transpose)
- [invert](#matrix4x4invert)
- [Constructor](#class-constructor)
- [determinant](#matrix4x4determinant)
- [isInvertible](#matrix4x4isinvertible)
- [isIdentity](#matrix4x4isidentity)
- [translate](#matrix4x4translate)
- [scale](#matrix4x4scale)
- [rotate](#matrix4x4rotate)
- [rotateX](#matrix4x4rotatex)
- [rotateY](#matrix4x4rotatey)
- [rotateZ](#matrix4x4rotatez)

---

## Class Constructor


#### **`Matrix4x4::nodiscard`**


 Transpose this matrix in-place

**Note:** This function RETURNS a new matrix and does not modify the current one. To modify the current matrix in-place, use the `transpose()` method.

Signature:

```cpp
 [[nodiscard("Use the transpose() method to transpose in-place")]] inline Matrix4x4 transposed() const;
```

**Returns:** New transposed matrix

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

## Class & Related Members


#### **`Matrix4x4::Matrix4x4`**


 Default constructor, initializes the matrix to the identity matrix

Signature:

```cpp
 inline Matrix4x4() : m_storage(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
```

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::Matrix4x4`**


 Construct a matrix from sixteen float components

Signature:

```cpp
 inline Matrix4x4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) : m_storage(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
```

**Parameters:**

- `m00`: Element at row 0, column 0
- `m01`: Element at row 0, column 1
- `m02`: Element at row 0, column 2
- `m03`: Element at row 0, column 3
- `m10`: Element at row 1, column 0
- `m11`: Element at row 1, column 1
- `m12`: Element at row 1, column 2
- `m13`: Element at row 1, column 3
- `m20`: Element at row 2, column 0
- `m21`: Element at row 2, column 1
- `m22`: Element at row 2, column 2
- `m23`: Element at row 2, column 3
- `m30`: Element at row 3, column 0
- `m31`: Element at row 3, column 1
- `m32`: Element at row 3, column 2
- `m33`: Element at row 3, column 3

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::Matrix4x4`**


 Construct a matrix from a float array of 16 elements in row-major order

Signature:

```cpp
 inline Matrix4x4(const float* elements);
```

**Parameters:**

- `elements`: Pointer to an array of 16 float elements

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::Matrix4x4`**


 Construct a matrix from standard Rotation, Scale, and Translation (RST) components

Signature:

```cpp
 inline Matrix4x4(const Vector3& scale, const Quaternion& rot, const Vector3& translation);
```

**Parameters:**

- `rot`: Rotation quaternion
- `scale`: Scale vector
- `translation`: Translation vector

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::Matrix4x4`**


 Copy constructor

Signature:

```cpp
 inline Matrix4x4(const Matrix4x4& other) = default;
```

**Parameters:**

- `other`: Matrix to copy

**This function has been available since:** v0.0.2

---

#### **`Operator = overload`**


 Assignment operator

Signature:

```cpp
 inline Matrix4x4& operator=(const Matrix4x4& other) = default;
```

**Parameters:**

- `other`: Matrix to assign from

**Returns:** Reference to this matrix after assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::Matrix4x4`**


 Move constructor

Signature:

```cpp
 inline Matrix4x4(Matrix4x4&& other) noexcept = default;
```

**Parameters:**

- `other`: Matrix to move from

**This function has been available since:** v0.0.2

---

#### **`Operator = overload`**


 Move assignment operator

Signature:

```cpp
 inline Matrix4x4& operator=(Matrix4x4&& other) noexcept = default;
```

**Parameters:**

- `other`: Matrix to move from

**Returns:** Reference to this matrix after move assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::m`**


 Get a matrix element by row and column

Signature:

```cpp
 inline float m(int row, int col) const;
```

**Parameters:**

- `row`: Row index in range [0, 3]
- `col`: Column index in range [0, 3]

**Returns:** Element value at the given row and column

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::setM`**


 Set a matrix element by row and column

Signature:

```cpp
 inline void setM(int row, int col, float value);
```

**Parameters:**

- `row`: Row index in range [0, 3]
- `col`: Column index in range [0, 3]
- `value`: New element value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::row`**


 Get a row as a Vector4

Signature:

```cpp
 inline Vector4 row(int row) const;
```

**Parameters:**

- `row`: Row index in range [0, 3]

**Returns:** Row vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::column`**


 Get a column as a Vector4

Signature:

```cpp
 inline Vector4 column(int col) const;
```

**Parameters:**

- `col`: Column index in range [0, 3]

**Returns:** Column vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::setRow`**


 Set a full row from a Vector4

Signature:

```cpp
 inline void setRow(int row, const Vector4& vec);
```

**Parameters:**

- `row`: Row index in range [0, 3]
- `vec`: Source row vector

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::setColumn`**


 Set a full column from a Vector4

Signature:

```cpp
 inline void setColumn(int col, const Vector4& vec);
```

**Parameters:**

- `col`: Column index in range [0, 3]
- `vec`: Source column vector

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`row`**


 Get a row vector by index using bracket syntax

Signature:

```cpp
 inline Vector4 operator[](int row) const;
```

**Parameters:**

- `row`: Row index in range [0, 3]

**Returns:** Row vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::data`**


 Get the data as a pointer to the underlying float array (row-major order)

Signature:

```cpp
 inline float* data();
```

**Returns:** Pointer to the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::data`**


 Get the data as a pointer to the underlying float array (row-major order)

Signature:

```cpp
 inline const float* data() const;
```

**Returns:** Pointer to the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::getTranslation`**


 Get the translation component of the matrix as a Vector3

Signature:

```cpp
 inline Vector3 getTranslation() const;
```

**Returns:** Translation vector (last column of the matrix)

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat4x4`**


 Multiply two matrices

Signature:

```cpp
 inline Matrix4x4 operator*(const Matrix4x4& other) const;
```

**Parameters:**

- `other`: Matrix to multiply with

**Returns:** Product matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Operator *= overload`**


 Multiply and assign matrix in-place

Signature:

```cpp
 inline Matrix4x4& operator*=(const Matrix4x4& other);
```

**Parameters:**

- `other`: Matrix to multiply with

**Returns:** Reference to this matrix after multiplication

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat4x4`**


 Add two matrices

Signature:

```cpp
 inline Matrix4x4 operator+(const Matrix4x4& other) const;
```

**Parameters:**

- `other`: Matrix to add

**Returns:** Sum matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Operator += overload`**


 Add and assign matrix in-place

Signature:

```cpp
 inline Matrix4x4& operator+=(const Matrix4x4& other);
```

**Parameters:**

- `other`: Matrix to add

**Returns:** Reference to this matrix after addition

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat4x4`**


 Subtract two matrices

Signature:

```cpp
 inline Matrix4x4 operator-(const Matrix4x4& other) const;
```

**Parameters:**

- `other`: Matrix to subtract

**Returns:** Difference matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Operator -= overload`**


 Subtract and assign matrix in-place

Signature:

```cpp
 inline Matrix4x4& operator-=(const Matrix4x4& other);
```

**Parameters:**

- `other`: Matrix to subtract

**Returns:** Reference to this matrix after subtraction

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Operator * overload`**


 Transform a vector by this matrix

Signature:

```cpp
 friend Vector4 operator*(const Vector4& vec, const Matrix4x4& mat);
```

**Parameters:**

- `vec`: Vector to transform

**Returns:** Transformed vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::transpose`**


 Transpose this matrix in-place

**Note:** This function modifies the current matrix to be its transpose. To get a new transposed matrix without modifying the current one, use the `transposed()` method.

Signature:

```cpp
 inline void transpose();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::invert`**


 Inverse this matrix in-place

**Note:** This function does NOT return a new matrix, it modifies the current matrix. To get a new inverted matrix, use the `inverse()` method.

Signature:

```cpp
 inline void invert();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

## Class Constructor


#### **`Matrix4x4::nodiscard`**


 Inverse this matrix

**Note:** This function does NOT modify the current matrix, it returns a new inverted matrix. To invert the current matrix in-place, use the `invert()` method.

Signature:

```cpp
 [[nodiscard("Use the invert() method to invert in-place")]] inline Matrix4x4 inverse() const;
```

**Returns:** Inverted matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

## Class & Related Members


#### **`Matrix4x4::determinant`**


 Compute the determinant of this matrix

Signature:

```cpp
 inline float determinant() const;
```

**Returns:** Determinant value

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::isInvertible`**


 Check whether this matrix is invertible

Signature:

```cpp
 inline bool isInvertible() const;
```

**Returns:** true if determinant is non-zero, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::isIdentity`**


 Check whether this matrix is the identity matrix

Signature:

```cpp
 inline bool isIdentity() const;
```

**Returns:** true if this matrix is identity, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::translate`**


 Apply translation offsets to matrix translation elements

Signature:

```cpp
 inline void translate(const Vector3& translation);
```

**Parameters:**

- `translation`: Translation vector

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::scale`**


 Apply component-wise scale to diagonal elements

Signature:

```cpp
 inline void scale(const Vector3& scale);
```

**Parameters:**

- `scale`: Scale vector

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::rotate`**


 Apply axis-angle rotation to this matrix in-place

Signature:

```cpp
 inline void rotate(float angle, const Vector3& axis);
```

**Parameters:**

- `angle`: Rotation angle in radians
- `axis`: Rotation axis

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::rotateX`**


 Rotate the matrix along the X axis in-place

Signature:

```cpp
 inline void rotateX(float angle);
```

**Parameters:**

- `angle`: Rotation angle in radians

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::rotateY`**


 Rotate the matrix along the Y axis in-place

Signature:

```cpp
 inline void rotateY(float angle);
```

**Parameters:**

- `angle`: Rotation angle in radians

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix4x4::rotateZ`**


 Rotate the matrix along the Z axis in-place

Signature:

```cpp
 inline void rotateZ(float angle);
```

**Parameters:**

- `angle`: Rotation angle in radians

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

