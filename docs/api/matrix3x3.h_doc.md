# Syngine API Documentation


## Matrix3x3.hpp header


[<- Back](../index.md)

[See source](./../../include/Syngine/Math/Matrix3x3.hpp)

---

## Goto: 


- [Constructor](#class-constructor)
- [Matrix3x3](#matrix3x3matrix3x3)
- [Matrix3x3](#matrix3x3matrix3x3)
- [Matrix3x3](#matrix3x3matrix3x3)
- [Matrix3x3](#matrix3x3matrix3x3)
- [Operator = overload](#matrix3x3operator=overload)
- [Matrix3x3](#matrix3x3matrix3x3)
- [Operator = overload](#matrix3x3operator=overload)
- [m](#matrix3x3m)
- [setM](#matrix3x3setm)
- [row](#matrix3x3row)
- [column](#matrix3x3column)
- [setRow](#matrix3x3setrow)
- [setColumn](#matrix3x3setcolumn)
- [row](#matrix3x3row)
- [data](#matrix3x3data)
- [data](#matrix3x3data)
- [DirectX::XMLoadFloat3x3](#matrix3x3directx::xmloadfloat3x3)
- [Operator *= overload](#matrix3x3operator*=overload)
- [DirectX::XMLoadFloat3x3](#matrix3x3directx::xmloadfloat3x3)
- [Operator += overload](#matrix3x3operator+=overload)
- [DirectX::XMLoadFloat3x3](#matrix3x3directx::xmloadfloat3x3)
- [Operator -= overload](#matrix3x3operator-=overload)
- [Operator * overload](#matrix3x3operator*overload)
- [transpose](#matrix3x3transpose)
- [invert](#matrix3x3invert)
- [Constructor](#class-constructor)
- [determinant](#matrix3x3determinant)
- [isInvertible](#matrix3x3isinvertible)
- [isIdentity](#matrix3x3isidentity)
- [translate](#matrix3x3translate)
- [scale](#matrix3x3scale)
- [rotate](#matrix3x3rotate)

---

## Class Constructor


#### **`Matrix3x3::nodiscard`**


 Get a new transposed matrix without modifying this one

**Note:** This function RETURNS a new matrix and does not modify the current one. To modify the current matrix in-place, use the `transpose()` method.

Signature:

```cpp
 [[nodiscard("Use the transpose() method to transpose in-place")]] inline Matrix3x3 transposed() const;
```

**Returns:** New transposed matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

## Class & Related Members


#### **`Matrix3x3::Matrix3x3`**


 Default constructor, initializes the matrix to the identity matrix

Signature:

```cpp
 inline Matrix3x3() : m_storage(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
```

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::Matrix3x3`**


 Construct a matrix from nine float components

Signature:

```cpp
 inline Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) : m_storage(m00, m01, m02, m10, m11, m12, m20, m21, m22);
```

**Parameters:**

- `m00`: Element at row 0, column 0
- `m01`: Element at row 0, column 1
- `m02`: Element at row 0, column 2
- `m10`: Element at row 1, column 0
- `m11`: Element at row 1, column 1
- `m12`: Element at row 1, column 2
- `m20`: Element at row 2, column 0
- `m21`: Element at row 2, column 1
- `m22`: Element at row 2, column 2

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::Matrix3x3`**


 Construct a matrix from a float array of 9 elements in row-major order

Signature:

```cpp
 inline Matrix3x3(const float* elements);
```

**Parameters:**

- `elements`: Pointer to an array of 9 float elements

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::Matrix3x3`**


 Copy constructor

Signature:

```cpp
 inline Matrix3x3(const Matrix3x3& other) = default;
```

**Parameters:**

- `other`: Matrix to copy

**This function has been available since:** v0.0.2

---

#### **`Operator = overload`**


 Assignment operator

Signature:

```cpp
 inline Matrix3x3& operator=(const Matrix3x3& other) = default;
```

**Parameters:**

- `other`: Matrix to assign from

**Returns:** Reference to this matrix after assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::Matrix3x3`**


 Move constructor

Signature:

```cpp
 inline Matrix3x3(Matrix3x3&& other) noexcept = default;
```

**Parameters:**

- `other`: Matrix to move from

**This function has been available since:** v0.0.2

---

#### **`Operator = overload`**


 Move assignment operator

Signature:

```cpp
 inline Matrix3x3& operator=(Matrix3x3&& other) noexcept = default;
```

**Parameters:**

- `other`: Matrix to move from

**Returns:** Reference to this matrix after move assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::m`**


 Get a matrix element by row and column

Signature:

```cpp
 inline float m(int row, int col) const;
```

**Parameters:**

- `row`: Row index in range [0, 2]
- `col`: Column index in range [0, 2]

**Returns:** Element value at the given row and column

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::setM`**


 Set a matrix element by row and column

Signature:

```cpp
 inline void setM(int row, int col, float value);
```

**Parameters:**

- `row`: Row index in range [0, 2]
- `col`: Column index in range [0, 2]
- `value`: New element value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::row`**


 Get a row as a Vector3

Signature:

```cpp
 inline Vector3 row(int row) const;
```

**Parameters:**

- `row`: Row index in range [0, 2]

**Returns:** Row vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::column`**


 Get a column as a Vector3

Signature:

```cpp
 inline Vector3 column(int col) const;
```

**Parameters:**

- `col`: Column index in range [0, 2]

**Returns:** Column vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::setRow`**


 Set a full row from a Vector3

Signature:

```cpp
 inline void setRow(int row, const Vector3& vec);
```

**Parameters:**

- `row`: Row index in range [0, 2]
- `vec`: Source row vector

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::setColumn`**


 Set a full column from a Vector3

Signature:

```cpp
 inline void setColumn(int col, const Vector3& vec);
```

**Parameters:**

- `col`: Column index in range [0, 2]
- `vec`: Source column vector

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`row`**


 Get a row vector by index using bracket syntax

Signature:

```cpp
 inline Vector3 operator[](int row) const;
```

**Parameters:**

- `row`: Row index in range [0, 2]

**Returns:** Row vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::data`**


 Get the data as a pointer to the underlying float array (row-major order)

Signature:

```cpp
 inline float* data();
```

**Returns:** Pointer to the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::data`**


 Get the data as a pointer to the underlying float array (row-major order)

Signature:

```cpp
 inline const float* data() const;
```

**Returns:** Pointer to the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat3x3`**


 Multiply two matrices

Signature:

```cpp
 inline Matrix3x3 operator*(const Matrix3x3& other) const;
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
 inline Matrix3x3& operator*=(const Matrix3x3& other);
```

**Parameters:**

- `other`: Matrix to multiply with

**Returns:** Reference to this matrix after multiplication

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat3x3`**


 Add two matrices

Signature:

```cpp
 inline Matrix3x3 operator+(const Matrix3x3& other) const;
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
 inline Matrix3x3& operator+=(const Matrix3x3& other);
```

**Parameters:**

- `other`: Matrix to add

**Returns:** Reference to this matrix after addition

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat3x3`**


 Subtract two matrices

Signature:

```cpp
 inline Matrix3x3 operator-(const Matrix3x3& other) const;
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
 inline Matrix3x3& operator-=(const Matrix3x3& other);
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
 friend Vector3 operator*(const Vector3& vec, const Matrix3x3& mat);
```

**Parameters:**

- `vec`: Vector to transform

**Returns:** Transformed vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::transpose`**


 Transpose this matrix in-place

**Note:** This does NOT return a new matrix, it modifies the current one. To get a new transposed matrix, use the `transposed()` method.

Signature:

```cpp
 inline void transpose();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::invert`**


 Invert this matrix in-place

Signature:

```cpp
 inline void invert();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

## Class Constructor


#### **`Matrix3x3::nodiscard`**


 Get a new inverted matrix without modifying this one

Signature:

```cpp
 [[nodiscard("Use the invert() method to invert in-place")]] inline Matrix3x3 inverse() const;
```

**Returns:** New inverted matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

## Class & Related Members


#### **`Matrix3x3::determinant`**


 Compute the determinant of this matrix

Signature:

```cpp
 inline float determinant() const;
```

**Returns:** Determinant value

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::isInvertible`**


 Check whether this matrix is invertible

Signature:

```cpp
 inline bool isInvertible() const;
```

**Returns:** true if determinant is non-zero, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::isIdentity`**


 Check whether this matrix is the identity matrix

Signature:

```cpp
 inline bool isIdentity() const;
```

**Returns:** true if this matrix is identity, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Matrix3x3::translate`**


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

#### **`Matrix3x3::scale`**


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

#### **`Matrix3x3::rotate`**


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

