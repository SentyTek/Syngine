# Syngine API Documentation


## CameraComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/CameraComponent.h)

---

## Goto:


- [Constructor](#class-constructor)
- [Camera](#synginecamera)
- [static](#cameracomponentstatic)
- [GetComponentType](#cameracomponentgetcomponenttype)
- [Clone](#cameracomponentclone)
- [Serialize](#cameracomponentserialize)
- [Init](#cameracomponentinit)
- [Update](#cameracomponentupdate)
- [SetPosition](#cameracomponentsetposition)
- [GetPosition](#cameracomponentgetposition)
- [SetFOV](#cameracomponentsetfov)
- [GetFOV](#cameracomponentgetfov)
- [SetFarPlane](#cameracomponentsetfarplane)
- [GetFarPlane](#cameracomponentgetfarplane)
- [SetAngles](#cameracomponentsetangles)
- [GetAngles](#cameracomponentgetangles)
- [Plane](#cameracomponentplane)
- [Frustum](#cameracomponentfrustum)

---

## Class Constructor


#### **`CameraComponent::CameraComponent`**


 Constructor for the CameraComponent class

#### This function is internal use only and not intended for public use!


**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 CameraComponent(GameObject* owner);
```

**Parameters:**

- `owner`: Pointer to the GameObject that owns this component

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::Camera`**


 Struct to hold camera data

Signature:

```cpp
struct Camera
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `float[3]` | `eye` | Camera position in world space |
| `float[3]` | `target` | Camera target position |
| `float[3]` | `up` | Camera up vector |
| `float` | `fov` | Field of view in degrees |
| `float` | `nearPlane` | Near clipping plane |
| `float` | `farPlane` | Far clipping plane |
| `float[16]` | `view` | View matrix |
| `float[16]` | `proj` | Projection matrix |
| `float` | `yaw` | Yaw angle in radians |
| `float` | `pitch` | Pitch angle in radians |
| `float` | `roll` | Roll angle in radians |

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::static`**


 CameraComponent class for managing camera functionality in a game object

Signature:

```cpp
 public: static constexpr Syngine::ComponentTypeID componentType = Syngine::SYN_COMPONENT_CAMERA; //* Camera component type
```

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::GetComponentType`**


 Get the type of this component

Signature:

```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```

**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::Clone`**


 Clone the CameraComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override;
```

**Returns:** A unique pointer to the cloned CameraComponent

---

#### **`CameraComponent::Serialize`**


 Serializes the CameraComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the CameraComponent's state

---

#### **`CameraComponent::Init`**


 Initialize the camera component

**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init() override;
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::Update`**


 Update the camera component

#### This function is internal use only and not intended for public use!


**Note:** This is called every frame to update the camera's view and projection matrices

Signature:

```cpp
 void Update(int viewId, int width, int height);
```

**Parameters:**

- `viewId`: ID of the view to update
- `width`: Width of the viewport
- `height`: Height of the viewport

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::SetPosition`**


 Set the camera position

Signature:

```cpp
 void SetPosition(float x, float y, float z);
```

**Parameters:**

- `x`: X coordinate of the camera position
- `y`: Y coordinate of the camera position
- `z`: Z coordinate of the camera position

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::GetPosition`**


 Get the camera position

Signature:

```cpp
 const float* GetPosition() const;
```

**Returns:** Array of 3 floats representing the camera position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::SetFOV`**


 Set the camera FOV

Signature:

```cpp
 void SetFOV(float fov);
```

**Parameters:**

- `fov`: Field of view in degrees

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::GetFOV`**


 Get the camera FOV

Signature:

```cpp
 float GetFOV() const;
```

**Returns:** Field of view in degrees

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::SetFarPlane`**


 Set the camera far clipping plane

Signature:

```cpp
 void SetFarPlane(float farPlane);
```

**Parameters:**

- `farPlane`: Far clipping plane distance

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::GetFarPlane`**


 Get the camera far clipping plane

Signature:

```cpp
 float GetFarPlane() const;
```

**Returns:** Far clipping plane distance

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::SetAngles`**


 Set the camera near clipping plane

Signature:

```cpp
 void SetAngles(float yaw, float pitch);
```

**Parameters:**

- `nearPlane`: Near clipping plane distance

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::GetAngles`**


 Get the camera angles

Signature:

```cpp
 void GetAngles(float& yaw, float& pitch) const;
```

**Parameters:**

- `yaw`: Reference to store the yaw angle in radians
- `pitch`: Reference to store the pitch angle in radians

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::Plane`**


 Structure representing a plane in 3D space

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 struct Plane
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `float[3]` | `normal` | Normal vector of the plane |
| `float` | `distance` | Distance from origin |

**This function has been available since:** v0.0.1

---

#### **`CameraComponent::Frustum`**


 Structure representing a frustum for view culling

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 struct Frustum
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `Plane` | `top` | Top plane |
| `Plane` | `bottom` | Bottom plane |
| `Plane` | `left` | Left plane |
| `Plane` | `right` | Right plane |
| `Plane` | `n` | Near plane |
| `Plane` | `f` | Far plane |

**This function has been available since:** v0.0.1

---
