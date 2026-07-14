# Syngine API Documentation

## Vector3.hpp header

[<- Back](../index.md)

[See source](./../../include/Syngine/Math/Vector3.hpp)

3D vector class for mathematical operations in 3D space

**Aliases:** `Vec3`, `SVec3`

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


## Popular Functions: 

- [Vector3(float value)](#vector3-vector3float-value)
- [Vector3(float x, float y, float z)](#vector3-vector3float-x-float-y-float-z)
- [data()](#vector3-data)
- [data()](#vector3-data-2)
- [x()](#vector3-x)
- [y()](#vector3-y)
- [z()](#vector3-z)
- [toRads()](#vector3-torads)
- [length()](#vector3-length)
- [normalize()](#vector3-normalize)
- [distance()](#vector3-distance)

## Additional Functions: 

### Constructors: 

- [Vector3()](#vector3-vector3)
- [Vector3(const Vector2& v, float z)](#vector3-vector3const-vector2-v-float-z)
- [Vector3(const bx::Vec3& v)](#vector3-vector3const-bx-vec3-v)
- [Vector3(const JPH::Vec3& v)](#vector3-vector3const-jph-vec3-v)
- [Vector3(const ::std::vector<float>& values)](#vector3-vector3const-std-vectorfloat-values)
- [Vector3(const Vector3& other)](#vector3-vector3const-vector3-other)

### Operator Overloads: 

- [Assignment operator overload](#assignment-operator-overload)
- [Conversion to std::vector operator overload](#conversion-to-std-vector-operator-overload)
- [Indexing operator overload](#indexing-operator-overload)
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

- [xy()](#vector3-xy)
- [xz()](#vector3-xz)
- [yz()](#vector3-yz)
- [setX()](#vector3-setx)
- [setY()](#vector3-sety)
- [setZ()](#vector3-setz)
- [toBxVec3()](#vector3-tobxvec3)
- [toJoltVec3()](#vector3-tojoltvec3)
- [toJoltRVec3()](#vector3-tojoltrvec3)
- [toDegs()](#vector3-todegs)
- [set()](#vector3-set)
- [isZero()](#vector3-iszero)
- [lengthSquared()](#vector3-lengthsquared)
- [normalized()](#vector3-normalized)
- [dot()](#vector3-dot)
- [cross()](#vector3-cross)
- [distanceSquared()](#vector3-distancesquared)
- [lerp()](#vector3-lerp)
- [clamp()](#vector3-clamp)
- [abs()](#vector3-abs)
- [reflect()](#vector3-reflect)
- [refract()](#vector3-refract)
- [min()](#vector3-min)
- [max()](#vector3-max)

---
<a id="vector3-vector3"></a>

## Class Constructor

#### **`Vector3()`**

 Default constructor, initializes the vector to (0.0, 0.0, 0.0)

Signature:
```cpp
 inline Vector3() : m_storage(0.0f, 0.0f, 0.0f);
```
**This function has been available since:** v0.0.2

---
<a id="vector3-vector3float-value"></a>

## Class Constructor

#### **`Vector3(float value)`**

 Construct a vector from a single float value, initializes all components to the same value

Signature:
```cpp
 inline Vector3(float value) : m_storage(value, value, value);
```
**Parameters:**
- `value`: Value to set all components

**This function has been available since:** v0.0.2

---
<a id="vector3-vector3float-x-float-y-float-z"></a>

## Class Constructor

#### **`Vector3(float x, float y, float z)`**

 Construct a vector from three float components

Signature:
```cpp
 inline Vector3(float x, float y, float z) : m_storage(x, y, z);
```
**Parameters:**
- `x`: X component of the vector
- `y`: Y component of the vector
- `z`: Z component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector3-vector3const-vector2-v-float-z"></a>

## Class Constructor

#### **`Vector3(const Vector2& v, float z)`**

 Construct a vector from a 2D vector and a Z component

Signature:
```cpp
 inline Vector3(const Vector2& v, float z) : m_storage(v.x(), v.y(), z);
```
**Parameters:**
- `v`: Source 2D vector
- `z`: Z component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector3-vector3const-bx-vec3-v"></a>

## Class Constructor

#### **`Vector3(const bx::Vec3& v)`**

 Construct a vector from a bgfx vector

Signature:
```cpp
 inline Vector3(const bx::Vec3& v) : m_storage(v.x, v.y, v.z);
```
**Parameters:**
- `v`: Source bgfx vector

**This function has been available since:** v0.0.2

---
<a id="vector3-vector3const-jph-vec3-v"></a>

## Class Constructor

#### **`Vector3(const JPH::Vec3& v)`**

 Construct a vector from a Jolt vector

Signature:
```cpp
 inline Vector3(const JPH::Vec3& v) : m_storage(v.GetX(), v.GetY(), v.GetZ());
```
**Parameters:**
- `v`: Source Jolt vector

**This function has been available since:** v0.0.2

---
<a id="vector3-vector3const-std-vectorfloat-values"></a>

## Class Constructor

#### **`Vector3(const ::std::vector<float>& values)`**

 Constructor from std::vector of floats, expects exactly 3 elements

Signature:
```cpp
 inline Vector3(const ::std::vector<float>& values);
```
**Parameters:**
- `values`: std::vector containing three float values

**This function has been available since:** v0.0.2

**Throws:** std::invalid_argument if the vector does not contain exactly 3 elements

---
<a id="vector3-vector3const-vector3-other"></a>

## Class Constructor

#### **`Vector3(const Vector3& other)`**

 Copy constructor

Signature:
```cpp
 inline Vector3(const Vector3& other) = default;
```
**Parameters:**
- `other`: Vector to copy

**This function has been available since:** v0.0.2

---
<a id="assignment-operator-overload"></a>

#### **`Assignment operator overload`**

 Assignment operator

Signature:
```cpp
 inline Vector3& operator=(const Vector3& other) = default;
```
**Parameters:**
- `other`: Vector to assign from

**Returns:** Reference to this vector after assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector3-data"></a>

#### **`Vector3::data()`**

 Get the raw data as a pointer to float array

Signature:
```cpp
 inline float* data();
```
**Returns:** Pointer to the first element of the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-data-2"></a>

#### **`Vector3::data()`**

 Get the raw data as a pointer to float array

Signature:
```cpp
 inline const float* data() const;
```
**Returns:** Pointer to the first element of the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-x"></a>

#### **`Vector3::x()`**

 Get the X component of the vector

Signature:
```cpp
 inline float x() const;
```
**Returns:** X component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-y"></a>

#### **`Vector3::y()`**

 Get the Y component of the vector

Signature:
```cpp
 inline float y() const;
```
**Returns:** Y component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-z"></a>

#### **`Vector3::z()`**

 Get the Z component of the vector

Signature:
```cpp
 inline float z() const;
```
**Returns:** Z component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-xy"></a>

#### **`Vector3::xy()`**

 Get the XY components as a 2D vector

Signature:
```cpp
 inline Vector2 xy() const;
```
**Returns:** Vector2 containing the X and Y components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-xz"></a>

#### **`Vector3::xz()`**

 Get the XZ components as a 2D vector

Signature:
```cpp
 inline Vector2 xz() const;
```
**Returns:** Vector2 containing the X and Z components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-yz"></a>

#### **`Vector3::yz()`**

 Get the YZ components as a 2D vector

Signature:
```cpp
 inline Vector2 yz() const;
```
**Returns:** Vector2 containing the Y and Z components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-setx"></a>

#### **`Vector3::setX()`**

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
<a id="vector3-sety"></a>

#### **`Vector3::setY()`**

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
<a id="vector3-setz"></a>

#### **`Vector3::setZ()`**

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
<a id="vector3-tobxvec3"></a>

#### **`Vector3::toBxVec3()`**

 Convert this vector to a bgfx vector

Signature:
```cpp
 inline bx::Vec3 toBxVec3() const;
```
**Returns:** Equivalent bgfx vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-tojoltvec3"></a>

#### **`Vector3::toJoltVec3()`**

 Convert this vector to a Jolt Vec3

Signature:
```cpp
 inline JPH::Vec3 toJoltVec3() const;
```
**Returns:** Equivalent Jolt Vec3

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-tojoltrvec3"></a>

#### **`Vector3::toJoltRVec3()`**

 Convert this vector to a Jolt RVec3

Signature:
```cpp
 inline JPH::RVec3 toJoltRVec3() const;
```
**Returns:** Equivalent Jolt RVec3

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-torads"></a>

#### **`Vector3::toRads()`**

 Convert this vector of euler angles to a vector of radians

Signature:
```cpp
 inline Vector3 toRads() const;
```
**Returns:** Vector3 containing the angles in radians

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-todegs"></a>

#### **`Vector3::toDegs()`**

 Convert this vector of radians to a vector of euler angles

Signature:
```cpp
 inline Vector3 toDegs() const;
```
**Returns:** Vector3 containing the angles in degrees

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="conversion-to-std-vector-operator-overload"></a>

#### **`Conversion to std::vector operator overload`**

 Convert this vector to a std::vector of floats

Signature:
```cpp
 inline operator std::vector<float>() const;
```
**Returns:** std::vector containing the X, Y, and Z components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="indexing-operator-overload"></a>

#### **`Indexing operator overload`**

 Get a component by index using bracket syntax

Signature:
```cpp
 inline float operator[] (int index) const;
```
**Parameters:**
- `index`: Component index in range [0, 2]

**Returns:** Component value at the given index

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-set"></a>

#### **`Vector3::set()`**

 Set a component by index

Signature:
```cpp
 inline void set(int index, float value);
```
**Parameters:**
- `index`: Component index in range [0, 2]
- `value`: New component value

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector3-iszero"></a>

#### **`Vector3::isZero()`**

 Check if the vector is a zero vector (all components are zero)

Signature:
```cpp
 inline bool isZero() const;
```
**Returns:** true if all components are zero, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="addition-operator-overload"></a>

#### **`Addition operator overload`**

 Add two vectors

Signature:
```cpp
 inline Vector3 operator+(const Vector3& other) const;
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
 inline Vector3 operator-(const Vector3& other) const;
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
 inline Vector3 operator*(float scalar) const;
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
 inline Vector3 operator/(float scalar) const;
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
 inline Vector3& operator+=(const Vector3& other);
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
 inline Vector3& operator-=(const Vector3& other);
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
 inline Vector3& operator*=(float scalar);
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
 inline Vector3& operator/=(float scalar);
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
 inline bool operator==(const Vector3& other) const;
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
 inline bool operator!=(const Vector3& other) const;
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
 inline bool operator<(const Vector3& other) const;
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
 inline bool operator<=(const Vector3& other) const;
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
 inline bool operator>(const Vector3& other) const;
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
 inline bool operator>=(const Vector3& other) const;
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
 inline Vector3 operator-() const;
```
**Returns:** New negated vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-length"></a>

#### **`Vector3::length()`**

 Calculate the length (magnitude) of the vector

Signature:
```cpp
 inline float length() const;
```
**Returns:** Length of the vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-lengthsquared"></a>

#### **`Vector3::lengthSquared()`**

 Calculate the squared length of the vector (more efficient than length)

Signature:
```cpp
 inline float lengthSquared() const;
```
**Returns:** Squared length of the vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-normalize"></a>

#### **`Vector3::normalize()`**

 Normalize the vector in-place to have a length of 1

**Note:** This function modifies the current vector. To get a new normalized vector without modifying the current one, use the `normalized()` method.

Signature:
```cpp
 inline void normalize();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector3-normalized"></a>

#### **`Vector3::normalized()`**

 Get the normalized (unit) vector in the same direction

**Note:** This function RETURNS a new normalized vector without modifying the current one. To normalize in-place, use the `normalize()` method.

Signature:
```cpp
 inline Vector3 normalized() const;
```
**Returns:** New normalized vector with length 1

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-dot"></a>

#### **`Vector3::dot()`**

 Calculate the dot product with another vector

Signature:
```cpp
 inline float dot(const Vector3& other) const;
```
**Parameters:**
- `other`: Vector to dot with

**Returns:** Dot product result

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-cross"></a>

#### **`Vector3::cross()`**

 Calculate the cross product with another vector

Signature:
```cpp
 inline Vector3 cross(const Vector3& other) const;
```
**Parameters:**
- `other`: Vector to cross with

**Returns:** Cross product vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-distance"></a>

#### **`Vector3::distance()`**

 Calculate the distance between this vector and another vector

Signature:
```cpp
 inline float distance(const Vector3& other) const;
```
**Parameters:**
- `other`: Target vector

**Returns:** Distance between vectors

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-distancesquared"></a>

#### **`Vector3::distanceSquared()`**

 Calculate the squared distance between this vector and another vector

Signature:
```cpp
 inline float distanceSquared(const Vector3& other) const;
```
**Parameters:**
- `other`: Target vector

**Returns:** Squared distance between vectors

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-lerp"></a>

#### **`Vector3::lerp()`**

 Linear interpolation between this vector and another

Signature:
```cpp
 inline Vector3 lerp(const Vector3& other, float t) const;
```
**Parameters:**
- `other`: Target vector to interpolate towards
- `t`: Interpolation factor (0 = this vector, 1 = other vector)

**Returns:** Interpolated vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-clamp"></a>

#### **`Vector3::clamp()`**

 Clamp vector components to a range defined by min and max vectors

Signature:
```cpp
 inline Vector3 clamp(const Vector3& min, const Vector3& max) const;
```
**Parameters:**
- `min`: Minimum bounds vector
- `max`: Maximum bounds vector

**Returns:** Clamped vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-abs"></a>

#### **`Vector3::abs()`**

 Get absolute value of vector components

Signature:
```cpp
 inline Vector3 abs() const;
```
**Returns:** Vector with absolute value components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-reflect"></a>

#### **`Vector3::reflect()`**

 Reflect this vector off a surface with the given normal

Signature:
```cpp
 inline Vector3 reflect(const Vector3& normal) const;
```
**Parameters:**
- `normal`: Surface normal vector

**Returns:** Reflected vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-refract"></a>

#### **`Vector3::refract()`**

 Refract this vector through a surface with the given normal and eta ratio

Signature:
```cpp
 inline Vector3 refract(const Vector3& normal, float eta) const;
```
**Parameters:**
- `normal`: Surface normal vector
- `eta`: Ratio of refractive indices

**Returns:** Refracted vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-min"></a>

#### **`Vector3::min()`**

 Get component-wise minimum with another vector

Signature:
```cpp
 inline Vector3 min(const Vector3& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** Vector with minimum components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector3-max"></a>

#### **`Vector3::max()`**

 Get component-wise maximum with another vector

Signature:
```cpp
 inline Vector3 max(const Vector3& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** Vector with maximum components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
