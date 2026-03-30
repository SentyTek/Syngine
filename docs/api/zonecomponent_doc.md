# Syngine API Documentation


## ZoneComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/ZoneComponent.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [ZoneShape](#synginezoneshape)
- [m_size](#zonecomponentm_size)
- [Clone](#zonecomponentclone)
- [Serialize](#zonecomponentserialize)
- [GetComponentType](#zonecomponentgetcomponenttype)
- [Init](#zonecomponentinit)
- [Update](#zonecomponentupdate)
- [GetShape](#zonecomponentgetshape)
- [GetPosition](#zonecomponentgetposition)
- [SetPosition](#zonecomponentsetposition)
- [GetSize](#zonecomponentgetsize)
- [SetSize](#zonecomponentsetsize)
- [GetRotation](#zonecomponentgetrotation)
- [SetRotation](#zonecomponentsetrotation)
- [IsActive](#zonecomponentisactive)
- [SetActive](#zonecomponentsetactive)
- [IsOneShot](#zonecomponentisoneshot)
- [_AddObject](#zonecomponent_addobject)
- [_HasOneTimeObject](#zonecomponent_hasonetimeobject)
- [_IsTrackingObject](#zonecomponent_istrackingobject)
- [_RemoveObject](#zonecomponent_removeobject)
- [AddTag](#zonecomponentaddtag)
- [RemoveTag](#zonecomponentremovetag)
- [HasTag](#zonecomponenthastag)
- [GetTags](#zonecomponentgettags)
- [SetTags](#zonecomponentsettags)
- [IsInZone](#zonecomponentisinzone)
- [IsInZone](#zonecomponentisinzone)
- [GetObjectsInZone](#zonecomponentgetobjectsinzone)
- [GetObjectsInZoneByTag](#zonecomponentgetobjectsinzonebytag)
- [_GetOwner](#zonecomponent_getowner)

---

## Class Constructor


#### **`ZoneComponent::ZoneComponent`**


 Constructor for ZoneComponent

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 ZoneComponent(GameObject* owner, ZoneShape shape, const float pos[3], const float size[3], bool oneShot = false);
```

**Parameters:**

- `owner`: The GameObject that owns this component.
- `shape`: The shape of the zone (box or sphere).
- `pos`: The position of the zone center.
- `size`: The size of the zone (whd for box, r for sphere).
- `oneShot`: Whether the zone is a one-shot zone (triggers only once per object).

---

## Class & Related Members


#### **`Syngine::ZoneShape`**


 Enum representing the shape of the zone.

Signature:

```cpp
enum class ZoneShape
```

---

#### **`ZoneComponent::m_size`**


 ZoneComponent is used to define an area within the game world that can be used to trigger events

Signature:

```cpp
 float m_size[3] = { 1.0f, 1.0f, 1.0f }; //* Size of the zone (whd for box, r for sphere)
```

---

#### **`ZoneComponent::Clone`**


 Clone the ZoneComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override;
```

**Returns:** A unique pointer to the cloned ZoneComponent

---

#### **`ZoneComponent::Serialize`**


 Serializes the ZoneComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the ZoneComponent's state

---

#### **`ZoneComponent::GetComponentType`**


 Gets the component type of this component.

Signature:

```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```

**Returns:** The component type of this component.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::Init`**


 Initializes the zone component with shape, position, and size.

#### This function is internal use only and not intended for public use!


**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init(ZoneShape shape, const float pos[3], const float size[3], bool oneShot);
```

**Parameters:**

- `shape`: The shape of the zone (box or sphere).
- `pos`: The position of the zone center.
- `size`: The size of the zone (whd for box, r for sphere).
- `oneShot`: Whether the zone is a one-shot zone (triggers only once per object).

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::Update`**


 Update the zone component.

**Note:** There is no specific update logic for the zone component

Signature:

```cpp
 void Update();
```

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::GetShape`**


 Get the shape of the zone.

Signature:

```cpp
 ZoneShape GetShape() const;
```

**Returns:** The shape of the zone (ZoneShape).

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::GetPosition`**


 Get the position of the zone center.

Signature:

```cpp
 void GetPosition(float outPos[3]) const;
```

**Parameters:**

- `outPos`: Output array to store the position (size 3).

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::SetPosition`**


 Set the position of the zone center.

Signature:

```cpp
 void SetPosition(const float pos[3]);
```

**Parameters:**

- `pos`: The new position of the zone center (size 3).

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::GetSize`**


 Get the size of the zone.

Signature:

```cpp
 void GetSize(float outSize[3]) const;
```

**Parameters:**

- `outSize`: Output array to store the size (size 3).

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::SetSize`**


 Set the size of the zone.

Signature:

```cpp
 void SetSize(const float size[3]);
```

**Parameters:**

- `size`: The new size of the zone (size 3).

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::GetRotation`**


 Get the rotation of the zone (for box shape).

Signature:

```cpp
 void GetRotation(float outRot[3]) const;
```

**Parameters:**

- `outRot`: Output array to store the rotation (size 3).

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::SetRotation`**


 Set the rotation of the zone (for box shape).

Signature:

```cpp
 void SetRotation(const float rot[3]);
```

**Parameters:**

- `rot`: The new rotation of the zone (size 3).

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::IsActive`**


 Check if the zone is active.

Signature:

```cpp
 bool IsActive() const;
```

**Returns:** True if the zone is active, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::SetActive`**


 Set the active state of the zone.

Signature:

```cpp
 void SetActive(bool active);
```

**Parameters:**

- `active`: True to activate the zone, false to deactivate.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::IsOneShot`**


 Check if the zone is a one-shot zone.

Signature:

```cpp
 bool IsOneShot() const;
```

**Returns:** True if the zone is a one-shot zone, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::_AddObject`**


 Add an object to the triggered list (for one-shot zones).

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 void _AddObject(GameObject* object);
```

**Parameters:**

- `object`: The GameObject to add.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::_HasOneTimeObject`**


 Check if an object is in the triggered list (for one-shot zones).

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _HasOneTimeObject(GameObject* object);
```

**Parameters:**

- `object`: The GameObject to check.

**Returns:** True if the object is in the triggered list, false otherwise. Returns false if not a one-shot zone

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::_IsTrackingObject`**


 Check if an object is currently inside the zone.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _IsTrackingObject(GameObject* object) const;
```

**Parameters:**

- `object`: The GameObject to check.

**Returns:** True if the object is inside the zone, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::_RemoveObject`**


 Remove an object from the triggered list (for one-shot zones).

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 void _RemoveObject(GameObject* object);
```

**Parameters:**

- `object`: The GameObject to remove.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::AddTag`**


 Add a tag to the zone.

Signature:

```cpp
 void AddTag(const std::string& tag);
```

**Parameters:**

- `tag`: The tag to add.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::RemoveTag`**


 Remove a tag from the zone.

Signature:

```cpp
 void RemoveTag(const std::string& tag);
```

**Parameters:**

- `tag`: The tag to remove.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::HasTag`**


 Check if the zone has a specific tag.

Signature:

```cpp
 bool HasTag(const std::string& tag) const;
```

**Parameters:**

- `tag`: The tag to check.

**Returns:** True if the zone has the specified tag, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::GetTags`**


 Get all tags of the zone.

Signature:

```cpp
 std::vector<std::string> GetTags() const;
```

**Returns:** A vector of all tags of the zone.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::SetTags`**


 Set several tags to the zone at once, replacing existing tags.

Signature:

```cpp
 void SetTags(const std::vector<std::string>& tags);
```

**Parameters:**

- `tags`: A vector of tags to set.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::IsInZone`**


 Check if a point is inside the zone.

Signature:

```cpp
 bool IsInZone(const float point[3]) const;
```

**Parameters:**

- `point`: The point to check (size 3).

**Returns:** True if the point is inside the zone, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::IsInZone`**


 Check if a GameObject is inside the zone.

**Preconditions:** The GameObject must have a TransformComponent.

Signature:

```cpp
 bool IsInZone(const GameObject* object) const;
```

**Parameters:**

- `object`: The GameObject to check.

**Returns:** True if the GameObject is inside the zone, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::GetObjectsInZone`**


 Get all GameObjects inside the zone.

**Preconditions:** The GameObjects must have a TransformComponent.

Signature:

```cpp
 std::vector<GameObject*> GetObjectsInZone() const;
```

**Returns:** A vector of pointers to GameObjects inside the zone.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::GetObjectsInZoneByTag`**


 Get all GameObjects inside the zone with a specific tag.

**Preconditions:** The GameObjects must have a TransformComponent.

Signature:

```cpp
 std::vector<GameObject*> GetObjectsInZoneByTag(const std::string& tag) const;
```

**Parameters:**

- `tag`: The tag to filter GameObjects.

**Returns:** A vector of pointers to GameObjects inside the zone with the specified tag.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ZoneComponent::_GetOwner`**


 Get the owner GameObject of this zone component.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 GameObject* _GetOwner() const;
```

**Returns:** Pointer to the owner GameObject.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `ZoneShape` | `m_shape` | Shape of the zone (box or sphere) |
| `float` | `m_pos` | Position of the zone center |
| `bool` | `m_active` | Whether the zone is active or not |
| `bool` | `m_oneShot` | Whether the zone is a one-shot zone |
| `std::vector<std::string>` | `m_tags` | Tags of the zone |

---

