# Syngine API Documentation


## Renderer.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Renderer.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [ViewID](#syngineviewid)
- [UniformType](#syngineuniformtype)
- [Uniform](#syngineuniform)
- [Program](#syngineprogram)
- [AddProgram](#rendereraddprogram)
- [AddProgram](#rendereraddprogram)
- [GetProgram](#renderergetprogram)
- [GetProgram](#renderergetprogram)
- [RemoveAllPrograms](#rendererremoveallprograms)
- [RemoveProgram](#rendererremoveprogram)
- [RemoveProgram](#rendererremoveprogram)
- [ReloadProgram](#rendererreloadprogram)
- [ReloadAllPrograms](#rendererreloadallprograms)
- [IsReady](#rendererisready)
- [RegisterUniform](#rendererregisteruniform)
- [SetUniform](#renderersetuniform)
- [GetSunDirection](#renderergetsundirection)
- [SetSunDirection](#renderersetsundirection)
- [_RenderFrame](#renderer_renderframe)
- [_RegisterGizmo](#renderer_registergizmo)
- [_CreateRenderer](#renderer_createrenderer)
- [_RenderGizmos](#renderer_rendergizmos)

---

#### **`Syngine::ViewID`**


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
| `VIEW_UI` | UI rendering |
| `VIEW_UI_DEBUG` | UI debug rendering |

**This function has been available since:** v0.0.1

---

#### **`Syngine::UniformType`**


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

#### **`Syngine::Uniform`**


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

**This function has been available since:** v0.0.1

---

#### **`Syngine::Program`**


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
| `int` | `id` | Unique ID of the shader program |
| `std::vector<Uniform>` | `uniforms` | List of shader uniforms |

---

#### **`Renderer::AddProgram`**


 Load a shader from vertex and fragment shader file paths and create a shader program

**Preconditions:** Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)

Signature:

```cpp
 static size_t AddProgram(const std::string& vsPath, const std::string& fsPath,
```

**Parameters:**

- `vsPath`: Path to the vertex shader file
- `fsPath`: Path to the fragment shader file
- `name`: Name of the shader program
- `viewId`: View ID for the shader program, defaults to VIEW_FORWARD

**Returns:** The ID of the newly created program on success, -1 on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::AddProgram`**


 Load a shader from a single file path and create a shader program

**Note:** Assumes the vertex shader is named "{name}.vert.bin" and fragment shader "{name}.frag.bin"

**Preconditions:** Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)

Signature:

```cpp
 static size_t AddProgram(const std::string& path, const std::string& name,
```

**Parameters:**

- `path`: Path to the shader file
- `name`: Name of the shader program
- `viewId`: View ID for the shader program, defaults to VIEW_FORWARD

**Returns:** The index of the newly created program on success, -1 on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::GetProgram`**


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

#### **`Renderer::GetProgram`**


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

#### **`Renderer::RemoveAllPrograms`**


 Remove all shader programs

Signature:

```cpp
 static bool RemoveAllPrograms();
```

**Returns:** True if all programs were removed successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::RemoveProgram`**


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

#### **`Renderer::RemoveProgram`**


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

#### **`Renderer::ReloadProgram`**


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

#### **`Renderer::ReloadAllPrograms`**


 Reload all shader programs

Signature:

```cpp
 static bool ReloadAllPrograms();
```

**Returns:** True if all programs were reloaded successfully, false otherwise

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::IsReady`**


 Check if the renderer is ready

Signature:

```cpp
 static bool IsReady();
```

**Returns:** True if the renderer is ready, false otherwise

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Renderer::RegisterUniform`**


 Register a uniform variable

**Preconditions:** The shader program must exist

Signature:

```cpp
 static size_t RegisterUniform(int program, const std::string& name, UniformType type);
```

**Parameters:**

- `program`: The ID of the shader program to register the uniform with
- `name`: The name of the uniform variable
- `type`: The type of the uniform variable

**Returns:** The ID of the registered uniform variable to use when setting, 0 if failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::SetUniform`**


 Set a uniform variable

Signature:

```cpp
 static void SetUniform(uint16_t id, const void* data);
```

**Parameters:**

- `id`: The ID of the uniform variable to set, returned from RegisterUniform
- `data`: Pointer to the data to set the uniform to

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::GetSunDirection`**


 Get the global sun light direction

Signature:

```cpp
 static void GetSunDirection(float* outLightDir);
```

**Parameters:**

- `outLightDir`: A 3 float array to store the light direction. The direction is a normalized vector in world space. X is east-west, Y is pitch, Z is north-south.

**Thread Safety:** not-safe

---

#### **`Renderer::SetSunDirection`**


 Set the global sun light direction

Signature:

```cpp
 static void SetSunDirection(const float* lightDir);
```

**Parameters:**

- `lightDir`: A 3 float array representing the light direction. Normalized direction vector in world space is expected.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::_RenderFrame`**


 Render a frame

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _RenderFrame(CameraComponent* camera, bool debug = false);
```

**Parameters:**

- `lightDir`: Direction of the light for lighting calculations
- `camera`: Pointer to the camera component for rendering
- `debug`: Whether to render debug information

**Returns:** If it rendered successfully

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::_RegisterGizmo`**


 Register a gizmo with a tag and optional size

#### This function is internal use only and not intended for public use!


**Note:** Loads texture from "default/gizmos/{tag}.png"

Signature:

```cpp
 void _RegisterGizmo(const std::string& tag, float size = 1.0f);
```

**Parameters:**

- `tag`: Name of the gizmo
- `size`: Size of the gizmo

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Renderer::_CreateRenderer`**


 Initialize the graphics system

#### This function is internal use only and not intended for public use!


**Note:** This should be called after the game window is created.

Signature:

```cpp
 static bool _CreateRenderer();
```

**Returns:** true on success, false on failure

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if initialization fails (e.g., bgfx::init() fails or missing files)

---

#### **`Renderer::_RenderGizmos`**


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
| `int` | `height` | Height of the game window in pixels |
| `bgfx::TextureHandle` | `texture` | Texture handle for the gizmo |
| `float` | `size` | Size of the gizmo. 1.0f is the default size, roughly 1 unit in world space |
| `SDL_Window*` | `win` | SDL window handle |

---

