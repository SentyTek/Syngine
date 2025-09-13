# Syngine API Documentation


## MeshComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/MeshComponent.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [GetComponentType](#synginegetcomponenttype)
- [Init](#syngineinit)
- [Update](#syngineupdate)
- [LoadMesh](#syngineloadmesh)
- [ReloadMesh](#synginereloadmesh)
- [UnloadMesh](#syngineunloadmesh)

---

## Class Constructor


#### **`Syngine::MeshComponent`**


 Constructor for the MeshComponent class

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 MeshComponent(GameObject* owner, const std::string& path = "",
```

**Parameters:**

- `owner`: Pointer to the GameObject that owns this component
- `path`: Path to the model file
- `loadTextures`: Whether to load textures for the model

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::GetComponentType`**


 Get the type of this component

Signature:

```cpp
 Components GetComponentType() override;
```

**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::Init`**


 Initialize the mesh component

**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init(const std::string& path = "", bool loadTextures = true);
```

**Parameters:**

- `path`: Path to the model file
- `loadTextures`: Whether to load textures for the model

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


 Update the mesh component

**Note:** There is no specific update logic for the mesh component

Signature:

```cpp
 void Update() {};
```

**This function has been available since:** v0.0.1

---

#### **`Syngine::LoadMesh`**


 Load a mesh from a file

Signature:

```cpp
 int LoadMesh(const std::string& path, bool loadTextures = true);
```

**Parameters:**

- `path`: Path to the model file
- `loadTextures`: Whether to load textures for the model

**Returns:** 0 on success, non-zero code on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::ReloadMesh`**


 Reload the mesh from the file

**Note:** Mostly used for reloading the mesh after changes in the model file

Signature:

```cpp
 int ReloadMesh();
```

**Returns:** 0 on success, non-zero on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::UnloadMesh`**


 Unload the mesh

#### This function is internal use only and not intended for public use!


**Note:** This is called when the component is removed or the GameObject is destroyed

Signature:

```cpp
 int UnloadMesh(); //TODO: Make this RAII in the destructor
```

**Returns:** 0 on success, non-zero on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `MeshData` | `meshData` | Mesh data for the GameObject |

---

