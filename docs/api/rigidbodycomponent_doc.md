# Syngine API Documentation


## RigidbodyComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/RigidbodyComponent.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [PhysicsShapes](#synginephysicsshapes)
- [ForceMode](#syngineforcemode)
- [shape](#syngineshape)
- [GetComponentType](#synginegetcomponenttype)
- [Clone](#syngineclone)
- [Serialize](#syngineserialize)
- [Init](#syngineinit)
- [RetryInitIfPending](#syngineretryinitifpending)
- [SyncBodyToTransform](#synginesyncbodytotransform)
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
- [AddForce](#syngineaddforce)
- [AddForceAtPosition](#syngineaddforceatposition)
- [AddTorque](#syngineaddtorque)

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

**Members:**

| Name | Description |
| --- | --- | 
| `SPHERE` | Sphere shape |
| `BOX` | Box shape |
| `CAPSULE` | Capsule shape |
| `CAPSULE_TAPERED` | Tapered capsule shape |
| `CYLINDER` | Cylinder shape |
| `CYLINDER_TAPERED` | Tapered cylinder shape |
| `CONE` | Cone shape |
| `CONVEX_HULL` | Convex hull shape |
| `PLANE` | Plane shape |
| `MESH` | Mesh shape |
| `COMPOUND` | Compound shape |

**This function has been available since:** v0.0.1

---

#### **`Syngine::ForceMode`**


 Enum for different force application modes

Signature:

```cpp
enum class ForceMode 
```

**Members:**

| Name | Description |
| --- | --- | 
| `FORCE` | Add a continuous force to the rigidbody using its mass (F = m * a) (Newtons) |
| `ACCELERATION` | Add a continuous acceleration to the rigidbody, ignoring its mass. Note: This is not well supported, and is equivalent to FORCE if you did NOT manually set the mass yourself. |
| `IMPULSE` | Add an instant force impulse, using its mass (I = m * dv) ( Newton-seconds) |
| `VELOCITY_CHANGE` | Change velocity instantaneously, ignoring its mass. |

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
 Syngine::ComponentTypeID GetComponentType() override;
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

#### **`Syngine::Serialize`**


 Serializes the RigidbodyComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the RigidbodyComponent's state

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

#### **`Syngine::RetryInitIfPending`**


 Retry deferred initialization when dependencies (like Transform) become available.

Signature:

```cpp
 void RetryInitIfPending();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::SyncBodyToTransform`**


 Push the current TransformComponent world pose into the physics body. Useful after runtime re-parenting or prefab child attachment.

Signature:

```cpp
 void SyncBodyToTransform();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


 Update the RigidbodyComponent

**Note:** This is called every frame to update the physics body

Signature:

```cpp
 void Update(float deltaTime) override;
```

**Parameters:**

- `deltaTime`: The physics timestep

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

#### **`Syngine::AddForce`**


 Add a force to the rigidbody

**Note:** Force values for visible movement may be higher than expected.

Signature:

```cpp
 void AddForce(const float* force, ForceMode mode = ForceMode::FORCE);
```

**Parameters:**

- `force`: The force to add (vec3)
- `mode`: The mode of force application

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::AddForceAtPosition`**


 Add a force to the rigidbody at a specific position

**Note:** Force values for visible movement may be higher than expected.

Signature:

```cpp
 void AddForceAtPosition(const float* force, const float* position,
```

**Parameters:**

- `force`: The force to add (vec3)
- `position`: The position to apply the force at (vec3)
- `mode`: The mode of force application

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::AddTorque`**


 Add a torque to the rigidbody

**Note:** Torque values for visible rotation may be higher than expected.

Signature:

```cpp
 void AddTorque(const float* torque, ForceMode mode = ForceMode::FORCE);
```

**Parameters:**

- `torque`: The torque to add (vec3)
- `mode`: The mode of torque application

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

