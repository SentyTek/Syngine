# Syngine API Documentation


## MetalBridge.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Graphics/MetalBridge.h)

---

## Goto: 


- [_GetSYNMetalView](#syngine_getsynmetalview)

---

#### **`Syngine::_GetSYNMetalView`**


 Get the Metal view for a given SDL window on macOS

#### This function is internal use only and not intended for public use!


**Note:** This function is specific to macOS and uses Objective-C to retrieve the Metal view from the SDL window.

Signature:

```cpp
void* _GetSYNMetalView(SDL_Window* win);
```

**Parameters:**

- `win`: The SDL window for which to get the Metal view

**Returns:** A pointer to the Metal view, or nullptr if not available

**This function has been available since:** v0.0.1

---

