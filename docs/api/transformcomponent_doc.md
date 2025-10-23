# Syngine API Documentation


## TransformComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/TransformComponent.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [Clone](#syngineclone)
- [Init](#syngineinit)
- [Update](#syngineupdate)
- [GetComponentType](#synginegetcomponenttype)
- [GetRotationAsEuler](#synginegetrotationaseuler)
- [SetPosition](#synginesetposition)
- [SetRotation](#synginesetrotation)
- [SetRotation](#synginesetrotation)
- [SetScale](#synginesetscale)
- [GetModelMatrix](#synginegetmodelmatrix)
- [GetPosition](#synginegetposition)

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

#### **`Syngine::Init`**


 Initializes the transform component with default values.

**Note:** This is an entirely optional step, as the transform component can and is initialized directly in the constructor.

Signature:

```cpp
 void Init(std::vector<float> position = {0.0f, 0.0f, 0.0f}, std::vector<float> rotation = {0.0f, 0.0f, 0.0f, 1.0f},
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
 Syngine::Components GetComponentType() override;
```

**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetRotationAsEuler`**


 Get the rotation of the transform as Euler angles (in radians)

Signature:

```cpp
 void GetRotationAsEuler(float& x, float& y, float& z) const;
```

**Parameters:**

- `x`: X component of the rotation
- `y`: Y component of the rotation
- `z`: Z component of the rotation

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetPosition`**


 Set the position of the transform

Signature:

```cpp
 void SetPosition(float x, float y,
```

**Parameters:**

- `x`: X component of the position
- `y`: Y component of the position
- `z`: Z component of the position

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetRotation`**


 Set the rotation of the transform as Euler angles (in radians)

Signature:

```cpp
 void SetRotation(float x, float y, float z);
```

**Parameters:**

- `x`: X component of the rotation
- `y`: Y component of the rotation
- `z`: Z component of the rotation

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetRotation`**


 Set the rotation of the transform as a quaternion

Signature:

```cpp
 void SetRotation(float x, float y, float z, float w);
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


 Set the scale of the transform

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

#### **`Syngine::GetModelMatrix`**


 Get a model matrix for the transform

Signature:

```cpp
 void GetModelMatrix(float* result);
```

**Parameters:**

- `result`: Array to fill with the model matrix (16 floats)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetPosition`**


 Get the position of the transform

Signature:

```cpp
 const float* GetPosition();
```

**Returns:** Array of 3 floats representing the position (x, y, z)

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `constexpr` | `Syngine` | Transform component type |
| `float` | `position` | Position of the transform |
| `float` | `rotation` | Rotation of the transform (Quaternion) |
| `float` | `scale` | Scale of the transform |

---

