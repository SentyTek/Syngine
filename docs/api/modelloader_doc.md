# Syngine API Documentation


## ModelLoader.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Utils/ModelLoader.h)

---

## Goto: 


- [Vertex](#synginevertex)
- [Material](#synginematerial)
- [SubMesh](#synginesubmesh)
- [MeshData](#synginemeshdata)
- [static](#modelloaderstatic)
- [_UnloadAllMeshes](#modelloader_unloadallmeshes)
- [_GetMeshes](#modelloader_getmeshes)
- [_GetMeshById](#modelloader_getmeshbyid)
- [_LoadModel](#assimploader_loadmodel)
- [_ReloadModel](#assimploader_reloadmodel)
- [processScene](#assimploaderprocessscene)
- [_ProcessMaterial](#assimploader_processmaterial)
- [_CreateDefaultMaterial](#assimploader_createdefaultmaterial)

---

#### **`Syngine::Vertex`**


 Vertex structure for mesh data

Signature:

```cpp
struct Vertex
```

**This function has been available since:** v0.0.1

---

#### **`Syngine::Material`**


 Material structure for mesh data

Signature:

```cpp
struct Material
```

**Members:**

| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Name of the material |
| `bgfx::TextureHandle` | `albedo` | Albedo texture handle |
| `BGFX_INVALID_HANDLE` | `Normal` | map texture handle |
| `BGFX_INVALID_HANDLE` | `Height` | map texture handle |
| `float[3]` | `uvScale` | UV scale for each texture type (albedo, normal, height) |
| `float` | `heightScale` | Matches blender displacement |
| `float` | `mixFactor` | Mix between detail and macro maps |
| `float` | `ambient` | Ambient floor |
| `bool` | `useVertexColor` | Whether to use vertex color or base color |
| `float[4]` | `baseColor` | RGBA base color |

**This function has been available since:** v0.0.1

---

#### **`Syngine::SubMesh`**


 SubMesh structure for storing submesh information

Signature:

```cpp
struct SubMesh
```

**Members:**

| Type | Name | Description |
| --- | --- | --- | 
| `uint32_t` | `indexStart` | Starting index in the index buffer for this submesh |
| `uint32_t` | `indexCount` | Number of indices in this submesh |
| `uint8_t` | `materialIndex` | Index of the material used by this submesh |
| `name` | `Name` | of the submesh (for debugging and editor purposes) |
| `float[3]` | `boundMin` | Minimum bounding box coordinates for the submesh |
| `float[3]` | `boundMax` | Maximum bounding box coordinates for the submesh |

**This function has been available since:** v0.0.1

---

#### **`Syngine::MeshData`**


 MeshData structure for storing mesh information

Signature:

```cpp
struct MeshData
```

**Members:**

| Type | Name | Description |
| --- | --- | --- | 
| `std::vector<Vertex>` | `vertices` | List of vertices in the mesh |
| `std::vector<uint32_t>` | `indices` | List of indices for indexed drawing |
| `std::vector<Material>` | `materials` | List of materials used by the mesh |
| `uint8_t` | `numMaterials` | Number of materials used by the mesh |
| `std::vector<SubMesh>` | `subMeshes` | List of submeshes in the mesh |
| `uint8_t` | `numSubMeshes` | Number of submeshes in the mesh |
| `bgfx::VertexBufferHandle` | `vbh` | Handle to the vertex buffer on the GPU |
| `bgfx::IndexBufferHandle` | `ibh` | Handle to the index buffer on the GPU |
| `int` | `id` | Unique ID for the mesh (for hot reloading and editor purposes) |
| `bool` | `valid` | Whether the mesh data is valid and can be rendered |
| `lastWriteTime` | `Last` | write time of the mesh file (for hot reloading) |

**This function has been available since:** v0.0.1

---

#### **`ModelLoader::static`**


 Model class for loading 3D models

Signature:

```cpp
 protected: static std::vector<MeshData> loadedMeshes;
```

**This function has been available since:** v0.0.1

---

#### **`ModelLoader::_UnloadAllMeshes`**


 Unloads all loaded models

#### This function is internal use only and not intended for public use!


**Note:** This is used to clear all loaded models, for example when the game is shutting down

**Postconditions:** All loaded models are unloaded and their resources are freed

Signature:

```cpp
 static void _UnloadAllMeshes();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`ModelLoader::_GetMeshes`**


 Get all loaded meshes

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static std::vector<MeshData>& _GetMeshes();
```

**Returns:** std::vector<MeshData>& A reference to the vector of all loaded meshes

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`ModelLoader::_GetMeshById`**


 Get a mesh by its ID

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static MeshData* _GetMeshById(int id);
```

**Parameters:**

- `id`: ID of the mesh to get

**Returns:** MeshData* Pointer to the mesh with the given ID, nullptr if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`AssimpLoader::_LoadModel`**


 AssimpLoader class for loading 3D models using Assimp

Signature:

```cpp
 public: /// @brief Loads a model from the specified path, returns true if /// successful /// @param out MeshData to fill with the loaded model /// @param meshStream Stream containing the model data /// @param loadTextures Whether to load textures for the model /// @return true if the model was loaded successfully, false otherwise /// @threadsafety not-safe /// @since v0.0.1 /// @internal bool _LoadModel(MeshData& out, scl::stream* meshStream, const std::string& assetPath, bool loadTextures) override;
```

**This function has been available since:** v0.0.1

---

#### **`AssimpLoader::_ReloadModel`**


 Reloads a model by its ID, returns true if successful

#### This function is internal use only and not intended for public use!


**Note:** This is used to reload models only when they change on disk for hot reloading

Signature:

```cpp
 bool _ReloadModel(MeshData& out, scl::stream* stream, const std::string& assetPath, int id) override;
```

**Parameters:**

- `out`: MeshData to fill with the reloaded model
- `id`: ID of the model to reload

**Returns:** true if the model was reloaded successfully, false otherwise

---

#### **`AssimpLoader::processScene`**


 Processes the Assimp scene and fills the MeshData structure

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static bool processScene(MeshData& out, const aiScene* scene, scl::stream* meshStream, bool loadTextures);
```

**Parameters:**

- `out`: MeshData to fill with the processed data
- `scene`: Assimp scene to process
- `meshStream`: Stream containing the model data (for resolving relative texture paths)
- `loadTextures`: Whether to load textures for the model

**Returns:** true if the scene was processed successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`AssimpLoader::_ProcessMaterial`**


 Processes an Assimp material and fills the Material structure

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static Material _ProcessMaterial(aiMaterial* aiMat, const aiScene* scene, scl::stream* meshStream, bool loadTextures);
```

**Parameters:**

- `aiMat`: Assimp material to process
- `meshStream`: Stream containing the model data (for resolving relative texture paths)
- `loadTextures`: Whether to load textures for the material

**Returns:** Material structure filled with the processed material data

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`AssimpLoader::_CreateDefaultMaterial`**


 Creates a default material with no textures

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static Material _CreateDefaultMaterial();
```

**Returns:** Material structure filled with default material data

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

