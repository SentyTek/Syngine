# Syngine

**Syngine** is a high-performance, cross-platform, modular, and modern C++ game engine built from the ground up for open-world and simulation-heavy physics based games. It focuses on flexibility, extensibility, developer control, and an incredibly powerful Lua modding API, with support for real-time detailed physics, procedural terrain generation, and more.

**Read the full Syngine documentation with examples and instructions [here](docs/index.md).**
**Get Started with Syngine: ["Quick" Start](docs/start.md).**

---

## Features
- **Modular Architecture** - Hot-swappable systems and a lightweight easy to use core
- **Input & Windowing** - SDL3-based windowing with a custom input manager with keyboard & mouse and gamepad support
- **Rendering** - Powered by bgfx for cross-platform games. Supports **Vulkan, Direct3D 12 and Metal** with user-moddable shaders and easy and powerful renderer management
- **Physics** - Jolt Physics for a fast and accurate collision system with high quality dynamics and constraints.
- **Scripting & Modding** - Integrated Lua & XML scripting for moddable gameplay logic and assets
- **Asset Pipeline** - Assimp + SPK, our custom binary bundle format with multithreaded LZ4 compression for speedy load times
- **Entity-Component-System** - Custom entities allows for easy creation, modularity and management of all game objects
- **Advanced tooling** - XML based asset pipeline, in-engine level editor, hot-reloading support, and more.

## Roadmap
Phase one: Core systems with Rendering, Physics, and Input subsystems - In progress. Complete by Q4 25
Phase two: Engine infrastructure and editing tools (Multithreading, job system, level editor, packages). Start by Winter 2025
Phase three: Simulation & Advanced rendering (Compute support, deferred rendering, audio subsystem, volumetrics, post processing)
Phase four: Modding, Multiplayer, UI systems
Phase five: More advanced asset management, animations, water, advanced tech

---

## Getting started
don't.

## Repository Layout

```
/include        -> User-facing header files
/src            -> Engine source files
    /Components -> Component type files for the Entity-Component-System
/lib            -> Other library files not in third_party abd custom libraries
/testgame       -> Files to be able to quickly test the engine
/third_party    -> Third party components like SDL
```

## Technologies used
- [SDL3](https://github.com/libsdl-org/SDL/tree/8e0b39f465c31492f9386d42e15b9df8a3bd7e21)
- [bgfx](https://github.com/bkaradzic/bgfx.cmake/tree/36a0d1d8d040113a954f99b1456121072dc77348)
- [Assimp](https://github.com/assimp/assimp/tree/ac5988422a7f1898b25c13fb5fb068c646a4c544)
- [Jolt Physics](https://github.com/jrouwe/JoltPhysics)
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)