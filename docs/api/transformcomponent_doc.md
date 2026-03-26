# Syngine API Documentation


## TransformComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/TransformComponent.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [Clone](#transformcomponentclone)
- [Serialize](#transformcomponentserialize)
- [Init](#transformcomponentinit)
- [Update](#transformcomponentupdate)
- [GetComponentType](#transformcomponentgetcomponenttype)
- [GetRotationEuler](#transformcomponentgetrotationeuler)
- [GetRotationQuaternion](#transformcomponentgetrotationquaternion)
- [GetModelMatrix](#transformcomponentgetmodelmatrix)
- [GetLocalMatrix](#transformcomponentgetlocalmatrix)
- [GetPosition](#transformcomponentgetposition)
- [GetScale](#transformcomponentgetscale)
- [GetLocalPosition](#transformcomponentgetlocalposition)
- [GetLocalRotation](#transformcomponentgetlocalrotation)
- [GetLocalScale](#transformcomponentgetlocalscale)
- [SetPosition](#transformcomponentsetposition)
- [SetRotationEuler](#transformcomponentsetrotationeuler)
- [SetRotationQuat](#transformcomponentsetrotationquat)
- [SetScale](#transformcomponentsetscale)
- [SetWorldPosition](#transformcomponentsetworldposition)
- [SetWorldRotationQuat](#transformcomponentsetworldrotationquat)
- [SetWorldRotationEuler](#transformcomponentsetworldrotationeuler)
- [SetWorldScale](#transformcomponentsetworldscale)
- [SetParent](#transformcomponentsetparent)
- [GetParent](#transformcomponentgetparent)
- [GetChildren](#transformcomponentgetchildren)
- [GetVersion](#transformcomponentgetversion)

---

## Class Constructor


#### **`TransformComponent::TransformComponent`**


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


#### **`TransformComponent::Clone`**


 Clone the TransformComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override;
```

**Returns:** A unique pointer to the cloned TransformComponent

---

#### **`TransformComponent::Serialize`**


 Serializes the TransformComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the TransformComponent's state

---

#### **`TransformComponent::Init`**


 Initializes the transform component with default values.

**Note:** This is an entirely optional step, as the transform component can and is initialized directly in the constructor.

Signature:

```cpp
 void Init(std::vector<float> position = { 0.0f, 0.0f, 0.0f }, std::vector<float> rotation = { 0.0f, 0.0f, 0.0f, 1.0f }, std::vector<float> scale = { 1.0f, 1.0f, 1.0f });
```

**Parameters:**

- `position`: Initial position of the GameObject
- `rotation`: Initial rotation of the GameObject (as a quaternion)
- `scale`: Initial scale of the GameObject

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::Update`**


 Update the transform component.

**Note:** There is no specific update logic for the transform component.

Signature:

```cpp
 void Update();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetComponentType`**


 Get the type of this component

Signature:

```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```

**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetRotationEuler`**


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

#### **`TransformComponent::GetRotationQuaternion`**


 Get the GLOBAL rotation of the transform as a quaternion

Signature:

```cpp
 void GetRotationQuaternion(float& x, float& y, float& z, float& w) const;
```

**Returns:** Array of 4 floats representing the quaternion (x, y, z, w)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetModelMatrix`**


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

#### **`TransformComponent::GetLocalMatrix`**


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

#### **`TransformComponent::GetPosition`**


 Get the GLOBAL position of the transform

Signature:

```cpp
 float* GetPosition();
```

**Returns:** Array of 3 floats representing the position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetScale`**


 Get the GLOBAL scale of the transform

Signature:

```cpp
 float* GetScale();
```

**Returns:** Array of 3 floats representing the scale (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetLocalPosition`**


 Get the LOCAL position of the transform

Signature:

```cpp
 float* GetLocalPosition();
```

**Returns:** Array of 3 floats representing the local position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetLocalRotation`**


 Get the LOCAL rotation of the transform as a quaternion

Signature:

```cpp
 float* GetLocalRotation();
```

**Returns:** Array of 4 floats representing the local rotation quaternion (x, y, z, w)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetLocalScale`**


 Get the LOCAL scale of the transform

Signature:

```cpp
 float* GetLocalScale();
```

**Returns:** Array of 3 floats representing the local scale (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::SetPosition`**


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

#### **`TransformComponent::SetRotationEuler`**


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

#### **`TransformComponent::SetRotationQuat`**


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

#### **`TransformComponent::SetScale`**


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

#### **`TransformComponent::SetWorldPosition`**


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

#### **`TransformComponent::SetWorldRotationQuat`**


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

#### **`TransformComponent::SetWorldRotationEuler`**


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

#### **`TransformComponent::SetWorldScale`**


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

#### **`TransformComponent::SetParent`**


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

#### **`TransformComponent::GetParent`**


 Get the parent transform of this transform

Signature:

```cpp
 TransformComponent* GetParent() const;
```

**Returns:** Pointer to the parent TransformComponent (or nullptr if none)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetChildren`**


 Get the child transforms of this transform

Signature:

```cpp
 std::vector<TransformComponent*>& GetChildren() const;
```

**Returns:** Vector of pointers to the child TransformComponents

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetVersion`**


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
| `float` | `m_rotation` | Rotation of the transform (Quaternion) |
| `float` | `m_scale` | Scale of the transform |
| `std::vector<TransformComponent*>` | `m_children` | Child transform components |

---

