# Syngine API Documentation

## CameraComponent.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/CameraComponent.h)

CameraComponent class for managing camera functionality in a game object @section CameraComponent

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

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

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [CameraComponent(GameObject* owner)](#cameracomponent-cameracomponentgameobject-owner)

### Enums and Structs: 

- [Camera](#syngine-camera)
- [Plane](#cameracomponent-plane)
- [Frustum](#cameracomponent-frustum)

### Functions: 

- [GetComponentType()](#cameracomponent-getcomponenttype)
- [Clone()](#cameracomponent-clone)
- [Serialize()](#cameracomponent-serialize)
- [Init()](#cameracomponent-init)
- [Update()](#cameracomponent-update)
- [SetPosition()](#cameracomponent-setposition)
- [GetPosition()](#cameracomponent-getposition)
- [SetFOV()](#cameracomponent-setfov)
- [GetFOV()](#cameracomponent-getfov)
- [SetFarPlane()](#cameracomponent-setfarplane)
- [GetFarPlane()](#cameracomponent-getfarplane)
- [SetAngles()](#cameracomponent-setangles)
- [SetAngles()](#cameracomponent-setangles-2)
- [GetAngles()](#cameracomponent-getangles)
- [GetCamera()](#cameracomponent-getcamera)

---
<a id="syngine-camera"></a>

#### **`Syngine::Camera()`**

 Struct to hold camera data

Signature:
```cpp
struct Camera
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `Math::Vector3` | `eye` | Camera position in world space |
| `Math::Vector3` | `target` | Camera target position |
| `Math::Vector3` | `up` | Camera up vector |
| `float` | `fov` | Field of view in degrees |
| `float` | `nearPlane` | Near clipping plane |
| `float` | `farPlane` | Far clipping plane |
| `Math::Matrix4x4` | `view` | View matrix |
| `Math::Matrix4x4` | `proj` | Projection matrix |
| `float` | `yaw` | Yaw angle in radians |
| `float` | `pitch` | Pitch angle in radians |
| `float` | `roll` | Roll angle in radians |
**This function has been available since:** v0.0.1

---
<a id="cameracomponent-cameracomponentgameobject-owner"></a>

## Class Constructor

#### **`CameraComponent(GameObject* owner)`**

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
<a id="cameracomponent-getcomponenttype"></a>

#### **`CameraComponent::GetComponentType()`**

 Get the type of this component

Signature:
```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```
**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-clone"></a>

#### **`CameraComponent::Clone()`**

 Clone the CameraComponent

Signature:
```cpp
 std::unique_ptr<Component> Clone() const override;
```
**Returns:** A unique pointer to the cloned CameraComponent

---
<a id="cameracomponent-serialize"></a>

#### **`CameraComponent::Serialize()`**

 Serializes the CameraComponent to a data node

Signature:
```cpp
 Serializer::DataNode Serialize() const override;
```
**Returns:** A pointer to the serialized data node representing the CameraComponent's state

---
<a id="cameracomponent-init"></a>

#### **`CameraComponent::Init()`**

 Initialize the camera component

**Note:** This should only be called when the component is added to a GameObject

Signature:
```cpp
 void Init() override;
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-update"></a>

#### **`CameraComponent::Update()`**

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
<a id="cameracomponent-setposition"></a>

#### **`CameraComponent::SetPosition()`**

 Set the camera position

Signature:
```cpp
 void SetPosition(Math::Vector3 position);
```
**Parameters:**
- `position`: Vector 3 representing the new camera position (x, y, z)

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-getposition"></a>

#### **`CameraComponent::GetPosition()`**

 Get the camera position

Signature:
```cpp
 Math::Vector3 GetPosition() const;
```
**Returns:** Vector 3 representing the camera position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-setfov"></a>

#### **`CameraComponent::SetFOV()`**

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
<a id="cameracomponent-getfov"></a>

#### **`CameraComponent::GetFOV()`**

 Get the camera FOV

Signature:
```cpp
 float GetFOV() const;
```
**Returns:** Field of view in degrees

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-setfarplane"></a>

#### **`CameraComponent::SetFarPlane()`**

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
<a id="cameracomponent-getfarplane"></a>

#### **`CameraComponent::GetFarPlane()`**

 Get the camera far clipping plane

Signature:
```cpp
 float GetFarPlane() const;
```
**Returns:** Far clipping plane distance

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-setangles"></a>

#### **`CameraComponent::SetAngles()`**

 Set the camera angles

**Note:** The angles are used to calculate the camera's orientation

Signature:
```cpp
 void SetAngles(float yaw, float pitch);
```
**Parameters:**
- `yaw`: Yaw angle in radians
- `pitch`: Pitch angle in radians

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-setangles-2"></a>

#### **`CameraComponent::SetAngles()`**

 Set the camera angles

**Note:** The angles are used to calculate the camera's orientation

Signature:
```cpp
 void SetAngles(Math::Vector2 angles);
```
**Parameters:**
- `angles`: Vector2 containing yaw and pitch angles in radians

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-getangles"></a>

#### **`CameraComponent::GetAngles()`**

 Get the camera angles

Signature:
```cpp
 Math::Vector2 GetAngles() const;
```
**Returns:** Vector2 containing yaw and pitch angles in radians

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-getcamera"></a>

#### **`CameraComponent::GetCamera()`**

 Get the camera

Signature:
```cpp
 Syngine::Camera GetCamera() const;
```
**Returns:** Camera struct containing the camera's position, target, up vector, FOV, near and far planes, view and projection matrices, and angles

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="cameracomponent-plane"></a>

#### **`CameraComponent::Plane()`**

 Structure representing a plane in 3D space

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 struct Plane
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `Math::Vector3` | `normal` | Normal vector of the plane |
| `float` | `distance` | Distance from origin |
**This function has been available since:** v0.0.1

---
<a id="cameracomponent-frustum"></a>

#### **`CameraComponent::Frustum()`**

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
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `constexpr` | `Syngine` | Camera component type |
---
