# Syngine API Documentation

## Quaternion.hpp header

[<- Back](../index.md)

[See source](./../../include/Syngine/Math/Quaternion.hpp)

Quaternion class for representing rotations in 3D space

<div style="background:#08082e; padding:15px; border-radius:8px; margin-bottom:20px; font-family:sans-serif;">
    <label for="mdSearch" style="font-weight:bold; display:block; margin-bottom:5px;">Search Functions:</label>
    <input type="text" id="mdSearch" placeholder="Type function name..." onkeyup="filterMarkdownDocs()" style="width:100%; padding:8px; border:1px solid #ccc; border-radius:4px; font-size:16px; background-color: rgb(60, 60, 60); color:antiquewhite">
</div>

<script>
function filterMarkdownDocs() {
    var input = document.getElementById('mdSearch').value.toLowerCase();
    // Targets common markdown containers or the whole document body
    var elements = document.querySelectorAll('h1, h2, h3, h4, p, li, pre, hr');

    elements.forEach(function(el) {
        // Skip the search box itself
        if (el.closest('#mdSearch') || el.id === 'mdSearch') return;

        var text = el.innerText.toLowerCase();
        if (text.includes(input)) {
            el.style.display = ""; // Show matching element
            //el.style.backgroundColor = input ? "#fff9c4" : ""; // Highlight if searching
        } else {
            el.style.display = input ? "none" : ""; // Hide if it doesn't match
        }
    });
}
</script>

---
## Goto: 


## Additional Functions: 

### Constructors: 

