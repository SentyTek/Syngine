# Syngine API Documentation

## DebugRenderer.h header

[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/DebugRenderer.h)

Internal class to manage batches of debug triangles. Not all functions are implemented or documented. @section DebugRenderer

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

- [DrawLine()](#batchimpl-drawline)
- [DrawGeometry()](#batchimpl-drawgeometry)
- [DrawText3D()](#batchimpl-drawtext3d)
- [DrawTriangle()](#batchimpl-drawtriangle)
- [CreateTriangleBatch()](#batchimpl-createtrianglebatch)
- [CreateTriangleBatch()](#batchimpl-createtrianglebatch-2)
- [DrawFrustum()](#batchimpl-drawfrustum)
- [DrawFrustum()](#batchimpl-drawfrustum-2)
- [DrawBox()](#batchimpl-drawbox)
- [DrawSphere()](#batchimpl-drawsphere)
- [RenderLines()](#batchimpl-renderlines)
- [ClearLines()](#batchimpl-clearlines)

---
<a id="batchimpl-drawline"></a>

#### **`BatchImpl::DrawLine()`**

 Draw a line with a color

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 virtual void DrawLine(JPH::RVec3Arg from, JPH::RVec3Arg to, JPH::ColorArg color) override;
```
**Parameters:**
- `from`: Starting point of the line
- `to`: Ending point of the line
- `color`: Color of the line

**This function has been available since:** v0.0.1

---
<a id="batchimpl-drawgeometry"></a>

#### **`BatchImpl::DrawGeometry()`**

 Draw geometry

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 virtual void DrawGeometry(JPH::RMat44Arg modelMatrix, const JPH::AABox& worldSpaceBounds, float LODScaleSq, JPH::ColorArg modelColor, const GeometryRef& geometry, ECullMode cullMode = ECullMode::CullBackFace, ECastShadow castShadow = ECastShadow::Off, EDrawMode drawMode = EDrawMode::Wireframe) override;
```
**Parameters:**
- `modelMatrix`: Transformation matrix for the geometry
- `worldSpaceBounds`: Bounding box in world space
- `LODScaleSq`: Level of detail scale squared
- `modelColor`: Color of the model
- `geometry`: Geometry reference to draw
- `cullMode`: Culling mode for the geometry
- `castShadow`: Whether to cast shadows
- `drawMode`: Draw mode for the geometry

**This function has been available since:** v0.0.1

---
<a id="batchimpl-drawtext3d"></a>

#### **`BatchImpl::DrawText3D()`**

 Draw a text string in 3D space (Not implemented)

Signature:
```cpp
 virtual void DrawText3D(JPH::RVec3Arg position, const std::string_view& inString, JPH::ColorArg color = JPH::Color::sWhite, float height = 0.5f) override;
```
**Parameters:**
- `position`: Position in 3D space
- `inString`: Text string to draw
- `color`: Color of the text
- `height`: Height of the text

---
<a id="batchimpl-drawtriangle"></a>

#### **`BatchImpl::DrawTriangle()`**

 Draw a triangle

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 virtual void DrawTriangle(JPH::RVec3Arg v1, JPH::RVec3Arg v2, JPH::RVec3Arg v3, JPH::ColorArg color, ECastShadow castShadow = ECastShadow::Off) override;
```
**Parameters:**
- `v1`: First vertex of the triangle
- `v2`: Second vertex of the triangle
- `v3`: Third vertex of the triangle
- `color`: Color of the triangle
- `castShadow`: Whether to cast shadows

**This function has been available since:** v0.0.1

---
<a id="batchimpl-createtrianglebatch"></a>

#### **`BatchImpl::CreateTriangleBatch()`**

 Create a batch of triangles

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
```
**Parameters:**
- `inTriangles`: Array of triangles to create the batch from
- `inTriangleCount`: Number of triangles in the array

**Returns:** Batch of triangles

**This function has been available since:** v0.0.1

---
<a id="batchimpl-createtrianglebatch-2"></a>

#### **`BatchImpl::CreateTriangleBatch()`**

 Create a batch of triangles from vertices and indices

Signature:
```cpp
 virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;
```
**Parameters:**
- `inVertices`: Array of vertices
- `inVertexCount`: Number of vertices in the array
- `inIndices`: Array of indices
- `inIndexCount`: Number of indices in the array

---
<a id="batchimpl-drawfrustum"></a>

#### **`BatchImpl::DrawFrustum()`**

 Draw a frustum from a camera object

#### This function is internal use only and not intended for public use!

**Preconditions:** Camera must be initialized

Signature:
```cpp
 void DrawFrustum(const Syngine::Camera& camera);
```
**Parameters:**
- `camera`: Camera to draw the frustum for

**This function has been available since:** v0.0.1

---
<a id="batchimpl-drawfrustum-2"></a>

#### **`BatchImpl::DrawFrustum()`**

 Draw a frustum from given view and projection matrices

#### This function is internal use only and not intended for public use!

**Preconditions:** View and projection matrices must be valid

Signature:
```cpp
 void DrawFrustum(const Math::Mat4& view, const Math::Mat4& proj);
```
**Parameters:**
- `view`: View matrix
- `proj`: Projection matrix

**This function has been available since:** v0.0.1

---
<a id="batchimpl-drawbox"></a>

#### **`BatchImpl::DrawBox()`**

 Draw a box given min and max points

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 void DrawBox(const Math::Vector3& min, const Math::Vector3& max, JPH::ColorArg color);
```
**Parameters:**
- `min`: Minimum point of the box
- `max`: Maximum point of the box
- `color`: Color of the box

**This function has been available since:** v0.0.1

---
<a id="batchimpl-drawsphere"></a>

#### **`BatchImpl::DrawSphere()`**

 Draw a sphere given center and radius

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 void DrawSphere(JPH::RVec3Arg center, float radius, JPH::ColorArg color);
```
**Parameters:**
- `center`: Center of the sphere
- `radius`: Radius of the sphere
- `color`: Color of the sphere

**This function has been available since:** v0.0.1

---
<a id="batchimpl-renderlines"></a>

#### **`BatchImpl::RenderLines()`**

 Render debug lines

#### This function is internal use only and not intended for public use!

**Preconditions:** Rendering system must be initialized

Signature:
```cpp
 void RenderLines(const Math::Mat4& view, const Math::Mat4& proj, int width, int height, bgfx::ProgramHandle program);
```
**Parameters:**
- `view`: View matrix
- `proj`: Projection matrix
- `width`: Width of the viewport
- `height`: Height of the viewport
- `program`: Shader program to use for rendering

**This function has been available since:** v0.0.1

---
<a id="batchimpl-clearlines"></a>

#### **`BatchImpl::ClearLines()`**

 Clear all debug lines

#### This function is internal use only and not intended for public use!

**Postconditions:** All debug lines are cleared

Signature:
```cpp
 void ClearLines();
```
**This function has been available since:** v0.0.1

---
