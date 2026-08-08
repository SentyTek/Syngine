# Syngine API Documentation

## UniformRegistry.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Resources/UniformRegistry.h)

---
## Goto: 


## Additional Functions: 

### Enums and Structs: 

- [UniformType](#syngine-uniformtype)
- [UniformFrequency](#syngine-uniformfrequency)
- [UniformDataProvider](#syngine-uniformdataprovider)

<a id="syngine-uniformtype"></a>

#### **`Syngine::UniformType()`**

 Different types of shader uniforms

Signature:
```cpp
enum class UniformType
```
**Members:**
| Name | Description |
| --- | --- | 
| `SAMPLER` | Sampler uniform |
| `VEC4` | Vec4 uniform |
| `MAT4` | Mat4 uniform |
| `MAT3` | Mat3 uniform |
| `UNKNOWN` | Unknown uniform type |
**This function has been available since:** v0.0.2

---
<a id="syngine-uniformfrequency"></a>

#### **`Syngine::UniformFrequency()`**

 Different frequencies at which shader uniforms can be updated

Signature:
```cpp
enum class UniformFrequency
```
**Members:**
| Name | Description |
| --- | --- | 
| `DRAW` | Uniform updated per draw call |
| `VIEW` | Uniform updated per view |
| `FRAME` | Uniform updated per frame |
| `UNKNOWN` | Count of uniform frequencies |
**This function has been available since:** v0.0.2

---
<a id="syngine-uniformdataprovider"></a>

#### **`Syngine::UniformDataProvider()`**

 Struct to hold information about a uniform data provider

Signature:
```cpp
struct UniformDataProvider
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `UniformType` | `type` | Type of the uniform |
| `UniformFrequency` | `frequency` | Frequency of uniform updates |
| `uint16_t` | `num` | Number of elements in the uniform (for arrays) |
| `UniformGetter` | `getter` | Function pointer to retrieve uniform data |
| `std::string` | `name` | Name of the uniform |
**This function has been available since:** v0.0.2

---
