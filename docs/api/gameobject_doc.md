# Syngine API Documentation

## GameObject.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/GameObject.h)

GameObject class representing an entity in the game world, such as a player, enemy, or item. They can have various components attached to them, such as a mesh, physics, AI, etc. @section GameObject

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

<div style="background:#08082e; padding:15px; border-radius:8px; margin-bottom:20px; font-family:sans-serif;">
    <label for="mdSearch" style="font-weight:bold; display:block; margin-bottom:5px;">Search Functions:</label>
    <input type="text" id="mdSearch" placeholder="Type function name..." onkeyup="filterMarkdownDocs()" style="width:100%; padding:8px; border:1px solid #ccc; border-radius:4px; font-size:16px; background-color: rgb(60, 60, 60); color:antiquewhite">
</div>

<script>
function filterMarkdownDocs() {
    var input = document.getElementById('mdSearch').value.toLowerCase();
    // Targets common markdown containers or the whole document body
    var elements = document.querySelectorAll('h1, h2, h3, h4, p, li, pre, hr');

    elements.forEach(function(el) {
        // Skip the search box itself
        if (el.closest('#mdSearch') || el.id === 'mdSearch') return;

        var text = el.innerText.toLowerCase();
        if (text.includes(input)) {
            el.style.display = ""; // Show matching element
            //el.style.backgroundColor = input ? "#fff9c4" : ""; // Highlight if searching
        } else {
            el.style.display = input ? "none" : ""; // Hide if it doesn't match
        }
    });
}
</script>

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [GameObject(std::string name, std::string type = "default", std::string initialTag = "")](#gameobject-gameobjectstd-string-name-std-string-type-default-std-string-initialtag)
- [GameObject(const Serializer::DataNode& data)](#gameobject-gameobjectconst-serializer-datanode-data)

### Functions: 

- [GetID()](#gameobject-getid)
- [_SetID()](#gameobject-_setid)
- [IsActive()](#gameobject-isactive)
- [SetActive()](#gameobject-setactive)
- [GetTags()](#gameobject-gettags)
- [AddTag()](#gameobject-addtag)
- [RemoveTag()](#gameobject-removetag)
- [HasTag()](#gameobject-hastag)
- [ClearTags()](#gameobject-cleartags)
- [GetComponentCount()](#gameobject-getcomponentcount)
- [RemoveComponent()](#gameobject-removecomponent)
- [HasComponent()](#gameobject-hascomponent)
- [GetComponent()](#gameobject-getcomponent)
- [GetComponent()](#gameobject-getcomponent-2)
- [AddComponent()](#gameobject-addcomponent)
- [GetComponents()](#gameobject-getcomponents)
- [Serialize()](#gameobject-serialize)
- [SetParent()](#gameobject-setparent)
- [GetParent()](#gameobject-getparent)
- [AddChild()](#gameobject-addchild)
- [RemoveChild()](#gameobject-removechild)
- [GetChildren()](#gameobject-getchildren)

---
<a id="gameobject-gameobjectstd-string-name-std-string-type-default-std-string-initialtag"></a>

## Class Constructor

#### **`GameObject(std::string name, std::string type = "default", std::string initialTag = "")`**

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
<a id="gameobject-gameobjectconst-serializer-datanode-data"></a>

## Class Constructor

#### **`GameObject(const Serializer::DataNode& data)`**

 Construct from a DataNode, used for deserialization

Signature:
```cpp
 GameObject(const Serializer::DataNode& data);
```
**Parameters:**
- `data`: DataNode representing the serialized GameObject

**This function has been available since:** v0.0.1

---
<a id="gameobject-getid"></a>

#### **`GameObject::GetID()`**

 Get the ID of the GameObject

Signature:
```cpp
 inline long GetID() noexcept;
```
**Returns:** ID of the GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-_setid"></a>

#### **`GameObject::_SetID()`**

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
<a id="gameobject-isactive"></a>

#### **`GameObject::IsActive()`**

 Check if the GameObject is active

Signature:
```cpp
 inline bool IsActive() const noexcept;
```
**Returns:** true if the GameObject is active, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-setactive"></a>

#### **`GameObject::SetActive()`**

 Set the active state of the GameObject

Signature:
```cpp
 void SetActive(bool active = true) noexcept;
```
**Parameters:**
- `active`: true to set the GameObject as active, false to set it as inactive

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-gettags"></a>

#### **`GameObject::GetTags()`**

 Get the tags of the GameObject

Signature:
```cpp
 inline std::vector<std::string> GetTags() const noexcept;
```
**Returns:** Vector of tags of the GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-addtag"></a>

#### **`GameObject::AddTag()`**

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
<a id="gameobject-removetag"></a>

#### **`GameObject::RemoveTag()`**

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
<a id="gameobject-hastag"></a>

#### **`GameObject::HasTag()`**

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
<a id="gameobject-cleartags"></a>

#### **`GameObject::ClearTags()`**

 Clear all tags from the GameObject

Signature:
```cpp
 void ClearTags();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-getcomponentcount"></a>

#### **`GameObject::GetComponentCount()`**

 Get the number of components attached to the GameObject

Signature:
```cpp
 size_t GetComponentCount() const noexcept;
```
**Returns:** Number of components attached to the GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-removecomponent"></a>

#### **`GameObject::RemoveComponent()`**

 Remove a component from the GameObject

Signature:
```cpp
 bool RemoveComponent(Syngine::ComponentTypeID type);
```
**Parameters:**
- `type`: Type of the component to remove

**Returns:** True if the component was removed successfully

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-hascomponent"></a>

#### **`GameObject::HasComponent()`**

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
<a id="gameobject-getcomponent"></a>

#### **`GameObject::GetComponent()`**

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
<a id="gameobject-getcomponent-2"></a>

#### **`GameObject::GetComponent()`**

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
<a id="gameobject-addcomponent"></a>

#### **`GameObject::AddComponent()`**

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
<a id="gameobject-getcomponents"></a>

#### **`GameObject::GetComponents()`**

 Get const access to the components map for iteration

Signature:
```cpp
 const std::map<ComponentTypeID, std::unique_ptr<Component>>& GetComponents() const;
```
**Returns:** Const reference to the components map

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-serialize"></a>

#### **`GameObject::Serialize()`**

 Serialize the GameObject and its components into a DataNode for saving

Signature:
```cpp
 Serializer::DataNode Serialize() const;
```
**Returns:** DataNode representing the serialized GameObject

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-setparent"></a>

#### **`GameObject::SetParent()`**

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
<a id="gameobject-getparent"></a>

#### **`GameObject::GetParent()`**

 Get the parent of the GameObject

Signature:
```cpp
 GameObject* GetParent() const;
```
**Returns:** Pointer to the parent GameObject, or nullptr if there is no parent

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobject-addchild"></a>

#### **`GameObject::AddChild()`**

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
<a id="gameobject-removechild"></a>

#### **`GameObject::RemoveChild()`**

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
<a id="gameobject-getchildren"></a>

#### **`GameObject::GetChildren()`**

 Get the children of the GameObject

Signature:
```cpp
 const std::vector<GameObject*>& GetChildren() const;
```
**Returns:** Vector of pointers to the child GameObjects

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Name of the GameObject, used for identification and |
| `std::string` | `type` | Type of the GameObject, used for shaders. |
| `std::string` | `gizmo` | Gizmo type for rendering in the editor, e.g., |
| `std::vector<std::string>` | `tags` | Tags for grouping and identifying GameObjects |
---
