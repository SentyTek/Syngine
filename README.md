# Syngine

**Syngine** is a proprietary, cross-platform, modular, and modern C++ game engine built from the ground up for open-world and simulation-heavy physics based games.

---

## Features
- **Modular Architecture** - Hot-swappable systems and a lightweight easy to use core
- **Input & Windowing** - SDL3-based windowing with a custom input manager with keyboard & mouse and gamepad support.
- **Rendering** - Powered by bgfx for cross-platform games. Supports **Vulkan, Direct3D 11, Metal, and OpenGL**
- **Physics** - Jolt Physics for a fast and accurate collision system with high quality dynamics and constraints.
- **Scripting & Modding** - Integrated Lua & XML scripting for moddable gameplay logic and assets
- **Asset Pipeline** - Assimp + SPK, our custom binary bundle format with multithreaded LZ4 compression for speedy load times
- **Entity-Component-System** - Completely modular entities allows for easy modding, creation, and management of all game objects

---

## Getting started
don't.

## Repository Layout

```
/include        -> User-facing header files
/src            -> Engine source files
    /Components -> Component type files for the Entity-Component-System
/lib            -> Other library files not in third_party abd custom libraries
```

## Technologies used
- [SDL3](https://github.com/libsdl-org/SDL/tree/8e0b39f465c31492f9386d42e15b9df8a3bd7e21)
- [bgfx](https://github.com/bkaradzic/bgfx.cmake/tree/36a0d1d8d040113a954f99b1456121072dc77348)
- [Assimp](https://github.com/assimp/assimp/tree/ac5988422a7f1898b25c13fb5fb068c646a4c544)
- [Jolt Physics](https://github.com/jrouwe/JoltPhysics)
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)