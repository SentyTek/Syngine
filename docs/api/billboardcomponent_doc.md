# Syngine API Documentation

## BillboardComponent.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/BillboardComponent.h)

Syngine Billboard Component. The BillboardComponent is used to represent a 2D model in the 3D game world. @section BillboardComponent

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [BillboardComponent(GameObject* owner, std::string texturePath, BillboardMode mode = BillboardMode::CAMERA_ALIGNED, float size = 1.0f)](#billboardcomponent-billboardcomponentgameobject-owner-std-string-texturepath-billboardmode-mode-billboardmode-camera_aligned-float-size-10f)
- [BillboardComponent(GameObject* owner, std::string bundlePath, std::string textureName, BillboardMode mode = BillboardMode::CAMERA_ALIGNED, float size = 1.0f)](#billboardcomponent-billboardcomponentgameobject-owner-std-string-bundlepath-std-string-texturename-billboardmode-mode-billboardmode-camera_aligned-float-size-10f)

### Enums and Structs: 

- [BillboardMode](#syngine-billboardmode)

### Functions: 

- [GetComponentType()](#billboardcomponent-getcomponenttype)
- [Serialize()](#billboardcomponent-serialize)
- [Init()](#billboardcomponent-init)
- [Update()](#billboardcomponent-update)
- [_GetTexture()](#billboardcomponent-_gettexture)
- [SetRotX()](#billboardcomponent-setrotx)
- [SetRotY()](#billboardcomponent-setroty)
- [SetRotZ()](#billboardcomponent-setrotz)
- [SetRot()](#billboardcomponent-setrot)
- [GetRot()](#billboardcomponent-getrot)
- [GetMode()](#billboardcomponent-getmode)
- [GetMinBounds()](#billboardcomponent-getminbounds)
- [GetMaxBounds()](#billboardcomponent-getmaxbounds)

---
<a id="syngine-billboardmode"></a>

#### **`Syngine::BillboardMode()`**

 Enum that describes how a billboard should render

Signature:
```cpp
enum class BillboardMode
```
**Members:**
| Name | Description |
| --- | --- | 
| `CAMERA_ALIGNED` | Always faces the camera |
| `AXIS_Y_ALIGNED` | Rotates around Y (vertical) axis only |
| `FIXED` | No rotation, fixed orientation |
| `COUNT` | Number of billboard modes (for validation) |
**This function has been available since:** v0.0.1

---
<a id="billboardcomponent-billboardcomponentgameobject-owner-std-string-texturepath-billboardmode-mode-billboardmode-camera_aligned-float-size-10f"></a>

## Class Constructor

#### **`BillboardComponent(GameObject* owner, std::string texturePath, BillboardMode mode = BillboardMode::CAMERA_ALIGNED, float size = 1.0f)`**

 Construct a new Billboard Component object

**Note:** The texturePath should be a valid path to a texture file, relative to the game's assets directory. owner GO handled by the ECS.

Signature:
```cpp
 BillboardComponent(GameObject* owner, std::string texturePath, BillboardMode mode = BillboardMode::CAMERA_ALIGNED, float size = 1.0f);
```
**Parameters:**
- `owner`: Pointer to the owner GameObject
- `texturePath`: Path to the billboard texture
- `mode`: Billboard rendering mode
- `size`: Size of the billboard

**This function has been available since:** v0.0.1

---
<a id="billboardcomponent-billboardcomponentgameobject-owner-std-string-bundlepath-std-string-texturename-billboardmode-mode-billboardmode-camera_aligned-float-size-10f"></a>

## Class Constructor

#### **`BillboardComponent(GameObject* owner, std::string bundlePath, std::string textureName, BillboardMode mode = BillboardMode::CAMERA_ALIGNED, float size = 1.0f)`**

 Construct a new Billboard Component objects

**Note:** The bundlePath should be a valid path to a shader bundle file, relative to the game's assets directory. The textureName should be the name of a texture contained within that shader bundle. owner GO handled by the ECS.

Signature:
```cpp
 BillboardComponent(GameObject* owner, std::string bundlePath, std::string textureName, BillboardMode mode = BillboardMode::CAMERA_ALIGNED, float size = 1.0f);
```
**Parameters:**
- `owner`: Pointer to the owner GameObject
- `bundlePath`: Path to the shader bundle containing the billboard texture
- `textureName`: Name of the texture within the shader bundle
- `mode`: Billboard rendering mode
- `size`: Size of the billboard

**This function has been available since:** v0.0.1

---
<a id="billboardcomponent-getcomponenttype"></a>

#### **`myBillboard.GetComponentType()`**

 Get the type of this component

Signature:
```cpp
 ComponentTypeID GetComponentType() override;
```
**Returns:** The component type as an enum value

---
<a id="billboardcomponent-serialize"></a>

#### **`myBillboard.Serialize()`**

 Serializes the BillboardComponent to a data node

Signature:
```cpp
 Serializer::DataNode Serialize() const override;
```
**Returns:** A pointer to the serialized data node representing the BillboardComponent's state

---
<a id="billboardcomponent-init"></a>

#### **`myBillboard.Init()`**

 Initialize the billboard component

#### This function is internal use only and not intended for public use!

**Note:** This should only be called when the component is added to a GameObject

Signature:
```cpp
 void Init(const std::string& bundlePath, const std::string& texturePath);
```
**Parameters:**
- `bundlePath`: Path to the shader bundle containing the billboard texture
- `texturePath`: Path to the billboard texture

**This function has been available since:** v0.0.1

---
<a id="billboardcomponent-update"></a>

#### **`myBillboard.Update()`**

 Update the billboard component. Unused.

Signature:
```cpp
 void Update(float deltaTime) override;
```
**Parameters:**
- `deltaTime`: Time elapsed since the last update, in seconds

**This function has been available since:** v0.0.1

---
<a id="billboardcomponent-_gettexture"></a>

#### **`myBillboard._GetTexture()`**

 Get the texture handle of the billboard

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 bgfx::TextureHandle _GetTexture() const;
```
**Returns:** bgfx::TextureHandle The texture handle

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="billboardcomponent-setrotx"></a>

#### **`myBillboard.SetRotX()`**

 Set rotation around X, Y, Z axes

Signature:
```cpp
 void SetRotX(float rotX);
```
**Parameters:**
- `rot`: Rotation around X axis in radians

---
<a id="billboardcomponent-setroty"></a>

#### **`myBillboard.SetRotY()`**

 Set rotation around X, Y, Z axes

Signature:
```cpp
 void SetRotY(float rotY);
```
**Parameters:**
- `rotY`: Rotation around Y axis in radians

---
<a id="billboardcomponent-setrotz"></a>

#### **`myBillboard.SetRotZ()`**

 Set rotation around X, Y, Z axes

Signature:
```cpp
 void SetRotZ(float rotZ);
```
**Parameters:**
- `rotZ`: Rotation around Z axis in radians

---
<a id="billboardcomponent-setrot"></a>

#### **`myBillboard.SetRot()`**

 Set rotation around X, Y, Z axes

Signature:
```cpp
 void SetRot(const Vector3& rot);
```
**Parameters:**
- `rot`: The rotation vector containing rotation around X, Y, Z axes in radians

---
<a id="billboardcomponent-getrot"></a>

#### **`myBillboard.GetRot()`**

 Get rotation around X, Y, Z axes

Signature:
```cpp
 Vector3 GetRot() const;
```
**Returns:** Vector3 The rotation vector containing rotation around X, Y, Z axes in radians

---
<a id="billboardcomponent-getmode"></a>

#### **`myBillboard.GetMode()`**

 Get the billboard mode

Signature:
```cpp
 BillboardMode GetMode() const;
```
**Returns:** BillboardMode The billboard rendering mode

---
<a id="billboardcomponent-getminbounds"></a>

#### **`myBillboard.GetMinBounds()`**

 Get the min bounds of the billboard's bounding box

Signature:
```cpp
 inline Vector3 GetMinBounds() const;
```
**Returns:** Vector3 The minimum corner of the bounding box

---
<a id="billboardcomponent-getmaxbounds"></a>

#### **`myBillboard.GetMaxBounds()`**

 Get the max bounds of the billboard's bounding box

Signature:
```cpp
 inline Vector3 GetMaxBounds() const;
```
**Returns:** Vector3 The maximum corner of the bounding box

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `BillboardMode` | `m_mode` | Billboard rendering mode |
| `std::string` | `m_texturePath` | Path to the billboard texture |
| `std::string` | `m_bundlePath` | Path to the shader bundle containing the billboard texture |
| `Vector3` | `m_rot` | Rotation around X, Y, Z axes in radians |
| `float` | `size` | Size of the billboard |
| `bool` | `receiveShadows` | Whether the billboard receives shadows |
| `bool` | `receiveSunLight` | Whether the billboard receives sunlight |
---
