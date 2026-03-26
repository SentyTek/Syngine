# Syngine API Documentation


## TransformComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/TransformComponent.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [Clone](#syngineclone)
- [Serialize](#syngineserialize)
- [Init](#syngineinit)
- [Update](#syngineupdate)
- [GetComponentType](#synginegetcomponenttype)
- [GetRotationEuler](#synginegetrotationeuler)
- [GetRotationQuaternion](#synginegetrotationquaternion)
- [GetModelMatrix](#synginegetmodelmatrix)
- [GetLocalMatrix](#synginegetlocalmatrix)
- [GetPosition](#synginegetposition)
- [GetScale](#synginegetscale)
- [GetLocalPosition](#synginegetlocalposition)
- [GetLocalRotation](#synginegetlocalrotation)
- [GetLocalScale](#synginegetlocalscale)
- [SetPosition](#synginesetposition)
- [SetRotationEuler](#synginesetrotationeuler)
- [SetRotationQuat](#synginesetrotationquat)
- [SetScale](#synginesetscale)
- [SetWorldPosition](#synginesetworldposition)
- [SetWorldRotationQuat](#synginesetworldrotationquat)
- [SetWorldRotationEuler](#synginesetworldrotationeuler)
- [SetWorldScale](#synginesetworldscale)
- [SetParent](#synginesetparent)
- [GetParent](#synginegetparent)
- [GetChildren](#synginegetchildren)
- [GetVersion](#synginegetversion)

---

## Class Constructor


#### **`Syngine::TransformComponent`**


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

## Class & Related Members


#### **`Syngine::Clone`**


 Clone the TransformComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override { return std::make_unique<TransformComponent>(*this);
```

**Returns:** A unique pointer to the cloned TransformComponent

---

#### **`Syngine::Serialize`**


 Serializes the TransformComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the TransformComponent's state

---

#### **`Syngine::Init`**


 Initializes the transform component with default values.

**Note:** This is an entirely optional step, as the transform component can and is initialized directly in the constructor.

Signature:

```cpp
 void Init(std::vector<float> position = { 0.0f, 0.0f, 0.0f }, std::vector<float> rotation = { 0.0f, 0.0f, 0.0f, 1.0f },
```

**Parameters:**

- `position`: Initial position of the GameObject
- `rotation`: Initial rotation of the GameObject (as a quaternion)
- `scale`: Initial scale of the GameObject

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


 Update the transform component.

**Note:** There is no specific update logic for the transform component.

Signature:

```cpp
 void Update() {};
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetComponentType`**


 Get the type of this component

Signature:

```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```

**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetRotationEuler`**


 Get the GLOBAL rotation of the transform as XYZ Euler angles (in radians)

Signature:

```cpp
 void GetRotationEuler(float& x, float& y, float& z) const;
```

**Parameters:**

- `x`: X component of the rotation
- `y`: Y component of the rotation
- `z`: Z component of the rotation

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetRotationQuaternion`**


 Get the GLOBAL rotation of the transform as a quaternion

Signature:

```cpp
 void GetRotationQuaternion(float& x, float& y, float& z, float& w) const;
```

**Returns:** Array of 4 floats representing the quaternion (x, y, z, w)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetModelMatrix`**


 Get a GLOBAL model matrix for the transform

Signature:

```cpp
 void GetModelMatrix(float* result);
```

**Parameters:**

- `result`: Array to fill with the model matrix (16 floats)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetLocalMatrix`**


 Get a LOCAL model matrix for the transform

Signature:

```cpp
 void GetLocalMatrix(float* result);
```

**Parameters:**

- `result`: Array to fill with the local model matrix (16 floats)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetPosition`**


 Get the GLOBAL position of the transform

Signature:

```cpp
 float* GetPosition();
```

**Returns:** Array of 3 floats representing the position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetScale`**


 Get the GLOBAL scale of the transform

Signature:

```cpp
 float* GetScale();
```

**Returns:** Array of 3 floats representing the scale (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetLocalPosition`**


 Get the LOCAL position of the transform

Signature:

```cpp
 float* GetLocalPosition();
```

**Returns:** Array of 3 floats representing the local position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetLocalRotation`**


 Get the LOCAL rotation of the transform as a quaternion

Signature:

```cpp
 float* GetLocalRotation();
```

**Returns:** Array of 4 floats representing the local rotation quaternion (x, y, z, w)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetLocalScale`**


 Get the LOCAL scale of the transform

Signature:

```cpp
 float* GetLocalScale();
```

**Returns:** Array of 3 floats representing the local scale (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetPosition`**


 Set the LOCAL position of the transform

Signature:

```cpp
 void SetPosition(float x, float y, float z);
```

**Parameters:**

- `x`: X component of the position
- `y`: Y component of the position
- `z`: Z component of the position

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetRotationEuler`**


 Set the LOCAL rotation of the transform as XYZ Euler angles (in degrees)

Signature:

```cpp
 void SetRotationEuler(float x, float y, float z);
```

**Parameters:**

- `x`: X component of the rotation
- `y`: Y component of the rotation
- `z`: Z component of the rotation

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetRotationQuat`**


 Set the LOCAL rotation of the transform as a quaternion

Signature:

```cpp
 void SetRotationQuat(float x, float y, float z, float w);
```

**Parameters:**

- `x`: X component of the quaternion
- `y`: Y component of the quaternion
- `z`: Z component of the quaternion
- `w`: W component of the quaternion

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetScale`**


 Set the LOCAL scale of the transform

Signature:

```cpp
 void SetScale(float x, float y, float z);
```

**Parameters:**

- `x`: X component of the scale
- `y`: Y component of the scale
- `z`: Z component of the scale

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetWorldPosition`**


 Set the GLOBAL position of the transform

Signature:

```cpp
 void SetWorldPosition(float x, float y, float z);
```

**Parameters:**

- `x`: X component of the position
- `y`: Y component of the position
- `z`: Z component of the position

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetWorldRotationQuat`**


 Set the GLOBAL rotation of the transform as a quaternion

Signature:

```cpp
 void SetWorldRotationQuat(float x, float y, float z, float w);
```

**Parameters:**

- `x`: X component of the quaternion
- `y`: Y component of the quaternion
- `z`: Z component of the quaternion
- `w`: W component of the quaternion

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetWorldRotationEuler`**


 Set the GLOBAL rotation of the transform as XYZ Euler angles (in radians)

Signature:

```cpp
 void SetWorldRotationEuler(float x, float y, float z);
```

**Parameters:**

- `x`: X component of the rotation
- `y`: Y component of the rotation
- `z`: Z component of the rotation

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetWorldScale`**


 Set the GLOBAL scale of the transform

Signature:

```cpp
 void SetWorldScale(float x, float y, float z);
```

**Parameters:**

- `x`: X component of the scale
- `y`: Y component of the scale
- `z`: Z component of the scale

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetParent`**


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

#### **`Syngine::GetParent`**


 Get the parent transform of this transform

Signature:

```cpp
 TransformComponent* GetParent() const;
```

**Returns:** Pointer to the parent TransformComponent (or nullptr if none)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetChildren`**


 Get the child transforms of this transform

Signature:

```cpp
 std::vector<TransformComponent*>& GetChildren() const;
```

**Returns:** Vector of pointers to the child TransformComponents

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetVersion`**


 Get the version number of this transform

Signature:

```cpp
 uint64_t GetVersion() const ;
```

**Returns:** Version number that increments on each change

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `float` | `m_position` | Position of the transform |
| `float` | `m_rotation` | Rotation of the transform (Quaternion) |
| `float` | `m_scale` | Scale of the transform |
| `std::vector<TransformComponent*>` | `m_children` | Child transform components |

---

