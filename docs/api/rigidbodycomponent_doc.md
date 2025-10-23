# Syngine API Documentation


## RigidbodyComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/RigidbodyComponent.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [PhysicsShapes](#synginephysicsshapes)
- [shape](#syngineshape)
- [GetComponentType](#synginegetcomponenttype)
- [Clone](#syngineclone)
- [Init](#syngineinit)
- [Update](#syngineupdate)
- [Destroy](#synginedestroy)
- [_GetBodyID](#syngine_getbodyid)
- [_GetPhysicsManager](#syngine_getphysicsmanager)
- [GetMass](#synginegetmass)
- [GetFriction](#synginegetfriction)
- [GetRestitution](#synginegetrestitution)
- [GetShapeParameters](#synginegetshapeparameters)
- [UpdateShapeParameters](#syngineupdateshapeparameters)
- [SetFriction](#synginesetfriction)
- [SetRestitution](#synginesetrestitution)

---

## Class Constructor


#### **`Syngine::RigidbodyComponent`**


 Constructor for the RigidbodyComponent class

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 RigidbodyComponent(GameObject* owner, Syngine::RigidbodyParameters params = {});
```

**Parameters:**

- `static`: Rigidbody component type
- `static`: Rigidbody component type
- `owner`: Pointer to the GameObject that owns this component
- `params`: Rigidbody parameters to initialize the component

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::PhysicsShapes`**


 Enum for different physics shapes supported by the RigidbodyComponent. These shapes are used to define the physical properties of the rigidbody.

Signature:

```cpp
enum class PhysicsShapes 
```

**This function has been available since:** v0.0.1

---

#### **`Syngine::shape`**


 Struct to hold parameters for the RigidbodyComponent. This struct is used to initialize the RigidbodyComponent with specific properties.

Signature:

```cpp
 PhysicsShapes shape = PhysicsShapes::BOX; // The shape of the rigidbody
```

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

#### **`Syngine::Clone`**


 Clone the RigidbodyComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override { return std::make_unique<RigidbodyComponent>(*this);
```

**Returns:** A unique pointer to the cloned RigidbodyComponent

---

#### **`Syngine::Init`**


 Initialize the RigidbodyComponent

**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init(Syngine::RigidbodyParameters params);
```

**Parameters:**

- `params`: Rigidbody parameters to initialize the component

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


 Update the RigidbodyComponent

**Note:** This is called every frame to update the physics body

Signature:

```cpp
 void Update(bool simulate);
```

**Parameters:**

- `simulate`: Whether to simulate physics or not

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Destroy`**


 Destroys the RigidbodyComponent and its associated physics body

**Note:** This is called when the component is removed or the GameObject is destroyed

Signature:

```cpp
 void Destroy(); //TODO: Move this to RAII in the destructor
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::_GetBodyID`**


 Get the BodyID of the physics body

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 JPH::BodyID _GetBodyID() const;
```

**Returns:** The ID of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::_GetPhysicsManager`**


 Gets the physics manager

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 Syngine::Phys* _GetPhysicsManager() const;
```

**Returns:** Pointer to the physics manager

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetMass`**


 Get the mass of the physics body.

**Note:** If this value is 0, Jolt has used the shape to calculate the mass, and therefore this value is not valid. It is not recommended to use this for anything really.

Signature:

```cpp
 float GetMass() const;
```

**Returns:** The mass of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetFriction`**


 Get the friction of the physics body.

Signature:

```cpp
 float GetFriction() const;
```

**Returns:** The friction of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetRestitution`**


 Get the restitution of the physics body.

Signature:

```cpp
 float GetRestitution() const;
```

**Returns:** The restitution of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetShapeParameters`**


 Get the shape parameters of the physics body

**Note:** This is a vector of floats that can be used to define the shape parameters, e.g., radius for sphere, half extents for box, etc.

Signature:

```cpp
 std::vector<float> GetShapeParameters() const;
```

**Returns:** The shape parameters of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::UpdateShapeParameters`**


 Set the shape parameters of the physics body

Signature:

```cpp
 void UpdateShapeParameters(const std::vector<float>& newShapeParameters);
```

**Parameters:**

- `newShapeParameters`: The new shape parameters to set

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetFriction`**


 Set the friction of the physics body

Signature:

```cpp
 void SetFriction(float newFriction);
```

**Parameters:**

- `newFriction`: The new friction value to set

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetRestitution`**


 Set the restitution of the physics body

Signature:

```cpp
 void SetRestitution(float newRestitution);
```

**Parameters:**

- `newRestitution`: The new restitution value to set

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

