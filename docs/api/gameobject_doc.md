# Syngine API Documentation


## GameObject.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/GameObject.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [GetID](#gameobjectgetid)
- [_SetID](#gameobject_setid)
- [IsActive](#gameobjectisactive)
- [SetActive](#gameobjectsetactive)
- [RemoveComponent](#gameobjectremovecomponent)
- [HasComponent](#gameobjecthascomponent)
- [GetComponent](#gameobjectgetcomponent)
- [AddComponent](#gameobjectaddcomponent)

---

## Class Constructor


#### **`GameObject::GameObject`**


 Constructor for the GameObject class

Signature:

```cpp
 GameObject(string name, string type = "default");
```

**Parameters:**

- `name`: Name of the GameObject
- `type`: Type of the GameObject, defaults to "default"

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`GameObject::GetID`**


 Get the ID of the GameObject

Signature:

```cpp
 inline long GetID() noexcept { return this->id; };
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
 inline void _SetID(long id) noexcept ;
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
 inline bool IsActive() const noexcept ;
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

#### **`GameObject::RemoveComponent`**


 Remove a component from the GameObject

Signature:

```cpp
 int RemoveComponent(Syngine::Components type);
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
 bool HasComponent(Syngine::Components type);
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
 template <typename T> T* GetComponent();
```

**Parameters:**

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

**Parameters:**

- `T`: Type of the component to add
- `args`: Arguments to pass to the component constructor

**Returns:** Pointer to the added component if successful, nullptr otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `string` | `type` | Type of the GameObject, used for categorization and |
| `string` | `gizmo` | Gizmo type for rendering in the editor, e.g., |

---

