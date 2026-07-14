# Syngine API Documentation

## Windowing.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Windowing.h)

Manages all of the windowing operations @section Window @internal

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

### Constructors: 

- [Window(const EngineConfig& config)](#window-windowconst-engineconfig-config)

### Functions: 

- [ShouldClose()](#window-shouldclose)
- [GetWidth()](#window-getwidth)
- [GetHeight()](#window-getheight)
- [GetAspectRatio()](#window-getaspectratio)
- [SetTitle()](#window-settitle)
- [SetVSync()](#window-setvsync)
- [SetWindowMode()](#window-setwindowmode)
- [SetMouseCursorVisible()](#window-setmousecursorvisible)
- [GetMousePosition()](#window-getmouseposition)
- [SetMousePosition()](#window-setmouseposition)
- [_SetShouldClose()](#window-_setshouldclose)
- [_SetContextCreated()](#window-_setcontextcreated)
- [_GetSDLWindow()](#window-_getsdlwindow)

---
<a id="window-windowconst-engineconfig-config"></a>

## Class Constructor

#### **`Window(const EngineConfig& config)`**

 Constructor for the Window class

Signature:
```cpp
 Window(const EngineConfig& config);
```
**Parameters:**
- `config`: Engine configuration options

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if initialization fails (e.g., SDL_CreateWindow() fails or missing files)

---
<a id="window-shouldclose"></a>

#### **`Window::ShouldClose()`**

 Checks if the window should close.

Signature:
```cpp
 bool ShouldClose() const noexcept;
```
**Returns:** True if the window should close, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="window-getwidth"></a>

#### **`Window::GetWidth()`**

 Gets the width of the window.

Signature:
```cpp
 int GetWidth() const noexcept;
```
**Returns:** The width of the window in pixels.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-getheight"></a>

#### **`Window::GetHeight()`**

 Gets the height of the window.

Signature:
```cpp
 int GetHeight() const noexcept;
```
**Returns:** The height of the window in pixels.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-getaspectratio"></a>

#### **`Window::GetAspectRatio()`**

 Gets the aspect ratio of the window.

Signature:
```cpp
 float GetAspectRatio() const noexcept;
```
**Returns:** The aspect ratio of the window.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-settitle"></a>

#### **`Window::SetTitle()`**

 Sets the title of the window.

Signature:
```cpp
 static void SetTitle(const std::string_view& title);
```
**Parameters:**
- `title`: The new title of the window.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-setvsync"></a>

#### **`Window::SetVSync()`**

 Sets the vertical sync state of the window.

Signature:
```cpp
 static void SetVSync(bool enabled);
```
**Parameters:**
- `enabled`: True to enable vertical sync, false to disable it.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-setwindowmode"></a>

#### **`Window::SetWindowMode()`**

 Sets the state of the window (bordered, fullscreen borderless, exclusive fullscreen)

Signature:
```cpp
 static void SetWindowMode(int mode);
```
**Parameters:**
- `mode`: The new window mode. 0 = bordered, 1 = fullscreen borderless, 2 = exclusive fullscreen

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-setmousecursorvisible"></a>

#### **`Window::SetMouseCursorVisible()`**

 Sets the visibility of the mouse cursor.

Signature:
```cpp
 static void SetMouseCursorVisible(bool visible);
```
**Parameters:**
- `visible`: True to show the cursor, false to hide it.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-getmouseposition"></a>

#### **`Window::GetMousePosition()`**

 Gets the current position of the mouse cursor.

Signature:
```cpp
 static void GetMousePosition(float& x, float& y);
```
**Parameters:**
- `x`: A float reference to be filled in with the X position of the mouse cursor.
- `y`: A float reference to be filled in with the Y position of the mouse cursor.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-setmouseposition"></a>

#### **`Window::SetMousePosition()`**

 Sets the current position of the mouse cursor.

Signature:
```cpp
 static void SetMousePosition(float& x, float& y);
```
**Parameters:**
- `x`: A float reference to be filled in with the X position of the mouse cursor.
- `y`: A float reference to be filled in with the Y position of the mouse cursor.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-_setshouldclose"></a>

#### **`Window::_SetShouldClose()`**

 Sets the window's close state.

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _SetShouldClose(bool enabled) noexcept;
```
**Parameters:**
- `enabled`: True to close the window, false to keep it open.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-_setcontextcreated"></a>

#### **`Window::_SetContextCreated()`**

 Sets the window's context creation state.

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _SetContextCreated(bool enabled) noexcept;
```
**Parameters:**
- `enabled`: True to create the context, false to destroy it.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="window-_getsdlwindow"></a>

#### **`Window::_GetSDLWindow()`**

 Gets the SDL window pointer

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static SDL_Window* _GetSDLWindow() noexcept;
```
**Returns:** The SDL window pointer.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
