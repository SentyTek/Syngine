# Syngine API Documentation


## BasicInput.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Core/BasicInput.h)

---

## Goto: 


- [enum](#basicinputenum)
- [RegisterKey](#basicinputregisterkey)
- [RegisterMouseButtonEvent](#basicinputregistermousebuttonevent)
- [RegisterMouseMoveEvent](#basicinputregistermousemoveevent)
- [_HandleEvent](#basicinput_handleevent)
- [GetKeyboardState](#basicinputgetkeyboardstate)

---

#### **`BasicInput::enum`**


 Manages user input for the application.

Signature:

```cpp
 public: enum class MouseMove {
```

---

#### **`BasicInput::RegisterKey`**


 Registers a callback for a specific key.

Signature:

```cpp
 static void RegisterKey(SDL_Keycode key, KeyCallback callback);
```

**Parameters:**

- `key`: The SDL keycode to register the callback for
- `callback`: The function to call when the key is pressed

---

#### **`BasicInput::RegisterMouseButtonEvent`**


 Registers a callback for mouse events

Signature:

```cpp
 static void RegisterMouseButtonEvent(MouseButton event, MouseButtonCallback callback);
```

**Parameters:**

- `event`: The mouse event to register the callback for
- `callback`: The function to call when the mouse event occurs

---

#### **`BasicInput::RegisterMouseMoveEvent`**


 Registers a callback for mouse scroll events

Signature:

```cpp
 static void RegisterMouseMoveEvent(MouseMove event, MouseMoveCallback callback);
```

**Parameters:**

- `callback`: The function to call when the mouse scroll event occurs

---

#### **`BasicInput::_HandleEvent`**


 Handles SDL events and triggers the appropriate callbacks.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static void _HandleEvent(const SDL_Event& event);
```

**Parameters:**

- `event`: The SDL event to handle

---

#### **`BasicInput::GetKeyboardState`**


 Gets the current state of the keyboard.

Signature:

```cpp
 static const bool* GetKeyboardState();
```

---

