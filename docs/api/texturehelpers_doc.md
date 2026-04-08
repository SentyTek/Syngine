# Syngine API Documentation


## TextureHelpers.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/TextureHelpers.h)

---

## Goto: 


- [LoadTextureFromMemory](#syngineloadtexturefrommemory)
- [LoadTextureFromFile](#syngineloadtexturefromfile)
- [LoadTextureFromBundle](#syngineloadtexturefrombundle)
- [CreateFlatTexture](#synginecreateflattexture)
- [CreateNoiseTexture](#synginecreatenoisetexture)

---

#### **`Syngine::LoadTextureFromMemory`**


 Loads a texture from memory

Signature:

```cpp
bgfx::TextureHandle LoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);
```

**Parameters:**

- `data`: Pointer to the texture data in memory
- `size`: Size of the texture data in bytes
- `name`: Name of the texture for debugging purposes

**Returns:** A bgfx::TextureHandle representing the loaded texture

---

#### **`Syngine::LoadTextureFromFile`**


 Loads a texture from file

Signature:

```cpp
bgfx::TextureHandle LoadTextureFromFile(const char* path);
```

**Parameters:**

- `path`: Path to the texture file

**Returns:** A bgfx::TextureHandle representing the loaded texture

---

#### **`Syngine::LoadTextureFromBundle`**


 Load a texture from a bundle

Signature:

```cpp
bgfx::TextureHandle LoadTextureFromBundle(const std::string& bundlePath, const std::string& textureName);
```

**Parameters:**

- `bundlePath`: Path to the shader bundle containing the texture
- `textureName`: Name of the texture within the shader bundle

**Returns:** A bgfx::TextureHandle representing the loaded texture

---

#### **`Syngine::CreateFlatTexture`**


 Creates a flat texture, a 1x1 grey pixel texture

Signature:

```cpp
bgfx::TextureHandle CreateFlatTexture();
```

**Returns:** A bgfx::TextureHandle representing a flat texture

---

#### **`Syngine::CreateNoiseTexture`**


 Creates a white noise texture

Signature:

```cpp
bgfx::TextureHandle CreateNoiseTexture(uint16_t width, uint16_t height);
```

**Parameters:**

- `width`: Width of the texture
- `height`: Height of the texture

**Returns:** A bgfx::TextureHandle representing the noise texture

---

