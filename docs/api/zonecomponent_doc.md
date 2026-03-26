# Syngine API Documentation


## ZoneComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/ZoneComponent.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [X](#synginex)
- [Clone](#syngineclone)
- [Serialize](#syngineserialize)
- [GetComponentType](#synginegetcomponenttype)
- [Init](#syngineinit)
- [Update](#syngineupdate)
- [GetShape](#synginegetshape)
- [GetPosition](#synginegetposition)
- [SetPosition](#synginesetposition)
- [GetSize](#synginegetsize)
- [SetSize](#synginesetsize)
- [GetRotation](#synginegetrotation)
- [SetRotation](#synginesetrotation)
- [IsActive](#syngineisactive)
- [SetActive](#synginesetactive)
- [IsOneShot](#syngineisoneshot)
- [_AddObject](#syngine_addobject)
- [_HasOneTimeObject](#syngine_hasonetimeobject)
- [_IsTrackingObject](#syngine_istrackingobject)
- [_RemoveObject](#syngine_removeobject)
- [AddTag](#syngineaddtag)
- [RemoveTag](#syngineremovetag)
- [HasTag](#synginehastag)
- [GetTags](#synginegettags)
- [SetTags](#synginesettags)
- [IsInZone](#syngineisinzone)
- [IsInZone](#syngineisinzone)
- [GetObjectsInZone](#synginegetobjectsinzone)
- [GetObjectsInZoneByTag](#synginegetobjectsinzonebytag)
- [_GetOwner](#syngine_getowner)

---

## Class Constructor


#### **`Syngine::ZoneComponent`**


 Constructor for ZoneComponent

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 ZoneComponent(GameObject* owner, ZoneShape shape,
```

**Parameters:**

- `1`: Size of the zone (whd for box, r for sphere)
- `ZoneShape`: Shape of the zone (box or sphere)
- `float`: Position of the zone center Rotation of the zone (for box shape). This is a
- `bool`: Whether the zone is active or not
- `bool`: Whether the zone is a one-shot zone
- `m_objectsIn`: Objects that are currently in the zone
- `m_triggeredObjects`: Objects that have triggered the zone (for one-shot zones)
- `std`: Tags of the zone
- `SYN_COMPONENT_ZONE`: Zone component type
- `owner`: The GameObject that owns this component.
- `shape`: The shape of the zone (box or sphere).
- `pos`: The position of the zone center.
- `size`: The size of the zone (whd for box, r for sphere).
- `oneShot`: Whether the zone is a one-shot zone (triggers only once per object).

---

## Class & Related Members


#### **`Syngine::X`**


 Enum representing the shape of the zone.

Signature:

```cpp
 BOX = 0, SPHERE = 1,
```

---

#### **`Syngine::Clone`**


 Clone the ZoneComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override { return std::make_unique<ZoneComponent>(*this);
```

**Returns:** A unique pointer to the cloned ZoneComponent

---

#### **`Syngine::Serialize`**


 Serializes the ZoneComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the ZoneComponent's state

---

#### **`Syngine::GetComponentType`**


 Gets the component type of this component.

Signature:

```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```

**Returns:** The component type of this component.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::Init`**


 Initializes the zone component with shape, position, and size.

#### This function is internal use only and not intended for public use!


**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init(ZoneShape shape, const float pos[3],
```

**Parameters:**

- `shape`: The shape of the zone (box or sphere).
- `pos`: The position of the zone center.
- `size`: The size of the zone (whd for box, r for sphere).
- `oneShot`: Whether the zone is a one-shot zone (triggers only once per object).

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


 Update the zone component.

**Note:** There is no specific update logic for the zone component

Signature:

```cpp
 void Update() {};
```

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetShape`**


 Get the shape of the zone.

Signature:

```cpp
 ZoneShape GetShape() const;
```

**Returns:** The shape of the zone (ZoneShape).

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetPosition`**


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

#### **`Syngine::SetPosition`**


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

#### **`Syngine::GetSize`**


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

#### **`Syngine::SetSize`**


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

#### **`Syngine::GetRotation`**


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

#### **`Syngine::SetRotation`**


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

#### **`Syngine::IsActive`**


 Check if the zone is active.

Signature:

```cpp
 bool IsActive() const;
```

**Returns:** True if the zone is active, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetActive`**


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

#### **`Syngine::IsOneShot`**


 Check if the zone is a one-shot zone.

Signature:

```cpp
 bool IsOneShot() const;
```

**Returns:** True if the zone is a one-shot zone, false otherwise.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::_AddObject`**


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

#### **`Syngine::_HasOneTimeObject`**


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

#### **`Syngine::_IsTrackingObject`**


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

#### **`Syngine::_RemoveObject`**


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

#### **`Syngine::AddTag`**


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

#### **`Syngine::RemoveTag`**


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

#### **`Syngine::HasTag`**


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

#### **`Syngine::GetTags`**


 Get all tags of the zone.

Signature:

```cpp
 std::vector<std::string> GetTags() const;
```

**Returns:** A vector of all tags of the zone.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetTags`**


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

#### **`Syngine::IsInZone`**


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

#### **`Syngine::IsInZone`**


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

#### **`Syngine::GetObjectsInZone`**


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

#### **`Syngine::GetObjectsInZoneByTag`**


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

#### **`Syngine::_GetOwner`**


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

