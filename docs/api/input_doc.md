# Syngine API Documentation


## Input.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Input.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [KeyUnbound](#synginekeyunbound)
- [AnyKeybind](#syngineanykeybind)
- [KeyShortcut](#synginekeyshortcut)
- [Scancode](#synginescancode)
- [Constructor](#class-constructor)
- [Constructor](#class-constructor)
- [Constructor](#class-constructor)
- [subType](#synginesubtype)
- [Operator == overload](#syngineoperator==overload)
- [_isTriggeredByEvent](#syngine_istriggeredbyevent)
- [KeySequence](#synginekeysequence)
- [Scancode](#synginescancode)
- [Constructor](#class-constructor)
- [Constructor](#class-constructor)
- [Constructor](#class-constructor)
- [subType](#synginesubtype)
- [reset](#synginereset)
- [next](#synginenext)
- [increment](#syngineincrement)
- [wasReset](#synginewasreset)
- [_isTriggeredByEvent](#syngine_istriggeredbyevent)
- [Operator == overload](#syngineoperator==overload)
- [KeyBinding](#synginekeybinding)
- [Constructor](#class-constructor)
- [Constructor](#class-constructor)
- [subType](#synginesubtype)
- [Operator == overload](#syngineoperator==overload)
- [_isTriggeredByEvent](#syngine_istriggeredbyevent)
- [_isTriggeredByEvent](#syngine_istriggeredbyevent)
- [](#inputaction)
- [onPressed](#inputactiononpressed)
- [onReleased](#inputactiononreleased)
- [onStateChanged](#inputactiononstatechanged)
- [Constructor](#class-constructor)
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


#### **`Syngine::of`**


 A tag enum representing each of the keybindable types

Signature:

```cpp
 // of modifiers SEQUENCE = 4, //* A keybind that is bound to a sequence of keycodes,
```

**Parameters:**

- `UNBOUND`: A keybind that is not bound to any key
- `KEYCODE`: A keybind that is bound to a specific keycode
- `SCANCODE`: A keybind that is bound to a specific scancode
- `SHORTCUT`: A keybind that is bound to a keycode or scancode and a set

**This function has been available since:** 0.0.1

---

## Class & Related Members


#### **`Syngine::KeyUnbound`**


 A type representing an unbound action, or the lack of a key binding

Signature:

```cpp
using KeyUnbound = std::monostate;
```

**Parameters:**

- `MOUSE_BUTTON`: A keybind that is bound to a mouse button

**This function has been available since:** 0.0.1

---

#### **`Syngine::AnyKeybind`**


 A type representing any keybind

Signature:

```cpp
using AnyKeybind = std::variant<KeyUnbound, Keycode,
```

**This function has been available since:** 0.0.1

---

#### **`Syngine::KeyShortcut`**


 A type representing a keyboard shortcut; a single keycode or scancode and any number of modifiers

Signature:

```cpp
struct KeyShortcut 
```

**This function has been available since:** 0.0.1

---

#### **`Syngine::Scancode`**




#### This function is internal use only and not intended for public use!


Signature:

```cpp
 std::variant<Keycode, Scancode> key;
```

**This function has been available since:** 0.0.1

---

## Class Constructor


#### **`Syngine::KeyShortcut`**


 `KeyShortcut`s aren't default-constructable

**Note:** Use a ``KeyBinding`` holding a ``KeyUnknown`` (the default constructor of `KeyBinding`) instead

Signature:

```cpp
 KeyShortcut() = delete;
```

**This function has been available since:** 0.0.1

---

## Class & Related Members


## Class Constructor


#### **`Syngine::KeyShortcut`**


 Create a ``KeyShortcut`` with no modifiers

Signature:

```cpp
 KeyShortcut(std::variant<Keycode, Scancode> key) : key(key), modifiers() ;
```

**Parameters:**

- `key`: The key to bind

**This function has been available since:** 0.0.1

---

## Class & Related Members


## Class Constructor


#### **`Syngine::KeyShortcut`**


 Create a ``KeyShortcut`` with the given modifiers

Signature:

```cpp
 KeyShortcut(std::variant<Keycode, Scancode> key, Keymod modifiers) : key(key), modifiers(modifiers) ;
```

**Parameters:**

- `key`: The key to bind
- `modifiers`: The modifiers to bind

**This function has been available since:** 0.0.1

---

## Class & Related Members


#### **`Syngine::subType`**


 Returns the ``KeybindType`` corresponding to the type this object contains, if any

Signature:

```cpp
 KeybindType subType() const { return static_cast<KeybindType>(key.index() + 1);
```

**This function has been available since:** 0.0.1

---

#### **`Operator == overload`**


 Equality operator for ``KeyShortcut``

Signature:

```cpp
 bool operator==(const KeyShortcut& other) const { return key == other.key && modifiers == other.modifiers;
```

**Parameters:**

- `other`: The ``KeyShortcut`` to compare against

**This function has been available since:** 0.0.1

---

#### **`Syngine::_isTriggeredByEvent`**


 Checks if the keyboard event triggers this binding

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _isTriggeredByEvent(SDL_KeyboardEvent event) const;
```

**Parameters:**

- `event`: The event to check against

**This function has been available since:** 0.0.1

---

#### **`Syngine::KeySequence`**


 An array of key bindings which is triggered when pressed in sequence

Signature:

```cpp
struct KeySequence 
```

**This function has been available since:** 0.0.1

---

#### **`Syngine::Scancode`**




#### This function is internal use only and not intended for public use!


Signature:

```cpp
 std::vector<std::variant<Keycode, Scancode, KeyShortcut>> bound;
```

**This function has been available since:** 0.0.1

---

## Class Constructor


#### **`Syngine::KeySequence`**


 `KeySequence`s aren't default-constructable and must hold at least one binding

**Note:** Use a ``KeyBinding`` holding a ``KeyUnknown`` (the default constructor of `KeyBinding`) instead

Signature:

```cpp
 KeySequence() = delete;
```

**This function has been available since:** 0.0.1

---

## Class & Related Members


## Class Constructor


#### **`Syngine::KeySequence`**


 Create a ``KeySequence`` with a single binding. This isn't very useful on it's own and may be removed

Signature:

```cpp
 KeySequence(std::variant<Keycode, Scancode, KeyShortcut> key) : bound{ key }, nextIndex(0) {
```

**Parameters:**

- `key`: The binding to bind

**This function has been available since:** 0.0.1

---

## Class & Related Members


## Class Constructor


#### **`Syngine::KeySequence`**


 Create a ``KeySequence`` with a list of bindings

Signature:

```cpp
 KeySequence( std::initializer_list<std::variant<Keycode, Scancode, KeyShortcut>>
```

**Parameters:**

- `list`: The list of bindings to bind

**This function has been available since:** 0.0.1

---

## Class & Related Members


#### **`Syngine::subType`**


 Returns the ``KeybindType`` corresponding to the type this object contains at the given index, or ``KeybindType::UNBOUND`` if the index is out of bounds

Signature:

```cpp
 KeybindType subType(uint32_t index) const { if (index < bound.size()) {
```

**Parameters:**

- `index`: The index to find the type of

**This function has been available since:** 0.0.1

---

#### **`Syngine::reset`**


 Resets the trigger progress of the sequence

**Note:** You probably shouldn't call this unless you really need to, and it might be removed

Signature:

```cpp
 void reset() ;
```

**This function has been available since:** 0.0.1

---

#### **`Syngine::next`**


 Gets the next key to be triggered

**Note:** You probably shouldn't call this unless you really need to, and it might be removed

Signature:

```cpp
 AnyKeybind next() { const std::variant<Keycode, Scancode, KeyShortcut> nextKey =
```

**This function has been available since:** 0.0.1

---

#### **`Syngine::increment`**


 Increments the trigger progress of the sequence, as if the previous key had been triggered

**Note:** You probably shouldn't call this unless you really need to, and it might be removed

Signature:

```cpp
 void increment() { this->nextIndex = (nextIndex + 1) % bound.size();
```

**This function has been available since:** 0.0.1

---

#### **`Syngine::wasReset`**


 Gets the reset state of the sequence

Signature:

```cpp
 bool wasReset() const ;
```

**This function has been available since:** 0.0.1

---

#### **`Syngine::_isTriggeredByEvent`**


 Determines if the keybind is triggered, increments the progress if it was, and resets the sequence if it wasn't

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _isTriggeredByEvent(SDL_KeyboardEvent event);
```

**Parameters:**

- `event`: The ``SDL_KeyboardEvent`` to check the sequence against

**This function has been available since:** 0.0.1

---

#### **`Operator == overload`**


 Equality operator for ``KeySequence``

Signature:

```cpp
 bool operator==(const KeySequence& other) const { return bound == other.bound;
```

**Parameters:**

- `other`: The ``KeySequence`` to compare against

**This function has been available since:** 0.0.1

---

#### **`Syngine::KeyBinding`**


 A container for holding any type of keybind

Signature:

```cpp
struct KeyBinding 
```

**This function has been available since:** 0.0.1

---

## Class Constructor


#### **`Syngine::KeyBinding`**


 Construct a `KeyBinding` containing a ``KeyUnbound``

Signature:

```cpp
 KeyBinding() : binding(KeyUnbound()) {};
```

**This function has been available since:** 0.0.1

---

## Class & Related Members


## Class Constructor


#### **`Syngine::KeyBinding`**


 Construct a `KeyBinding` from ``AnyKeybind``

Signature:

```cpp
 KeyBinding(AnyKeybind bind) : binding(bind) ;
```

**Parameters:**

- `keycode`: The keycode to bind

**This function has been available since:** 0.0.1

---

## Class & Related Members


#### **`Syngine::subType`**


 Returns the ``KeybindType`` corresponding to the type this object contains

Signature:

```cpp
 KeybindType subType() const { return static_cast<KeybindType>(binding.index());
```

**This function has been available since:** 0.0.1

---

#### **`Operator == overload`**


 Equality operator for ``KeyBinding``

Signature:

```cpp
 bool operator==(const KeyBinding& other) const { return binding == other.binding;
```

**Parameters:**

- `other`: The ``KeyBinding`` to compare against

**This function has been available since:** 0.0.1

---

#### **`Syngine::_isTriggeredByEvent`**


 Determines if the KeyBinding is triggered by a keyboard event

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _isTriggeredByEvent(SDL_KeyboardEvent event);
```

**Parameters:**

- `event`: The ``SDL_KeyboardEvent`` to check against

**This function has been available since:** 0.0.1

---

#### **`Syngine::_isTriggeredByEvent`**


 Determines if the KeyBinding is triggered by a keyboard event

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 bool _isTriggeredByEvent(SDL_MouseButtonEvent event);
```

**Parameters:**

- `event`: The ``SDL_KeyboardEvent`` to check against

**This function has been available since:** 0.0.1

---

#### **`InputAction::`**




Signature:

```cpp
 /// @brief The callback called when the action is triggered
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::onPressed`**




Signature:

```cpp
 std::function<void()> onPressed = []() -> void {};
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::onReleased`**


 The callback called when the action is released

Signature:

```cpp
 std::function<void()> onReleased = []() -> void {};
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::onStateChanged`**


 The callback called whenever the trigger state of the action changes

Signature:

```cpp
 std::function<void()> onStateChanged = []() -> void {};
```

**This function has been available since:** 0.0.1

---

## Class Constructor


#### **`InputAction::InputAction`**


 Make an InputAction with a binding and callbacks

**Note:** Logs a fatal error and crashes the program if the identifier is not unique

Signature:

```cpp
 InputAction(const std::string& identifier, const std::string& name,
```

**Parameters:**

- `identifier`: The identifier for the input action. This is required and *must* be unique
- `name`: The name for the input action. This is the display name that is shown in the bindings menu
- `category`: The category for the input action. This is used for organizing actions in the bindings menu
- `binding`: The binding for the input action
- `callbacks`: The callbacks to be called when the input action is triggered

---

## Class & Related Members


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

**This function has been available since:** 0.0.1

---

#### **`InputAction::wasPressed`**


 Returns true if the binding was pressed since the previous frame

Signature:

```cpp
 bool wasPressed();
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::wasReleased`**


 Returns true if the binding was released since the last frame

Signature:

```cpp
 bool wasReleased();
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::stateChanged`**


 Returns true if the binding's state has changed since the last frame

Signature:

```cpp
 bool stateChanged();
```

**This function has been available since:** 0.0.1

---

#### **`InputAction::setOnPressedCallback`**


 Sets the `onPressed` callback

Signature:

```cpp
 void setOnPressedCallback(std::function<void()> callback) { callbacks.onPressed = callback;
```

**Parameters:**

- `callback`: The callback to set

**This function has been available since:** 0.0.1

---

#### **`InputAction::setOnReleasedCallback`**


 Sets the `onReleased` callback

Signature:

```cpp
 void setOnReleasedCallback(std::function<void()> callback) { callbacks.onReleased = callback;
```

**Parameters:**

- `callback`: The callback to set

**This function has been available since:** 0.0.1

---

#### **`InputAction::setOnStateChangedCallback`**


 Sets the `onStateChanged` callback

Signature:

```cpp
 void setOnStateChangedCallback(std::function<void()> callback) { callbacks.onStateChanged = callback;
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
 static void RegisterAction(const std::string& identifier, const std::string& name,
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
 bool operator==(const InputAction& other) const { return binding == other.binding;
```

**Parameters:**

- `other`: The ``InputAction`` to compare against

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

