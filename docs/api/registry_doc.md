# Syngine API Documentation

## Registry.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Registry.h)

Registry class to manage GameObjects. Owns all GameObjects, stores indexed sublists of GameObjects for fast lookups

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


## Additional Functions: 

### Functions: 

- [AddGameObject()](#registry-addgameobject)
- [RemoveGameObject()](#registry-removegameobject)
- [RemoveGameObjectById()](#registry-removegameobjectbyid)
- [Clear()](#registry-clear)
- [GetGameObjectByName()](#registry-getgameobjectbyname)
- [GetGameObjectsByType()](#registry-getgameobjectsbytype)
- [GetGameObjectById()](#registry-getgameobjectbyid)
- [GetGameObjectsWithComponent()](#registry-getgameobjectswithcomponent)
- [GetAllGameObjects()](#registry-getallgameobjects)
- [GetPhysicsObjects()](#registry-getphysicsobjects)
- [GetRenderableObjects()](#registry-getrenderableobjects)
- [GetScriptedObjects()](#registry-getscriptedobjects)
- [GetGizmos()](#registry-getgizmos)
- [GetGameObjectCount()](#registry-getgameobjectcount)
- [_NotifyComponentAdded()](#registry-_notifycomponentadded)
- [_NotifyComponentRemoved()](#registry-_notifycomponentremoved)

---
<a id="registry-addgameobject"></a>

#### **`Registry::AddGameObject()`**

 Add a GameObject to the registry.

Signature:
```cpp
 static int AddGameObject(GameObject* GameObject) noexcept;
```
**Parameters:**
- `GameObject`: The GameObject to add.

**Returns:** 0 on success, -1 if failure.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="registry-removegameobject"></a>

#### **`Registry::RemoveGameObject()`**

 Remove a GameObject from the registry.

Signature:
```cpp
 static int RemoveGameObject(GameObject* GameObject) noexcept;
```
**Parameters:**
- `GameObject`: The GameObject to remove.

**Returns:** 0 on success, 1 if the GameObject was not found.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="registry-removegameobjectbyid"></a>

#### **`Registry::RemoveGameObjectById()`**

 Remove a GameObject by ID.

Signature:
```cpp
 static int RemoveGameObjectById(int id) noexcept;
```
**Parameters:**
- `id`: The ID of the GameObject to remove.

**Returns:** 0 on success, 1 if the GameObject was not found.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="registry-clear"></a>

#### **`Registry::Clear()`**

 Clear all objects

**Postconditions:** All GameObjects are removed from the registry.

Signature:
```cpp
 static void Clear() noexcept;
```
**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="registry-getgameobjectbyname"></a>

#### **`Registry::GetGameObjectByName()`**

 Get a GameObject by name. Only returns the first match. Is slow.

Signature:
```cpp
 static GameObject* GetGameObjectByName(const std::string_view& name) noexcept;
```
**Parameters:**
- `name`: The name of the GameObject to find.

**Returns:** GameObject* The GameObject with the given name, nullptr if not found.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getgameobjectsbytype"></a>

#### **`Registry::GetGameObjectsByType()`**

 Get all GameObjects of a specific type, as defined by the `type` parameter when creating the GameObject.

Signature:
```cpp
 static std::vector<GameObject*> GetGameObjectsByType(const std::string_view& type) noexcept;
```
**Parameters:**
- `type`: The type of GameObject to find.

**Returns:** std::vector A vector of GameObjects of the specified type.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getgameobjectbyid"></a>

#### **`Registry::GetGameObjectById()`**

 Get a GameObject by ID.

Signature:
```cpp
 static GameObject* GetGameObjectById(int id) noexcept;
```
**Parameters:**
- `id`: The ID of the GameObject to find.

**Returns:** GameObject* The GameObject with the given ID, nullptr if not found.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getgameobjectswithcomponent"></a>

#### **`Registry::GetGameObjectsWithComponent()`**

 Gets all ``GameObjects`` with a specific component.

Signature:
```cpp
 static std::vector<GameObject*> GetGameObjectsWithComponent(Syngine::ComponentTypeID type) noexcept;
```
**Parameters:**
- `type`: The component type to search for.

**Returns:** A vector of GameObject pointers with the component.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getallgameobjects"></a>

#### **`Registry::GetAllGameObjects()`**

 Get all GameObjects in the registry.

Signature:
```cpp
 static inline std::unordered_map<int, GameObject*>& GetAllGameObjects() noexcept;
```
**Returns:** std::unordered_map A reference to the unordered map of all GameObjects. The map is indexed by GameObject ID.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getphysicsobjects"></a>

#### **`Registry::GetPhysicsObjects()`**

 Get all GameObjects that have physics enabled.

Signature:
```cpp
 static inline std::vector<GameObject*> GetPhysicsObjects() noexcept;
```
**Returns:** std::vector A vector of GameObjects that have physics enabled.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getrenderableobjects"></a>

#### **`Registry::GetRenderableObjects()`**

 Get all GameObjects that are renderable (have Mesh and Transform components).

Signature:
```cpp
 static inline std::vector<GameObject*> GetRenderableObjects() noexcept;
```
**Returns:** std::vector A vector of GameObjects that are renderable.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getscriptedobjects"></a>

#### **`Registry::GetScriptedObjects()`**

 Get all GameObjects that have a script attached.

Signature:
```cpp
 static inline std::vector<GameObject*> GetScriptedObjects() noexcept;
```
**Returns:** std::vector A vector of GameObjects that have a script attached.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getgizmos"></a>

#### **`Registry::GetGizmos()`**

 Get all GameObjects that are gizmos.

Signature:
```cpp
 static inline std::vector<GameObject*> GetGizmos() noexcept;
```
**Returns:** std::vector A vector of GameObjects that are gizmos.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-getgameobjectcount"></a>

#### **`Registry::GetGameObjectCount()`**

 Get the total number of GameObjects in the registry.

Signature:
```cpp
 static inline size_t GetGameObjectCount() noexcept;
```
**Returns:** size_t The number of GameObjects in the registry.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="registry-_notifycomponentadded"></a>

#### **`Registry::_NotifyComponentAdded()`**

 Internal call to notify the registry that a component has been added to a GameObject. This is used to update the indexed sublists.

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _NotifyComponentAdded(GameObject* gameobject, Syngine::ComponentTypeID type) noexcept;
```
**Parameters:**
- `gameobject`: The GameObject to which the component was added.
- `type`: The type of component that was added.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="registry-_notifycomponentremoved"></a>

#### **`Registry::_NotifyComponentRemoved()`**

 Internal call to notify the registry that a component has been removed from a GameObject. This is used to update the indexed sublists.

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _NotifyComponentRemoved(GameObject* gameobject, Syngine::ComponentTypeID type) noexcept;
```
**Parameters:**
- `gameobject`: The GameObject from which the component was removed.
- `type`: The type of component that was removed.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
