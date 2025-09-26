# Syngine API Documentation


## CameraComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/CameraComponent.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [yaw](#syngineyaw)
- [GetComponentType](#synginegetcomponenttype)
- [Init](#syngineinit)
- [Update](#syngineupdate)
- [SetPosition](#synginesetposition)
- [GetPosition](#synginegetposition)
- [SetFOV](#synginesetfov)
- [GetFOV](#synginegetfov)
- [SetFarPlane](#synginesetfarplane)
- [GetFarPlane](#synginegetfarplane)
- [SetAngles](#synginesetangles)
- [GetAngles](#synginegetangles)

---

## Class Constructor


#### **`Syngine::CameraComponent`**


 Constructor for the CameraComponent class

#### This function is internal use only and not intended for public use!


**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 CameraComponent(GameObject* owner);
```

**Parameters:**

- `static`: constexpr Syngine::Components componentType Camera component type
- `owner`: Pointer to the GameObject that owns this component

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::yaw`**


 Struct to hold camera data

Signature:

```cpp
 float yaw = 0.0f; //* Yaw angle in radians
```

**Parameters:**

- `float`: eye[3] = {0.0f, 0.0f, -5.0f}; //* Camera position in world space Camera position in world space
- `float`: target[3] = {0.0f, 0.0f, 0.0f}; //* Camera target position Camera target position
- `float`: up[3] = {0.0f, 1.0f, 0.0f}; //* Camera up vector Camera up vector
- `float`: fov Field of view in degrees
- `float`: nearPlane Near clipping plane
- `float`: farPlane Far clipping plane
- `float`: view[16]; //* View matrix View matrix
- `float`: proj[16]; //* Projection matrix Projection matrix

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetComponentType`**


 Get the type of this component

Signature:

```cpp
 Syngine::Components GetComponentType() override;
```

**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::Init`**


 Initialize the camera component

**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init() {}; // No specific initialization needed
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


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

#### **`Syngine::SetPosition`**


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

#### **`Syngine::GetPosition`**


 Get the camera position

Signature:

```cpp
 const float* GetPosition() const;
```

**Returns:** Array of 3 floats representing the camera position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetFOV`**


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

#### **`Syngine::GetFOV`**


 Get the camera FOV

Signature:

```cpp
 float GetFOV() const;
```

**Returns:** Field of view in degrees

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetFarPlane`**


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

#### **`Syngine::GetFarPlane`**


 Get the camera far clipping plane

Signature:

```cpp
 float GetFarPlane() const;
```

**Returns:** Far clipping plane distance

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetAngles`**


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

#### **`Syngine::GetAngles`**


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

