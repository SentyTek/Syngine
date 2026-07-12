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
- [GetWorldRotationEuler](#transformcomponentgetworldrotationeuler)
- [GetWorldRotationQuaternion](#transformcomponentgetworldrotationquaternion)
- [GetModelMatrix](#transformcomponentgetmodelmatrix)
- [GetLocalMatrix](#transformcomponentgetlocalmatrix)
- [GetWorldPosition](#transformcomponentgetworldposition)
- [GetWorldScale](#transformcomponentgetworldscale)
- [GetPosition](#transformcomponentgetposition)
- [GetRotationQuaternion](#transformcomponentgetrotationquaternion)
- [GetRotationEuler](#transformcomponentgetrotationeuler)
- [GetScale](#transformcomponentgetscale)
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
 void Init(Vector3 position = Vector3(), Quaternion rotation = Quaternion(), Vector3 scale = Vector3(1.0f));
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

#### **`TransformComponent::GetWorldRotationEuler`**


 Get the GLOBAL rotation of the transform as XYZ Euler angles (in radians)

Signature:

```cpp
 Vector3 GetWorldRotationEuler() const;
```

**Returns:** Vector3 representing the rotation (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetWorldRotationQuaternion`**


 Get the GLOBAL rotation of the transform as a quaternion

Signature:

```cpp
 Quaternion GetWorldRotationQuaternion() const;
```

**Returns:** Quaternion representing the rotation (x, y, z, w)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::GetModelMatrix`**


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

#### **`TransformComponent::GetLocalMatrix`**


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

#### **`TransformComponent::GetWorldPosition`**


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

#### **`TransformComponent::GetWorldScale`**


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

#### **`TransformComponent::GetPosition`**


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

#### **`TransformComponent::GetRotationQuaternion`**


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

#### **`TransformComponent::GetRotationEuler`**


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

#### **`TransformComponent::GetScale`**


 Get the LOCAL scale of the transform

Signature:

```cpp
 Vector3 GetScale() const;
```

**Returns:** Vector3 representing the local scale (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`TransformComponent::SetPosition`**


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

#### **`TransformComponent::SetRotationEuler`**


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

#### **`TransformComponent::SetRotationQuat`**


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

#### **`TransformComponent::SetScale`**


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

#### **`TransformComponent::SetWorldPosition`**


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

#### **`TransformComponent::SetWorldRotationQuat`**


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

#### **`TransformComponent::SetWorldRotationEuler`**


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

#### **`TransformComponent::SetWorldScale`**


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
| `Quaternion` | `m_rotation` | Rotation of the transform (Quaternion) |
| `Vector3` | `m_scale` | Scale of the transform |
| `std::vector<TransformComponent*>` | `m_children` | Child transform components |

---