- [Quaternion()](#quaternion-quaternion)
- [Quaternion(float x, float y, float z, float w)](#quaternion-quaternionfloat-x-float-y-float-z-float-w)
- [Quaternion(const Vector3& axis, float angle)](#quaternion-quaternionconst-vector3-axis-float-angle)
- [Quaternion(const Vector3& eulerAngles)](#quaternion-quaternionconst-vector3-eulerangles)
- [Quaternion(const ::std::vector<float>& values)](#quaternion-quaternionconst-std-vectorfloat-values)
- [Quaternion(const JPH::Quat& quat)](#quaternion-quaternionconst-jph-quat-quat)
- [Quaternion(const Quaternion& other)](#quaternion-quaternionconst-quaternion-other)
- [Quaternion(Quaternion&& other)](#quaternion-quaternionquaternion-other)

### Operator Overloads: 

- [Conversion to std::vector operator overload](#conversion-to-std-vector-operator-overload)
- [Assignment operator overload](#assignment-operator-overload)
- [Assignment operator overload](#assignment-operator-overload-2)
- [Multiplication operator overload](#multiplication-operator-overload)
- [Multiplication operator overload](#multiplication-operator-overload-2)
- [Addition operator overload](#addition-operator-overload)
- [Subtraction operator overload](#subtraction-operator-overload)

### Functions: 

- [toBxQuat()](#quaternion-tobxquat)
- [toJoltQuat()](#quaternion-tojoltquat)
- [x()](#quaternion-x)
- [y()](#quaternion-y)
- [z()](#quaternion-z)
- [w()](#quaternion-w)
- [setX()](#quaternion-setx)
- [setY()](#quaternion-sety)
- [setZ()](#quaternion-setz)
- [setW()](#quaternion-setw)
- [conjugate()](#quaternion-conjugate)
- [nodiscard()](#quaternion-nodiscard)
- [normalize()](#quaternion-normalize)
- [nodiscard()](#quaternion-nodiscard-2)
- [invert()](#quaternion-invert)
- [nodiscard()](#quaternion-nodiscard-3)
- [dot()](#quaternion-dot)
- [toMatrix3x3()](#quaternion-tomatrix3x3)
- [toAxisAngle()](#quaternion-toaxisangle)
- [toEulerAngles()](#quaternion-toeulerangles)
- [slerp()](#quaternion-slerp)
- [lerp()](#quaternion-lerp)

---
<a id="quaternion-quaternion"></a>

## Class Constructor

#### **`Quaternion()`**

 Default constructor, initializes the quaternion to (0.0, 0.0, 0.0, 1.0)

Signature:
```cpp
 inline Quaternion() : m_storage(0.0f, 0.0f, 0.0f, 1.0f);
```
**This function has been available since:** v0.0.2

---
<a id="quaternion-quaternionfloat-x-float-y-float-z-float-w"></a>

## Class Constructor

#### **`Quaternion(float x, float y, float z, float w)`**

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
<a id="quaternion-quaternionconst-vector3-axis-float-angle"></a>

## Class Constructor

#### **`Quaternion(const Vector3& axis, float angle)`**

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
<a id="quaternion-quaternionconst-vector3-eulerangles"></a>

## Class Constructor

#### **`Quaternion(const Vector3& eulerAngles)`**

 Construct a quaternion from Euler angles (pitch, yaw, roll)

Signature:
```cpp
 inline Quaternion(const Vector3& eulerAngles);
```
**Parameters:**
- `eulerAngles`: Euler angles in radians (pitch, yaw, roll)

**This function has been available since:** v0.0.2

---
<a id="quaternion-quaternionconst-std-vectorfloat-values"></a>

## Class Constructor

#### **`Quaternion(const ::std::vector<float>& values)`**

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
<a id="quaternion-quaternionconst-jph-quat-quat"></a>

## Class Constructor

#### **`Quaternion(const JPH::Quat& quat)`**

 Construct a quaternion from a Jolt::Quat

Signature:
```cpp
 inline Quaternion(const JPH::Quat& quat) : m_storage(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW());
```
**Parameters:**
- `quat`: Jolt::Quat to convert from

**This function has been available since:** v0.0.2

---
<a id="quaternion-tobxquat"></a>

#### **`Quaternion::toBxQuat()`**

 Convert this quaternion to a bx::Quaternion

Signature:
```cpp
 inline bx::Quaternion toBxQuat() const;
```
**Returns:** bx::Quaternion representation of this quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-tojoltquat"></a>

#### **`Quaternion::toJoltQuat()`**

 Convert this quaternion to a Jolt::Quat

Signature:
```cpp
 inline JPH::Quat toJoltQuat() const;
```
**Returns:** Jolt::Quat representation of this quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="conversion-to-std-vector-operator-overload"></a>

#### **`Conversion to std::vector operator overload`**

 Convert this quaternion to a std::vector of floats

Signature:
```cpp
 inline operator std::vector<float>() const;
```
**Returns:** std::vector containing the four components of the quaternion

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-quaternionconst-quaternion-other"></a>

## Class Constructor

#### **`Quaternion(const Quaternion& other)`**

 Copy constructor

Signature:
```cpp
 inline Quaternion(const Quaternion& other) = default;
```
**Parameters:**
- `other`: Quaternion to copy

**This function has been available since:** v0.0.2

---
<a id="assignment-operator-overload"></a>

#### **`Assignment operator overload`**

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
<a id="quaternion-quaternionquaternion-other"></a>

## Class Constructor

#### **`Quaternion(Quaternion&& other)`**

 Move constructor

Signature:
```cpp
 inline Quaternion(Quaternion&& other) noexcept = default;
```
**Parameters:**
- `other`: Quaternion to move from

**This function has been available since:** v0.0.2

---
<a id="assignment-operator-overload-2"></a>

#### **`Assignment operator overload`**

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
<a id="quaternion-x"></a>

#### **`Quaternion::x()`**

 Get the X component of the quaternion

Signature:
```cpp
 inline float x() const;
```
**Returns:** X component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-y"></a>

#### **`Quaternion::y()`**

 Get the Y component of the quaternion

Signature:
```cpp
 inline float y() const;
```
**Returns:** Y component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-z"></a>

#### **`Quaternion::z()`**

 Get the Z component of the quaternion

Signature:
```cpp
 inline float z() const;
```
**Returns:** Z component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-w"></a>

#### **`Quaternion::w()`**

 Get the W component of the quaternion

Signature:
```cpp
 inline float w() const;
```
**Returns:** W component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-setx"></a>

#### **`Quaternion::setX()`**

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
<a id="quaternion-sety"></a>

#### **`Quaternion::setY()`**

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
<a id="quaternion-setz"></a>

#### **`Quaternion::setZ()`**

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
<a id="quaternion-setw"></a>

#### **`Quaternion::setW()`**

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
<a id="multiplication-operator-overload"></a>

#### **`Multiplication operator overload`**

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
<a id="multiplication-operator-overload-2"></a>

#### **`Multiplication operator overload`**

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
<a id="addition-operator-overload"></a>

#### **`Addition operator overload`**

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
<a id="subtraction-operator-overload"></a>

#### **`Subtraction operator overload`**

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
<a id="quaternion-conjugate"></a>

#### **`Quaternion::conjugate()`**

 Conjugate this quaternion in-place

**Note:** This function does NOT return a new quaternion, it modifies the current one. To get a new conjugated quaternion, use the `conjugated()` method.

Signature:
```cpp
 inline void conjugate();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-nodiscard"></a>

#### **`Quaternion::nodiscard()`**

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
<a id="quaternion-normalize"></a>

#### **`Quaternion::normalize()`**

 Normalize this quaternion in-place

**Note:** This function does NOT return a new quaternion, it modifies the current one. To get a new normalized quaternion, use the `normalized()`

Signature:
```cpp
 inline void normalize();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-nodiscard-2"></a>

#### **`Quaternion::nodiscard()`**

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
<a id="quaternion-invert"></a>

#### **`Quaternion::invert()`**

 Invert this quaternion in-place

**Note:** This function does NOT return a new quaternion, it modifies the current one. To get a new inverted quaternion, use the `inverse()` method.

Signature:
```cpp
 inline void invert();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-nodiscard-3"></a>

#### **`Quaternion::nodiscard()`**

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
<a id="quaternion-dot"></a>

#### **`Quaternion::dot()`**

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
<a id="quaternion-tomatrix3x3"></a>

#### **`Quaternion::toMatrix3x3()`**

 Convert this quaternion to a 3x3 rotation matrix

Signature:
```cpp
 inline Matrix3x3 toMatrix3x3() const;
```
**Returns:** Equivalent rotation matrix

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-toaxisangle"></a>

#### **`Quaternion::toAxisAngle()`**

 Convert this quaternion to axis-angle form

Signature:
```cpp
 inline Vector4 toAxisAngle() const;
```
**Returns:** Vector4 where XYZ is the normalized axis and W is angle in radians

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-toeulerangles"></a>

#### **`Quaternion::toEulerAngles()`**

 Convert this quaternion to Euler angles (pitch, yaw, roll)

Signature:
```cpp
 inline Vector3 toEulerAngles() const;
```
**Returns:** Euler angles in radians as (pitch X, yaw Y, roll Z)

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="quaternion-slerp"></a>

#### **`Quaternion::slerp()`**

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
<a id="quaternion-lerp"></a>

#### **`Quaternion::lerp()`**

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
