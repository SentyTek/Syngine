# Syngine API Documentation

## Renderer.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Renderer.h)

Renderer class to manage rendering and shader programs @section Renderer

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [Renderer(int width, int height, const RendererConfig& config)](#renderer-rendererint-width-int-height-const-rendererconfig-config)

### Enums and Structs: 

- [ViewID](#syngine-viewid)
- [UniformType](#syngine-uniformtype)
- [Uniform](#syngine-uniform)
- [Program](#syngine-program)
- [RendererConfig](#syngine-rendererconfig)

### Functions: 

- [AddProgram()](#renderer-addprogram)
- [AddProgram()](#renderer-addprogram-2)
- [GetProgram()](#renderer-getprogram)
- [GetProgram()](#renderer-getprogram-2)
- [RemoveAllPrograms()](#renderer-removeallprograms)
- [RemoveProgram()](#renderer-removeprogram)
- [RemoveProgram()](#renderer-removeprogram-2)
- [ReloadProgram()](#renderer-reloadprogram)
- [ReloadAllPrograms()](#renderer-reloadallprograms)
- [IsReady()](#renderer-isready)
- [RegisterUniform()](#renderer-registeruniform)
- [SetUniform()](#renderer-setuniform)
- [SetUniform()](#renderer-setuniform-2)
- [GetSunDirection()](#renderer-getsundirection)
- [SetSunDirection()](#renderer-setsundirection)
- [SetGizmoSize()](#renderer-setgizmosize)
- [GetGizmoSize()](#renderer-getgizmosize)
- [_RegisterGizmo()](#renderer-_registergizmo)
- [GetUniform()](#renderer-getuniform)
- [SetPseudoCamera()](#renderer-setpseudocamera)
- [_CreateRenderer()](#renderer-_createrenderer)
- [_RenderGizmos()](#renderer-_rendergizmos)
- [_GetProgram()](#renderer-_getprogram)

---
<a id="syngine-viewid"></a>

#### **`Syngine::ViewID()`**

 Collection of view IDs for rendering. Rendered in the order they are defined here.

Signature:
```cpp
enum ViewID : bgfx::ViewId
```
**Members:**
| Name | Description |
| --- | --- | 
| `VIEW_SHADOW` | Shadow map rendering |
| `VIEW_SKY` | Skybox rendering |
| `VIEW_GBUFFER` | G-Buffer rendering for deferred shading |
| `VIEW_LIGHTING` | Lighting pass for deferred shading |
| `VIEW_FORWARD` | Forward rendering pass for translucent objects |
| `VIEW_BILLBOARD` | Billboard rendering |
| `VIEW_DEBUG` | Debug rendering pass for debug rendering |
| `VIEW_BILL_DBG` | Billboard debug rendering |
| `VIEW_AO` | Ambient occlusion passes (3 passes) |
| `VIEW_POSTPROCESS` | Post-processing effects passes (Max 8 passes) |
| `VIEW_UI` | UI rendering |
| `VIEW_UI_DEBUG` | UI debug rendering |
**This function has been available since:** v0.0.1

---
<a id="syngine-uniformtype"></a>

#### **`Syngine::UniformType()`**

 Different types of shader uniforms

Signature:
```cpp
enum UniformType
```
**Members:**
| Name | Description |
| --- | --- | 
| `UNIFORM_SAMPLER` | Texture sampler uniform |
| `UNIFORM_VEC4` | Vec4 uniform |
| `UNIFORM_MAT4` | Mat4 uniform |
| `UNIFORM_MAT3` | Mat3 uniform |
| `UNIFORM_UNKNOWN` | Unknown uniform type |
**This function has been available since:** v0.0.1

---
<a id="syngine-uniform"></a>

#### **`Syngine::Uniform()`**

 Struct to hold shader uniform information

Signature:
```cpp
struct Uniform
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `bgfx::UniformHandle` | `handle` | Handle for the uniform |
| `UniformType` | `type` | Type of the uniform |
| `void*` | `data` | Pointer to the uniform data |
| `std::string` | `name` | Name of the uniform |
| `uint16_t` | `num` | Number of elements in the uniform (for arrays) |
**This function has been available since:** v0.0.1

---
<a id="syngine-program"></a>

#### **`Syngine::Program()`**

 Program structure to hold shader program information

Signature:
```cpp
struct Program
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `bgfx::ProgramHandle` | `program` | Shader program handle |
| `unsigned short` | `viewId` | View ID for rendering |
| `std::string` | `name` | Name of the shader program |
| `std::string` | `vsPath` | Vertex shader path |
| `std::string` | `fsPath` | Fragment shader path |
| `std::string` | `bundlePath` | Bundle path if loaded from a bundle |
| `int` | `id` | Unique ID of the shader program |
| `std::vector<Uniform>` | `uniforms` | List of shader uniforms |
---
<a id="syngine-rendererconfig"></a>

#### **`Syngine::RendererConfig()`**

 To manage renderer configuration

Signature:
```cpp
struct RendererConfig
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `true` | `Whether` | to load settings from file on initialization. This |
| `bool` | `useShadows` | Whether to use shadow mapping |
| `int` | `shadowDist` | Distance for shadow rendering |
| `bool` | `vsync` | Whether to enable vertical sync |
| `false` | `(only` | if DebugModes.Enabled == true) Pseudo camera is a |
| `bool` | `useSSAO` | Whether to enable SSAO |
---
<a id="renderer-rendererint-width-int-height-const-rendererconfig-config"></a>

## Class Constructor

#### **`Renderer(int width, int height, const RendererConfig& config)`**

 Constructor for the Renderer class

Signature:
```cpp
 Renderer(int width, int height, const RendererConfig& config);
```
**Parameters:**
- `width`: Width of the game window in pixels
- `height`: Height of the game window in pixels
- `config`: Renderer configuration options

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if initialization fails (e.g., bgfx::init() fails or missing files)

---
<a id="renderer-addprogram"></a>

#### **`Renderer::AddProgram()`**

 Load a shader from a single file path and create a shader program

**Note:** Assumes the vertex shader is named "{name}.vert.bin" and fragment shader "{name}.frag.bin"

**Preconditions:** Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)

Signature:
```cpp
 static size_t AddProgram(const std::string& path, const std::string& name, Syngine::ViewID viewId = Syngine::VIEW_FORWARD);
```
**Parameters:**
- `path`: Path to the shader file
- `name`: Name of the shader program
- `viewId`: View ID for the shader program, defaults to VIEW_FORWARD

**Returns:** The index of the newly created program on success, -1 on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-addprogram-2"></a>

#### **`Renderer::AddProgram()`**

 Load a shader from a bundle file and create a shader program

**Note:** Assumes the vertex shader is named "{path}.vert.bin" and fragment shader "{path}.frag.bin" within the bundle

**Preconditions:** Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)

Signature:
```cpp
 static size_t AddProgram(const std::string& bundlePath, const std::string& path, const std::string& name, Syngine::ViewID viewId = Syngine::VIEW_FORWARD);
```
**Parameters:**
- `bundlePath`: Path to the bundle file containing the shader
- `path`: Path to the shader file within the bundle (without .vert.bin or .frag.bin suffix)
- `name`: Name of the shader program
- `viewId`: View ID for the shader program, defaults to VIEW_FORWARD

**Returns:** The index of the newly created program on success, -1 on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-getprogram"></a>

#### **`Renderer::GetProgram()`**

 Get a shader program by name. Returns first matching program

Signature:
```cpp
 static Program GetProgram(const std::string_view& name);
```
**Parameters:**
- `name`: Name of the shader program

**Returns:** Program with the shader program handle, or an invalid handle if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="renderer-getprogram-2"></a>

#### **`Renderer::GetProgram()`**

 Get a shader program by ID.

Signature:
```cpp
 static Program GetProgram(size_t id);
```
**Parameters:**
- `id`: ID of the shader program

**Returns:** Program with the shader program handle, or an invalid handle if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="renderer-removeallprograms"></a>

#### **`Renderer::RemoveAllPrograms()`**

 Remove all shader programs

Signature:
```cpp
 static bool RemoveAllPrograms();
```
**Returns:** True if all programs were removed successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-removeprogram"></a>

#### **`Renderer::RemoveProgram()`**

 Remove a shader program by name

Signature:
```cpp
 static bool RemoveProgram(Syngine::ViewID viewId, const std::string_view& name);
```
**Parameters:**
- `viewId`: The view to remove the program from
- `name`: Name of the shader program to remove

**Returns:** True if the program was removed successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-removeprogram-2"></a>

#### **`Renderer::RemoveProgram()`**

 Remove a shader program by id

Signature:
```cpp
 static bool RemoveProgram(Syngine::ViewID viewId, size_t id);
```
**Parameters:**
- `viewId`: The view to remove the program from
- `id`: ID of the shader program to remove

**Returns:** True if the program was removed successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-reloadprogram"></a>

#### **`Renderer::ReloadProgram()`**

 Reload a shader program by name

Signature:
```cpp
 static bool ReloadProgram(Syngine::ViewID viewId, const std::string_view& name);
```
**Parameters:**
- `name`: Name of the shader program to reload

**Returns:** True if the program was reloaded successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-reloadallprograms"></a>

#### **`Renderer::ReloadAllPrograms()`**

 Reload all shader programs

Signature:
```cpp
 static bool ReloadAllPrograms();
```
**Returns:** True if all programs were reloaded successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-isready"></a>

#### **`Renderer::IsReady()`**

 Check if the renderer is ready

Signature:
```cpp
 static bool IsReady();
```
**Returns:** True if the renderer is ready, false otherwise

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="renderer-registeruniform"></a>

#### **`Renderer::RegisterUniform()`**

 Register a uniform variable

**Preconditions:** The shader program must exist

Signature:
```cpp
 static size_t RegisterUniform(size_t program, const std::string& name, UniformType type, uint16_t num = 1);
```
**Parameters:**
- `program`: The ID of the shader program to register the uniform with
- `name`: The name of the uniform variable
- `type`: The type of the uniform variable
- `num`: The number of elements for the uniform variable (default is 1)

**Returns:** The ID of the registered uniform variable to use when setting, 0 if failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-setuniform"></a>

#### **`Renderer::SetUniform()`**

 Set a uniform variable

Signature:
```cpp
 static void SetUniform(size_t id, const void* data, uint16_t num = 1);
```
**Parameters:**
- `id`: The ID of the uniform variable to set, returned from RegisterUniform
- `data`: Pointer to the data to set the uniform to
- `num`: The number of elements to set (default is 1)

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-setuniform-2"></a>

#### **`Renderer::SetUniform()`**

 Set a uniform variable

Signature:
```cpp
 static void SetUniform(size_t id, const Math::Vector4 data, uint16_t num = 1);
```
**Parameters:**
- `id`: The ID of the uniform variable to set, returned from RegisterUniform
- `data`: The Vector4 data to set the uniform to
- `num`: The number of elements to set (default is 1)

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-getsundirection"></a>

#### **`Renderer::GetSunDirection()`**

 Get the global sun light direction

Signature:
```cpp
 static Math::Vector3 GetSunDirection();
```
**Returns:** A vector3 representing the normalized sun light direction in world space

**Thread Safety:** not-safe

---
<a id="renderer-setsundirection"></a>

#### **`Renderer::SetSunDirection()`**

 Set the global sun light direction

**Note:** The direction should be normalized and in world space coordinates.

Signature:
```cpp
 static void SetSunDirection(const Math::Vector3 lightDir);
```
**Parameters:**
- `lightDir`: A Vector3 representing the normalized sun light direction in world space

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-setgizmosize"></a>

#### **`Renderer::SetGizmoSize()`**

 Set the default gizmo size

Signature:
```cpp
 static void SetGizmoSize(float size);
```
**Parameters:**
- `size`: Size of the gizmo

**This function has been available since:** v0.0.1

---
<a id="renderer-getgizmosize"></a>

#### **`Renderer::GetGizmoSize()`**

 Get the default gizmo size

Signature:
```cpp
 static float GetGizmoSize();
```
**Returns:** Size of the gizmo

**This function has been available since:** v0.0.1

---
<a id="renderer-_registergizmo"></a>

#### **`Renderer::_RegisterGizmo()`**

 Register a gizmo with a tag and optional size

#### This function is internal use only and not intended for public use!

**Note:** Loads texture from "default/gizmos/{tag}.png"

Signature:
```cpp
 void _RegisterGizmo(const std::string& tag);
```
**Parameters:**
- `tag`: Name of the gizmo
- `size`: Size of the gizmo

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-getuniform"></a>

#### **`Renderer::GetUniform()`**

 Get a uniform by ID

Signature:
```cpp
 static Uniform* GetUniform(size_t id);
```
**Parameters:**
- `id`: ID of the uniform (Returned by RegisterUniform)

**Returns:** Pointer to the Uniform struct, or nullptr if not found

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="renderer-setpseudocamera"></a>

#### **`Renderer::SetPseudoCamera()`**

 Set which CameraComponent to use as the pseudo camera for rendering

Signature:
```cpp
 static void SetPseudoCamera(CameraComponent* camera);
```
**Parameters:**
- `camera`: Pointer to the CameraComponent to use as the pseudo camera

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-_createrenderer"></a>

#### **`Renderer::_CreateRenderer()`**

 Initialize the graphics system

#### This function is internal use only and not intended for public use!

**Note:** This should be called after the game window is created.

Signature:
```cpp
 static bool _CreateRenderer(const RendererConfig& config);
```
**Returns:** true on success, false on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if initialization fails (e.g., bgfx::init() fails or missing files)

---
<a id="renderer-_rendergizmos"></a>

#### **`Renderer::_RenderGizmos()`**

 Render all nearby gizmos

#### This function is internal use only and not intended for public use!

**Note:** This will render all registered gizmos that are within 1000 units of the camera's position.

Signature:
```cpp
 void _RenderGizmos(CameraComponent* camera);
```
**Parameters:**
- `camera`: Pointer to the camera component for rendering

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-_getprogram"></a>

#### **`Renderer::_GetProgram()`**

 Internal helper for GetProgram.

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static Program* _GetProgram(size_t id);
```
**Parameters:**
- `id`: ID of the shader program

**Returns:** Pointer to the Program struct, or nullptr if not found

**Thread Safety:** read-only

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `int` | `width` | Width of the game window in pixels |
| `int` | `height` | Height of the game window in pixels |
| `std::string` | `m_title` | Title of the game window |
| `bool` | `m_isReady` | Whether the renderer is initialized and ready |
| `static` | `std` | Registry of shader uniforms |
| `static` | `std` | Registry of gizmos |
| `float` | `m_gizmoSize` | Default size for gizmos |
| `static` | `std` | Shader programs organized by view ID |
| `CameraComponent*` | `m_pseudoCamera` | Pseudo camera for rendering when enabled in debug mode |
| `DebugRender*` | `m_drender` | Debug renderer instance |
| `bool` | `m_isFirstFrame` | Whether this is the first frame being rendered |
---
