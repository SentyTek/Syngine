# Syngine API Documentation


## PlayerComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/PlayerComponent.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [E](#synginee)
- [GetComponentType](#synginegetcomponenttype)
- [Clone](#syngineclone)
- [Init](#syngineinit)
- [_HandleInput](#syngine_handleinput)
- [Update](#syngineupdate)
- [_PostPhysicsUpdate](#syngine_postphysicsupdate)
- [GetPlayerState](#synginegetplayerstate)
- [GetRotation](#synginegetrotation)
- [SetRotation](#synginesetrotation)

---

## Class Constructor


#### **`Syngine::PlayerComponent`**


 Constructor for PlayerComponent

#### This function is internal use only and not intended for public use!


**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 PlayerComponent(GameObject* owner, Syngine::CameraComponent* camera);
```

**Parameters:**

- `static`: Player component type
- `owner`: The GameObject that owns this component.
- `camera`: The camera component to use for the player.
- `win`: The SDL window for the player.

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::E`**


 Enum representing the different states a player can be in. Each state corresponds to a specific action or condition of the player

Signature:

```cpp
 IDLE = 0, WALKING = 1,
```

---

#### **`Syngine::GetComponentType`**


 Gets the component type

Signature:

```cpp
 Syngine::Components GetComponentType() override;
```

**Returns:** The component type of this component.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::Clone`**


 Clone the PlayerComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override { return std::make_unique<PlayerComponent>(*this);
```

**Returns:** A unique pointer to the cloned PlayerComponent

---

#### **`Syngine::Init`**


 Initializes the player component with camera, window, and physics manager.

#### This function is internal use only and not intended for public use!


**Note:** This should only be called when the component is added to a GameObject

Signature:

```cpp
 void Init(Syngine::CameraComponent* camera);
```

**Parameters:**

- `camera`: The camera component to use for the player.
- `win`: The SDL window for the player.
- `physicsManager`: The physics manager to use for the player.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::_HandleInput`**


 Handles input events for the player component.

#### This function is internal use only and not intended for public use!


**Note:** This is called every time an input event occurs, such as key presses or mouse movements.

Signature:

```cpp
 void _HandleInput(const SDL_Event& event);
```

**Parameters:**

- `event`: The SDL Events.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::Update`**


 Updates the player. Mostly handles movement, physics, and updating the state.

#### This function is internal use only and not intended for public use!


**Note:** This is called every frame to update the player's position, rotation, and state.

Signature:

```cpp
 void Update(const bool* keystate, float deltaTime);
```

**Parameters:**

- `keystate`: The current state of the keyboard.
- `simulate`: Whether to simulate physics or not.
- `deltaTime`: The time since the last update.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::_PostPhysicsUpdate`**


 Updates position and camera after physics simulation.

#### This function is internal use only and not intended for public use!


**Note:** This is called after the physics simulation to update the player's position and camera based on the physics simulation results.

Signature:

```cpp
 void _PostPhysicsUpdate();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetPlayerState`**


 Gets the current player state.

Signature:

```cpp
 PlayerState GetPlayerState() const;
```

**Returns:** The current player state.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::GetRotation`**


 Gets the player's world space rotation.

Signature:

```cpp
 float* GetRotation() const;
```

**Returns:** pointer to an array of two floats, representing yaw and pitch respectively.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Syngine::SetRotation`**


 Sets the player's world space rotation.

Signature:

```cpp
 void SetRotation(float yaw, float pitch);
```

**Parameters:**

- `yaw`: The yaw rotation in radians.
- `pitch`: The pitch rotation in radians.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

