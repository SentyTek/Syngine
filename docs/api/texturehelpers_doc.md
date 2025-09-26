# Syngine API Documentation


## TextureHelpers.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/TextureHelpers.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [LoadTextureFromFile](#syngineloadtexturefromfile)
- [CreateFlatTexture](#synginecreateflattexture)

---

## Class Constructor


#### **`Syngine::bgfx::TextureHandleLoadTextureFromMemory`**


 Loads a texture from memory

Signature:

```cpp
bgfx::TextureHandleLoadTextureFromMemory(const uint8_t* data, size_t size, const char* name);
```

**Parameters:**

- `data`: Pointer to the texture data in memory
- `size`: Size of the texture data in bytes
- `name`: Name of the texture for debugging purposes

**Returns:** A bgfx::TextureHandle representing the loaded texture

---

## Class & Related Members


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

#### **`Syngine::CreateFlatTexture`**


 Creates a flat texture, a 1x1 grey pixel texture

Signature:

```cpp
bgfx::TextureHandle CreateFlatTexture();
```

**Returns:** A bgfx::TextureHandle representing a flat texture

---

