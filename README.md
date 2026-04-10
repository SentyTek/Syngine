# Syngine

**Syngine** is a high-performance, cross-platform, modular, and modern C++ game engine built from the ground up for open-world and simulation-heavy physics based games. It focuses on flexibility, extensibility, developer control, and an incredibly powerful Lua modding API, with support for real-time detailed physics, procedural terrain generation, and more.

**Read the full Syngine documentation with examples and instructions [here](docs/index.md).**
**Get Started with Syngine: ["Quick" Start](docs/start.md).**

---

## Features
- **Input & Windowing** - SDL3-based windowing with a custom input manager with keyboard & mouse and gamepad support with Steamworks
- **Rendering** - Powered by bgfx for cross-platform games. Supports **Vulkan, Direct3D 12 and Metal** with user-moddable shaders and easy and powerful renderer management
- **Physics** - Jolt Physics for a fast and accurate collision system with high quality dynamics and constraints.
- **Scripting & Modding** - Deeply integrated Lua scripting SDK for moddable gameplay logic and assets
- **Asset Pipeline** - Assimp + SPK, our custom binary bundle format, multithreaded for speedy load times
- **Actor-Component System** - Custom entities allows for easy creation, modularity and management of all game objects. Combines ease of use of Unity GameObjects and cache benefits of Entity-Component-Systems
- **Advanced tooling** - XML based asset pipeline, in-engine level editor, hot-reloading support, and more

## Roadmap (Subject to change)
- Phase one: Core systems with Rendering, Physics, and Input subsystems - Completed Q4 2025
- Phase two: Engine infrastructure and editing tools (Multithreading, job system, level editor, ~~packages~~). - In progress. Complete by Q3 2026.
- Phase three: Simulation & Advanced rendering (Steamworks integration, Compute support, Forward+ rendering, audio subsystem, cooler rendering techniques)
- Phase four: Modding, UI systems
- Phase five: More advanced asset management, animations, advanced tech

---

## Repository Layout

```
cmake/          -> CMake files
default/        -> Included files such as shaders and gizmos to get games running quickly
docs/           -> Documentation on the engine
include/        -> User-facing header files
src/            -> Engine source files
lib/            -> Other library files not in third_party and custom libraries
third_party/    -> Third party components like SDL and Jolt
```

## Technologies used
- [SDL3](https://github.com/libsdl-org/SDL/tree/8e0b39f465c31492f9386d42e15b9df8a3bd7e21)
- [bgfx](https://github.com/bkaradzic/bgfx.cmake/tree/36a0d1d8d040113a954f99b1456121072dc77348)
- [Assimp](https://github.com/assimp/assimp/tree/ac5988422a7f1898b25c13fb5fb068c646a4c544)
- [Jolt Physics](https://github.com/jrouwe/JoltPhysics)
- [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)
- [miniscl.hpp](https://github.com/MerianBerry/SCL)
- [sol2](https://github.com/SentyTek/sol2) (our patched fork of [sol2](https://github.com/ThePhD/sol2))
- [Lua 5.5](https://lua.org/)

## License
Syngine is licensed under the MIT license
See LICENSE for details
Third-party licenses are listed in THIRD_PARTY_LICENSES.txt
