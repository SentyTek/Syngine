# Syngine API Documentation


## GameObject.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/GameObject.h)

---

## Goto:


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [Constructor](#class-constructor)
- [GetID](#gameobjectgetid)
- [_SetID](#gameobject_setid)
- [IsActive](#gameobjectisactive)
- [SetActive](#gameobjectsetactive)
- [GetTags](#gameobjectgettags)
- [AddTag](#gameobjectaddtag)
- [RemoveTag](#gameobjectremovetag)
- [HasTag](#gameobjecthastag)
- [ClearTags](#gameobjectcleartags)
- [GetComponentCount](#gameobjectgetcomponentcount)
- [RemoveComponent](#gameobjectremovecomponent)
- [HasComponent](#gameobjecthascomponent)
- [GetComponent](#gameobjectgetcomponent)
- [GetComponent](#gameobjectgetcomponent)
- [AddComponent](#gameobjectaddcomponent)
- [GetComponents](#gameobjectgetcomponents)
- [Serialize](#gameobjectserialize)
- [SetParent](#gameobjectsetparent)
- [GetParent](#gameobjectgetparent)
- [AddChild](#gameobjectaddchild)
- [RemoveChild](#gameobjectremovechild)

---

## Class Constructor


#### **`GameObject::GameObject`**


 Constructor for the GameObject class

Signature:

```cpp
 GameObject(std::string name, std::string type = "default", std::string initialTag = "");
```

**Parameters:**

- `name`: Name of the GameObject
- `type`: Type of the GameObject, defaults to "default"

**This function has been available since:** v0.0.1

---

## Class & Related Members


## Class Constructor


#### **`GameObject::GameObject`**


 Construct from a DataNode, used for deserialization

Signature:

```cpp
 GameObject(const Serializer::DataNode& data);
```

**Parameters:**

- `data`: DataNode representing the serialized GameObject

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`GameObject::GetID`**


 Get the ID of the GameObject

Signature:

```cpp
 inline long GetID() noexcept;
```

**Returns:** ID of the GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::_SetID`**


 Set the ID of the GameObject

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 inline void _SetID(long id) noexcept;
```

**Parameters:**

- `id`: ID of the GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::IsActive`**


 Check if the GameObject is active

Signature:

```cpp
 inline bool IsActive() const noexcept;
```

**Returns:** true if the GameObject is active, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::SetActive`**


 Set the active state of the GameObject

Signature:

```cpp
 void SetActive(bool active) noexcept;
```

**Parameters:**

- `active`: true to set the GameObject as active, false to set it as inactive

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::GetTags`**


 Get the tags of the GameObject

Signature:

```cpp
 inline std::vector<std::string> GetTags() const noexcept;
```

**Returns:** Vector of tags of the GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::AddTag`**


 Add a tag to the GameObject

Signature:

```cpp
 void AddTag(const std::string& tag);
```

**Parameters:**

- `tag`: Tag to add

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::RemoveTag`**


 Remove a tag from the GameObject

Signature:

```cpp
 void RemoveTag(const std::string& tag);
```

**Parameters:**

- `tag`: Tag to remove

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::HasTag`**


 Check if the GameObject has a specific tag

Signature:

```cpp
 bool HasTag(const std::string& tag) const;
```

**Parameters:**

- `tag`: Tag to check

**Returns:** true if the tag exists, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::ClearTags`**


 Clear all tags from the GameObject

Signature:

```cpp
 void ClearTags();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::GetComponentCount`**


 Get the number of components attached to the GameObject

Signature:

```cpp
 size_t GetComponentCount() const noexcept;
```

**Returns:** Number of components attached to the GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::RemoveComponent`**


 Remove a component from the GameObject

Signature:

```cpp
 int RemoveComponent(Syngine::ComponentTypeID type);
```

**Parameters:**

- `type`: Type of the component to remove

**Returns:** 0 on success, -1 if the component was not found

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::HasComponent`**


 Check if the GameObject has a component of the specified type

Signature:

```cpp
 bool HasComponent(Syngine::ComponentTypeID type);
```

**Parameters:**

- `type`: Type of the component to check

**Returns:** true if the component exists, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::GetComponent`**


 Get a component of the specified type

Signature:

```cpp
 Component* GetComponent(Syngine::ComponentTypeID type) const;
```

**Parameters:**

- `type`: Type of the component to get

**Returns:** Pointer to the component if it exists, nullptr otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::GetComponent`**


 Get a component of the specified type

Signature:

```cpp
 template <typename T> T* GetComponent() const;
```

**Template Parameters:**

- `T`: Type of the component to get

**Returns:** Pointer to the component if it exists, nullptr otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::AddComponent`**


 Add a component to the GameObject

Signature:

```cpp
 template <typename T, typename... Args> T* AddComponent(Args&&... args);
```

**Template Parameters:**

- `T`: Type of the component to add

**Parameters:**

- `args`: Arguments to pass to the component constructor

**Returns:** Pointer to the added component if successful, nullptr otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::GetComponents`**


 Get const access to the components map for iteration

Signature:

```cpp
 const std::map<ComponentTypeID, std::unique_ptr<Component>>& GetComponents() const;
```

**Returns:** Const reference to the components map

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::Serialize`**


 Serialize the GameObject and its components into a DataNode for saving

Signature:

```cpp
 Serializer::DataNode Serialize() const;
```

**Returns:** DataNode representing the serialized GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::SetParent`**


 Set the parent of the GameObject

Signature:

```cpp
 void SetParent(GameObject* parent);
```

**Parameters:**

- `parent`: Pointer to the parent GameObject

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::GetParent`**


 Get the parent of the GameObject

Signature:

```cpp
 GameObject* GetParent() const;
```

**Returns:** Pointer to the parent GameObject, or nullptr if there is no parent

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::AddChild`**


 Add a child GameObject to this GameObject

Signature:

```cpp
 void AddChild(GameObject* child);
```

**Parameters:**

- `child`: Pointer to the child GameObject to add

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`GameObject::RemoveChild`**


 Remove a child GameObject from this GameObject

Signature:

```cpp
 void RemoveChild(GameObject* child);
```

**Parameters:**

- `child`: Pointer to the child GameObject to remove

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- |
| `std::string` | `type` | Type of the GameObject, used for shaders. |
| `std::string` | `gizmo` | Gizmo type for rendering in the editor, e.g., |
| `std::vector<std::string>` | `tags` | Tags for grouping and identifying GameObjects |

---
