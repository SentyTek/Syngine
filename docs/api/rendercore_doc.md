# Syngine API Documentation


## RenderCore.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/RenderCore.h)

---

## Goto: 


- [_RenderFrame](#rendercore_renderframe)
- [_Initialize](#rendercore_initialize)
- [_Shutdown](#rendercore_shutdown)
- [_SetResolution](#rendercore_setresolution)
- [_GetDefaultUniform](#rendercore_getdefaultuniform)
- [](#rendercore)
- [](#rendercore)
- [shadows](#rendercoreshadows)

---

#### **`RenderCore::_RenderFrame`**


 Render a single frame. Calls several internal rendering functions.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static bool _RenderFrame(CameraComponent* camera, DebugModes debug);
```

**Parameters:**

- `camera`: Pointer to the camera component for rendering
- `debug`: Debug modes for rendering

**Returns:** true on success, false on failure

---

#### **`RenderCore::_Initialize`**


 Initialize the RenderCore system

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static bool _Initialize(const RendererConfig& config);
```

**Parameters:**

- `config`: Renderer configuration options

**Returns:** true on success, false on failure

---

#### **`RenderCore::_Shutdown`**


 Shutdown the RenderCore system

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static void _Shutdown();
```

---

#### **`RenderCore::_SetResolution`**


 Set the resolution of the renderer

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static bool _SetResolution(int width, int height);
```

**Parameters:**

- `width`: Width in pixels
- `height`: Height in pixels

---

#### **`RenderCore::_GetDefaultUniform`**


 Get default uniform data by name

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static Uniform* _GetDefaultUniform(const std::string& name);
```

**Parameters:**

- `name`: Name of the uniform

**Returns:** Pointer to the Uniform struct, or nullptr if not found

---

#### **`RenderCore::`**


 Struct representing a material instance for rendering. Used for binding textures and uniforms.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 uint8_t stage;
```

---

#### **`RenderCore::`**


 Struct representing a render packet for submission to the GPU.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bgfx::VertexBufferHandle vbh;
```

---

#### **`RenderCore::shadows`**


 Struct to hold counts of drawn objects per rendering pass

Signature:

```cpp
 uint32_t shadows = 0;
```

**Parameters:**

- `static`: std::vector<RenderPacket> m_renderPackets Collected render packets for the current frame
- `_CollectRenderPackets(CameraComponent*`: camera); //* Collect render packets Collect render packets
- `static`: float m_maxSmallObjDistance Small objects get culled beyond this distance
- `bgfx::FrameBufferHandle`: sceneFB Framebuffer for scene rendering
- `bgfx::TextureHandle`: sceneColor Color texture for scene rendering (RGBA16F)
- `bgfx::TextureHandle`: sceneDepth Depth texture for scene rendering (D24S8)
- `bgfx::TextureHandle`: sceneNormal Normal texture for scene rendering (RGBA8)
- `bgfx::FrameBufferHandle`: ssaoFB Framebuffer for SSAO rendering
- `bgfx::FrameBufferHandle`: ssaoBlurHFB Temp framebuffer for SSAO blurring (horizontal)
- `bgfx::FrameBufferHandle`: ssaoBlurVFB Temp framebuffer for SSAO blurring (vertical)
- `bgfx::TextureHandle`: ssaoTex SSAO texture (R8)
- `bgfx::TextureHandle`: ssaoBlurH SSAO texture mid-blur (R8)
- `bgfx::TextureHandle`: ssaoBlurFinal SSAO texture post-blur (Use this one) (R8)
- `bgfx::TextureHandle`: shadowDepth Shadow map depth texture handle
- `bgfx::FrameBufferHandle`: shadowFB Shadow map framebuffer
- `static`: std::unordered_map<std::string, uint16_t> m_defaultUniformIds Default uniform IDs
- `static`: RendererConfig m_config Render configuration
- `m_billboardVbh`: Vertex buffer handle for billboards
- `m_billboardIbh`: Index buffer handle for billboards
- `static`: bgfx::VertexBufferHandle m_fsQuadVbh Vertex buffer handle for fullscreen quad
- `static`: SDL_Window* win SDL window handle
- `dummy`: Dummy vertex buffer handle for rendering
- `static`: DebugRender* m_drender Debug renderer instance
- `m_isFirstFrame`: Whether this is the first frame being rendered

---

