# Syngine API Documentation

## RigidbodyComponent.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/RigidbodyComponent.h)

Syngine Rigidbody Component The RigidbodyComponent is used to represent a physics body in the game world. It holds the BodyID and the shape of the physics body from Jolt, among other properties. @section RigidbodyComponent

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [RigidbodyComponent(GameObject* owner, Syngine::RigidbodyParameters params = {})](#rigidbodycomponent-rigidbodycomponentgameobject-owner-syngine-rigidbodyparameters-params)

### Enums and Structs: 

- [PhysicsShapes](#syngine-physicsshapes)
- [ForceMode](#syngine-forcemode)
- [RigidbodyParameters](#syngine-rigidbodyparameters)

### Functions: 

- [GetComponentType()](#rigidbodycomponent-getcomponenttype)
- [Clone()](#rigidbodycomponent-clone)
- [Serialize()](#rigidbodycomponent-serialize)
- [Init()](#rigidbodycomponent-init)
- [RetryInitIfPending()](#rigidbodycomponent-retryinitifpending)
- [SyncBodyToTransform()](#rigidbodycomponent-syncbodytotransform)
- [Update()](#rigidbodycomponent-update)
- [Destroy()](#rigidbodycomponent-destroy)
- [_GetBodyID()](#rigidbodycomponent-_getbodyid)
- [_GetPhysicsManager()](#rigidbodycomponent-_getphysicsmanager)
- [GetMass()](#rigidbodycomponent-getmass)
- [GetFriction()](#rigidbodycomponent-getfriction)
- [GetRestitution()](#rigidbodycomponent-getrestitution)
- [GetShapeParameters()](#rigidbodycomponent-getshapeparameters)
- [UpdateShapeParameters()](#rigidbodycomponent-updateshapeparameters)
- [SetFriction()](#rigidbodycomponent-setfriction)
- [SetRestitution()](#rigidbodycomponent-setrestitution)
- [AddForce()](#rigidbodycomponent-addforce)
- [AddForceAtPosition()](#rigidbodycomponent-addforceatposition)
- [AddTorque()](#rigidbodycomponent-addtorque)

---
<a id="syngine-physicsshapes"></a>

#### **`Syngine::PhysicsShapes()`**

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
<a id="syngine-forcemode"></a>

#### **`Syngine::ForceMode()`**

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
| `VELOCITY_CHANGE` | Change velocity instantaneously, ignoring its mass. Note: This is not well supported, and is equivalent to IMPULSE if you did NOT manually set the mass yourself. |
**This function has been available since:** v0.0.1

---
<a id="syngine-rigidbodyparameters"></a>

#### **`Syngine::RigidbodyParameters()`**

 Struct to hold parameters for the RigidbodyComponent. This struct is used to initialize the RigidbodyComponent with specific properties.

Signature:
```cpp
struct RigidbodyParameters
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `PhysicsShapes` | `shape` | The shape of the rigidbody |
| `float` | `mass` | Mass of the rigidbody. If 0 (which it is by default), Jolt will calculate it based on the shape. |
| `float` | `friction` | Friction coefficient |
| `float` | `restitution` | Restitution coefficient (bounciness) |
| `Math::Vector3` | `shapeParameters` | Additional parameters for the shape, e.g., radius for sphere, half extents for box |
| `JPH::EMotionType` | `motionType` | Motion type of the rigidbody |
| `JPH::ObjectLayer` | `layer` | Layer of the rigidbody |
| `std::vector<CompoundShapePart>` | `compoundParts` | Parts for compound shape |
**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-rigidbodycomponentgameobject-owner-syngine-rigidbodyparameters-params"></a>

## Class Constructor

#### **`RigidbodyComponent(GameObject* owner, Syngine::RigidbodyParameters params = {})`**

 Constructor for the RigidbodyComponent class

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:
```cpp
 RigidbodyComponent(GameObject* owner, Syngine::RigidbodyParameters params = {});
```
**Parameters:**
- `owner`: Pointer to the GameObject that owns this component
- `params`: Rigidbody parameters to initialize the component

**Members:**

| Type | Name | Description |
| --- | --- | --- | 
| `PhysicsShapes` | `shape` | The shape of the rigidbody |
| `float` | `mass` | Mass of the rigidbody. If 0 (which it is by default), Jolt will calculate it based on the shape. |
| `float` | `friction` | Friction coefficient |
| `float` | `restitution` | Restitution coefficient (bounciness) |
| `Math::Vector3` | `shapeParameters` | Additional parameters for the shape, e.g., radius for sphere, half extents for box |
| `JPH::EMotionType` | `motionType` | Motion type of the rigidbody |
| `JPH::ObjectLayer` | `layer` | Layer of the rigidbody |
| `std::vector<CompoundShapePart>` | `compoundParts` | Parts for compound shape |

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-getcomponenttype"></a>

#### **`RigidbodyComponent::GetComponentType()`**

 Get the type of this component

Signature:
```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```
**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-clone"></a>

#### **`RigidbodyComponent::Clone()`**

 Clone the RigidbodyComponent

Signature:
```cpp
 std::unique_ptr<Component> Clone() const override;
```
**Returns:** A unique pointer to the cloned RigidbodyComponent

---
<a id="rigidbodycomponent-serialize"></a>

#### **`RigidbodyComponent::Serialize()`**

 Serializes the RigidbodyComponent to a data node

Signature:
```cpp
 Serializer::DataNode Serialize() const override;
```
**Returns:** A pointer to the serialized data node representing the RigidbodyComponent's state

---
<a id="rigidbodycomponent-init"></a>

#### **`RigidbodyComponent::Init()`**

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
<a id="rigidbodycomponent-retryinitifpending"></a>

#### **`RigidbodyComponent::RetryInitIfPending()`**

 Retry deferred initialization when dependencies (like Transform) become available.

Signature:
```cpp
 void RetryInitIfPending();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-syncbodytotransform"></a>

#### **`RigidbodyComponent::SyncBodyToTransform()`**

 Push the current TransformComponent world pose into the physics body. Useful after runtime re-parenting or prefab child attachment.

Signature:
```cpp
 void SyncBodyToTransform();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-update"></a>

#### **`RigidbodyComponent::Update()`**

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
<a id="rigidbodycomponent-destroy"></a>

#### **`RigidbodyComponent::Destroy()`**

 Destroys the RigidbodyComponent and its associated physics body

**Note:** This is called when the component is removed or the GameObject is destroyed

Signature:
```cpp
 void Destroy(); //TODO: Move this to RAII in the destructor
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-_getbodyid"></a>

#### **`RigidbodyComponent::_GetBodyID()`**

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
<a id="rigidbodycomponent-_getphysicsmanager"></a>

#### **`RigidbodyComponent::_GetPhysicsManager()`**

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
<a id="rigidbodycomponent-getmass"></a>

#### **`RigidbodyComponent::GetMass()`**

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
<a id="rigidbodycomponent-getfriction"></a>

#### **`RigidbodyComponent::GetFriction()`**

 Get the friction of the physics body.

Signature:
```cpp
 float GetFriction() const;
```
**Returns:** The friction of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-getrestitution"></a>

#### **`RigidbodyComponent::GetRestitution()`**

 Get the restitution of the physics body.

Signature:
```cpp
 float GetRestitution() const;
```
**Returns:** The restitution of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-getshapeparameters"></a>

#### **`RigidbodyComponent::GetShapeParameters()`**

 Get the shape parameters of the physics body

**Note:** This is a vector of floats that can be used to define the shape parameters, e.g., radius for sphere, half extents for box, etc.

Signature:
```cpp
 Math::Vector3 GetShapeParameters() const;
```
**Returns:** The shape parameters of the physics body

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-updateshapeparameters"></a>

#### **`RigidbodyComponent::UpdateShapeParameters()`**

 Set the shape parameters of the physics body

Signature:
```cpp
 void UpdateShapeParameters(const Math::Vector3 newShapeParameters);
```
**Parameters:**
- `newShapeParameters`: The new shape parameters to set

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-setfriction"></a>

#### **`RigidbodyComponent::SetFriction()`**

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
<a id="rigidbodycomponent-setrestitution"></a>

#### **`RigidbodyComponent::SetRestitution()`**

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
<a id="rigidbodycomponent-addforce"></a>

#### **`RigidbodyComponent::AddForce()`**

 Add a force to the rigidbody

**Note:** Force values for visible movement may be higher than expected.

Signature:
```cpp
 void AddForce(const Math::Vector3 force, ForceMode mode = ForceMode::FORCE);
```
**Parameters:**
- `force`: The force to add (vec3)
- `mode`: The mode of force application. See ForceMode enum for details.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-addforceatposition"></a>

#### **`RigidbodyComponent::AddForceAtPosition()`**

 Add a force to the rigidbody at a specific position

**Note:** Force values for visible movement may be higher than expected.

Signature:
```cpp
 void AddForceAtPosition(const Math::Vector3 force, const Math::Vector3 position, ForceMode mode = ForceMode::FORCE);
```
**Parameters:**
- `force`: The force to add (vec3)
- `position`: The position to apply the force at (vec3)
- `mode`: The mode of force application

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="rigidbodycomponent-addtorque"></a>

#### **`RigidbodyComponent::AddTorque()`**

 Add a torque to the rigidbody

**Note:** Torque values for visible rotation may be higher than expected.

Signature:
```cpp
 void AddTorque(const Math::Vector3 torque, ForceMode mode = ForceMode::FORCE);
```
**Parameters:**
- `torque`: The torque to add (vec3)
- `mode`: The mode of torque application. Note that ACCELERATION and VELOCITY_CHANGE modes are not well supported for torque, and will be treated the same as FORCE and IMPULSE respectively. Use with caution.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `PhysicsShapes` | `shape` | The shape of the part |
| `Math::Vector3` | `shapeParameters` | Parameters for the shape, e.g., radius for sphere, half extents for box |
| `Math::Vector3` | `position` | Local position offset |
| `Math::Quaternion` | `rotation` | Local rotation quaternion |
| `constexpr` | `Syngine` | Rigidbody component type |
---
