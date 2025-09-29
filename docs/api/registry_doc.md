# Syngine API Documentation


## Registry.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Registry.h)

---

## Goto: 


- [AddGameObject](#registryaddgameobject)
- [RemoveGameObject](#registryremovegameobject)
- [RemoveGameObjectById](#registryremovegameobjectbyid)
- [Clear](#registryclear)
- [GetGameObjectByName](#registrygetgameobjectbyname)
- [GetGameObjectsByType](#registrygetgameobjectsbytype)
- [GetGameObjectById](#registrygetgameobjectbyid)
- [GetGameObjectsWithComponent](#registrygetgameobjectswithcomponent)
- [GetAllGameObjects](#registrygetallgameobjects)
- [GetPhysicsObjects](#registrygetphysicsobjects)
- [GetRenderableObjects](#registrygetrenderableobjects)
- [GetScriptedObjects](#registrygetscriptedobjects)
- [GetGizmos](#registrygetgizmos)
- [GetGameObjectCount](#registrygetgameobjectcount)
- [_NotifyComponentAdded](#registry_notifycomponentadded)
- [_NotifyComponentRemoved](#registry_notifycomponentremoved)

---

#### **`Registry::AddGameObject`**




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

#### **`Registry::RemoveGameObject`**


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

#### **`Registry::RemoveGameObjectById`**


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

#### **`Registry::Clear`**


 Clear all objects

**Postconditions:** All GameObjects are removed from the registry.

Signature:

```cpp
 static void Clear() noexcept;
```

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`Registry::GetGameObjectByName`**


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

#### **`Registry::GetGameObjectsByType`**


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

#### **`Registry::GetGameObjectById`**


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

#### **`Registry::GetGameObjectsWithComponent`**


 Gets all ``GameObjects`` with a specific component.

Signature:

```cpp
 static std::vector<GameObject*> GetGameObjectsWithComponent(Syngine::Components type) noexcept;
```

**Parameters:**

- `type`: The component type to search for.

**Returns:** A vector of GameObject pointers with the component.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Registry::GetAllGameObjects`**


 Get all GameObjects in the registry.

Signature:

```cpp
 static inline std::unordered_map<int, GameObject*>& GetAllGameObjects() noexcept { return m_AllObjects;
```

**Returns:** std::unordered_map A reference to the unordered map of all GameObjects. The map is indexed by GameObject ID.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Registry::GetPhysicsObjects`**


 Get all GameObjects that have physics enabled.

Signature:

```cpp
 static inline std::vector<GameObject*> GetPhysicsObjects() noexcept { return m_PhysicsObjects;
```

**Returns:** std::vector A vector of GameObjects that have physics enabled.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Registry::GetRenderableObjects`**


 Get all GameObjects that are renderable (have Mesh and Transform components).

Signature:

```cpp
 static inline std::vector<GameObject*> GetRenderableObjects() noexcept { return m_RenderableObjects;
```

**Returns:** std::vector A vector of GameObjects that are renderable.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Registry::GetScriptedObjects`**


 Get all GameObjects that have a script attached.

Signature:

```cpp
 static inline std::vector<GameObject*> GetScriptedObjects() noexcept { return m_ScriptedObjects;
```

**Returns:** std::vector A vector of GameObjects that have a script attached.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Registry::GetGizmos`**


 Get all GameObjects that are gizmos.

Signature:

```cpp
 static inline std::vector<GameObject*> GetGizmos() noexcept { return m_Gizmos;
```

**Returns:** std::vector A vector of GameObjects that are gizmos.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Registry::GetGameObjectCount`**


 Get the total number of GameObjects in the registry.

Signature:

```cpp
 static inline size_t GetGameObjectCount() noexcept { return m_AllObjects.size();
```

**Returns:** size_t The number of GameObjects in the registry.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Registry::_NotifyComponentAdded`**


 Internal call to notify the registry that a component has been added to a GameObject. This is used to update the indexed sublists.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static void _NotifyComponentAdded(GameObject* gameobject, Syngine::Components type) noexcept;
```

**Parameters:**

- `gameobject`: The GameObject to which the component was added.
- `type`: The type of component that was added.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`Registry::_NotifyComponentRemoved`**


 Internal call to notify the registry that a component has been removed from a GameObject. This is used to update the indexed sublists.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static void _NotifyComponentRemoved(GameObject* gameobject, Syngine::Components type) noexcept;
```

**Parameters:**

- `gameobject`: The GameObject from which the component was removed.
- `type`: The type of component that was removed.

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

