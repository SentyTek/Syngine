# Syngine API Documentation

## ShaderManager.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Resources/ShaderManager.h)

Class to manage shaders and their associated metadata @section ShaderManager @internal

**This class has been available since:** v0.0.2. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 


## Additional Functions: 

### Enums and Structs: 

- [MaterialParameterDesc](#syngine-materialparameterdesc)
- [TextureParameterDesc](#syngine-textureparameterdesc)
- [EngineUniform](#shader-engineuniform)
- [EngineSampler](#shader-enginesampler)

### Functions: 

- [LoadShader()](#shadermanager-loadshader)
- [UnloadShader()](#shadermanager-unloadshader)
- [ReloadShader()](#shadermanager-reloadshader)
- [UnloadAllShaders()](#shadermanager-unloadallshaders)
- [ReloadAllShaders()](#shadermanager-reloadallshaders)
- [GetProgramsByViewID()](#shadermanager-getprogramsbyviewid)
- [Get()](#shadermanager-get)
- [Get()](#shadermanager-get-2)

---
<a id="syngine-materialparameterdesc"></a>

#### **`Syngine::MaterialParameterDesc()`**

 Struct to hold information about a material parameter

Signature:
```cpp
struct MaterialParameterDesc
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Name of the material parameter |
| `UniformType` | `type` | Type of the material parameter (e.g., VEC4, MAT4) |
| `uint16_t` | `count` | Number of elements in the parameter (for arrays) |
| `handle` | `Handle` | to the bgfx uniform associated with this parameter |
| `defaultStorage` | `Default` | storage for the parameter (used for |
**This function has been available since:** v0.0.2

---
<a id="syngine-textureparameterdesc"></a>

#### **`Syngine::TextureParameterDesc()`**

 Struct to hold information about a texture parameter

Signature:
```cpp
struct TextureParameterDesc
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Name of the texture parameter |
| `uint8_t` | `stage` | Stage of the texture parameter |
| `bgfx::UniformHandle` | `handle` | Handle to the bgfx uniform associated with |
**This function has been available since:** v0.0.2

---
<a id="shader-engineuniform"></a>

#### **`Shader::EngineUniform()`**

 Struct to hold information about an engine uniform

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 struct EngineUniform
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `BGFX_INVALID_HANDLE` | `Handle` | to the bgfx uniform |
| `nullptr` | `Function` | pointer to retrieve uniform data |
| `uint16_t` | `count` | Number of elements in the uniform (for arrays) |
**This function has been available since:** v0.0.2

---
<a id="shader-enginesampler"></a>

#### **`Shader::EngineSampler()`**

 Struct to hold information about an engine sampler

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 struct EngineSampler
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Name of the engine sampler |
| `BGFX_INVALID_HANDLE` | `Handle` | to the bgfx uniform |
| `uint8_t` | `stage` | Stage of the engine sampler |
| `nullptr` | `Function` | pointer to retrieve uniform data |
**This function has been available since:** v0.0.2

---
<a id="shadermanager-loadshader"></a>

#### **`ShaderManager::LoadShader()`**

 Load a shader from a bundle and return its ID

Signature:
```cpp
 static size_t LoadShader(const std::string& bundlePath, const std::string& shaderName, const ViewID viewId);
```
**Parameters:**
- `bundlePath`: Path to the bundle containing the shader
- `shaderName`: Name of the shader to load
- `viewId`: View ID associated with the shader

**Returns:** size_t ID of the loaded shader, or BGFX_INVALID_HANDLE if loading failed

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="shadermanager-unloadshader"></a>

#### **`ShaderManager::UnloadShader()`**

 Unload a shader by its ID

Signature:
```cpp
 static bool UnloadShader(size_t shaderId);
```
**Parameters:**
- `shaderId`: ID of the shader to unload

**Returns:** true if the shader was successfully unloaded, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="shadermanager-reloadshader"></a>

#### **`ShaderManager::ReloadShader()`**

 Reload a shader by its ID

Signature:
```cpp
 static bool ReloadShader(size_t shaderId);
```
**Parameters:**
- `shaderId`: ID of the shader to reload

**Returns:** true if the shader was successfully reloaded, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="shadermanager-unloadallshaders"></a>

#### **`ShaderManager::UnloadAllShaders()`**

 Unload all loaded shaders

Signature:
```cpp
 static bool UnloadAllShaders();
```
**Returns:** true if all shaders were successfully unloaded, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="shadermanager-reloadallshaders"></a>

#### **`ShaderManager::ReloadAllShaders()`**

 Reload all loaded shaders

Signature:
```cpp
 static bool ReloadAllShaders();
```
**Returns:** true if all shaders were successfully reloaded, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="shadermanager-getprogramsbyviewid"></a>

#### **`ShaderManager::GetProgramsByViewID()`**

 Get all loaded shaders for a specific view ID

Signature:
```cpp
 static std::vector<Shader*> GetProgramsByViewID(ViewID viewId);
```
**Parameters:**
- `viewId`: View ID to filter shaders by

**Returns:** std::vector<Shader*> Vector of pointers to shaders associated with the specified view ID

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="shadermanager-get"></a>

#### **`ShaderManager::Get()`**

 Get a shader by its ID

Signature:
```cpp
 static Shader* Get(size_t shaderId);
```
**Parameters:**
- `shaderId`: ID of the shader to retrieve

**Returns:** Shader* Pointer to the shader with the specified ID, or nullptr if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="shadermanager-get-2"></a>

#### **`ShaderManager::Get()`**

 Get a shader by its name

**Note:** It is generally recommened to use ID for shader retrieval, as names are not guaranteed to be unique and considerably slower to search for

Signature:
```cpp
 static Shader* Get(const std::string& shaderName);
```
**Parameters:**
- `shaderName`: Name of the shader to retrieve

**Returns:** Shader* Pointer to the shader with the specified name, or nullptr if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
