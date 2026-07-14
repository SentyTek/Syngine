# Syngine API Documentation

## MeshComponent.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/MeshComponent.h)

Component for rendering 3D mesh models. Supports loading 3D asset files and uploading custom mesh data. @section MeshComponent

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

- [MeshComponent(GameObject* owner, const std::string& path, bool loadTextures = true)](#meshcomponent-meshcomponentgameobject-owner-const-std-string-path-bool-loadtextures-true)
- [MeshComponent(GameObject* owner, const std::string& bundlePath, const std::string& texturePath, bool loadTextures = true)](#meshcomponent-meshcomponentgameobject-owner-const-std-string-bundlepath-const-std-string-texturepath-bool-loadtextures-true)

### Enums and Structs: 

- [MeshAABB](#syngine-meshaabb)

### Functions: 

- [GetComponentType()](#meshcomponent-getcomponenttype)
- [Clone()](#meshcomponent-clone)
- [Serialize()](#meshcomponent-serialize)
- [Init()](#meshcomponent-init)
- [Update()](#meshcomponent-update)
- [LoadMesh()](#meshcomponent-loadmesh)
- [ReloadMesh()](#meshcomponent-reloadmesh)
- [UnloadMesh()](#meshcomponent-unloadmesh)
- [IsMeshLoaded()](#meshcomponent-ismeshloaded)
- [UploadMesh()](#meshcomponent-uploadmesh)
- [GetAABB()](#meshcomponent-getaabb)
- [GetSubmeshCount()](#meshcomponent-getsubmeshcount)
- [GetSubmeshMaterialIndex()](#meshcomponent-getsubmeshmaterialindex)
- [SetSubmeshMaterialIndex()](#meshcomponent-setsubmeshmaterialindex)
- [GetMaterialUVScale()](#meshcomponent-getmaterialuvscale)
- [SetMaterialUVScale()](#meshcomponent-setmaterialuvscale)
- [GetObjectUVScaleOverride()](#meshcomponent-getobjectuvscaleoverride)
- [SetObjectUVScaleOverride()](#meshcomponent-setobjectuvscaleoverride)

---
<a id="syngine-meshaabb"></a>

#### **`Syngine::MeshAABB()`**

 Structure representing an axis-aligned bounding box (AABB)

Signature:
```cpp
struct MeshAABB
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `Math::Vector3` | `min` | Minimum corner of the AABB |
| `Math::Vector3` | `max` | Maximum corner of the AABB |
| `Math::Vector3` | `center` | Center of the AABB |
| `Math::Vector3` | `halfExtents` | Half extents of |
**This function has been available since:** v0.0.1

---
<a id="meshcomponent-meshcomponentgameobject-owner-const-std-string-path-bool-loadtextures-true"></a>

## Class Constructor

#### **`MeshComponent(GameObject* owner, const std::string& path, bool loadTextures = true)`**

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
<a id="meshcomponent-meshcomponentgameobject-owner-const-std-string-bundlepath-const-std-string-texturepath-bool-loadtextures-true"></a>

## Class Constructor

#### **`MeshComponent(GameObject* owner, const std::string& bundlePath, const std::string& texturePath, bool loadTextures = true)`**

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
<a id="meshcomponent-getcomponenttype"></a>

#### **`MeshComponent::GetComponentType()`**

 Get the type of this component

Signature:
```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```
**Returns:** The component type as an enum value

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="meshcomponent-clone"></a>

#### **`MeshComponent::Clone()`**

 Clone the MeshComponent

Signature:
```cpp
 std::unique_ptr<Component> Clone() const override;
```
**Returns:** A unique pointer to the cloned MeshComponent

---
<a id="meshcomponent-serialize"></a>

#### **`MeshComponent::Serialize()`**

 Serializes the MeshComponent to a data node

Signature:
```cpp
 Serializer::DataNode Serialize() const override;
```
**Returns:** A pointer to the serialized data node representing the MeshComponent's state

---
<a id="meshcomponent-init"></a>

#### **`MeshComponent::Init()`**

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
<a id="meshcomponent-update"></a>

#### **`MeshComponent::Update()`**

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
<a id="meshcomponent-loadmesh"></a>

#### **`MeshComponent::LoadMesh()`**

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
<a id="meshcomponent-reloadmesh"></a>

#### **`MeshComponent::ReloadMesh()`**

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
<a id="meshcomponent-unloadmesh"></a>

#### **`MeshComponent::UnloadMesh()`**

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
<a id="meshcomponent-ismeshloaded"></a>

#### **`MeshComponent::IsMeshLoaded()`**

 Check if the mesh is loaded

Signature:
```cpp
 bool IsMeshLoaded() const;
```
**Returns:** true if the mesh is loaded, false otherwise

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="meshcomponent-uploadmesh"></a>

#### **`MeshComponent::UploadMesh()`**

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
<a id="meshcomponent-getaabb"></a>

#### **`MeshComponent::GetAABB()`**

 Get the axis-aligned bounding box (AABB) of the mesh

Signature:
```cpp
 MeshAABB& GetAABB();
```
**Returns:** Reference to the MeshAABB structure

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="meshcomponent-getsubmeshcount"></a>

#### **`MeshComponent::GetSubmeshCount()`**

 Get the submesh count

Signature:
```cpp
 uint8_t GetSubmeshCount() const;
```
**Returns:** Number of submeshes in the mesh

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="meshcomponent-getsubmeshmaterialindex"></a>

#### **`MeshComponent::GetSubmeshMaterialIndex()`**

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
<a id="meshcomponent-setsubmeshmaterialindex"></a>

#### **`MeshComponent::SetSubmeshMaterialIndex()`**

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
<a id="meshcomponent-getmaterialuvscale"></a>

#### **`MeshComponent::GetMaterialUVScale()`**

 Get UV scale for specific texture type and material

Signature:
```cpp
 Math::Vector2 GetMaterialUVScale(uint8_t materialIndex, uint8_t textureType) const;
```
**Parameters:**
- `materialIndex`: Index of the material to query
- `textureType`: Type of the texture (0 = albedo, 1 = normal, 2 = height)

**Returns:** UV scale for the specified material and texture type

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="meshcomponent-setmaterialuvscale"></a>

#### **`MeshComponent::SetMaterialUVScale()`**

 Set UV scale for specific texture type and material

Signature:
```cpp
 bool SetMaterialUVScale(uint8_t materialIndex, uint8_t textureType, Math::Vector2 uvScale);
```
**Parameters:**
- `materialIndex`: Index of the material to modify
- `textureType`: Type of the texture (0 = albedo, 1 = normal, 2 = height)
- `uvScale`: UV scale to set for the specified material and texture type

**Returns:** true if the UV scale was successfully set, false if the material index is invalid

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="meshcomponent-getobjectuvscaleoverride"></a>

#### **`MeshComponent::GetObjectUVScaleOverride()`**

 Gets the UV scale override for the whole object

Signature:
```cpp
 float GetObjectUVScaleOverride() const;
```
**Returns:** UV scale override for the whole object. 1.0f by default.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="meshcomponent-setobjectuvscaleoverride"></a>

#### **`MeshComponent::SetObjectUVScaleOverride()`**

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
| `bool` | `receiveShadows` | Whether the mesh receives shadows |
| `bool` | `castShadows` | Whether the mesh casts shadows |
| `bool` | `receiveSunLight` | Whether the mesh receives sunlight |
| `MeshData` | `meshData` | Mesh data for the GameObject |
| `mutable` | `MeshAABB` | Axis-aligned bounding box of the mesh |
| `float` | `m_objectUVScaleOverride` | UV scale override for the whole object |
| `std::string` | `m_bundlePath` | Path to the shader bundle containing the mesh |
| `std::string` | `m_texturePath` | Path to the mesh within the bundle |
---
