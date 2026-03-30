# Syngine API Documentation


## BillboardComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/BillboardComponent.h)

---

## Goto:


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [BillboardMode](#synginebillboardmode)
- [GetComponentType](#synginegetcomponenttype)
- [Serialize](#syngineserialize)
- [Init](#syngineinit)
- [Update](#syngineupdate)
- [_GetTexture](#syngine_gettexture)
- [SetRotX](#synginesetrotx)
- [SetRotY](#synginesetroty)
- [SetRotZ](#synginesetrotz)
- [SetRot](#synginesetrot)
- [GetRot](#synginegetrot)
- [GetMode](#synginegetmode)

---

## Class Constructor


#### **`Syngine::BillboardComponent`**


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

## Class & Related Members


#### **`Syngine::BillboardMode`**


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

#### **`Syngine::GetComponentType`**


 Get the type of this component

Signature:

```cpp
 ComponentTypeID GetComponentType() override;
```

**Returns:** The component type as an enum value

---

#### **`Syngine::Serialize`**


 Serializes the BillboardComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the BillboardComponent's state

---

#### **`Syngine::Init`**


 Initialize the billboard component

#### This function is internal use only and not intended for public use!


**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init(const std::string& texturePath);
```

**Parameters:**

- `texturePath`: Path to the billboard texture

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


 Update the billboard component. Unused.

Signature:

```cpp
 void Update(float deltaTime) override;
```

**Parameters:**

- `deltaTime`: Time elapsed since the last update, in seconds

**This function has been available since:** v0.0.1

---

#### **`Syngine::_GetTexture`**


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

#### **`Syngine::SetRotX`**


 Set rotation around X, Y, Z axes

Signature:

```cpp
 void SetRotX(float rotX);
```

**Parameters:**

- `rot`: Rotation around X axis in radians

---

#### **`Syngine::SetRotY`**


 Set rotation around X, Y, Z axes

Signature:

```cpp
 void SetRotY(float rotY);
```

**Parameters:**

- `rot`: Rotation around X axis in radians

---

#### **`Syngine::SetRotZ`**


 Set rotation around X, Y, Z axes

Signature:

```cpp
 void SetRotZ(float rotZ);
```

**Parameters:**

- `rot`: Rotation around X axis in radians

---

#### **`Syngine::SetRot`**


 Set rotation around X, Y, Z axes

Signature:

```cpp
 void SetRot(float rotX, float rotY, float rotZ);
```

**Parameters:**

- `rotX`: Rotation around X axis in radians
- `rotY`: Rotation around Y axis in radians
- `rotZ`: Rotation around Z axis in radians

---

#### **`Syngine::GetRot`**


 Get rotation around X, Y, Z axes

Signature:

```cpp
 float* GetRot();
```

**Returns:** float* Pointer to array of 3 floats representing rotation

---

#### **`Syngine::GetMode`**


 Get the billboard mode

Signature:

```cpp
 BillboardMode GetMode() const;
```

**Returns:** BillboardMode The billboard rendering mode

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- |
| `std::string` | `m_texturePath` | Path to the billboard texture |
| `float` | `m_rot` | Rotation of the billboard |
| `float` | `size` | Size of the billboard |
| `bool` | `receiveShadows` | Whether the billboard receives shadows |
| `bool` | `receiveSunLight` | Whether the billboard receives sunlight |

---
