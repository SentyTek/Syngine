# Syngine Documentation
# Concept: GameObjects & Components
[<-- Back](../index.md)

**GameObjects** are the core of Syngine. Every game and every scene within a game is made up of GameObjects. They control everything from how an object appears on the screen, to the functionality of your game, to even rendering the game itself. They can represent anything from a playable character to terrain to a level load trigger. This guide will introduce GameObjects, what they do, how to add them to your game, and how to use them effectively.

## How to use GameObjects
Adding a GameObject to your project is one line of code:
```cpp
auto* myPlayer = new Syngine::GameObject("player");
```

On it's own, a GameObject won't do much. **Components** are needed to give functionality to GameObjects. For example, the `MeshComponent` needs to be added to give the object a 3D mesh to render, or the `RigidbodyComponent` is needed to give an object physics.

Syngine has many components out of the box to add to your GameObjects to give them functionality.

To add a component to a GameObject, it is simple:
```cpp
// Load the mesh "player.glb" from the "meshes.spk" bundle, with loadTextures=false.
myPlayer->AddComponent<Syngine::MeshComponent>("meshes/meshes.spk", "player.glb", false);
```
