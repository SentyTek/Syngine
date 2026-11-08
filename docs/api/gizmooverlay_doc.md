# Syngine API Documentation

## GizmoOverlay.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Rendering/GizmoOverlay.h)

Immediate-mode list of colored meshes drawn in VIEW_GIZMO. They are drawn over scene geometry and physics wireframes, but below billboard gizmos. Items are cleared at the end of every rendered frame, so they must be submitted every frame. @details The pass uses whichever shader program is loaded for VIEW_GIZMO (see ShaderManager::LoadShader). That program should read a_position and a_normal, and may declare `uniform vec4 u_gizmoColor;` which is set per item.

**This class has been available since:** v0.0.2. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Functions: 

- [SetMeshBundle()](#gizmooverlay-setmeshbundle)
- [Preload()](#gizmooverlay-preload)
- [Submit()](#gizmooverlay-submit)
- [_Draw()](#gizmooverlay-_draw)
- [_EndFrame()](#gizmooverlay-_endframe)
- [_Shutdown()](#gizmooverlay-_shutdown)

---
<a id="gizmooverlay-setmeshbundle"></a>

#### **`GizmoOverlay::SetMeshBundle()`**

 Set the bundle gizmo meshes are loaded from

Signature:
```cpp
 static void SetMeshBundle(const std::string& bundlePath);
```
**Parameters:**
- `bundlePath`: Bundle path (e.g. "meshes/meshes.spk")

---
<a id="gizmooverlay-preload"></a>

#### **`GizmoOverlay::Preload()`**

 Synchronously load meshes from the bundle set by SetMeshBundle, one at a time. Meant to be called during startup asset import. Meshes that fail to load are logged and skipped.

Signature:
```cpp
 static void Preload(const std::vector<std::string>& meshNames);
```
**Parameters:**
- `meshNames`: Mesh names within the bundle (no extension)

---
<a id="gizmooverlay-submit"></a>

#### **`GizmoOverlay::Submit()`**

 Queue an item for the next rendered frame

Signature:
```cpp
 static void Submit(const Item& item);
```
---
<a id="gizmooverlay-_draw"></a>

#### **`GizmoOverlay::_Draw()`**



#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _Draw(uint16_t viewId, uint64_t state, uint16_t program);
```
---
<a id="gizmooverlay-_endframe"></a>

#### **`GizmoOverlay::_EndFrame()`**



#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _EndFrame();
```
---
<a id="gizmooverlay-_shutdown"></a>

#### **`GizmoOverlay::_Shutdown()`**



#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _Shutdown();
```
---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `meshName` | Mesh name within the bundle |
| `Math::Mat4` | `model` | World matrix |
| `Math::Vec4` | `color` | RGBA, passed as u_gizmoColor |
---
