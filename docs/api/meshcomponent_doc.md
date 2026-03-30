# Syngine API Documentation


## MeshComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/MeshComponent.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [MeshAABB](#synginemeshaabb)
- [bool](#meshcomponentbool)
- [GetComponentType](#meshcomponentgetcomponenttype)
- [Clone](#meshcomponentclone)
- [Serialize](#meshcomponentserialize)
- [Init](#meshcomponentinit)
- [Update](#meshcomponentupdate)
- [LoadMesh](#meshcomponentloadmesh)
- [ReloadMesh](#meshcomponentreloadmesh)
- [UnloadMesh](#meshcomponentunloadmesh)
- [IsMeshLoaded](#meshcomponentismeshloaded)
- [UploadMesh](#meshcomponentuploadmesh)
- [GetAABB](#meshcomponentgetaabb)

---

## Class Constructor


#### **`MeshComponent::MeshComponent`**


 Constructor for the MeshComponent class

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 MeshComponent(GameObject* owner, const std::string& path = "", bool loadTextures = true);
```

**Parameters:**

- `owner`: Pointer to the GameObject that owns this component
- `path`: Path to the model file
- `loadTextures`: Whether to load textures for the model

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::MeshAABB`**


 Structure representing an axis-aligned bounding box (AABB)

Signature:

```cpp
struct MeshAABB
```

**Members:**

| Type | Name | Description |
| --- | --- | --- | 
| `float[3]` | `min` | Minimum corner of the AABB |
| `float[3]` | `max` | Maximum corner of the AABB |
| `float[3]` | `center` | Center of the AABB |
| `float[3]` | `halfExtents` | Half extents of the AABB |

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::bool`**


 Component for rendering 3D mesh models. Supports loading 3D asset files and uploading custom mesh data.

Signature:

```cpp
 public: bool receiveShadows = true; //* Whether the mesh receives shadows
```

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::GetComponentType`**


 Get the type of this component

Signature:

```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```

**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::Clone`**


 Clone the MeshComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override;
```

**Returns:** A unique pointer to the cloned MeshComponent

---

#### **`MeshComponent::Serialize`**


 Serializes the MeshComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the MeshComponent's state

---

#### **`MeshComponent::Init`**


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

#### **`MeshComponent::Update`**


 Update the mesh component. Unused.

**Note:** There is no specific update logic for the mesh component

Signature:

```cpp
 void Update(float deltaTime) override;
```

**Parameters:**

- `deltaTime`: Time elapsed since the last update in seconds

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::LoadMesh`**


 Load a mesh from a file

Signature:

```cpp
 bool LoadMesh(const std::string& path, bool loadTextures = true);
```

**Parameters:**

- `path`: Path to the model file
- `loadTextures`: Whether to load textures for the model

**Returns:** 0 on success, non-zero code on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::ReloadMesh`**


 Reload the mesh from the file

**Note:** Mostly used for reloading the mesh after changes in the model file

Signature:

```cpp
 bool ReloadMesh();
```

**Returns:** 0 on success, non-zero on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::UnloadMesh`**


 Unload the mesh

#### This function is internal use only and not intended for public use!


**Note:** This is called when the component is removed or the GameObject is destroyed

Signature:

```cpp
 bool UnloadMesh();
```

**Returns:** 0 on success, non-zero on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::IsMeshLoaded`**


 Check if the mesh is loaded

Signature:

```cpp
 bool IsMeshLoaded() const;
```

**Returns:** true if the mesh is loaded, false otherwise

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::UploadMesh`**


 Upload mesh data directly

Signature:

```cpp
 bool UploadMesh(std::vector<float> vertices, std::vector<uint32_t> indices, std::vector<uint8_t> baseColor = {});
```

**Parameters:**

- `vertices`: Vector of vertex data. Expected format per vertex: [pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uv0.u, uv0.v, color.r, color.g, color.b, color.a] However, if you provide a baseColor, do NOT include color data in the vertices.
- `indices`: Vector of index data.
- `baseColor`: Optional base color to apply to the entire mesh if vertex colors are not provided. Format: [r, g, b, [a]] (0-255, a is optional)

**Returns:** true on success, false on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::GetAABB`**


 Get the axis-aligned bounding box (AABB) of the mesh

Signature:

```cpp
 MeshAABB& GetAABB();
```

**Returns:** Reference to the MeshAABB structure

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `bool` | `castShadows` | Whether the mesh casts shadows |
| `bool` | `receiveSunLight` | Whether the mesh receives sunlight |
| `MeshData` | `meshData` | Mesh data for the GameObject |
| `mutable` | `MeshAABB` | Axis-aligned bounding box of the mesh |

---

