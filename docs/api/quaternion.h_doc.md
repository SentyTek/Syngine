# Syngine API Documentation


## Quaternion.hpp header


[<- Back](../index.md)

[See source](./../../include/Syngine/Math/Quaternion.hpp)

---

## Goto: 


- [Constructor](#class-constructor)
- [Quaternion](#quaternionquaternion)
- [Quaternion](#quaternionquaternion)
- [Quaternion](#quaternionquaternion)
- [Quaternion](#quaternionquaternion)
- [Quaternion](#quaternionquaternion)
- [toBxQuat](#quaterniontobxquat)
- [toJoltQuat](#quaterniontojoltquat)
- [Operator std::vector<float> overload](#quaternionoperatorstd::vector<float>overload)
- [Quaternion](#quaternionquaternion)
- [Operator = overload](#quaternionoperator=overload)
- [Quaternion](#quaternionquaternion)
- [Operator = overload](#quaternionoperator=overload)
- [x](#quaternionx)
- [y](#quaterniony)
- [z](#quaternionz)
- [w](#quaternionw)
- [setX](#quaternionsetx)
- [setY](#quaternionsety)
- [setZ](#quaternionsetz)
- [setW](#quaternionsetw)
- [DirectX::XMLoadFloat4](#quaterniondirectx::xmloadfloat4)
- [DirectX::XMQuaternionNormalize](#quaterniondirectx::xmquaternionnormalize)
- [DirectX::XMLoadFloat4](#quaterniondirectx::xmloadfloat4)
- [DirectX::XMLoadFloat4](#quaterniondirectx::xmloadfloat4)
- [conjugate](#quaternionconjugate)
- [Constructor](#class-constructor)
- [normalize](#quaternionnormalize)
- [Constructor](#class-constructor)
- [invert](#quaternioninvert)
- [Constructor](#class-constructor)
- [dot](#quaterniondot)
- [toMatrix3x3](#quaterniontomatrix3x3)
- [toAxisAngle](#quaterniontoaxisangle)
- [toEulerAngles](#quaterniontoeulerangles)
- [slerp](#quaternionslerp)
- [lerp](#quaternionlerp)

---

## Class Constructor


#### **`Quaternion::::std::vector<float>&`**


 Constructor from std::vector of floats, expects exactly 4 elements

Signature:

```cpp
 inline Quaternion(const ::std::vector<float>& values);
```

**Parameters:**

- `values`: std::vector containing four float values

**This function has been available since:** v0.0.2

**Throws:** std::invalid_argument if the vector does not contain exactly 4 elements

---

## Class & Related Members


#### **`Quaternion::Quaternion`**


 Default constructor, initializes the quaternion to (0.0, 0.0, 0.0, 1.0)

Signature:

```cpp
 inline Quaternion() : m_storage(0.0f, 0.0f, 0.0f, 1.0f);
```

**This function has been available since:** v0.0.2

---

#### **`Quaternion::Quaternion`**


 Construct a quaternion from four float components

Signature:

```cpp
 inline Quaternion(float x, float y, float z, float w) : m_storage(x, y, z, w);
```

**Parameters:**

- `x`: X component of the quaternion
- `y`: Y component of the quaternion
- `z`: Z component of the quaternion
- `w`: W component of the quaternion

**This function has been available since:** v0.0.2

---

#### **`Quaternion::Quaternion`**


 Construct a quaternion from a rotation axis and angle

Signature:

```cpp
 inline Quaternion(const Vector3& axis, float angle);
```

**Parameters:**

- `axis`: Rotation axis (normalized internally)
- `angle`: Rotation angle in radians

**This function has been available since:** v0.0.2

---

#### **`Quaternion::Quaternion`**


 Construct a quaternion from Euler angles (pitch, yaw, roll)

Signature:

```cpp
 inline Quaternion(const Vector3& eulerAngles);
```

**Parameters:**

- `eulerAngles`: Euler angles in radians (pitch, yaw, roll)

**This function has been available since:** v0.0.2

---

#### **`Quaternion::Quaternion`**


 Construct a quaternion from a Jolt::Quat

Signature:

```cpp
 inline Quaternion(const JPH::Quat& quat) : m_storage(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW());
```

**Parameters:**

- `quat`: Jolt::Quat to convert from

**This function has been available since:** v0.0.2

---

#### **`Quaternion::toBxQuat`**


 Convert this quaternion to a bx::Quaternion

Signature:

```cpp
 inline bx::Quaternion toBxQuat() const;
```

**Returns:** bx::Quaternion representation of this quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::toJoltQuat`**


 Convert this quaternion to a Jolt::Quat

Signature:

```cpp
 inline JPH::Quat toJoltQuat() const;
```

**Returns:** Jolt::Quat representation of this quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Operator std::vector<float> overload`**


 Convert this quaternion to a std::vector of floats

Signature:

```cpp
 inline operator std::vector<float>() const;
```

**Returns:** std::vector containing the four components of the quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::Quaternion`**


 Copy constructor

Signature:

```cpp
 inline Quaternion(const Quaternion& other) = default;
```

**Parameters:**

- `other`: Quaternion to copy

**This function has been available since:** v0.0.2

---

#### **`Operator = overload`**


 Assignment operator

Signature:

```cpp
 inline Quaternion& operator=(const Quaternion& other) = default;
```

**Parameters:**

- `other`: Quaternion to assign from

**Returns:** Reference to this quaternion after assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::Quaternion`**


 Move constructor

Signature:

```cpp
 inline Quaternion(Quaternion&& other) noexcept = default;
```

**Parameters:**

- `other`: Quaternion to move from

**This function has been available since:** v0.0.2

---

#### **`Operator = overload`**


 Move assignment operator

Signature:

```cpp
 inline Quaternion& operator=(Quaternion&& other) noexcept = default;
```

**Parameters:**

- `other`: Quaternion to move from

**Returns:** Reference to this quaternion after move assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::x`**


 Get the X component of the quaternion

Signature:

```cpp
 inline float x() const;
```

**Returns:** X component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::y`**


 Get the Y component of the quaternion

Signature:

```cpp
 inline float y() const;
```

**Returns:** Y component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::z`**


 Get the Z component of the quaternion

Signature:

```cpp
 inline float z() const;
```

**Returns:** Z component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::w`**


 Get the W component of the quaternion

Signature:

```cpp
 inline float w() const;
```

**Returns:** W component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::setX`**


 Set the X component of the quaternion

Signature:

```cpp
 inline void setX(float x);
```

**Parameters:**

- `x`: New X component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::setY`**


 Set the Y component of the quaternion

Signature:

```cpp
 inline void setY(float y);
```

**Parameters:**

- `y`: New Y component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::setZ`**


 Set the Z component of the quaternion

Signature:

```cpp
 inline void setZ(float z);
```

**Parameters:**

- `z`: New Z component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::setW`**


 Set the W component of the quaternion

Signature:

```cpp
 inline void setW(float w);
```

**Parameters:**

- `w`: New W component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat4`**


 Compose this quaternion with another quaternion

Signature:

```cpp
 inline Quaternion operator*(const Quaternion& other) const;
```

**Parameters:**

- `other`: Quaternion to multiply with

**Returns:** Composed quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMQuaternionNormalize`**


 Rotate a 3D vector by this quaternion

Signature:

```cpp
 inline Vector3 operator*(const Vector3& vec) const;
```

**Parameters:**

- `vec`: Vector to rotate

**Returns:** Rotated vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat4`**


 Add two quaternions component-wise

Signature:

```cpp
 inline Quaternion operator+(const Quaternion& other) const;
```

**Parameters:**

- `other`: Quaternion to add

**Returns:** Sum quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`DirectX::XMLoadFloat4`**


 Subtract two quaternions component-wise

Signature:

```cpp
 inline Quaternion operator-(const Quaternion& other) const;
```

**Parameters:**

- `other`: Quaternion to subtract

**Returns:** Difference quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::conjugate`**


 Conjugate this quaternion in-place

**Note:** This function does NOT return a new quaternion, it modifies the current one. To get a new conjugated quaternion, use the `conjugated()` method.

Signature:

```cpp
 inline void conjugate();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

## Class Constructor


#### **`Quaternion::nodiscard`**


 Return a new conjugated quaternion without modifying the current one

**Note:** This function RETURNS a new quaternion and does not modify the current one. To modify the current quaternion in-place, use the `conjugate()` method.

Signature:

```cpp
 [[nodiscard("Use the conjugate() method to modify the current quaternion in-place")]] inline Quaternion conjugated() const;
```

**Returns:** New conjugated quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

## Class & Related Members


#### **`Quaternion::normalize`**


 Normalize this quaternion in-place

**Note:** This function does NOT return a new quaternion, it modifies the current one. To get a new normalized quaternion, use the `normalized()`

Signature:

```cpp
 inline void normalize();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

## Class Constructor


#### **`Quaternion::nodiscard`**


 Return a new normalized quaternion without modifying the current one

**Note:** This function RETURNS a new quaternion and does not modify the current one. To modify the current quaternion in-place, use the `normalize()` method.

Signature:

```cpp
 [[nodiscard("Use the normalize() method to modify the current quaternion in-place")]] inline Quaternion normalized() const;
```

**Returns:** New normalized quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

## Class & Related Members


#### **`Quaternion::invert`**


 Invert this quaternion in-place

**Note:** This function does NOT return a new quaternion, it modifies the current one. To get a new inverted quaternion, use the `inverse()` method.

Signature:

```cpp
 inline void invert();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---

## Class Constructor


#### **`Quaternion::nodiscard`**


 Return a new inverted quaternion without modifying the current one

**Note:** This function RETURNS a new quaternion and does not modify the current one. To modify the current quaternion in-place, use the `invert()` method.

Signature:

```cpp
 [[nodiscard("Use the invert() method to modify the current quaternion in-place")]] inline Quaternion inverse() const;
```

**Returns:** New inverted quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

## Class & Related Members


#### **`Quaternion::dot`**


 Calculate the dot product with another quaternion

Signature:

```cpp
 inline float dot(const Quaternion& other) const;
```

**Parameters:**

- `other`: Quaternion to dot with

**Returns:** Dot product result

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::toMatrix3x3`**


 Convert this quaternion to a 3x3 rotation matrix

Signature:

```cpp
 inline Matrix3x3 toMatrix3x3() const;
```

**Returns:** Equivalent rotation matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::toAxisAngle`**


 Convert this quaternion to axis-angle form

Signature:

```cpp
 inline Vector4 toAxisAngle() const;
```

**Returns:** Vector4 where XYZ is the normalized axis and W is angle in radians

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::toEulerAngles`**


 Convert this quaternion to Euler angles (pitch, yaw, roll)

Signature:

```cpp
 inline Vector3 toEulerAngles() const;
```

**Returns:** Euler angles in radians as (pitch X, yaw Y, roll Z)

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::slerp`**


 Spherically interpolate between this quaternion and another

Signature:

```cpp
 inline Quaternion slerp(const Quaternion& other, float t) const;
```

**Parameters:**

- `other`: Target quaternion
- `t`: Interpolation factor in [0, 1]

**Returns:** Interpolated quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

#### **`Quaternion::lerp`**


 Linearly interpolate between this quaternion and another, then normalize

Signature:

```cpp
 inline Quaternion lerp(const Quaternion& other, float t) const;
```

**Parameters:**

- `other`: Target quaternion
- `t`: Interpolation factor in [0, 1]

**Returns:** Interpolated unit quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---

