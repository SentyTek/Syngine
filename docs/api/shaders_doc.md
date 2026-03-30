# Syngine API Documentation


## Shaders.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/Shaders.h)

---

## Goto:


- [_LoadShader](#syngine_loadshader)

---

#### **`Syngine::_LoadShader`**


 Load a shader from a file.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
inline bgfx::ShaderHandle _LoadShader(const char* shaderPath);
```

**Parameters:**

- `shaderPath`: The path to the shader file.

**Returns:** bgfx::ShaderHandle The handle to the loaded shader, or BGFX_INVALID_HANDLE on failure.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
