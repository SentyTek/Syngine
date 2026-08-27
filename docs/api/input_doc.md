# Syngine API Documentation

## Input.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Input.h)

A bound input action @note InputActions are *not* anonymous and *must* have a unique identifier, which cannot be changed @section Input

---
## Goto: 


## Additional Functions: 

### Constructors: 

- [InputAction(const std::string& identifier, const std::string& name, const std::string& category = "", Syngine::KeyBinding binding = KeyBinding(), Callbacks callbacks = { .onPressed = []() -> void {}, .onReleased = []() -> void {}, .onStateChanged = []() -> void {} })](#inputaction-inputactionconst-std-string-identifier-const-std-string-name-const-std-string-category-syngine-keybinding-binding-keybinding-callbacks-callbacks-onpressed-void-onreleased-void-onstatechanged-void)
- [InputAction(const InputAction& other)](#inputaction-inputactionconst-inputaction-other)
- [InputAction](#inputaction-inputaction)
- [InputAction(InputAction&& other)](#inputaction-inputactioninputaction-other)
- [InputAction](#inputaction-inputaction-2)
- [InputAction()](#inputaction-inputaction-3)

### Operator Overloads: 

- [Equality operator overload](#equality-operator-overload)

### Enums and Structs: 

- [KeybindType](#syngine-keybindtype)
- [KeyShortcut](#syngine-keyshortcut)
- [KeySequence](#syngine-keysequence)
- [KeyBinding](#syngine-keybinding)
- [Callbacks](#inputaction-callbacks)

### Functions: 

- [isPressed()](#inputaction-ispressed)
- [wasPressed()](#inputaction-waspressed)
- [wasReleased()](#inputaction-wasreleased)
- [stateChanged()](#inputaction-statechanged)
- [setOnPressedCallback()](#inputaction-setonpressedcallback)
- [setOnReleasedCallback()](#inputaction-setonreleasedcallback)
- [setOnStateChangedCallback()](#inputaction-setonstatechangedcallback)
- [RegisterAction()](#inputaction-registeraction)
- [UnregisterActionsByPrefix()](#inputaction-unregisteractionsbyprefix)
- [_HandleEvent()](#inputaction-_handleevent)
- [RegisterMouseMoveEvent()](#inputaction-registermousemoveevent)
- [RegisterScrollEvent()](#inputaction-registerscrollevent)
- [void()](#inputaction-void)
- [void()](#inputaction-void-2)
- [_Registry](#inputaction-_registry)
- [_HomelessShelter](#inputaction-_homelessshelter)
- [EnsureUniqueIdentifier()](#inputaction-ensureuniqueidentifier)
- [StringToScancode()](#inputaction-stringtoscancode)
- [ScancodeToString()](#inputaction-scancodetostring)
- [StringToKeycode()](#inputaction-stringtokeycode)
- [KeycodeToString()](#inputaction-keycodetostring)
- [KeymodToString()](#inputaction-keymodtostring)
- [StringToKeymod()](#inputaction-stringtokeymod)

---
<a id="syngine-keybindtype"></a>

#### **`Syngine::KeybindType()`**

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
<a id="syngine-keyshortcut"></a>

#### **`Syngine::KeyShortcut()`**

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
<a id="syngine-keysequence"></a>

#### **`Syngine::KeySequence()`**

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
<a id="syngine-keybinding"></a>

#### **`Syngine::KeyBinding()`**

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
<a id="inputaction-callbacks"></a>

#### **`InputAction::Callbacks()`**

 The callback called whenever the trigger state of the action changes

Signature:
```cpp
 struct Callbacks
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-inputactionconst-std-string-identifier-const-std-string-name-const-std-string-category-syngine-keybinding-binding-keybinding-callbacks-callbacks-onpressed-void-onreleased-void-onstatechanged-void"></a>

## Class Constructor

#### **`InputAction(const std::string& identifier, const std::string& name, const std::string& category = "", Syngine::KeyBinding binding = KeyBinding(), Callbacks callbacks = { .onPressed = []() -> void {}, .onReleased = []() -> void {}, .onStateChanged = []() -> void {} })`**

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
<a id="inputaction-inputactionconst-inputaction-other"></a>

## Class Constructor

#### **`InputAction(const InputAction& other)`**

 InputActions are non-copyable because of their unique identifiers

Signature:
```cpp
 InputAction(const InputAction& other) = delete;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-inputaction"></a>

## Class Constructor

#### **`InputAction`**

 InputActions are non-copyable because of their unique identifiers

Signature:
```cpp
 InputAction& operator=(const InputAction& other) = delete;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-inputactioninputaction-other"></a>

## Class Constructor

#### **`InputAction(InputAction&& other)`**

 For now, InputActions are non-movable to keep the registry system intact

Signature:
```cpp
 InputAction(InputAction&& other) = delete;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-inputaction-2"></a>

## Class Constructor

#### **`InputAction`**

 For now, InputActions are non-movable to keep the registry system intact

Signature:
```cpp
 InputAction& operator=(InputAction&& other) = delete;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-inputaction-3"></a>

## Class Constructor

#### **`InputAction()`**

 Deconstructs and cleans up the InputAction

Signature:
```cpp
 ~InputAction();
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-ispressed"></a>

#### **`InputAction::isPressed()`**

 Returns true if the binding is currently being pressed

Signature:
```cpp
 bool isPressed();
```
**Returns:** `true` for every frame the binding is triggered, `false` otherwise

**This function has been available since:** 0.0.1

---
<a id="inputaction-waspressed"></a>

#### **`InputAction::wasPressed()`**

 Returns true if the binding was pressed since the previous frame

Signature:
```cpp
 bool wasPressed();
```
**Returns:** `true` the first frame the binding is triggered, `false` otherwise

**This function has been available since:** 0.0.1

---
<a id="inputaction-wasreleased"></a>

#### **`InputAction::wasReleased()`**

 Returns true if the binding was released since the last frame

Signature:
```cpp
 bool wasReleased();
```
**Returns:** `true` the first frame the binding is released, `false` otherwise

**This function has been available since:** 0.0.1

---
<a id="inputaction-statechanged"></a>

#### **`InputAction::stateChanged()`**

 Returns true if the binding's state has changed since the last frame

Signature:
```cpp
 bool stateChanged();
```
**Returns:** `true` on every frame where the state is different from the last frame, `false` otherwise

**This function has been available since:** 0.0.1

---
<a id="inputaction-setonpressedcallback"></a>

#### **`InputAction::setOnPressedCallback()`**

 Sets the `onPressed` callback

Signature:
```cpp
 void setOnPressedCallback(std::function<void()> callback);
```
**Parameters:**
- `callback`: The callback to set

**This function has been available since:** 0.0.1

---
<a id="inputaction-setonreleasedcallback"></a>

#### **`InputAction::setOnReleasedCallback()`**

 Sets the `onReleased` callback

Signature:
```cpp
 void setOnReleasedCallback(std::function<void()> callback);
```
**Parameters:**
- `callback`: The callback to set

**This function has been available since:** 0.0.1

---
<a id="inputaction-setonstatechangedcallback"></a>

#### **`InputAction::setOnStateChangedCallback()`**

 Sets the `onStateChanged` callback

Signature:
```cpp
 void setOnStateChangedCallback(std::function<void()> callback);
```
**Parameters:**
- `callback`: The callback to set

**This function has been available since:** 0.0.1

---
<a id="inputaction-registeraction"></a>

#### **`InputAction::RegisterAction()`**

 Registers an action which calls the provided callback when the action is pressed

**Note:** Logs a fatal error and crashes the program if the identifier is not unique

Signature:
```cpp
 static InputAction* RegisterAction(const std::string& identifier, const std::string& name, const std::string& category, KeyBinding binding, Callbacks callbacks);
```
**Parameters:**
- `identifier`: The identifier for the input action. This is required and *must* be unique
- `name`: The name for the input action. This is the display name that is shown in the bindings menu
- `category`: The category for the input action. This is used for organizing actions in the bindings menu
- `binding`: The binding for the input action
- `callbacks`: The callbacks to call when the action state updates

**Returns:** A pointer to the registered action

**This function has been available since:** 0.0.1

---
<a id="inputaction-unregisteractionsbyprefix"></a>

#### **`InputAction::UnregisterActionsByPrefix()`**

 Unregister all actions created through RegisterAction whose identifiers start with a given prefix.

Signature:
```cpp
 static size_t UnregisterActionsByPrefix(const std::string& prefix);
```
**Parameters:**
- `prefix`: Identifier prefix to match.

**Returns:** Number of removed actions.

**This function has been available since:** 0.0.1

---
<a id="equality-operator-overload"></a>

#### **`Equality operator overload`**

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
<a id="inputaction-_handleevent"></a>

#### **`InputAction::_HandleEvent()`**

 Handle input events. This should *not* be called outside of Core::HandleEvents()

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _HandleEvent(SDL_Event event);
```
**Parameters:**
- `event`: The SDL event to handle

**This function has been available since:** 0.0.1

**See also:** Core::HandleEvents()

---
<a id="inputaction-registermousemoveevent"></a>

#### **`InputAction::RegisterMouseMoveEvent()`**

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
<a id="inputaction-registerscrollevent"></a>

#### **`InputAction::RegisterScrollEvent()`**

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
<a id="inputaction-void"></a>

#### **`InputAction::void()`**

 Temporary storage for the mouse move event callback

**Note:** This will be removed in favor of a better API at some point

Signature:
```cpp
 static std::function<void(float, float)> _MouseMoveCallback;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-void-2"></a>

#### **`InputAction::void()`**

 Temporary storage for the scroll event callback

**Note:** This will be removed in favor of a better API at some point

Signature:
```cpp
 static std::function<void(float, float)> _ScrollCallback;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-_registry"></a>

#### **`InputAction::_Registry()`**

 An array of pointers to every active input action for tracking and updating. Each pointer is guaranteed to point to an existing object and be unique

Signature:
```cpp
 static std::vector<InputAction*> _Registry;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-_homelessshelter"></a>

#### **`InputAction::_HomelessShelter()`**

 This is where input actions go if they're not handled as objects directly

**Note:** This will be removed in favor of a better API at some point

Signature:
```cpp
 static std::list<InputAction> _HomelessShelter;
```
**This function has been available since:** 0.0.1

---
<a id="inputaction-ensureuniqueidentifier"></a>

#### **`InputAction::EnsureUniqueIdentifier()`**

 Logs a fatal error and halts the program if the provided identifier is not unique

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void EnsureUniqueIdentifier(const std::string& identifier);
```
**Parameters:**
- `identifier`: The identifier to check

**This function has been available since:** 0.0.1

---
<a id="inputaction-stringtoscancode"></a>

#### **`InputAction::StringToScancode()`**

 Converts an identifier to a ``Scancode``

Signature:
```cpp
Scancode StringToScancode(const std::string& code);
```
**Parameters:**
- `code`: The string to convert from

**Returns:** The scancode if it succeeded, or ``Scancode::_UNKNOWN`` if it failed

**This function has been available since:** 0.0.1

---
<a id="inputaction-scancodetostring"></a>

#### **`InputAction::ScancodeToString()`**

 Converts a ``Scancode`` to its unique identifier

Signature:
```cpp
std::string ScancodeToString(const Scancode& code);
```
**Parameters:**
- `code`: The ``Scancode`` to convert from

**Returns:** The identifier if it succeeded, or ``""`` if it failed

**This function has been available since:** 0.0.1

---
<a id="inputaction-stringtokeycode"></a>

#### **`InputAction::StringToKeycode()`**

 Converts an identifier to a ``Keycode``

Signature:
```cpp
Keycode StringToKeycode(const std::string& key);
```
**Parameters:**
- `key`: The string to convert from

**Returns:** The keycode if it succeeded, or ``Keycode::_UNKNOWN`` if it failed

**This function has been available since:** 0.0.1

---
<a id="inputaction-keycodetostring"></a>

#### **`InputAction::KeycodeToString()`**

 Converts a ``Keycode`` to its unique identifier

Signature:
```cpp
std::string KeycodeToString(const Keycode& key);
```
**Parameters:**
- `key`: The ``Keycode`` to convert from

**Returns:** The identifier if it succeeded, or ``""`` if it failed

**This function has been available since:** 0.0.1

---
<a id="inputaction-keymodtostring"></a>

#### **`InputAction::KeymodToString()`**



**Note:** The return value doesn't have any duplicates

Signature:
```cpp
std::vector<std::string> KeymodToString(const Keymod& keymod);
```
---
<a id="inputaction-stringtokeymod"></a>

#### **`InputAction::StringToKeymod()`**



**Note:** This will automatically de-duplicate names

Signature:
```cpp
Keymod StringToKeymod(const std::vector<std::string>& keymod);
```
---
