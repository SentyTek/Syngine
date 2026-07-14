# Syngine API Documentation

## TextureHelpers.h header

[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/TextureHelpers.h)

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

- [LoadTextureFromMemory()](#syngine-loadtexturefrommemory)
- [LoadTextureFromFile()](#syngine-loadtexturefromfile)
- [LoadTextureFromBundle()](#syngine-loadtexturefrombundle)
- [CreateFlatTexture()](#syngine-createflattexture)
- [CreateNoiseTexture()](#syngine-createnoisetexture)

---
<a id="syngine-loadtexturefrommemory"></a>

#### **`Syngine::LoadTextureFromMemory()`**

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
<a id="syngine-loadtexturefromfile"></a>

#### **`Syngine::LoadTextureFromFile()`**

 Loads a texture from file

Signature:
```cpp
bgfx::TextureHandle LoadTextureFromFile(const char* path);
```
**Parameters:**
- `path`: Path to the texture file

**Returns:** A bgfx::TextureHandle representing the loaded texture

---
<a id="syngine-loadtexturefrombundle"></a>

#### **`Syngine::LoadTextureFromBundle()`**

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
<a id="syngine-createflattexture"></a>

#### **`Syngine::CreateFlatTexture()`**

 Creates a flat texture, a 1x1 grey pixel texture

Signature:
```cpp
bgfx::TextureHandle CreateFlatTexture();
```
**Returns:** A bgfx::TextureHandle representing a flat texture

---
<a id="syngine-createnoisetexture"></a>

#### **`Syngine::CreateNoiseTexture()`**

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
