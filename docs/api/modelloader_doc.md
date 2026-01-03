# Syngine API Documentation


## ModelLoader.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Utils/ModelLoader.h)

---

## Goto: 


- [Vertex](#synginevertex)
- [tileDetail](#synginetiledetail)
- [MeshData](#synginemeshdata)
- [static](#synmodelloaderstatic)
- [_UnloadAllMeshes](#synmodelloader_unloadallmeshes)
- [_GetMeshes](#synmodelloader_getmeshes)
- [_GetMeshById](#synmodelloader_getmeshbyid)
- [_LoadModel](#synmodelloader_loadmodel)
- [_ReloadModel](#synmodelloader_reloadmodel)
- [processScene](#synmodelloaderprocessscene)

---

#### **`Syngine::Vertex`**


 Vertex structure for mesh data

Signature:

```cpp
struct Vertex 
```

**This function has been available since:** v0.0.1

---

#### **`Syngine::tileDetail`**


 Material structure for mesh data

Signature:

```cpp
 float tileDetail = 30.0f; //* How many repeats of detail maps
```

**Parameters:**

- `std::string`: name Name of the material
- `bgfx::TextureHandle`: albedo Albedo texture handle
- `bgfx::TextureHandle`: normalMap Normal map texture handle
- `bgfx::TextureHandle`: heightMap Height map texture handle

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
| `float` | `heightScale` | Matches blender displacement |
| `float` | `mixFactor` | Mix between detail and macro maps |
| `float` | `ambient` | Ambient floor |
| `float` | `baseColor[4]` | = { 1.0f, 1.0f, 1.0f, 1.0f }; //* RGBA base color RGBA base color |
| `bool` | `useVertexColor` | Whether to use vertex color or base color |
| `std::vector<Vertex>` | `vertices` | Vertices of the mesh |
| `std::vector<uint32_t>` | `indices` | Indices of the mesh |
| `std::vector<Material>` | `materials` | Materials of the mesh |
| `bgfx::VertexBufferHandle` | `vbh` | Vertex buffer handle |
| `bgfx::IndexBufferHandle` | `ibh` | Index buffer handle |
| `uint32_t` | `numVertices` | Number of vertices in the mesh |
| `uint32_t` | `numIndices` | Number of indices in the mesh |
| `uint8_t` | `numMaterials` | Number of materials in the mesh |
| `bool` | `hasTextures` | Whether the mesh has textures |
| `std::string` | `path` | Path to the mesh file (relative to the game root) |
| `int` | `id` | Unique ID of the mesh |
| `std::filesystem::file_time_type` | `lastWriteTime` | For tracking changes |
| `bool` | `valid` | Meshes are hardly ever actually invalid, but this is useful for reloading |

**This function has been available since:** v0.0.1

---

#### **`SynModelLoader::static`**


 SynModelLoader class for loading 3D models

Signature:

```cpp
 protected: static std::vector<MeshData> loadedMeshes;
```

**This function has been available since:** v0.0.1

---

#### **`SynModelLoader::_UnloadAllMeshes`**


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

#### **`SynModelLoader::_GetMeshes`**


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

#### **`SynModelLoader::_GetMeshById`**


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

#### **`SynModelLoader::_LoadModel`**


 Loads a model from the specified path, returns true if successful

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _LoadModel(MeshData& out, const std::string& path,
```

**Parameters:**

- `out`: MeshData to fill with the loaded model
- `path`: Path to the model file
- `loadTextures`: Whether to load textures for the model

**Returns:** true if the model was loaded successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`SynModelLoader::_ReloadModel`**


 Reloads a model by its ID, returns true if successful

#### This function is internal use only and not intended for public use!


**Note:** This is used to reload models only when they change on disk for hot reloading

Signature:

```cpp
 bool _ReloadModel(MeshData& out, int id) override;
```

**Parameters:**

- `out`: MeshData to fill with the reloaded model
- `id`: ID of the model to reload

**Returns:** true if the model was reloaded successfully, false otherwise

---

#### **`SynModelLoader::processScene`**


 Processes the Assimp scene and fills the MeshData structure

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static bool processScene(MeshData& meshData, const aiScene* scene, const std::string& path, bool loadTextures);
```

**Parameters:**

- `meshData`: MeshData to fill with the processed data
- `scene`: Assimp scene to process
- `path`: Path to the model file
- `loadTextures`: Whether to load textures for the model

**Returns:** true if the scene was processed successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

