# Syngine API Documentation

## PlayerComponent.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Components/PlayerComponent.h)

PlayerComponent is responsible for handling player input, movement, and state. It manages the player's position, rotation, and interactions with the game world. @section PlayerComponent

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [PlayerComponent(GameObject* owner, Syngine::CameraComponent* camera)](#playercomponent-playercomponentgameobject-owner-syngine-cameracomponent-camera)

### Enums and Structs: 

- [PlayerState](#syngine-playerstate)

### Functions: 

- [GetComponentType()](#playercomponent-getcomponenttype)
- [Clone()](#playercomponent-clone)
- [Serialize()](#playercomponent-serialize)
- [Init()](#playercomponent-init)
- [_HandleInput()](#playercomponent-_handleinput)
- [Update()](#playercomponent-update)
- [PostPhysicsUpdate()](#playercomponent-postphysicsupdate)
- [GetPlayerState()](#playercomponent-getplayerstate)
- [GetRotation()](#playercomponent-getrotation)
- [SetRotation()](#playercomponent-setrotation)
- [SetRotation()](#playercomponent-setrotation-2)

---
<a id="syngine-playerstate"></a>

#### **`Syngine::PlayerState()`**

 Enum representing the different states a player can be in. Each state corresponds to a specific action or condition of the player

Signature:
```cpp
enum class PlayerState
```
---
<a id="playercomponent-playercomponentgameobject-owner-syngine-cameracomponent-camera"></a>

## Class Constructor

#### **`PlayerComponent(GameObject* owner, Syngine::CameraComponent* camera)`**

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
<a id="playercomponent-getcomponenttype"></a>

#### **`PlayerComponent::GetComponentType()`**

 Gets the component type

Signature:
```cpp
 Syngine::ComponentTypeID GetComponentType() override;
```
**Returns:** The component type of this component.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="playercomponent-clone"></a>

#### **`PlayerComponent::Clone()`**

 Clone the PlayerComponent

Signature:
```cpp
 std::unique_ptr<Component> Clone() const override;
```
**Returns:** A unique pointer to the cloned PlayerComponent

---
<a id="playercomponent-serialize"></a>

#### **`PlayerComponent::Serialize()`**

 Serializes the PlayerComponent to a data node

Signature:
```cpp
 Serializer::DataNode Serialize() const override;
```
**Returns:** A pointer to the serialized data node representing the PlayerComponent's state

---
<a id="playercomponent-init"></a>

#### **`PlayerComponent::Init()`**

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
<a id="playercomponent-_handleinput"></a>

#### **`PlayerComponent::_HandleInput()`**

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
<a id="playercomponent-update"></a>

#### **`PlayerComponent::Update()`**

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
<a id="playercomponent-postphysicsupdate"></a>

#### **`PlayerComponent::PostPhysicsUpdate()`**

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
<a id="playercomponent-getplayerstate"></a>

#### **`PlayerComponent::GetPlayerState()`**

 Gets the current player state.

Signature:
```cpp
 PlayerState GetPlayerState() const;
```
**Returns:** The current player state.

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="playercomponent-getrotation"></a>

#### **`PlayerComponent::GetRotation()`**

 Gets the player's world space rotation.

Signature:
```cpp
 Math::Vector2 GetRotation() const;
 Math::Vector2 GetRotation() const;
```
**Returns:** Vector2, where x is the yaw and y is the pitch, in radians

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="playercomponent-setrotation"></a>

#### **`PlayerComponent::SetRotation()`**

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
<a id="playercomponent-setrotation"></a>

#### **`PlayerComponent::SetRotation()`**

 Sets the player's world space rotation.

Signature:
```cpp
 void SetRotation(Math::Vector2 rotation);
```
**Parameters:**
- `yaw`: The yaw rotation in radians.
- `pitch`: The pitch rotation in radians.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="playercomponent-setrotation-2"></a>

#### **`PlayerComponent::SetRotation()`**

 Sets the player's world space rotation.

Signature:
```cpp
 void SetRotation(Math::Vector2 rotation);
```
**Parameters:**
- `rotation`: Vector2, where x is the yaw and y is the pitch, in radians

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `constexpr` | `Syngine` | Player component type |
| `float` | `maxPitchAngle` | Max vertical angle for the camera pitch (in degrees). |
| `float` | `sprintMult` | Multiplier for sprinting speed. |
| `float` | `crouchSpeed` | Speed when crouching. |
| `float` | `moveSpeed` | Default movement speed of the player. |
| `float` | `mouseSens` | Mouse sensitivity for camera movement. |
| `float` | `standHeight` | Height of the player when standing. |
| `float` | `crouchHeight` | Height of the player when crouching. |
| `float` | `playerRadius` | Radius of the player collider. |
| `float` | `slideDecay` | Rate at which sliding speed decays (Meters/second^2). |
| `float` | `slideSpeedMult` | Multiplier for slide speed based on sprinting speed. |
| `bool` | `enableMovement` | Whether player movement is enabled (on by default). |
| `bool` | `enableSliding` | Whether player sliding is enabled (on by default). |
| `bool` | `enableJumping` | Whether player jumping is enabled (on by default). |
| `bool` | `enableSprinting` | Whether player sprinting is enabled (on by default). |
| `bool` | `enableCrouching` | Whether player crouching is enabled (on by default). |
| `float` | `normalFov` | Normal field of view for the camera. |
| `float` | `sprintFov` | Field of view when sprinting. |
| `float` | `crouchFov` | Field of view when crouching. |
| `float` | `slideFov` | Field of view when sliding. |
| `float` | `m_eyeHeight` | Current eye height, used for smooth crouching/standing transitions |
---
