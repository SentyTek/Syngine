# Syngine API Documentation


## PlayerComponent.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/PlayerComponent.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [PlayerState](#syngineplayerstate)
- [static](#playercomponentstatic)
- [GetComponentType](#playercomponentgetcomponenttype)
- [Clone](#playercomponentclone)
- [Serialize](#playercomponentserialize)
- [Init](#playercomponentinit)
- [_HandleInput](#playercomponent_handleinput)
- [Update](#playercomponentupdate)
- [PostPhysicsUpdate](#playercomponentpostphysicsupdate)
- [GetPlayerState](#playercomponentgetplayerstate)
- [GetRotation](#playercomponentgetrotation)
- [SetRotation](#playercomponentsetrotation)

---

## Class Constructor


#### **`PlayerComponent::PlayerComponent`**


 Constructor for PlayerComponent

#### This function is internal use only and not intended for public use!


**Note:** This should only be called by GameObject::AddComponent<T>()

Signature:

```cpp
 PlayerComponent(GameObject* owner, Syngine::CameraComponent* camera);
```

**Parameters:**

- `owner`: The GameObject that owns this component.
- `camera`: The camera component to use for the player.
- `win`: The SDL window for the player.

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::PlayerState`**


 Enum representing the different states a player can be in. Each state corresponds to a specific action or condition of the player

Signature:

```cpp
enum class PlayerState
```

---

#### **`PlayerComponent::static`**


 PlayerComponent is responsible for handling player input, movement, and state. It manages the player's position, rotation, and interactions with the game world.

Signature:

```cpp
 public: static constexpr Syngine::ComponentTypeID componentType = SYN_COMPONENT_PLAYER; //* Player component type
```

---

#### **`PlayerComponent::GetComponentType`**


 Gets the component type

Signature:

```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```

**Returns:** The component type of this component.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`PlayerComponent::Clone`**


 Clone the PlayerComponent

Signature:

```cpp
 std::unique_ptr<Component> Clone() const override;
```

**Returns:** A unique pointer to the cloned PlayerComponent

---

#### **`PlayerComponent::Serialize`**


 Serializes the PlayerComponent to a data node

Signature:

```cpp
 Serializer::DataNode Serialize() const override;
```

**Returns:** A pointer to the serialized data node representing the PlayerComponent's state

---

#### **`PlayerComponent::Init`**


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

#### **`PlayerComponent::_HandleInput`**


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

#### **`PlayerComponent::Update`**


 Updates the player. Mostly handles movement, physics, and updating the state.

**Note:** This is called every frame to update the player's position, rotation, and state. Fetches keyboard state internally.

Signature:

```cpp
 void Update(float deltaTime) override;
```

**Parameters:**

- `deltaTime`: The time since the last update.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`PlayerComponent::PostPhysicsUpdate`**


 Updates position and camera after physics simulation.

#### This function is internal use only and not intended for public use!


**Note:** This is called after the physics simulation to update the player's position and camera based on the physics simulation results.

Signature:

```cpp
 void PostPhysicsUpdate() override;
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`PlayerComponent::GetPlayerState`**


 Gets the current player state.

Signature:

```cpp
 PlayerState GetPlayerState() const;
```

**Returns:** The current player state.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`PlayerComponent::GetRotation`**


 Gets the player's world space rotation.

Signature:

```cpp
 float* GetRotation() const;
```

**Returns:** pointer to an array of two floats, representing yaw and pitch respectively.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`PlayerComponent::SetRotation`**


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

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `float` | `maxPitchAngle` | Max vertical angle for the camera pitch (in degrees). |
| `float` | `sprintMult` | Multiplier for sprinting speed. |
| `float` | `crouchSpeed` | Speed when crouching. |
| `float` | `moveSpeed` | Default movement speed of the player. |
| `float` | `mouseSens` | Mouse sensitivity for camera movement. |
| `float` | `standHeight` | Height of the player when standing. |
| `float` | `crouchHeight` | Height of the player when crouching. |
| `float` | `playerRadius` | Radius of the player collider. |
| `bool` | `enableMovement` | Whether player movement is enabled (on by default). |
| `bool` | `enableSliding` | Whether player sliding is enabled (on by default). |
| `bool` | `enableJumping` | Whether player jumping is enabled (on by default). |
| `bool` | `enableSprinting` | Whether player sprinting is enabled (on by default). |
| `bool` | `enableCrouching` | Whether player crouching is enabled (on by default). |

---

