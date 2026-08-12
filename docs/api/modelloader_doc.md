# Syngine API Documentation

## ModelLoader.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Resources/ModelLoader.h)

AssimpLoader class for loading 3D models using Assimp @section ModelLoader

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 


## Additional Functions: 

### Enums and Structs: 

- [Vertex](#syngine-vertex)
- [SubMesh](#syngine-submesh)
- [ModelData](#syngine-modeldata)

### Functions: 

- [_UnloadAllMeshes()](#modelloader-_unloadallmeshes)
- [_GetMeshes()](#modelloader-_getmeshes)
- [_GetMeshById()](#modelloader-_getmeshbyid)
- [_LoadModel()](#assimploader-_loadmodel)
- [_ReloadModel()](#assimploader-_reloadmodel)
- [processScene()](#assimploader-processscene)
- [_ProcessMaterial()](#assimploader-_processmaterial)

---
<a id="syngine-vertex"></a>

#### **`Syngine::Vertex()`**

 Vertex structure for mesh data

Signature:
```cpp
struct Vertex
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `Math::Vector3` | `pos` | Position of the vertex in 3D space |
| `Math::Vector3` | `normal` | Normal vector at the vertex |
| `Math::Vector2` | `uv0` | Primary texture coordinates (macro UV) |
| `Math::Vector2` | `uv1` | Secondary texture coordinates (for detail maps) |
| `Math::Vector4` | `color` | Vertex color (RGBA) |
| `Math::Vector4` | `tangent` | Tangent vector at the vertex (for normal mapping) |
**This function has been available since:** v0.0.1

---
<a id="syngine-submesh"></a>

#### **`Syngine::SubMesh()`**

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
| `Math::Vector3` | `boundMin` | Minimum corner of the axis-aligned bounding box |
| `Math::Vector3` | `boundMax` | Maximum corner of the axis-aligned bounding box |
**This function has been available since:** v0.0.1

---
<a id="syngine-modeldata"></a>

#### **`Syngine::ModelData()`**

 ModelData structure for storing mesh information

Signature:
```cpp
struct ModelData
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::vector<Vertex>` | `vertices` | List of vertices in the mesh |
| `std::vector<uint32_t>` | `indices` | List of indices for indexed drawing |
| `std::vector<SubMesh>` | `subMeshes` | List of submeshes in the mesh |
| `materials` | `Per-mesh` | material instances used by the mesh |
| `uint8_t` | `numSubMeshes` | Number of submeshes in the mesh |
| `uint8_t` | `numMaterials` | Number of materials used by the mesh |
| `bgfx::VertexBufferHandle` | `vbh` | Handle to the vertex buffer on the GPU |
| `bgfx::IndexBufferHandle` | `ibh` | Handle to the index buffer on the GPU |
| `int` | `id` | Unique ID for the mesh (for hot reloading and editor purposes) |
| `bool` | `valid` | Whether the mesh data is valid and can be rendered |
| `lastWriteTime` | `Last` | write time of the mesh file (for hot reloading) |
| `localMin` | `Minimum` | corner of the local axis-aligned bounding box |
| `localMax` | `Maximum` | corner of the local axis-aligned bounding box |
**This function has been available since:** v0.0.1

---
<a id="modelloader-_unloadallmeshes"></a>

#### **`ModelLoader::_UnloadAllMeshes()`**

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
<a id="modelloader-_getmeshes"></a>

#### **`ModelLoader::_GetMeshes()`**

 Get all loaded meshes

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static std::vector<ModelData>& _GetMeshes();
```
**Returns:** std::vector<ModelData>& A reference to the vector of all loaded models

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="modelloader-_getmeshbyid"></a>

#### **`ModelLoader::_GetMeshById()`**

 Get a mesh by its ID

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static ModelData* _GetMeshById(int id);
```
**Parameters:**
- `id`: ID of the mesh to get

**Returns:** ModelData* Pointer to the mesh with the given ID, nullptr if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="assimploader-_loadmodel"></a>

#### **`AssimpLoader::_LoadModel()`**

 Loads a model from the specified path, returns true if successful

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 bool _LoadModel(ModelData& out, scl::stream* meshStream, const std::string& assetPath, bool loadTextures) override;
```
**Parameters:**
- `out`: ModelData to fill with the loaded model
- `meshStream`: Stream containing the model data
- `loadTextures`: Whether to load textures for the model

**Returns:** true if the model was loaded successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="assimploader-_reloadmodel"></a>

#### **`AssimpLoader::_ReloadModel()`**

 Reloads a model by its ID, returns true if successful

#### This function is internal use only and not intended for public use!

**Note:** This is used to reload models only when they change on disk for hot reloading

Signature:
```cpp
 bool _ReloadModel(ModelData& out, scl::stream* stream, const std::string& assetPath, int id) override;
```
**Parameters:**
- `out`: ModelData to fill with the reloaded model
- `id`: ID of the model to reload

**Returns:** true if the model was reloaded successfully, false otherwise

---
<a id="assimploader-processscene"></a>

#### **`AssimpLoader::processScene()`**

 Processes the Assimp scene and fills the MeshData structure

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static bool processScene(ModelData& out, const aiScene* scene, scl::stream* meshStream, bool loadTextures = true);
```
**Parameters:**
- `out`: ModelData to fill with the processed data
- `scene`: Assimp scene to process
- `meshStream`: Stream containing the model data (for resolving relative texture paths)

**Returns:** true if the scene was processed successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="assimploader-_processmaterial"></a>

#### **`AssimpLoader::_ProcessMaterial()`**

 Processes an Assimp material and fills the Material structure

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static MaterialInstance _ProcessMaterial(aiMaterial* aiMat, const aiScene* scene, scl::stream* meshStream, bool loadTextures = true);
```
**Parameters:**
- `aiMat`: Assimp material to process
- `meshStream`: Stream containing the model data (for resolving relative texture paths)
- `loadTextures`: Whether to load textures for the material

**Returns:** Material structure filled with the processed material data

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
