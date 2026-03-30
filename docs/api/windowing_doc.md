# Syngine API Documentation


## Windowing.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Windowing.h)

---

## Goto: 


- [SDL_CreateWindow](#windowsdl_createwindow)
- [ShouldClose](#windowshouldclose)
- [GetWidth](#windowgetwidth)
- [GetHeight](#windowgetheight)
- [GetAspectRatio](#windowgetaspectratio)
- [SetTitle](#windowsettitle)
- [SetVSync](#windowsetvsync)
- [SetWindowMode](#windowsetwindowmode)
- [SetMouseCursorVisible](#windowsetmousecursorvisible)
- [GetMousePosition](#windowgetmouseposition)
- [SetMousePosition](#windowsetmouseposition)
- [_SetShouldClose](#window_setshouldclose)
- [_SetContextCreated](#window_setcontextcreated)
- [_GetSDLWindow](#window_getsdlwindow)

---

#### **`Window::SDL_CreateWindow`**


 Manages all of the windowing operations

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 public: /// @brief Constructor for the Window class /// @param config Engine configuration options /// @throws std::runtime_error if initialization fails (e.g., SDL_CreateWindow() /// fails or missing files) /// @threadsafety not-safe /// @since v0.0.1 Window(const EngineConfig& config);
```

---

#### **`Window::ShouldClose`**


 Checks if the window should close.

Signature:

```cpp
 bool ShouldClose() const noexcept;
```

**Returns:** True if the window should close, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---

#### **`Window::GetWidth`**


 Gets the width of the window.

Signature:

```cpp
 int GetWidth() const noexcept;
```

**Returns:** The width of the window in pixels.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Window::GetHeight`**


 Gets the height of the window.

Signature:

```cpp
 int GetHeight() const noexcept;
```

**Returns:** The height of the window in pixels.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Window::GetAspectRatio`**


 Gets the aspect ratio of the window.

Signature:

```cpp
 float GetAspectRatio() const noexcept;
```

**Returns:** The aspect ratio of the window.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Window::SetTitle`**


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

#### **`Window::SetVSync`**


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

#### **`Window::SetWindowMode`**


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

#### **`Window::SetMouseCursorVisible`**


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

#### **`Window::GetMousePosition`**


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

#### **`Window::SetMousePosition`**


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

#### **`Window::_SetShouldClose`**


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

#### **`Window::_SetContextCreated`**


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

#### **`Window::_GetSDLWindow`**


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

