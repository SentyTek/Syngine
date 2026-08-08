# Syngine API Documentation

## Renderer.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Rendering/Renderer.h)

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
- [RendererConfig](#syngine-rendererconfig)

### Functions: 

- [IsReady()](#renderer-isready)
- [SetUniform()](#renderer-setuniform)
- [GetSunDirection()](#renderer-getsundirection)
- [SetSunDirection()](#renderer-setsundirection)
- [SetGizmoSize()](#renderer-setgizmosize)
- [GetGizmoSize()](#renderer-getgizmosize)
- [_RegisterGizmo()](#renderer-_registergizmo)
- [SetPseudoCamera()](#renderer-setpseudocamera)
- [GetPseudoCamera()](#renderer-getpseudocamera)
- [SetActiveCamera()](#renderer-setactivecamera)
- [GetActiveCamera()](#renderer-getactivecamera)
- [_CreateRenderer()](#renderer-_createrenderer)
- [_RenderGizmos()](#renderer-_rendergizmos)

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
| `int` | `shadowMapSize` | Size of the shadow map texture (default: 2048) |
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
 static void SetSunDirection(const Math::Vector3& lightDir);
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
<a id="renderer-getpseudocamera"></a>

#### **`Renderer::GetPseudoCamera()`**

 Get the active pseudo camera used for rendering.

Signature:
```cpp
 static CameraComponent* GetPseudoCamera();
```
**Returns:** Pointer to the active CameraComponent used for rendering

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

---
<a id="renderer-setactivecamera"></a>

#### **`Renderer::SetActiveCamera()`**

 Set which CameraComponent to use as the active camera for rendering

Signature:
```cpp
 static void SetActiveCamera(CameraComponent* camera);
```
**Parameters:**
- `camera`: Pointer to the CameraComponent to use as the active camera

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="renderer-getactivecamera"></a>

#### **`Renderer::GetActiveCamera()`**

 Get the active camera used for rendering.

Signature:
```cpp
 static CameraComponent* GetActiveCamera();
```
**Returns:** Pointer to the active CameraComponent used for rendering

**Thread Safety:** read-only

**This function has been available since:** v0.0.2

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
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `int` | `width` | Width of the game window in pixels |
| `int` | `height` | Height of the game window in pixels |
| `std::string` | `m_title` | Title of the game window |
| `bool` | `m_isReady` | Whether the renderer is initialized and ready |
| `float` | `m_gizmoSize` | Default size for gizmos |
| `CameraComponent*` | `m_camera` | Active camera used for rendering |
| `CameraComponent*` | `m_pseudoCamera` | Pseudo camera for rendering when |
| `DebugRender*` | `m_drender` | Debug renderer instance |
| `Math::Vector3` | `m_sunDir` | Direction of the sun light in world space |
---
