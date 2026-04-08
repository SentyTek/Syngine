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
- [Constructor](#class-constructor)
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
- [GetSubmeshCount](#meshcomponentgetsubmeshcount)
- [GetSubmeshMaterialIndex](#meshcomponentgetsubmeshmaterialindex)
- [SetSubmeshMaterialIndex](#meshcomponentsetsubmeshmaterialindex)
- [GetMaterialUVScale](#meshcomponentgetmaterialuvscale)
- [SetMaterialUVScale](#meshcomponentsetmaterialuvscale)
- [GetObjectUVScaleOverride](#meshcomponentgetobjectuvscaleoverride)
- [SetObjectUVScaleOverride](#meshcomponentsetobjectuvscaleoverride)

---

## Class Constructor


#### **`MeshComponent::MeshComponent`**


 Constructor for the MeshComponent class

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 MeshComponent(GameObject* owner, const std::string& path, bool loadTextures = true);
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

## Class Constructor


#### **`MeshComponent::MeshComponent`**


 Constructor for the MeshComponent class

**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 MeshComponent(GameObject* owner, const std::string& bundlePath, const std::string& texturePath, bool loadTextures = true);
```

**Parameters:**

- `owner`: Pointer to the GameObject that owns this component
- `bundlePath`: Path to the shader bundle containing the mesh
- `texturePath`: Path to the mesh within the bundle
- `loadTextures`: Whether to load textures for the model

**This function has been available since:** v0.0.1

---

## Class & Related Members


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
 void Init(const std::string& bundlePath, const std::string& texturePath, bool loadTextures = true);
```

**Parameters:**

- `bundlePath`: Path to the shader bundle containing the mesh
- `texturePath`: Path to the mesh within the bundle
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
 bool LoadMesh(const std::string& bundlePath, const std::string& texturePath, bool loadTextures = true);
```

**Parameters:**

- `bundlePath`: Path to the shader bundle containing the mesh
- `texturePath`: Path to the mesh within the bundle
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

#### **`MeshComponent::GetSubmeshCount`**


 Get the submesh count

Signature:

```cpp
 uint8_t GetSubmeshCount() const;
```

**Returns:** Number of submeshes in the mesh

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::GetSubmeshMaterialIndex`**


 Get material index for a specific submesh

Signature:

```cpp
 uint8_t GetSubmeshMaterialIndex(uint8_t submeshIndex) const;
```

**Parameters:**

- `submeshIndex`: Index of the submesh to query

**Returns:** Material index for the specified submesh, or 255 if invalid index

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::SetSubmeshMaterialIndex`**


 Assign a material to a specific submesh

Signature:

```cpp
 bool SetSubmeshMaterialIndex(uint8_t submeshIndex, uint8_t materialIndex);
```

**Parameters:**

- `submeshIndex`: Index of the submesh to assign the material to
- `materialIndex`: Index of the material to assign to the submesh

**Returns:** true if the assignment was successful, false if the submesh index is invalid

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::GetMaterialUVScale`**


 Get UV scale for specific texture type and material

Signature:

```cpp
 float* GetMaterialUVScale(uint8_t materialIndex, uint8_t textureType) const;
```

**Parameters:**

- `materialIndex`: Index of the material to query
- `textureType`: Type of the texture (0 = albedo, 1 = normal, 2 = height)

**Returns:** UV scale for the specified material and texture type

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::SetMaterialUVScale`**


 Set UV scale for specific texture type and material

Signature:

```cpp
 bool SetMaterialUVScale(uint8_t materialIndex, uint8_t textureType, float uvScale[2]);
```

**Parameters:**

- `materialIndex`: Index of the material to modify
- `textureType`: Type of the texture (0 = albedo, 1 = normal, 2 = height)
- `uvScale`: UV scale to set for the specified material and texture type

**Returns:** true if the UV scale was successfully set, false if the material index is invalid

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::GetObjectUVScaleOverride`**


 Gets the UV scale override for the whole object

Signature:

```cpp
 float GetObjectUVScaleOverride() const;
```

**Returns:** UV scale override for the whole object. 1.0f by default.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`MeshComponent::SetObjectUVScaleOverride`**


 Sets the UV scale override for the whole object

Signature:

```cpp
 void SetObjectUVScaleOverride(float uvScaleOverride);
```

**Parameters:**

- `uvScaleOverride`: UV scale override to set for the whole object

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `bool` | `castShadows` | Whether the mesh casts shadows |
| `bool` | `receiveSunLight` | Whether the mesh receives sunlight |
| `MeshData` | `meshData` | Mesh data for the GameObject |
| `mutable` | `MeshAABB` | Axis-aligned bounding box of the mesh |
| `float` | `m_objectUVScaleOverride` | UV scale override for the whole object |
| `std::string` | `m_bundlePath` | Path to the shader bundle containing the mesh |
| `std::string` | `m_texturePath` | Path to the mesh within the bundle |

---

