# Syngine API Documentation

## Vector2.hpp header

[<- Back](../index.md)

[See source](./../../include/Syngine/Math/Vector2.hpp)

2D vector class for mathematical operations in 2D space

---
## Goto: 


## Additional Functions: 

### Constructors: 

- [Vector2()](#vector2-vector2)
- [Vector2(float value)](#vector2-vector2float-value)
- [Vector2(const ::std::vector<float>& values)](#vector2-vector2const-std-vectorfloat-values)
- [Vector2(float x, float y)](#vector2-vector2float-x-float-y)
- [Vector2(const Vector2& other)](#vector2-vector2const-vector2-other)
- [Vector2(Vector2&& other)](#vector2-vector2vector2-other)

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
- [Subtraction operator overload](#subtraction-operator-overload-2)
- [Less than operator overload](#less-than-operator-overload)
- [Less than or equal to operator overload](#less-than-or-equal-to-operator-overload)
- [Greater than operator overload](#greater-than-operator-overload)
- [Greater than or equal to operator overload](#greater-than-or-equal-to-operator-overload)

### Functions: 

- [data()](#vector2-data)
- [data()](#vector2-data-2)
- [x()](#vector2-x)
- [y()](#vector2-y)
- [setX()](#vector2-setx)
- [setY()](#vector2-sety)
- [isZero()](#vector2-iszero)
- [length()](#vector2-length)
- [lengthSquared()](#vector2-lengthsquared)
- [distance()](#vector2-distance)
- [distanceSquared()](#vector2-distancesquared)
- [normalize()](#vector2-normalize)
- [nodiscard()](#vector2-nodiscard)
- [dot()](#vector2-dot)
- [cross()](#vector2-cross)
- [lerp()](#vector2-lerp)
- [clamp()](#vector2-clamp)
- [reflect()](#vector2-reflect)
- [refract()](#vector2-refract)
- [min()](#vector2-min)
- [max()](#vector2-max)
- [abs()](#vector2-abs)

---
<a id="vector2-vector2"></a>

## Class Constructor

#### **`Vector2()`**

 Default constructor, initializes vector to (0.0, 0.0)

Signature:
```cpp
 inline Vector2() : m_storage(0.0f, 0.0f);
```
**This function has been available since:** v0.0.2

---
<a id="vector2-vector2float-value"></a>

## Class Constructor

#### **`Vector2(float value)`**

 Constructor with a single float value, initializes both components to the same value

Signature:
```cpp
 inline Vector2(float value) : m_storage(value, value);
```
**Parameters:**
- `value`: Value to set both X and Y components

**This function has been available since:** v0.0.2

---
<a id="vector2-vector2const-std-vectorfloat-values"></a>

## Class Constructor

#### **`Vector2(const ::std::vector<float>& values)`**

 Constructor from std::vector of floats, expects exactly 2 elements

Signature:
```cpp
 inline Vector2(const ::std::vector<float>& values);
```
**Parameters:**
- `values`: std::vector containing two float values

**This function has been available since:** v0.0.2

**Throws:** std::invalid_argument if the vector does not contain exactly 2 elements

---
<a id="vector2-vector2float-x-float-y"></a>

## Class Constructor

#### **`Vector2(float x, float y)`**

 Constructor with x and y values

Signature:
```cpp
 inline Vector2(float x, float y) : m_storage(x, y);
```
**Parameters:**
- `x`: X component of the vector
- `y`: Y component of the vector

**This function has been available since:** v0.0.2

---
<a id="vector2-vector2const-vector2-other"></a>

## Class Constructor

#### **`Vector2(const Vector2& other)`**

 Copy constructor

Signature:
```cpp
 inline Vector2(const Vector2& other) = default;
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
 inline Vector2& operator=(const Vector2& other) = default;
```
**Parameters:**
- `other`: Vector to assign from

**Returns:** Reference to this vector after assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector2-vector2vector2-other"></a>

## Class Constructor

#### **`Vector2(Vector2&& other)`**

 Move constructor

Signature:
```cpp
 inline Vector2(Vector2&& other) noexcept = default;
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
 inline Vector2& operator=(Vector2&& other) noexcept = default;
```
**Parameters:**
- `other`: Vector to move from

**Returns:** Reference to this vector after move assignment

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector2-data"></a>

#### **`Vector2::data()`**

 Get the raw data as a pointer to float array

Signature:
```cpp
 inline float* data();
```
**Returns:** Pointer to the first element of the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-data-2"></a>

#### **`Vector2::data()`**

 Get the raw data as a pointer to float array

Signature:
```cpp
 inline const float* data() const;
```
**Returns:** Pointer to the first element of the float array

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-x"></a>

#### **`Vector2::x()`**

 Get the X component of the vector

Signature:
```cpp
 inline float x() const;
```
**Returns:** X component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-y"></a>

#### **`Vector2::y()`**

 Get the Y component of the vector

Signature:
```cpp
 inline float y() const;
```
**Returns:** Y component

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-setx"></a>

#### **`Vector2::setX()`**

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
<a id="vector2-sety"></a>

#### **`Vector2::setY()`**

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
<a id="vector2-iszero"></a>

#### **`Vector2::isZero()`**

 Check if the vector is a zero vector (both components are zero)

Signature:
```cpp
 inline bool isZero() const;
```
**Returns:** true if both X and Y are zero, false otherwise

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
**Returns:** std::vector containing the X and Y components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="addition-operator-overload"></a>

#### **`Addition operator overload`**

 Add two vectors

Signature:
```cpp
 inline Vector2 operator+(const Vector2& other) const;
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
 inline Vector2 operator-(const Vector2& other) const;
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
 inline Vector2 operator*(float scalar) const;
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
 inline Vector2 operator/(float scalar) const;
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
 inline Vector2& operator+=(const Vector2& other);
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
 inline Vector2& operator-=(const Vector2& other);
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
 inline Vector2& operator*=(float scalar);
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
 inline Vector2& operator/=(float scalar);
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
 inline bool operator==(const Vector2& other) const;
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
 inline bool operator!=(const Vector2& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if vectors are not equal, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="subtraction-operator-overload-2"></a>

#### **`Subtraction operator overload`**

 Negate the vector

Signature:
```cpp
 inline Vector2 operator-() const;
```
**Returns:** New negated vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="less-than-operator-overload"></a>

#### **`Less than operator overload`**

 Check if all components are less than another vector

Signature:
```cpp
 inline bool operator<(const Vector2& other) const;
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
 inline bool operator<=(const Vector2& other) const;
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
 inline bool operator>(const Vector2& other) const;
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
 inline bool operator>=(const Vector2& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** true if all components are greater or equal, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-length"></a>

#### **`Vector2::length()`**

 Calculate the length (magnitude) of the vector

Signature:
```cpp
 inline float length() const;
```
**Returns:** Length of the vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-lengthsquared"></a>

#### **`Vector2::lengthSquared()`**

 Calculate the squared length of the vector (more efficient than length)

Signature:
```cpp
 inline float lengthSquared() const;
```
**Returns:** Squared length of the vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-distance"></a>

#### **`Vector2::distance()`**

 Calculate the distance between this vector and another vector

Signature:
```cpp
 inline float distance(const Vector2& other) const;
```
**Parameters:**
- `other`: Target vector

**Returns:** Distance between vectors

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-distancesquared"></a>

#### **`Vector2::distanceSquared()`**

 Calculate the squared distance between this vector and another vector

Signature:
```cpp
 inline float distanceSquared(const Vector2& other) const;
```
**Parameters:**
- `other`: Target vector

**Returns:** Squared distance between vectors

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-normalize"></a>

#### **`Vector2::normalize()`**

 Get the normalized (unit) vector in the same direction

Signature:
```cpp
 inline Vector2& normalize();
```
**Returns:** This function does NOT return a new vector, it modifies the current one. To get a new normalized vector, use the `normalized()` method.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="vector2-nodiscard"></a>

#### **`Vector2::nodiscard()`**

 Get the normalized (unit) vector in the same direction

Signature:
```cpp
 [[nodiscard("Use the normalize() method to normalize in-place")]] inline Vector2 normalized() const;
```
**Returns:** This function RETURNS a new normalized vector without modifying the current one. To normalize in-place, use the `normalize()` method.

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-dot"></a>

#### **`Vector2::dot()`**

 Calculate the dot product with another vector

Signature:
```cpp
 inline float dot(const Vector2& other) const;
```
**Parameters:**
- `other`: Vector to dot with

**Returns:** Dot product result

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-cross"></a>

#### **`Vector2::cross()`**

 Calculate the cross product with another vector

Signature:
```cpp
 inline float cross(const Vector2& other) const;
```
**Parameters:**
- `other`: Vector to cross with

**Returns:** Cross product result (in 2D, returns scalar Z component)

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-lerp"></a>

#### **`Vector2::lerp()`**

 Linear interpolation between this vector and another

Signature:
```cpp
 inline Vector2 lerp(const Vector2& other, float t) const;
```
**Parameters:**
- `other`: Target vector to interpolate towards
- `t`: Interpolation factor (0 = this vector, 1 = other vector)

**Returns:** Interpolated vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-clamp"></a>

#### **`Vector2::clamp()`**

 Clamp vector components to a range defined by min and max vectors

Signature:
```cpp
 inline Vector2 clamp(const Vector2& min, const Vector2& max) const;
```
**Parameters:**
- `min`: Minimum bounds vector
- `max`: Maximum bounds vector

**Returns:** Clamped vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-reflect"></a>

#### **`Vector2::reflect()`**

 Reflect this vector off a surface with the given normal

Signature:
```cpp
 inline Vector2 reflect(const Vector2& normal) const;
```
**Parameters:**
- `normal`: Surface normal vector

**Returns:** Reflected vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-refract"></a>

#### **`Vector2::refract()`**

 Refract this vector through a surface with the given normal and eta ratio

Signature:
```cpp
 inline Vector2 refract(const Vector2& normal, float eta) const;
```
**Parameters:**
- `normal`: Surface normal vector
- `eta`: Ratio of refractive indices

**Returns:** Refracted vector

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-min"></a>

#### **`Vector2::min()`**

 Get component-wise minimum with another vector

Signature:
```cpp
 inline Vector2 min(const Vector2& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** Vector with minimum components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-max"></a>

#### **`Vector2::max()`**

 Get component-wise maximum with another vector

Signature:
```cpp
 inline Vector2 max(const Vector2& other) const;
```
**Parameters:**
- `other`: Vector to compare

**Returns:** Vector with maximum components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="vector2-abs"></a>

#### **`Vector2::abs()`**

 Get absolute value of vector components

Signature:
```cpp
 inline Vector2 abs() const;
```
**Returns:** Vector with absolute value components

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
