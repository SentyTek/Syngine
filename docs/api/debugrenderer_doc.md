# Syngine API Documentation


## DebugRenderer.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/DebugRenderer.h)

---

## Goto: 


- [DrawLine](#synginedrawline)
- [DrawGeometry](#synginedrawgeometry)
- [DrawText3D](#synginedrawtext3d)
- [DrawTriangle](#synginedrawtriangle)
- [CreateTriangleBatch](#synginecreatetrianglebatch)
- [CreateTriangleBatch](#synginecreatetrianglebatch)
- [DrawFrustum](#synginedrawfrustum)
- [RenderLines](#synginerenderlines)
- [ClearLines](#syngineclearlines)

---

#### **`Syngine::DrawLine`**


 Draw a line with a color

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 virtual void DrawLine(JPH::RVec3Arg from, JPH::RVec3Arg to,
```

**Parameters:**

- `from`: Starting point of the line
- `to`: Ending point of the line
- `color`: Color of the line

**This function has been available since:** v0.0.1

---

#### **`Syngine::DrawGeometry`**


 Draw geometry

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 virtual void DrawGeometry(JPH::RMat44Arg modelMatrix, const JPH::AABox& worldSpaceBounds,
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

#### **`Syngine::DrawText3D`**


 Draw a text string in 3D space (Not implemented)

Signature:

```cpp
 virtual void DrawText3D(JPH::RVec3Arg position, const std::string_view& inString,
```

**Parameters:**

- `position`: Position in 3D space
- `inString`: Text string to draw
- `color`: Color of the text
- `height`: Height of the text

---

#### **`Syngine::DrawTriangle`**


 Draw a triangle

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 virtual void DrawTriangle(JPH::RVec3Arg v1,
```

**Parameters:**

- `v1`: First vertex of the triangle
- `v2`: Second vertex of the triangle
- `v3`: Third vertex of the triangle
- `color`: Color of the triangle
- `castShadow`: Whether to cast shadows

**This function has been available since:** v0.0.1

---

#### **`Syngine::CreateTriangleBatch`**


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

#### **`Syngine::CreateTriangleBatch`**


 Create a batch of triangles from vertices and indices

Signature:

```cpp
 virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const Vertex* inVertices,
```

**Parameters:**

- `inVertices`: Array of vertices
- `inVertexCount`: Number of vertices in the array
- `inIndices`: Array of indices
- `inIndexCount`: Number of indices in the array

---

#### **`Syngine::DrawFrustum`**


 Draw a frustum

#### This function is internal use only and not intended for public use!


**Preconditions:** Camera must be initialized

Signature:

```cpp
 void DrawFrustum(Syngine::Camera camera);
```

**Parameters:**

- `camera`: Camera to draw the frustum for

**This function has been available since:** v0.0.1

---

#### **`Syngine::RenderLines`**


 Render debug lines

#### This function is internal use only and not intended for public use!


**Preconditions:** Rendering system must be initialized

Signature:

```cpp
 void RenderLines(const float* view, const float* proj,
```

**Parameters:**

- `view`: View matrix
- `proj`: Projection matrix
- `width`: Width of the viewport
- `height`: Height of the viewport
- `program`: Shader program to use for rendering

**This function has been available since:** v0.0.1

---

#### **`Syngine::ClearLines`**


 Clear all debug lines

#### This function is internal use only and not intended for public use!


**Postconditions:** All debug lines are cleared

Signature:

```cpp
 void ClearLines();
```

**This function has been available since:** v0.0.1

---

