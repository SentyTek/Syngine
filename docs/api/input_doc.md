# Syngine API Documentation


## Input.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Input.h)

---

## Goto:


- [Constructor](#class-constructor)
- [KeybindType](#synginekeybindtype)
- [KeyShortcut](#synginekeyshortcut)
- [KeySequence](#synginekeysequence)
- [KeyBinding](#synginekeybinding)
- [:](#inputaction:)
- [onReleased](#inputactiononreleased)
- [onStateChanged](#inputactiononstatechanged)
- [Constructor](#class-constructor)
- [Operator = overload](#inputactionoperator=overload)
- [Constructor](#class-constructor)
- [Operator = overload](#inputactionoperator=overload)
- [Constructor](#class-constructor)
- [isPressed](#inputactionispressed)
- [wasPressed](#inputactionwaspressed)
- [wasReleased](#inputactionwasreleased)
- [stateChanged](#inputactionstatechanged)
- [setOnPressedCallback](#inputactionsetonpressedcallback)
- [setOnReleasedCallback](#inputactionsetonreleasedcallback)
- [setOnStateChangedCallback](#inputactionsetonstatechangedcallback)
- [RegisterAction](#inputactionregisteraction)
- [Operator == overload](#inputactionoperator==overload)
- [_HandleEvent](#inputaction_handleevent)
- [RegisterMouseMoveEvent](#inputactionregistermousemoveevent)
- [RegisterScrollEvent](#inputactionregisterscrollevent)
- [void](#inputactionvoid)
- [void](#inputactionvoid)
- [_Registry](#inputaction_registry)
- [_HomelessShelter](#inputaction_homelessshelter)
- [EnsureUniqueIdentifier](#inputactionensureuniqueidentifier)

---

## Class Constructor


#### **`InputAction::InputAction`**


 Make an InputAction with a binding and callbacks

**Note:** Logs a fatal error and crashes the program if the identifier is not unique

Signature:

```cpp
 InputAction(const std::string& identifier, const std::string& name, const std::string& category = "", Syngine::KeyBinding binding = KeyBinding(), Callbacks callbacks = { .onPressed = []() -> void {}, .onReleased = []() -> void {}, .onStateChanged = []() -> void {} });
```

**Parameters:**

- `identifier`: The identifier for the input action. This is required and *must* be unique
- `name`: The name for the input action. This is the display name that is shown in the bindings menu
- `category`: The category for the input action. This is used for organizing actions in the bindings menu
- `binding`: The binding for the input action
- `callbacks`: The callbacks to be called when the input action is triggered

---

## Class & Related Members


#### **`Syngine::KeybindType`**


 A tag enum representing each of the keybindable types

Signature:

```cpp
enum class KeybindType : size_t
```

**Members:**

| Name | Description |
| --- | --- |
| `UNBOUND` | A keybind that is not bound to any key |
| `KEYCODE` | A keybind that is bound to a specific keycode |
| `SCANCODE` | A keybind that is bound to a specific scancode |
| `SHORTCUT` | A keybind that is bound to a keycode or scancode and a set |
| `SEQUENCE` | A keybind that is bound to a sequence of keycodes, |
| `MOUSE_BUTTON` | A keybind that is bound to a mouse button |

**This function has been available since:** 0.0.1

---

#### **`Syngine::KeyShortcut`**


 Checks if the keyboard event triggers this binding

#### This function is internal use only and not intended for public use!


**Note:** Use a ``KeyBinding`` holding a ``KeyUnknown`` (the default constructor of `KeyBinding`) instead

Signature:

```cpp
struct KeyShortcut
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `key` | `The` | key to bind |
| `key` | `The` | key to bind |
| `modifiers` | `The` | modifiers to bind |
| `other` | `The` | ``KeyShortcut`` to compare against |
| `event` | `The` | event to check against |

**Returns:** `true` if the event triggers the binding, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`Syngine::KeySequence`**


 Equality operator for ``KeySequence``

#### This function is internal use only and not intended for public use!


**Note:** You probably shouldn't call this unless you really need to, and it might be removed

Signature:

```cpp
struct KeySequence
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `key` | `The` | binding to bind |
| `list` | `The` | list of bindings to bind |
| `index` | `The` | index to find the type of |
| `event` | `The` | ``SDL_KeyboardEvent`` to check the sequence against |
| `other` | `The` | ``KeySequence`` to compare against |

**Returns:** `true` if the objects are equal, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`Syngine::KeyBinding`**


 Determines if the KeyBinding is triggered by a keyboard event

#### This function is internal use only and not intended for public use!


Signature:

```cpp
struct KeyBinding
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `keycode` | `The` | keycode to bind |
| `other` | `The` | ``KeyBinding`` to compare against |
| `event` | `The` | ``SDL_KeyboardEvent`` to check against |
| `event` | `The` | ``SDL_KeyboardEvent`` to check against |

**Returns:** `true` if the contained subtype is triggered by the event, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`InputAction:::`**


 A bound input action

**Note:** InputActions are *not* anonymous and *must* have a unique identifier, which cannot be changed

Signature:

```cpp
 public: /// @brief A container for callbacks /// @since 0.0.1 struct Callbacks { /// @brief The callback called when the action is triggered /// @since 0.0.1 std::function<void()> onPressed = []() -> void;
```

---

#### **`InputAction::onReleased`**


 The callback called when the action is released

Signature:

```cpp
 std::function<void()> onReleased = []() -> void;
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::onStateChanged`**


 The callback called whenever the trigger state of the action changes

Signature:

```cpp
 std::function<void()> onStateChanged = []() -> void;
```

**This function has been available since:** 0.0.1

---

## Class Constructor


#### **`InputAction::InputAction`**


 InputActions are non-copyable because of their unique identifiers

Signature:

```cpp
 InputAction(const InputAction& other) = delete;
```

**This function has been available since:** 0.0.1

---

## Class & Related Members


#### **`Operator = overload`**


 InputActions are non-copyable because of their unique identifiers

Signature:

```cpp
 InputAction& operator=(const InputAction& other) = delete;
```

**This function has been available since:** 0.0.1

---

## Class Constructor


#### **`InputAction::InputAction`**


 For now, InputActions are non-movable to keep the registry system intact

Signature:

```cpp
 InputAction(InputAction&& other) = delete;
```

**This function has been available since:** 0.0.1

---

## Class & Related Members


#### **`Operator = overload`**


 For now, InputActions are non-movable to keep the registry system intact

Signature:

```cpp
 InputAction& operator=(InputAction&& other) = delete;
```

**This function has been available since:** 0.0.1

---

## Class Constructor


#### **`InputAction::InputAction`**


 Deconstructs and cleans up the InputAction

Signature:

```cpp
 ~InputAction();
```

**This function has been available since:** 0.0.1

---

## Class & Related Members


#### **`InputAction::isPressed`**


 Returns true if the binding is currently being pressed

Signature:

```cpp
 bool isPressed();
```

**Returns:** `true` for every frame the binding is triggered, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`InputAction::wasPressed`**


 Returns true if the binding was pressed since the previous frame

Signature:

```cpp
 bool wasPressed();
```

**Returns:** `true` the first frame the binding is triggered, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`InputAction::wasReleased`**


 Returns true if the binding was released since the last frame

Signature:

```cpp
 bool wasReleased();
```

**Returns:** `true` the first frame the binding is released, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`InputAction::stateChanged`**


 Returns true if the binding's state has changed since the last frame

Signature:

```cpp
 bool stateChanged();
```

**Returns:** `true` on every frame where the state is different from the last frame, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`InputAction::setOnPressedCallback`**


 Sets the `onPressed` callback

Signature:

```cpp
 void setOnPressedCallback(std::function<void()> callback);
```

**Parameters:**

- `callback`: The callback to set

**This function has been available since:** 0.0.1

---

#### **`InputAction::setOnReleasedCallback`**


 Sets the `onReleased` callback

Signature:

```cpp
 void setOnReleasedCallback(std::function<void()> callback);
```

**Parameters:**

- `callback`: The callback to set

**This function has been available since:** 0.0.1

---

#### **`InputAction::setOnStateChangedCallback`**


 Sets the `onStateChanged` callback

Signature:

```cpp
 void setOnStateChangedCallback(std::function<void()> callback);
```

**Parameters:**

- `callback`: The callback to set

**This function has been available since:** 0.0.1

---

#### **`InputAction::RegisterAction`**


 Registers an action which calls the provided callback when the action is pressed

**Note:** Logs a fatal error and crashes the program if the identifier is not unique

Signature:

```cpp
 static void RegisterAction(const std::string& identifier, const std::string& name, const std::string& category, KeyBinding binding, Callbacks callbacks);
```

**Parameters:**

- `identifier`: The identifier for the input action. This is required and *must* be unique
- `name`: The name for the input action. This is the display name that is shown in the bindings menu
- `category`: The category for the input action. This is used for organizing actions in the bindings menu
- `binding`: The binding for the input action
- `callbacks`: The callbacks to call when the action state updates

**This function has been available since:** 0.0.1

---

#### **`Operator == overload`**


 Equality operator for ``InputAction``

**Note:** Does not check for full equality, only for equal bindings, as InputActions are guaranteed to be unique

Signature:

```cpp
 bool operator==(const InputAction& other) const;
```

**Parameters:**

- `other`: The ``InputAction`` to compare against

**Returns:** `true` if the objects are equal, `false` otherwise

**This function has been available since:** 0.0.1

---

#### **`InputAction::_HandleEvent`**


 Handle input events. This should *not* be called outside of Core::HandleEvents()

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static void _HandleEvent(SDL_Event event);
```

**Parameters:**

- `event`: The SDL event to handle

**This function has been available since:** 0.0.1

**See also:**

-  Core::HandleEvents()

---

#### **`InputAction::RegisterMouseMoveEvent`**


 Temporary mouse move event registration

**Note:** This will be removed in favor of a better API at some point

Signature:

```cpp
 static void RegisterMouseMoveEvent(std::function<void(float, float)> callback);
```

**Parameters:**

- `callback`: The callback to call on mouse move

**This function has been available since:** 0.0.1

---

#### **`InputAction::RegisterScrollEvent`**


 Temporary mouse scroll event registration

**Note:** This will be removed in favor of a better API at some point

Signature:

```cpp
 static void RegisterScrollEvent(std::function<void(float, float)> callback);
```

**Parameters:**

- `callback`: The callback to call on mouse move

**This function has been available since:** 0.0.1

---

#### **`InputAction::void`**


 Temporary storage for the mouse move event callback

**Note:** This will be removed in favor of a better API at some point

Signature:

```cpp
 static std::function<void(float, float)> _MouseMoveCallback;
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::void`**


 Temporary storage for the scroll event callback

**Note:** This will be removed in favor of a better API at some point

Signature:

```cpp
 static std::function<void(float, float)> _ScrollCallback;
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::_Registry`**


 An array of pointers to every active input action for tracking and updating. Each pointer is guaranteed to point to an existing object and be unique

Signature:

```cpp
 static std::vector<InputAction*> _Registry;
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::_HomelessShelter`**


 This is where input actions go if they're not handled as objects directly

**Note:** This will be removed in favor of a better API at some point

Signature:

```cpp
 static std::list<InputAction> _HomelessShelter;
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::EnsureUniqueIdentifier`**


 Logs a fatal error and halts the program if the provided identifier is not unique

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static void EnsureUniqueIdentifier(const std::string& identifier);
```

**Parameters:**

- `identifer`: The identifier to check

**This function has been available since:** 0.0.1

---
