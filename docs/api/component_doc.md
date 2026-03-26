# Syngine API Documentation


## Component.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Component.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [DefaultComponents](#synginedefaultcomponents)
- [GetComponentType](#componentgetcomponenttype)
- [Clone](#componentclone)
- [Serialize](#componentserialize)

---

#### **`Syngine::DefaultComponents`**


 An enum of built-in component types.

Signature:

```cpp
enum DefaultComponents : ComponentTypeID 
```

**Members:**

| Name | Description |
| --- | --- | 
| `SYN_COMPONENT_MESH` | 3D mesh or model |
| `SYN_COMPONENT_TRANSFORM` | Position, rotation, scale |
| `SYN_COMPONENT_AI` | AI logic/behavior |
| `SYN_COMPONENT_PLAYER` | Player controller |
| `SYN_COMPONENT_CAMERA` | Camera logic |
| `SYN_COMPONENT_LIGHT` | Lighting |
| `SYN_COMPONENT_AUDIO_EMIT` | Audio source |
| `SYN_COMPONENT_AUDIO_LISTEN` | Audio listener |
| `SYN_COMPONENT_PARTICLE` | Particle system |
| `SYN_COMPONENT_UI` | UI elements |
| `SYN_COMPONENT_SCRIPT` | Custom scripting |
| `SYN_COMPONENT_ANIMATION` | Animation controller |
| `SYN_COMPONENT_RIGIDBODY` | Physics rigidbody |
| `SYN_COMPONENT_PHYSICS_MOTOR` | Physics motor/actuator |
| `SYN_COMPONENT_PHYSICS_CLOTH` | Cloth simulation |
| `SYN_COMPONENT_PHYSICS_JOINT` | Physics joint/constraint |
| `SYN_COMPONENT_TERRAIN` | Terrain |
| `SYN_COMPONENT_ZONE` | Trigger zone or area |
| `SYN_COMPONENT_BILLBOARD` | Billboard component |
| `SYN_COMPONENT_COUNT` | Total number of component types |

---

#### **`Component::GetComponentType`**




Signature:

```cpp
 virtual ComponentTypeID GetComponentType() = 0;
```

**Returns:** The type of the component, as defined in the Components enum

---

#### **`Component::Clone`**


 Clone the component, used for copying components when duplicating GameObjects or when copying components from one GameObject to another

Signature:

```cpp
 virtual std::unique_ptr<Component> Clone() const = 0;
```

**Returns:** A unique pointer to the cloned component

---

#### **`Component::Serialize`**


 Serialize the component to a data node

Signature:

```cpp
 virtual Serializer::DataNode Serialize() const = 0;
```

**Returns:** A pointer to the serialized data node representing the component's state

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `bool` | `isEnabled` | Whether the component is enabled or not |
| `GameObject*` | `m_owner` | The owner of the component, the GameObject it is attached to |

---

