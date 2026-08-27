# Syngine API Documentation

## GameObjectRegistry.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Scene/GameObjectRegistry.h)

Registry class to manage GameObjects. Owns all GameObjects, stores indexed sublists of GameObjects for fast lookups

---
## Goto: 


## Additional Functions: 

### Functions: 

- [CreateGameObject()](#gameobjectregistry-creategameobject)
- [Instantiate()](#gameobjectregistry-instantiate)
- [CloneGameObject()](#gameobjectregistry-clonegameobject)
- [RemoveGameObject()](#gameobjectregistry-removegameobject)
- [RemoveGameObjectById()](#gameobjectregistry-removegameobjectbyid)
- [Clear()](#gameobjectregistry-clear)
- [GetGameObjectByName()](#gameobjectregistry-getgameobjectbyname)
- [GetGameObjectsByType()](#gameobjectregistry-getgameobjectsbytype)
- [GetGameObjectById()](#gameobjectregistry-getgameobjectbyid)
- [GetGameObjectsWithComponent()](#gameobjectregistry-getgameobjectswithcomponent)
- [GetAllGameObjects()](#gameobjectregistry-getallgameobjects)
- [GetPhysicsObjects()](#gameobjectregistry-getphysicsobjects)
- [GetRenderableObjects()](#gameobjectregistry-getrenderableobjects)
- [GetScriptedObjects()](#gameobjectregistry-getscriptedobjects)
- [GetGizmos()](#gameobjectregistry-getgizmos)
- [GetGameObjectCount()](#gameobjectregistry-getgameobjectcount)
- [_NotifyComponentAdded()](#gameobjectregistry-_notifycomponentadded)
- [_NotifyComponentRemoved()](#gameobjectregistry-_notifycomponentremoved)
- [GetFirstActiveDirectionalLight()](#gameobjectregistry-getfirstactivedirectionallight)

---
<a id="gameobjectregistry-creategameobject"></a>

#### **`GameObjectRegistry::CreateGameObject()`**

 Create a new GameObject and add it to the registry. The GameObject is owned by the registry and will be deleted when the registry is cleared or the GameObject is removed.

Signature:
```cpp
 static GameObject& CreateGameObject(std::string name, std::string type = "default", std::vector<std::string> tags = {}) noexcept;
```
**Parameters:**
- `name`: The name of the GameObject.
- `type`: The type of the GameObject.
- `tags`: A vector of tags to assign to the GameObject.

**Returns:** A reference to the newly created GameObject.

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="gameobjectregistry-instantiate"></a>

#### **`GameObjectRegistry::Instantiate()`**

 Create a new GameObject from a serialized DataNode and add it to the registry. The GameObject is owned by the registry and will be deleted when the registry is cleared or the GameObject is removed.

Signature:
```cpp
 static GameObject& Instantiate(const Serializer::Prefab& prefab) noexcept;
```
**Parameters:**
- `dataNode`: The DataNode containing the serialized GameObject data.

**Returns:** A reference to the newly created GameObject.

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="gameobjectregistry-clonegameobject"></a>

#### **`GameObjectRegistry::CloneGameObject()`**

 Clone an existing GameObject and add the clone to the registry.

Signature:
```cpp
 static GameObject& CloneGameObject(const GameObject& original) noexcept;
```
**Parameters:**
- `original`: The GameObject to clone.

**Returns:** A reference to the newly created clone GameObject.

**Thread Safety:** safe

**This function has been available since:** v0.0.2

---
<a id="gameobjectregistry-removegameobject"></a>

#### **`GameObjectRegistry::RemoveGameObject()`**

 Remove a GameObject from the registry. Defers until the end of the frame.

Signature:
```cpp
 static void RemoveGameObject(GameObject* GameObject) noexcept;
```
**Parameters:**
- `GameObject`: The GameObject to remove.

**Returns:** true if the GameObject was removed, false if it was not found.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-removegameobjectbyid"></a>

#### **`GameObjectRegistry::RemoveGameObjectById()`**

 Remove a GameObject by ID. Defers until the end of the frame.

Signature:
```cpp
 static void RemoveGameObjectById(int id) noexcept;
```
**Parameters:**
- `id`: The ID of the GameObject to remove.

**Returns:** true if the GameObject was removed, false if it was not found.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-clear"></a>

#### **`GameObjectRegistry::Clear()`**

 Clear all objects

**Postconditions:** All GameObjects are removed from the registry.

Signature:
```cpp
 static void Clear() noexcept;
```
**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getgameobjectbyname"></a>

#### **`GameObjectRegistry::GetGameObjectByName()`**

 Get a GameObject by name. Only returns the first match. Is slow.

Signature:
```cpp
 static const GameObject* GetGameObjectByName(std::string_view name) noexcept;
```
**Parameters:**
- `name`: The name of the GameObject to find.

**Returns:** The GameObject with the given name, nullptr if not found.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getgameobjectsbytype"></a>

#### **`GameObjectRegistry::GetGameObjectsByType()`**

 Get all GameObjects of a specific type, as defined by the `type` parameter when creating the GameObject.

Signature:
```cpp
 static std::vector<GameObject*> GetGameObjectsByType(std::string_view type) noexcept;
```
**Parameters:**
- `type`: The type of GameObject to find.

**Returns:** A vector of GameObjects of the specified type.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getgameobjectbyid"></a>

#### **`GameObjectRegistry::GetGameObjectById()`**

 Get a GameObject by ID.

Signature:
```cpp
 static const GameObject* GetGameObjectById(int id) noexcept;
```
**Parameters:**
- `id`: The ID of the GameObject to find.

**Returns:** The GameObject with the given ID.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getgameobjectswithcomponent"></a>

#### **`GameObjectRegistry::GetGameObjectsWithComponent()`**

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
<a id="gameobjectregistry-getallgameobjects"></a>

#### **`GameObjectRegistry::GetAllGameObjects()`**

 Get all GameObjects in the registry.

Signature:
```cpp
 static inline const std::unordered_map<int, GameObject>& GetAllGameObjects() noexcept;
```
**Returns:** A reference to the unordered map of all GameObjects. The map is indexed by GameObject ID.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getphysicsobjects"></a>

#### **`GameObjectRegistry::GetPhysicsObjects()`**

 Get all GameObjects that have physics enabled.

Signature:
```cpp
 static inline std::vector<GameObject*> GetPhysicsObjects() noexcept;
```
**Returns:** A vector of GameObjects that have physics enabled.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getrenderableobjects"></a>

#### **`GameObjectRegistry::GetRenderableObjects()`**

 Get all GameObjects that are renderable (have Mesh and Transform components).

Signature:
```cpp
 static inline std::vector<GameObject*> GetRenderableObjects() noexcept;
```
**Returns:** A vector of GameObjects that are renderable.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getscriptedobjects"></a>

#### **`GameObjectRegistry::GetScriptedObjects()`**

 Get all GameObjects that have a script attached.

Signature:
```cpp
 static inline std::vector<GameObject*> GetScriptedObjects() noexcept;
```
**Returns:** A vector of GameObjects that have a script attached.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getgizmos"></a>

#### **`GameObjectRegistry::GetGizmos()`**

 Get all GameObjects that are gizmos.

Signature:
```cpp
 static inline std::vector<GameObject*> GetGizmos() noexcept;
```
**Returns:** A vector of GameObjects that are gizmos.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-getgameobjectcount"></a>

#### **`GameObjectRegistry::GetGameObjectCount()`**

 Get the total number of GameObjects in the registry.

Signature:
```cpp
 static inline size_t GetGameObjectCount() noexcept;
```
**Returns:** The number of GameObjects in the registry.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="gameobjectregistry-_notifycomponentadded"></a>

#### **`GameObjectRegistry::_NotifyComponentAdded()`**

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
<a id="gameobjectregistry-_notifycomponentremoved"></a>

#### **`GameObjectRegistry::_NotifyComponentRemoved()`**

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
<a id="gameobjectregistry-getfirstactivedirectionallight"></a>

#### **`GameObjectRegistry::GetFirstActiveDirectionalLight()`**

 Get the first active directional light in the scene.

Signature:
```cpp
 static DirectionalLightComponent* GetFirstActiveDirectionalLight() noexcept;
```
**Returns:** A pointer to the first active DirectionalLightComponent, or nullptr if none are active.

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
