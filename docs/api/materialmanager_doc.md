# Syngine API Documentation

## MaterialManager.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Resources/MaterialManager.h)

Class to manage materials and their instances @section MaterialManager

**This class has been available since:** v0.0.2. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [Material(const std::string& name)](#material-materialconst-std-string-name)
- [Material(const std::string& name, Shader* shader)](#material-materialconst-std-string-name-shader-shader)
- [MaterialInstance(const Material& material)](#materialinstance-materialinstanceconst-material-material)

### Enums and Structs: 

- [MaterialParameter](#syngine-materialparameter)
- [TextureParameter](#syngine-textureparameter)

### Functions: 

- [_SetDefault()](#material-_setdefault)
- [_SetDefaultTexture()](#material-_setdefaulttexture)
- [CreateInstance()](#material-createinstance)
- [Clone()](#material-clone)
- [_FindParameter()](#materialinstance-_findparameter)
- [_FindTexture()](#materialinstance-_findtexture)
- [GetMaterial()](#materialinstance-getmaterial)
- [GetShader()](#materialinstance-getshader)
- [Set()](#materialinstance-set)
- [Set()](#materialinstance-set-2)
- [SetTexture()](#materialinstance-settexture)
- [SetTexture()](#materialinstance-settexture-2)
- [Destroy()](#materialinstance-destroy)
- [inline](#materialinstance-inline)
- [Material::CreateInstance()](#materialinstance-material-createinstance)
- [Material::Clone()](#materialinstance-material-clone)
- [GetMaterialFromFile()](#materialmanager-getmaterialfromfile)
- [GetMaterialFromBundle()](#materialmanager-getmaterialfrombundle)
- [GetDefaultMaterialPBR()](#materialmanager-getdefaultmaterialpbr)
- [GetFallbackAlbedoTexture()](#materialmanager-getfallbackalbedotexture)
- [GetFallbackNormalTexture()](#materialmanager-getfallbacknormaltexture)
- [MaterialExists()](#materialmanager-materialexists)
- [GetMaterialByName()](#materialmanager-getmaterialbyname)
- [CreateMaterial()](#materialmanager-creatematerial)

---
<a id="syngine-materialparameter"></a>

#### **`Syngine::MaterialParameter()`**

 Struct to hold information about a material parameter

Signature:
```cpp
struct MaterialParameter
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Name of the material parameter |
| `std::array<std::byte,` | `64>` | storage Storage for the parameter data (used |
**This function has been available since:** v0.0.2

---
<a id="syngine-textureparameter"></a>

#### **`Syngine::TextureParameter()`**

 Struct to hold information about a texture parameter

Signature:
```cpp
struct TextureParameter
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Name of the texture parameter |
| `handle` | `Handle` | to the bgfx texture associated with this parameter |
| `uint32_t` | `samplerFlags` | Sampler flags for the texture parameter |
| `uint8_t` | `stage` | Stage of the texture parameter |
**This function has been available since:** v0.0.2

---
<a id="material-materialconst-std-string-name"></a>

## Class Constructor

#### **`Material(const std::string& name)`**

 Private constructor to create a Material with a given name. Use MaterialManager::CreateMaterial to create a new material.

#### This function is internal use only and not intended for public use!

**Note:** Highly recommended to NOT use this constructor directly.

Signature:
```cpp
 Material(const std::string& name) : name(name), shader(nullptr);
```
---
<a id="material-_setdefault"></a>

#### **`Material::_SetDefault()`**

 Set the default value for a material parameter

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 void _SetDefault(const std::string& paramName, const void* data, size_t size);
```
**Parameters:**
- `paramName`: Name of the material parameter
- `data`: Pointer to the data to set as the default value
- `size`: Size of the data in bytes

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="material-_setdefaulttexture"></a>

#### **`Material::_SetDefaultTexture()`**

 Set the default texture for a material parameter

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 void _SetDefaultTexture(const std::string& paramName, bgfx::TextureHandle texture, uint32_t samplerFlags, uint8_t stage);
```
**Parameters:**
- `paramName`: Name of the material parameter
- `texture`: Handle to the texture to set as the default value
- `samplerFlags`: Sampler flags for the texture
- `stage`: Stage of the texture parameter

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="material-materialconst-std-string-name-shader-shader"></a>

## Class Constructor

#### **`Material(const std::string& name, Shader* shader)`**

 Construct a Material with a given name and shader

#### This function is internal use only and not intended for public use!

**Note:** Generally recommended to use MaterialManager::CreateMaterial to create a new material.

Signature:
```cpp
 Material(const std::string& name, Shader* shader) : name(name), shader(shader);
```
**Parameters:**
- `name`: Name of the material
- `shader`: Pointer to the associated shader

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="material-createinstance"></a>

#### **`Material::CreateInstance()`**

 Create a MaterialInstance from this Material

Signature:
```cpp
 MaterialInstance CreateInstance() const;
```
**Returns:** MaterialInstance A new MaterialInstance with default values

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="material-clone"></a>

#### **`Material::Clone()`**

 Clone this Material into a new MaterialInstance

Signature:
```cpp
 MaterialInstance Clone() const;
```
**Returns:** MaterialInstance A new MaterialInstance with default values

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-_findparameter"></a>

#### **`MaterialInstance::_FindParameter()`**

 Find a material parameter by name

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 const MaterialParameter* _FindParameter(const std::string& paramName) const;
```
**Parameters:**
- `paramName`: Name of the material parameter

**Returns:** const MaterialParameter* Pointer to the material parameter, or nullptr if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialinstance-_findtexture"></a>

#### **`MaterialInstance::_FindTexture()`**

 Find a texture parameter by name

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 const TextureParameter* _FindTexture(const std::string& paramName) const;
```
**Parameters:**
- `paramName`: Name of the texture parameter

**Returns:** const TextureParameter* Pointer to the texture parameter, or nullptr if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialinstance-materialinstanceconst-material-material"></a>

## Class Constructor

#### **`MaterialInstance(const Material& material)`**

 Construct a MaterialInstance from a given Material

Signature:
```cpp
 explicit MaterialInstance(const Material& material) : m_material(&material);
```
**Parameters:**
- `material`: Reference to the Material to create the instance from

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-getmaterial"></a>

#### **`MaterialInstance::GetMaterial()`**

 Get the associated Material of this MaterialInstance

Signature:
```cpp
 const Material* GetMaterial() const;
```
**Returns:** const Material* Pointer to the associated Material

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialinstance-getshader"></a>

#### **`MaterialInstance::GetShader()`**

 Get the associated Shader of this MaterialInstance

Signature:
```cpp
 Shader* GetShader() const;
```
**Returns:** Shader* Pointer to the associated Shader

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialinstance-set"></a>

#### **`MaterialInstance::Set()`**

 Set the value of a material parameter by name

Signature:
```cpp
 inline void Set(const std::string& paramName, const void* data, size_t size);
```
**Parameters:**
- `paramName`: Name of the material parameter
- `data`: Pointer to the data to set
- `size`: Size of the data in bytes

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-set-2"></a>

#### **`MaterialInstance::Set()`**

 Set the value of a material parameter by index

Signature:
```cpp
 inline void Set(uint16_t paramIndex, const void* data, size_t size);
```
**Parameters:**
- `paramIndex`: Index of the material parameter
- `data`: Pointer to the data to set
- `size`: Size of the data in bytes

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-settexture"></a>

#### **`MaterialInstance::SetTexture()`**

 Set the value of a texture parameter by name

Signature:
```cpp
 inline void SetTexture(const std::string& paramName, bgfx::TextureHandle texture, uint32_t samplerFlags, uint8_t stage);
```
**Parameters:**
- `paramName`: Name of the texture parameter
- `texture`: Handle to the texture to set
- `samplerFlags`: Sampler flags for the texture
- `stage`: Stage of the texture parameter

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-settexture-2"></a>

#### **`MaterialInstance::SetTexture()`**

 Set the value of a texture parameter by index

Signature:
```cpp
 inline void SetTexture(uint16_t paramIndex, bgfx::TextureHandle texture, uint32_t samplerFlags, uint8_t stage);
```
**Parameters:**
- `paramIndex`: Index of the texture parameter
- `texture`: Handle to the texture to set
- `samplerFlags`: Sampler flags for the texture
- `stage`: Stage of the texture parameter

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-destroy"></a>

#### **`MaterialInstance::Destroy()`**

 Destroy the material instance and release any associated resources

Signature:
```cpp
 inline void Destroy();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-inline"></a>

#### **`MaterialInstance::inline()`**

 Get the value of a material parameter by name

Signature:
```cpp
 template <typename T> inline T Get(const std::string& paramName) const = delete;
```
**Template Parameters:**
- `T`: Type of the material parameter (e.g., Math::Vector4, Math::Mat4)

**Parameters:**
- `paramName`: Name of the material parameter

**Returns:** T Value of the material parameter, or default value if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialinstance-material-createinstance"></a>

#### **`MaterialInstance::Material::CreateInstance()`**

 Create a MaterialInstance from this Material

Signature:
```cpp
inline MaterialInstance Material::CreateInstance() const;
```
**Returns:** MaterialInstance A new MaterialInstance with default values

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialinstance-material-clone"></a>

#### **`MaterialInstance::Material::Clone()`**

 Clone this Material into a new MaterialInstance

Signature:
```cpp
inline MaterialInstance Material::Clone() const;
```
**Returns:** MaterialInstance A new MaterialInstance with default values

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="materialmanager-getmaterialfromfile"></a>

#### **`MaterialManager::GetMaterialFromFile()`**

 Get a material by its name

Signature:
```cpp
 static Material& GetMaterialFromFile(const std::string& filePath);
```
**Parameters:**
- `name`: Name of the material to get

**Returns:** Material& Reference to the material with the given name or a dummy material if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialmanager-getmaterialfrombundle"></a>

#### **`MaterialManager::GetMaterialFromBundle()`**

 Get a material from a bundle by its name

Signature:
```cpp
 static Material& GetMaterialFromBundle(const std::string& bundlePath, const std::string& materialName);
```
**Parameters:**
- `bundlePath`: Path to the bundle
- `materialName`: Name of the material to get

**Returns:** Material& Reference to the material with the given name or a dummy material if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialmanager-getdefaultmaterialpbr"></a>

#### **`MaterialManager::GetDefaultMaterialPBR()`**

 Get the default PBR material

Signature:
```cpp
 static Material& GetDefaultMaterialPBR();
```
**Returns:** Material& Reference to the default PBR material

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialmanager-getfallbackalbedotexture"></a>

#### **`MaterialManager::GetFallbackAlbedoTexture()`**

 Get the fallback albedo texture

Signature:
```cpp
 static bgfx::TextureHandle GetFallbackAlbedoTexture();
```
**Returns:** bgfx::TextureHandle Handle to the fallback albedo texture

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialmanager-getfallbacknormaltexture"></a>

#### **`MaterialManager::GetFallbackNormalTexture()`**

 Get the fallback normal texture

Signature:
```cpp
 static bgfx::TextureHandle GetFallbackNormalTexture();
```
**Returns:** bgfx::TextureHandle Handle to the fallback normal texture

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialmanager-materialexists"></a>

#### **`MaterialManager::MaterialExists()`**

 Check if a material with the given name exists

Signature:
```cpp
 static bool MaterialExists(const std::string& name);
```
**Parameters:**
- `name`: Name of the material to check

**Returns:** true if the material exists, false otherwise

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialmanager-getmaterialbyname"></a>

#### **`MaterialManager::GetMaterialByName()`**

 Get a material by its name

Signature:
```cpp
 static Material& GetMaterialByName(const std::string& name);
```
**Parameters:**
- `name`: Name of the material to get

**Returns:** Material& Reference to the material with the given name or a dummy material if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="materialmanager-creatematerial"></a>

#### **`MaterialManager::CreateMaterial()`**

 Create a new material with the given name and shader

Signature:
```cpp
 static Material& CreateMaterial(const std::string& name, Shader* shader);
```
**Parameters:**
- `name`: Name of the material to create
- `shader`: Pointer to the shader to associate with the material

**Returns:** Material& Reference to the newly created material

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `const` | `std` | Name of the material |
| `Shader*` | `const` | Pointer to the associated shader |
---
