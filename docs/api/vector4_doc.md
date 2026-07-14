# Syngine API Documentation

## Vector4.hpp header

[<- Back](../index.md)

[See source](./../../include/Syngine/Math/Vector4.hpp)

4D vector class for mathematical operations in 4D space

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

- [Vector4()](#vector4-vector4)
- [Vector4(float value)](#vector4-vector4float-value)
- [Vector4(float x, float y, float z, float w)](#vector4-vector4float-x-float-y-float-z-float-w)
- [Vector4(const Vector3& v, float w = 1.0f)](#vector4-vector4const-vector3-v-float-w-10f)
- [Vector4(const Vector2& v, float z, float w)](#vector4-vector4const-vector2-v-float-z-float-w)
- [Vector4(const bx::Vec3& v, float w = 1.0f)](#vector4-vector4const-bx-vec3-v-float-w-10f)
- [Vector4(const JPH::Vec3& v, float w = 1.0f)](#vector4-vector4const-jph-vec3-v-float-w-10f)
- [Vector4(const JPH::Vec4& v)](#vector4-vector4const-jph-vec4-v)
- [Vector4(const ::std::vector<float>& values)](#vector4-vector4const-std-vectorfloat-values)
- [Vector4(const Vector4& other)](#vector4-vector4const-vector4-other)
- [Vector4(Vector4&& other)](#vector4-vector4vector4-other)

### Operator Overloads: 

- [Assignment operator overload](#assignment-operator-overload)
- [Assignment operator overload](#assignment-operator-overload-2)
- [Conversion to std::vector operator overload](#conversion-to-std-vector-operator-overload)
- [Addition operator overload](#addition-operator-overload)
- [Subtraction operator overload](#subtraction-operator-overload)
- [Multiplication operator overload](#multiplication-operator-overload)
- [Division operator overload](#division-operator-overload)
- [Addition assignment operator overload](#addition-assignment-operator-overload)
- [Subtraction assignment operator overload](#subtraction-assignment-operator-overload)
- [Multiplication assignment operator overload](#multiplication-assignment-operator-overload)
- [Division assignment operator overload](#division-assignment-operator-overload)
- [Equality operator overload](#equality-operator-overload)
- [Inequality operator overload](#inequality-operator-overload)
- [Less than operator overload](#less-than-operator-overload)
- [Less than or equal to operator overload](#less-than-or-equal-to-operator-overload)
- [Greater than operator overload](#greater-than-operator-overload)
- [Greater than or equal to operator overload](#greater-than-or-equal-to-operator-overload)
- [Subtraction operator overload](#subtraction-operator-overload-2)

### Functions: 

- [data()](#vector4-data)
- [data()](#vector4-data-2)
- [x()](#vector4-x)
- [y()](#vector4-y)
- [z()](#vector4-z)
- [w()](#vector4-w)
- [xy()](#vector4-xy)
- [xz()](#vector4-xz)
- [yz()](#vector4-yz)
- [zw()](#vector4-zw)
- [xyz()](#vector4-xyz)
- [setX()](#vector4-setx)
- [setY()](#vector4-sety)
- [setZ()](#vector4-setz)
- [setW()](#vector4-setw)
- [set()](#vector4-set)
- [toBxVec3()](#vector4-tobxvec3)
- [toJoltVec3()](#vector4-tojoltvec3)
- [toJoltRVec3()](#vector4-tojoltrvec3)
- [toJoltVec4()](#vector4-tojoltvec4)
- [isZero()](#vector4-iszero)
- [normalize()](#vector4-normalize)
- [nodiscard()](#vector4-nodiscard)
- [length()](#vector4-length)
- [lengthSquared()](#vector4-lengthsquared)
- [dot()](#vector4-dot)
- [cross()](#vector4-cross)
- [distance()](#vector4-distance)
- [distanceSquared()](#vector4-distancesquared)
- [lerp()](#vector4-lerp)
- [clamp()](#vector4-clamp)
- [abs()](#vector4-abs)
- [reflect()](#vector4-reflect)
- [refract()](#vector4-refract)
- [min()](#vector4-min)
- [max()](#vector4-max)

---
<a id="vector4-vector4"></a>

## Class Constructor

#### **`Vector4()`**

 Default constructor, initializes the vector to (0.0, 0.0, 0.0, 0.0)

Signature:
```cpp
 inline Vector4() : m_storage(0.0f, 0.0f, 0.0f, 0.0f);
```
**This function has been available since:** v0.0.2

---
<a id="vector4-vector4float-value"></a>

## Class Constructor

#### **`Vector4(float value)`**

 Construct a vector from a single float value, initializes all components to the same value

Signature:
```cpp
 inline Vector4(float value) : m_storage(value, value, value, value);
```
**Parameters:**
- `value`: Value to set all components

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4float-x-float-y-float-z-float-w"></a>

## Class Constructor

#### **`Vector4(float x, float y, float z, float w)`**

 Construct a vector from four float components

Signature:
```cpp
 inline Vector4(float x, float y, float z, float w) : m_storage(x, y, z, w);
```
**Parameters:**
- `x`: X component of the vector
- `y`: Y component of the vector
- `z`: Z component of the vector
- `w`: W component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4const-vector3-v-float-w-10f"></a>

## Class Constructor

#### **`Vector4(const Vector3& v, float w = 1.0f)`**

 Construct a vector from a 3D vector and a W component

Signature:
```cpp
 inline Vector4(const Vector3& v, float w = 1.0f) : m_storage(v.x(), v.y(), v.z(), w);
```
**Parameters:**
- `v`: Source 3D vector
- `w`: W component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4const-vector2-v-float-z-float-w"></a>

## Class Constructor

#### **`Vector4(const Vector2& v, float z, float w)`**

 Construct a vector from a 2D vector and Z/W components

Signature:
```cpp
 inline Vector4(const Vector2& v, float z, float w) : m_storage(v.x(), v.y(), z, w);
```
**Parameters:**
- `v`: Source 2D vector
- `z`: Z component of the vector
- `w`: W component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4const-bx-vec3-v-float-w-10f"></a>

## Class Constructor

#### **`Vector4(const bx::Vec3& v, float w = 1.0f)`**

 Construct a vector from a bgfx vector and W component

Signature:
```cpp
 inline Vector4(const bx::Vec3& v, float w = 1.0f) : m_storage(v.x, v.y, v.z, w);
```
**Parameters:**
- `v`: Source bgfx vector
- `w`: W component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4const-jph-vec3-v-float-w-10f"></a>

## Class Constructor

#### **`Vector4(const JPH::Vec3& v, float w = 1.0f)`**

 Construct a vector from a Jolt Vec3 and W component

Signature:
```cpp
 inline Vector4(const JPH::Vec3& v, float w = 1.0f) : m_storage(v.GetX(), v.GetY(), v.GetZ(), w);
```
**Parameters:**
- `v`: Source Jolt Vec3
- `w`: W component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4const-jph-vec4-v"></a>

## Class Constructor

#### **`Vector4(const JPH::Vec4& v)`**

 Construct a vector from a Jolt Vec4

Signature:
```cpp
 inline Vector4(const JPH::Vec4& v) : m_storage(v.GetX(), v.GetY(), v.GetZ(), v.GetW());
```
**Parameters:**
- `v`: Source Jolt Vec4

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4const-std-vectorfloat-values"></a>

## Class Constructor

#### **`Vector4(const ::std::vector<float>& values)`**

 Constructor from std::vector of floats, expects exactly 4 elements

Signature:
```cpp
 inline Vector4(const ::std::vector<float>& values);
```
**Parameters:**
- `values`: std::vector containing four float values

**This function has been available since:** v0.0.2

**Throws:** std::invalid_argument if the vector does not contain exactly 4 elements

---
<a id="vector4-vector4const-vector4-other"></a>

## Class Constructor

#### **`Vector4(const Vector4& other)`**

 Copy constructor

Signature:
```cpp
 inline Vector4(const Vector4& other) = default;
```
**Parameters:**
- `other`: Vector to copy from

**This function has been available since:** v0.0.2

---
<a id="assignment-operator-overload"></a>

#### **`Assignment operator overload`**

 Assign from another vector

Signature:
```cpp
 inline Vector4& operator=(const Vector4& other) = default;
```
**Parameters:**
- `other`: Vector to assign from

**Returns:** Reference to this vector after assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector4-vector4vector4-other"></a>

## Class Constructor

#### **`Vector4(Vector4&& other)`**

 Move constructor

Signature:
```cpp
 inline Vector4(Vector4&& other) noexcept = default;
```
**Parameters:**
- `other`: Vector to move from

**This function has been available since:** v0.0.2

---
<a id="assignment-operator-overload-2"></a>

#### **`Assignment operator overload`**

 Move assignment operator

Signature:
```cpp
 inline Vector4& operator=(Vector4&& other) noexcept = default;
```
**Parameters:**
- `other`: Vector to move from

**Returns:** Reference to this vector after move assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector4-data"></a>

#### **`Vector4::data()`**

 Get the raw data as a pointer to float array

Signature:
```cpp
 inline float* data();
```
**Returns:** Pointer to the first element of the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-data-2"></a>

#### **`Vector4::data()`**

 Get the raw data as a pointer to float array

Signature:
```cpp
 inline const float* data() const;
```
**Returns:** Pointer to the first element of the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-x"></a>

#### **`Vector4::x()`**

 Get the X component of the vector

Signature:
```cpp
 inline float x() const;
```
**Returns:** X component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-y"></a>

#### **`Vector4::y()`**

 Get the Y component of the vector

Signature:
```cpp
 inline float y() const;
```
**Returns:** Y component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-z"></a>

#### **`Vector4::z()`**

 Get the Z component of the vector

Signature:
```cpp
 inline float z() const;
```
**Returns:** Z component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-w"></a>

#### **`Vector4::w()`**

 Get the W component of the vector

Signature:
```cpp
 inline float w() const;
```
**Returns:** W component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-xy"></a>

#### **`Vector4::xy()`**

 Get the XY components as a 2D vector

Signature:
```cpp
 inline Vector2 xy() const;
```
**Returns:** Vector2 containing the X and Y components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-xz"></a>

#### **`Vector4::xz()`**

 Get the XZ components as a 2D vector

Signature:
```cpp
 inline Vector2 xz() const;
```
**Returns:** Vector2 containing the X and Z components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-yz"></a>

#### **`Vector4::yz()`**

 Get the YZ components as a 2D vector

Signature:
```cpp
 inline Vector2 yz() const;
```
**Returns:** Vector2 containing the Y and Z components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-zw"></a>

#### **`Vector4::zw()`**

 Get the ZW components as a 2D vector

Signature:
```cpp
 inline Vector2 zw() const;
```
**Returns:** Vector2 containing the Z and W components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-xyz"></a>

#### **`Vector4::xyz()`**

 Get the XYZ components as a 3D vector

Signature:
```cpp
 inline Vector3 xyz() const;
```
**Returns:** Vector3 containing the X, Y, and Z components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-setx"></a>

#### **`Vector4::setX()`**

 Set the X component of the vector

Signature:
```cpp
 inline void setX(float x);
```
**Parameters:**
- `x`: New X component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector4-sety"></a>

#### **`Vector4::setY()`**

 Set the Y component of the vector

Signature:
```cpp
 inline void setY(float y);
```
**Parameters:**
- `y`: New Y component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector4-setz"></a>

#### **`Vector4::setZ()`**

 Set the Z component of the vector

Signature:
```cpp
 inline void setZ(float z);
```
**Parameters:**
- `z`: New Z component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector4-setw"></a>

#### **`Vector4::setW()`**

 Set the W component of the vector

Signature:
```cpp
 inline void setW(float w);
```
**Parameters:**
- `w`: New W component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector4-set"></a>

#### **`Vector4::set()`**

 Set a component of the vector by index (0=X, 1=Y, 2=Z, 3=W)

Signature:
```cpp
 inline void set(size_t index, float value);
```
**Parameters:**
- `index`: Index of the component to set
- `value`: New value for the component

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

**Throws:** std::out_of_range if index is not in [0, 3]

---
<a id="vector4-tobxvec3"></a>

#### **`Vector4::toBxVec3()`**

 Convert this vector to a bgfx vector by dropping W

Signature:
```cpp
 inline bx::Vec3 toBxVec3() const;
```
**Returns:** Equivalent bgfx Vec3 from XYZ components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-tojoltvec3"></a>

#### **`Vector4::toJoltVec3()`**

 Convert this vector to a Jolt Vec3 by dropping W

Signature:
```cpp
 inline JPH::Vec3 toJoltVec3() const;
```
**Returns:** Equivalent Jolt Vec3 from XYZ components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-tojoltrvec3"></a>

#### **`Vector4::toJoltRVec3()`**

 Convert this vector to a Jolt RVec3 by dropping W

Signature:
```cpp
 inline JPH::RVec3 toJoltRVec3() const;
```
**Returns:** Equivalent Jolt RVec3 from XYZ components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-tojoltvec4"></a>

#### **`Vector4::toJoltVec4()`**

 Convert this vector to a Jolt Vec4

Signature:
```cpp
 inline JPH::Vec4 toJoltVec4() const;
```
**Returns:** Equivalent Jolt Vec4

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-iszero"></a>

#### **`Vector4::isZero()`**

 Check if the vector is a zero vector (all components are zero)

Signature:
```cpp
 inline bool isZero() const;
```
**Returns:** true if all components are zero, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="conversion-to-std-vector-operator-overload"></a>

#### **`Conversion to std::vector operator overload`**

 Convert this vector to a std::vector of floats

Signature:
```cpp
 inline operator ::std::vector<float>() const;
```
**Returns:** std::vector containing the X, Y, Z, and W components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="addition-operator-overload"></a>

#### **`Addition operator overload`**

 Add two vectors

Signature:
```cpp
 inline Vector4 operator+(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to add

**Returns:** New vector representing the sum

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="subtraction-operator-overload"></a>

#### **`Subtraction operator overload`**

 Subtract two vectors

Signature:
```cpp
 inline Vector4 operator-(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to subtract

**Returns:** New vector representing the difference

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="multiplication-operator-overload"></a>

#### **`Multiplication operator overload`**

 Scale vector by a scalar value

Signature:
```cpp
 inline Vector4 operator*(float scalar) const;
```
**Parameters:**
- `scalar`: Scalar multiplier

**Returns:** New scaled vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="division-operator-overload"></a>

#### **`Division operator overload`**

 Divide vector by a scalar value

Signature:
```cpp
 inline Vector4 operator/(float scalar) const;
```
**Parameters:**
- `scalar`: Scalar divisor

**Returns:** New divided vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="addition-assignment-operator-overload"></a>

#### **`Addition assignment operator overload`**

 Add vector in-place

Signature:
```cpp
 inline Vector4& operator+=(const Vector4& other);
```
**Parameters:**
- `other`: Vector to add

**Returns:** Reference to this vector after addition

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="subtraction-assignment-operator-overload"></a>

#### **`Subtraction assignment operator overload`**

 Subtract vector in-place

Signature:
```cpp
 inline Vector4& operator-=(const Vector4& other);
```
**Parameters:**
- `other`: Vector to subtract

**Returns:** Reference to this vector after subtraction

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="multiplication-assignment-operator-overload"></a>

#### **`Multiplication assignment operator overload`**

 Scale vector in-place

Signature:
```cpp
 inline Vector4& operator*=(float scalar);
```
**Parameters:**
- `scalar`: Scalar multiplier

**Returns:** Reference to this vector after scaling

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="division-assignment-operator-overload"></a>

#### **`Division assignment operator overload`**

 Divide vector in-place

Signature:
```cpp
 inline Vector4& operator/=(float scalar);
```
**Parameters:**
- `scalar`: Scalar divisor

**Returns:** Reference to this vector after division

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="equality-operator-overload"></a>

#### **`Equality operator overload`**

 Check if two vectors are equal

Signature:
```cpp
 inline bool operator==(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if vectors are equal, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="inequality-operator-overload"></a>

#### **`Inequality operator overload`**

 Check if two vectors are not equal

Signature:
```cpp
 inline bool operator!=(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if vectors are not equal, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="less-than-operator-overload"></a>

#### **`Less than operator overload`**

 Check if all components are less than another vector

Signature:
```cpp
 inline bool operator<(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if all components are less, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="less-than-or-equal-to-operator-overload"></a>

#### **`Less than or equal to operator overload`**

 Check if all components are less than or equal to another vector

Signature:
```cpp
 inline bool operator<=(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if all components are less or equal, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="greater-than-operator-overload"></a>

#### **`Greater than operator overload`**

 Check if all components are greater than another vector

Signature:
```cpp
 inline bool operator>(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if all components are greater, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="greater-than-or-equal-to-operator-overload"></a>

#### **`Greater than or equal to operator overload`**

 Check if all components are greater than or equal to another vector

Signature:
```cpp
 inline bool operator>=(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if all components are greater or equal, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="subtraction-operator-overload-2"></a>

#### **`Subtraction operator overload`**

 Negate the vector

Signature:
```cpp
 inline Vector4 operator-() const;
```
**Returns:** New negated vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-normalize"></a>

#### **`Vector4::normalize()`**

 Normalize the vector in-place

**Note:** This function modifies the current vector to have a length of 1. To get a new normalized vector without modifying the current one, use the `normalized()` method.

Signature:
```cpp
 inline void normalize();
```
**Returns:** Reference to this vector after normalization

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector4-nodiscard"></a>

#### **`Vector4::nodiscard()`**

 Normalize the vector and return a new normalized vector

**Note:** This function RETURNS a new normalized vector without modifying the current one. To normalize in-place, use the `normalize()` method.

Signature:
```cpp
 [[nodiscard("Use the normalize() method to normalize in-place")]] inline Vector4 normalized() const;
```
**Returns:** New normalized vector with length 1

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-length"></a>

#### **`Vector4::length()`**

 Calculate the length (magnitude) of the vector

Signature:
```cpp
 inline float length() const;
```
**Returns:** Length of the vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-lengthsquared"></a>

#### **`Vector4::lengthSquared()`**

 Calculate the squared length of the vector (more efficient than length)

Signature:
```cpp
 inline float lengthSquared() const;
```
**Returns:** Squared length of the vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-dot"></a>

#### **`Vector4::dot()`**

 Calculate the dot product with another vector

Signature:
```cpp
 inline float dot(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to dot with

**Returns:** Dot product result

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-cross"></a>

#### **`Vector4::cross()`**

 Compute a 3D cross product using XYZ components, with W set to 0

Signature:
```cpp
 inline Vector4 cross(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to cross with

**Returns:** Cross product vector in XYZ and W = 0

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-distance"></a>

#### **`Vector4::distance()`**

 Calculate the distance between this vector and another vector

Signature:
```cpp
 inline float distance(const Vector4& other) const;
```
**Parameters:**
- `other`: Target vector

**Returns:** Distance between vectors

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-distancesquared"></a>

#### **`Vector4::distanceSquared()`**

 Calculate the squared distance between this vector and another vector

Signature:
```cpp
 inline float distanceSquared(const Vector4& other) const;
```
**Parameters:**
- `other`: Target vector

**Returns:** Squared distance between vectors

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-lerp"></a>

#### **`Vector4::lerp()`**

 Linear interpolation between this vector and another

Signature:
```cpp
 inline Vector4 lerp(const Vector4& other, float t) const;
```
**Parameters:**
- `other`: Target vector to interpolate towards
- `t`: Interpolation factor (0 = this vector, 1 = other vector)

**Returns:** Interpolated vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-clamp"></a>

#### **`Vector4::clamp()`**

 Clamp vector components to a range defined by min and max vectors

Signature:
```cpp
 inline Vector4 clamp(const Vector4& min, const Vector4& max) const;
```
**Parameters:**
- `min`: Minimum bounds vector
- `max`: Maximum bounds vector

**Returns:** Clamped vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-abs"></a>

#### **`Vector4::abs()`**

 Get absolute value of vector components

Signature:
```cpp
 inline Vector4 abs() const;
```
**Returns:** Vector with absolute value components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-reflect"></a>

#### **`Vector4::reflect()`**

 Reflect this vector off a surface with the given normal

Signature:
```cpp
 inline Vector4 reflect(const Vector4& normal) const;
```
**Parameters:**
- `normal`: Surface normal vector

**Returns:** Reflected vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-refract"></a>

#### **`Vector4::refract()`**

 Refract this vector through a surface with the given normal and eta ratio

Signature:
```cpp
 inline Vector4 refract(const Vector4& normal, float eta) const;
```
**Parameters:**
- `normal`: Surface normal vector
- `eta`: Ratio of refractive indices

**Returns:** Refracted vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-min"></a>

#### **`Vector4::min()`**

 Get component-wise minimum with another vector

Signature:
```cpp
 inline Vector4 min(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** Vector with minimum components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector4-max"></a>

#### **`Vector4::max()`**

 Get component-wise maximum with another vector

Signature:
```cpp
 inline Vector4 max(const Vector4& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** Vector with maximum components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
