# Syngine API Documentation

## TransformComponent.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/TransformComponent.h)

Component to handle position, rotation, and scale of a GameObject. @section TransformComponent

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [TransformComponent(GameObject* owner)](#transformcomponent-transformcomponentgameobject-owner)

### Functions: 

- [Clone()](#transformcomponent-clone)
- [Serialize()](#transformcomponent-serialize)
- [Init()](#transformcomponent-init)
- [Update()](#transformcomponent-update)
- [GetComponentType()](#transformcomponent-getcomponenttype)
- [GetWorldRotationEuler()](#transformcomponent-getworldrotationeuler)
- [GetWorldRotationQuaternion()](#transformcomponent-getworldrotationquaternion)
- [GetModelMatrix()](#transformcomponent-getmodelmatrix)
- [GetLocalMatrix()](#transformcomponent-getlocalmatrix)
- [GetWorldPosition()](#transformcomponent-getworldposition)
- [GetWorldScale()](#transformcomponent-getworldscale)
- [GetPosition()](#transformcomponent-getposition)
- [GetRotationQuaternion()](#transformcomponent-getrotationquaternion)
- [GetRotationEuler()](#transformcomponent-getrotationeuler)
- [GetScale()](#transformcomponent-getscale)
- [SetPosition()](#transformcomponent-setposition)
- [SetRotationEuler()](#transformcomponent-setrotationeuler)
- [SetRotationQuat()](#transformcomponent-setrotationquat)
- [SetScale()](#transformcomponent-setscale)
- [SetWorldPosition()](#transformcomponent-setworldposition)
- [SetWorldRotationQuat()](#transformcomponent-setworldrotationquat)
- [SetWorldRotationEuler()](#transformcomponent-setworldrotationeuler)
- [SetWorldScale()](#transformcomponent-setworldscale)
- [SetParent()](#transformcomponent-setparent)
- [GetParent()](#transformcomponent-getparent)
- [GetChildren()](#transformcomponent-getchildren)
- [GetVersion()](#transformcomponent-getversion)

---
<a id="transformcomponent-transformcomponentgameobject-owner"></a>

## Class Constructor

#### **`TransformComponent(GameObject* owner)`**

 Constructor for the TransformComponent class

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:
```cpp
 TransformComponent(GameObject* owner);
```
**Parameters:**
- `owner`: Pointer to the GameObject that owns this component

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-clone"></a>

#### **`TransformComponent::Clone()`**

 Clone the TransformComponent

Signature:
```cpp
 std::unique_ptr<Component> Clone() const override;
```
**Returns:** A unique pointer to the cloned TransformComponent

---
<a id="transformcomponent-serialize"></a>

#### **`TransformComponent::Serialize()`**

 Serializes the TransformComponent to a data node

Signature:
```cpp
 Serializer::DataNode Serialize() const override;
```
**Returns:** A pointer to the serialized data node representing the TransformComponent's state

---
<a id="transformcomponent-init"></a>

#### **`TransformComponent::Init()`**

 Initializes the transform component with default values.

**Note:** This is an entirely optional step, as the transform component can and is initialized directly in the constructor.

Signature:
```cpp
 void Init(Vector3 position = Vector3(), Quaternion rotation = Quaternion(), Vector3 scale = Vector3(1.0f));
```
**Parameters:**
- `position`: Initial position of the GameObject
- `rotation`: Initial rotation of the GameObject (as a quaternion)
- `scale`: Initial scale of the GameObject

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-update"></a>

#### **`TransformComponent::Update()`**

 Update the transform component.

**Note:** There is no specific update logic for the transform component.

Signature:
```cpp
 void Update();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getcomponenttype"></a>

#### **`TransformComponent::GetComponentType()`**

 Get the type of this component

Signature:
```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```
**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getworldrotationeuler"></a>

#### **`TransformComponent::GetWorldRotationEuler()`**

 Get the GLOBAL rotation of the transform as XYZ Euler angles (in radians)

Signature:
```cpp
 Vector3 GetWorldRotationEuler() const;
```
**Returns:** Vector3 representing the rotation (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getworldrotationquaternion"></a>

#### **`TransformComponent::GetWorldRotationQuaternion()`**

 Get the GLOBAL rotation of the transform as a quaternion

Signature:
```cpp
 Quaternion GetWorldRotationQuaternion() const;
```
**Returns:** Quaternion representing the rotation (x, y, z, w)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getmodelmatrix"></a>

#### **`TransformComponent::GetModelMatrix()`**

 Get a GLOBAL model matrix for the transform

**Note:** This matrix is computed from the GLOBAL position, rotation, and scale

Signature:
```cpp
 Mat4 GetModelMatrix() const;
```
**Returns:** 4x4 matrix filled with the global model matrix

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getlocalmatrix"></a>

#### **`TransformComponent::GetLocalMatrix()`**

 Get a LOCAL model matrix for the transform

**Note:** This matrix is computed from the LOCAL position, rotation, and scale

Signature:
```cpp
 Mat4 GetLocalMatrix() const;
```
**Returns:** 4x4 matrix filled with the local model matrix

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getworldposition"></a>

#### **`TransformComponent::GetWorldPosition()`**

 Get the GLOBAL position of the transform

**Note:** GLOBAL position

Signature:
```cpp
 Vector3 GetWorldPosition() const;
```
**Returns:** Vector3 representing the position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getworldscale"></a>

#### **`TransformComponent::GetWorldScale()`**

 Get the GLOBAL scale of the transform

**Note:** GLOBAL scale

Signature:
```cpp
 Vector3 GetWorldScale() const;
```
**Returns:** Vector3 representing the scale (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getposition"></a>

#### **`TransformComponent::GetPosition()`**

 Get the LOCAL position of the transform

**Note:** LOCAL position

Signature:
```cpp
 Vector3 GetPosition() const;
```
**Returns:** Vector3 representing the position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getrotationquaternion"></a>

#### **`TransformComponent::GetRotationQuaternion()`**

 Get the LOCAL rotation of the transform as a quaternion

**Note:** LOCAL rotation

Signature:
```cpp
 Quaternion GetRotationQuaternion() const;
```
**Returns:** Quaternion representing the rotation (x, y, z, w)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getrotationeuler"></a>

#### **`TransformComponent::GetRotationEuler()`**

 Get the LOCAL rotation of the transform as XYZ Euler angles (in radians)

**Note:** LOCAL rotation

Signature:
```cpp
 Vector3 GetRotationEuler() const;
```
**Returns:** Vector3 representing the rotation (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getscale"></a>

#### **`TransformComponent::GetScale()`**

 Get the LOCAL scale of the transform

Signature:
```cpp
 Vector3 GetScale() const;
```
**Returns:** Vector3 representing the local scale (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setposition"></a>

#### **`TransformComponent::SetPosition()`**

 Set the LOCAL position of the transform

Signature:
```cpp
 void SetPosition(Vector3 position);
```
**Parameters:**
- `position`: New local position as a Vector3

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setrotationeuler"></a>

#### **`TransformComponent::SetRotationEuler()`**

 Set the LOCAL rotation of the transform as XYZ Euler angles (in radians)

Signature:
```cpp
 void SetRotationEuler(Vector3 rotation);
```
**Parameters:**
- `rotation`: New local rotation as XYZ Euler angles (in radians)

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setrotationquat"></a>

#### **`TransformComponent::SetRotationQuat()`**

 Set the LOCAL rotation of the transform as a quaternion

Signature:
```cpp
 void SetRotationQuat(Quaternion rotation);
```
**Parameters:**
- `rotation`: New local rotation as a quaternion

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setscale"></a>

#### **`TransformComponent::SetScale()`**

 Set the LOCAL scale of the transform

Signature:
```cpp
 void SetScale(Vector3 scale);
```
**Parameters:**
- `scale`: New local scale as a Vector3

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setworldposition"></a>

#### **`TransformComponent::SetWorldPosition()`**

 Set the GLOBAL position of the transform

Signature:
```cpp
 void SetWorldPosition(Vector3 position);
```
**Parameters:**
- `position`: New global position as a Vector3

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setworldrotationquat"></a>

#### **`TransformComponent::SetWorldRotationQuat()`**

 Set the GLOBAL rotation of the transform as a quaternion

Signature:
```cpp
 void SetWorldRotationQuat(Quaternion rotation);
```
**Parameters:**
- `rotation`: New global rotation as a quaternion

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setworldrotationeuler"></a>

#### **`TransformComponent::SetWorldRotationEuler()`**

 Set the GLOBAL rotation of the transform as XYZ Euler angles (in radians)

Signature:
```cpp
 void SetWorldRotationEuler(Vector3 rotation);
```
**Parameters:**
- `rotation`: New global rotation as XYZ Euler angles (in radians)

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setworldscale"></a>

#### **`TransformComponent::SetWorldScale()`**

 Set the GLOBAL scale of the transform

Signature:
```cpp
 void SetWorldScale(Vector3 scale);
```
**Parameters:**
- `scale`: New global scale as a Vector3

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-setparent"></a>

#### **`TransformComponent::SetParent()`**

 Set the parent transform of this transform

**Note:** This will automatically update the child list of the parent

Signature:
```cpp
 void SetParent(TransformComponent* parent);
```
**Parameters:**
- `parent`: Pointer to the parent TransformComponent

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getparent"></a>

#### **`TransformComponent::GetParent()`**

 Get the parent transform of this transform

Signature:
```cpp
 TransformComponent* GetParent() const;
```
**Returns:** Pointer to the parent TransformComponent (or nullptr if none)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getchildren"></a>

#### **`TransformComponent::GetChildren()`**

 Get the child transforms of this transform

Signature:
```cpp
 std::vector<TransformComponent*>& GetChildren() const;
```
**Returns:** Vector of pointers to the child TransformComponents

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="transformcomponent-getversion"></a>

#### **`TransformComponent::GetVersion()`**

 Get the version number of this transform

Signature:
```cpp
 uint64_t GetVersion() const;
```
**Returns:** Version number that increments on each change

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `Vector3` | `m_position` | Position of the transform |
| `Quaternion` | `m_rotation` | Rotation of the transform (Quaternion) |
| `Vector3` | `m_scale` | Scale of the transform |
| `std::vector<TransformComponent*>` | `m_children` | Child transform components |
---
