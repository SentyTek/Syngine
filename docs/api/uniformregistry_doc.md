# Syngine API Documentation

## UniformRegistry.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Resources/UniformRegistry.h)

Class to manage uniform data providers and their associated bgfx uniform handles @section Uniforms

**This class has been available since:** v0.0.2. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 


## Additional Functions: 

### Enums and Structs: 

- [UniformType](#syngine-uniformtype)
- [UniformFrequency](#syngine-uniformfrequency)
- [UniformDataProvider](#syngine-uniformdataprovider)

### Functions: 

- [GetInstance()](#uniformregistry-getinstance)
- [_GetUniformHandle()](#uniformregistry-_getuniformhandle)
- [RegisterProvider()](#uniformregistry-registerprovider)
- [FindProvider()](#uniformregistry-findprovider)
- [DestroyAllUniforms()](#uniformregistry-destroyalluniforms)
- [GetContext()](#uniformregistry-getcontext)

---
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
<a id="uniformregistry-getinstance"></a>

#### **`UniformRegistry::GetInstance()`**

 Get the singleton instance of the UniformRegistry

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static UniformRegistry& GetInstance();
```
**Returns:** UniformRegistry& Reference to the singleton instance

**Thread Safety:** thread-safe

**This function has been available since:** v0.0.2

---
<a id="uniformregistry-_getuniformhandle"></a>

#### **`UniformRegistry::_GetUniformHandle()`**

 Get the bgfx uniform handle for a given uniform name

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static bgfx::UniformHandle _GetUniformHandle(const std::string& name);
```
**Parameters:**
- `name`: Name of the uniform

**Returns:** bgfx::UniformHandle Handle to the bgfx uniform

**Thread Safety:** thread-safe

**This function has been available since:** v0.0.2

---
<a id="uniformregistry-registerprovider"></a>

#### **`UniformRegistry::RegisterProvider()`**

 Register a uniform data provider with the registry

Signature:
```cpp
 static void RegisterProvider(const std::string& name, UniformDataProvider provider);
```
**Parameters:**
- `name`: Name of the uniform
- `provider`: UniformDataProvider struct containing the provider

**Thread Safety:** thread-safe

**This function has been available since:** v0.0.2

---
<a id="uniformregistry-findprovider"></a>

#### **`UniformRegistry::FindProvider()`**

 Find a uniform data provider by name

Signature:
```cpp
 static UniformDataProvider* FindProvider(const std::string& name);
```
**Parameters:**
- `name`: Name of the uniform

**Returns:** UniformDataProvider* Pointer to the provider, nullptr if not found

**Thread Safety:** thread-safe

**This function has been available since:** v0.0.2

---
<a id="uniformregistry-destroyalluniforms"></a>

#### **`UniformRegistry::DestroyAllUniforms()`**

 Destroys all registered uniform data providers and their associated bgfx uniform handles

Signature:
```cpp
 static void DestroyAllUniforms();
```
**Thread Safety:** thread-safe

**This function has been available since:** v0.0.2

---
<a id="uniformregistry-getcontext"></a>

#### **`UniformRegistry::GetContext()`**

 Convert a void pointer to a specific type T and return a reference to it

Signature:
```cpp
 template <typename T> static const T& GetContext(const void* ptr);
```
**Template Parameters:**
- `T`: The type to convert the pointer to

**Parameters:**
- `ptr`: The void pointer to convert

**Returns:** const T& Reference to the converted type

**Thread Safety:** thread-safe

**This function has been available since:** v0.0.2

---
