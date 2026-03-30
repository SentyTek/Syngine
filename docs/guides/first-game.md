# Syngine Documentation
# Guide: Creating your first game
[<-- Back](../index.md)

This guide will show you through the basics of Syngine's Actor-Component system, the way we create things to show in and manage the game, and the Input system, how we can interact with the game.

One quick note: We also recommend simply including `Syngine/Syngine.h`, this will handle all dependencies for you.

## The Actor-Component System
In Syngine, every entity in your game world (players, enemies, environment pieces) is known as a `GameObject`. To give these objects properties and behaviors, we attach `Component`s to them.

### Creating a GameObject
You can create a basic GameObject specifying its name and a type.

```cpp
#include "Syngine/Syngine.h"

// Instantiate a new GameObject
// GameObject(name, shader[, tags])
auto* myObject = new Syngine::GameObject("Player", "default", "PlayerTag");
```
*Note: In Syngine, GameObjects are usually registered and managed centrally by the `Registry`.*

### Adding Components
A plain GameObject doesn't do much. You add functionality by calling `AddComponent<T>()`. Common components include `TransformComponent`, `MeshComponent`, `RigidbodyComponent`, and `PlayerComponent`.

Here is how you might turn an empty GameObject into an interactive box:
```cpp
#include "Syngine/Syngine.h"

// 1. Add position and rotation
myObject->AddComponent<Syngine::TransformComponent>()->SetPosition(0.0f, 5.0f, 0.0f);
// OR
auto* transform = myObject->AddComponent<Syngine::TransformComponent>();
transform->SetPosition(0.f, 5.f, 0.f);

// 2. Add visual representation
myObject->AddComponent<Syngine::MeshComponent>("meshes/box.gltf"); //All paths are based out of assets/

// 3. Add game physics
Syngine::RigidbodyParameters params;
params.mass = 1.0f;
params.shape = Syngine::CollisionShape::BOX;
myObject->AddComponent<Syngine::RigidbodyComponent>(params);
```

---

## Interacting with the Game: The Input System
To control your GameObjects or manage game states, you interact with Syngine's Input system. Input actions map key bindings (like `W`, `Space`, or mouse buttons) to C++ callbacks.

### Registering Input Actions
Every input action needs a **unique identifier**. You can set a display name, an action category, the key to bind, and most importantly, the `Callbacks` object indicating what happens when the button is pressed, held, or released.

```cpp
#include "Syngine/Core/Input.h"

void OnJumpPressed() {
    // Jump logic here
}

// Map the SPACE key to the OnJumpPressed function
Syngine::InputAction::RegisterAction(
    "player.jump",                           // Unique Identifier
    "Jump",                                  // Display Name
    "Player Controls",                       // Category
    Syngine::KeyBinding(Syngine::Keycode::SPACE),
    { .onPressed = OnJumpPressed }           // Callbacks
);
```
```cpp
Syngine::InputAction::RegisterAction(
    "identifier",
    "display name",
    "category",
    keybinding
    {
        .onPressed = callback1,
        .onReleased = callback2,
        .onStateChanged = callback3
    }
)
```
With these building blocks, you're ready to start filling your world with dynamic, responsive GameObjects! Of course, feel free to inspect headers, use intellisense, or view the [API Docs](../index.md#api-reference) to get further information on the API.
