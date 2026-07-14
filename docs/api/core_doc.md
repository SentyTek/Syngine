# Syngine API Documentation

## Core.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Core.h)

Core class to manage the application @section Core

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [Core(const EngineConfig config)](#core-coreconst-engineconfig-config)

### Enums and Structs: 

- [HardwareSpecs](#syngine-hardwarespecs)
- [EngineConfig](#syngine-engineconfig)
- [DebugModes](#syngine-debugmodes)
- [Context](#core-context)
- [_FrameCounter](#core-_framecounter)

### Functions: 

- [Initialize()](#core-initialize)
- [Get()](#core-get)
- [IsRunning()](#core-isrunning)
- [HandleEvents()](#core-handleevents)
- [Update()](#core-update)
- [Render()](#core-render)
- [GetPhysicsManager()](#core-getphysicsmanager)
- [GetSystemSpecifications()](#core-getsystemspecifications)
- [SetSimulationState()](#core-setsimulationstate)
- [GetSimulationState()](#core-getsimulationstate)
- [IsInitialized()](#core-isinitialized)
- [RunLua()](#core-runlua)
- [IsPhysicsEnabled()](#core-isphysicsenabled)
- [SetDebugMode()](#core-setdebugmode)
- [GetDebugMode()](#core-getdebugmode)
- [_GetContext()](#core-_getcontext)
- [_GetConfig()](#core-_getconfig)
- [_HandleKeyEvent()](#core-_handlekeyevent)

---
<a id="syngine-hardwarespecs"></a>

#### **`Syngine::HardwareSpecs()`**

 Struct to hold hardware specifications

**Note:** This should only be called after window creation, as it relies on SDL for some information.

Signature:
```cpp
struct HardwareSpecs
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `osName` | Operating system name |
| `std::string` | `cpuModel` | CPU model name |
| `std::string` | `cpuArch` | CPU architecture (e.g., x86_64, ARM) |
| `int` | `cpuCores` | Number of CPU cores |
| `int` | `ramMB` | Amount of RAM in MB |
| `int` | `screenWidth` | Width of the screen in pixels |
| `int` | `screenHeight` | Height of the screen in pixels |
| `int` | `winWidth` | Width of the game window in pixels |
| `int` | `winHeight` | Height of the game window in pixels |
| `std::string` | `gpuVendorID` | GPU vendor PCI ID formatted as hex |
| `std::string` | `gpuName` | GPU adapter name |
| `int` | `maxTextureSize` | Maximum texture size supported by the GPU |
| `bool` | `supportsCompute` | Whether the GPU supports compute shaders |
| `bool` | `supports3DTextures` | Whether the GPU supports 3D textures |
**This function has been available since:** v0.0.1

---
<a id="syngine-engineconfig"></a>

#### **`Syngine::EngineConfig()`**

 Struct to hold engine config

Signature:
```cpp
struct EngineConfig
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `gameName` | Title of the game window |
| `int` | `windowWidth` | Width of the game window in pixels |
| `int` | `windowHeight` | Height of the game window in pixels |
| `bool` | `usePhysics` | Whether to initialize the physics system |
| `bool` | `useLua` | Whether to initialize the Lua scripting system |
| `bool` | `headless` | Whether to run in headless mode (no window, for |
**This function has been available since:** v0.0.1

---
<a id="syngine-debugmodes"></a>

#### **`Syngine::DebugModes()`**

 the various debug modes possible

Signature:
```cpp
struct DebugModes
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `bool` | `Enabled` | Global debug toggle |
| `bool` | `PhysWireframes` | Whether to draw physics wireframes |
| `bool` | `Gizmos` | Gizmos such as cameras, lights, and audio sources |
| `bool` | `CSMBounds` | Cascading Shadow Map zone bounds. |
| `bool` | `DrawBoundingBoxes` | Whether to draw mesh bounding boxes |
**This function has been available since:** v0.0.1

---
<a id="core-coreconst-engineconfig-config"></a>

## Class Constructor

#### **`Core(const EngineConfig config)`**

 Constructor for the Core class

Signature:
```cpp
 Core(const EngineConfig config);
```
**Parameters:**
- `config`: Engine configuration (Syngine::EngineConfig)

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if initialization fails (e.g., SDL_Init() fails or missing files)

---
<a id="core-initialize"></a>

#### **`Core::Initialize()`**

 Initialize the core system. Creates a window, renderer, and other subsystems.

Signature:
```cpp
 static bool Initialize(const RendererConfig rendererConfig);
```
**Parameters:**
- `rendererConfig`: Renderer configuration options (Syngine::RendererConfig)

**Returns:** True on success, false on failure

**This function has been available since:** v0.0.1

---
<a id="core-get"></a>

#### **`Core::Get()`**

 Get the global Core instance

Signature:
```cpp
 static Core* Get();
```
**Returns:** Pointer to the global Core instance

**This function has been available since:** v0.0.1

---
<a id="core-isrunning"></a>

#### **`Core::IsRunning()`**

 Check if the application is running

**Note:** Used to determine if the main loop should continue

Signature:
```cpp
 bool IsRunning();
```
**Returns:** True if the application is running, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-handleevents"></a>

#### **`Core::HandleEvents()`**

 Handle events for the application

Signature:
```cpp
 bool HandleEvents();
```
**Returns:** True if events were handled, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-update"></a>

#### **`Core::Update()`**

 Update the application state

Signature:
```cpp
 bool Update();
```
**Returns:** True if the update was successful, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-render"></a>

#### **`Core::Render()`**

 Render the application

Signature:
```cpp
 bool Render(CameraComponent* camera);
```
**Returns:** True if the render was successful, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-getphysicsmanager"></a>

#### **`Core::GetPhysicsManager()`**

 Get the physics manager.

Signature:
```cpp
 Syngine::Phys* GetPhysicsManager();
```
**Returns:** Pointer to the physics manager, or nullptr if not initialized

---
<a id="core-getsystemspecifications"></a>

#### **`Core::GetSystemSpecifications()`**

 Get system specifications

**Preconditions:** Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)

Signature:
```cpp
 Syngine::HardwareSpecs GetSystemSpecifications();
```
**Returns:** Hardware specifications of the system

**This function has been available since:** v0.0.1

---
<a id="core-setsimulationstate"></a>

#### **`Core::SetSimulationState()`**

 Set the simulation state

Signature:
```cpp
 void SetSimulationState(bool simulate);
```
**Parameters:**
- `simulate`: True to enable simulation, false to disable

**This function has been available since:** v0.0.1

---
<a id="core-getsimulationstate"></a>

#### **`Core::GetSimulationState()`**

 Get the simulation state

Signature:
```cpp
 bool GetSimulationState();
```
**Returns:** True if simulation is enabled, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-isinitialized"></a>

#### **`Core::IsInitialized()`**

 Check if the Core is initialized

Signature:
```cpp
 static bool IsInitialized() noexcept;
```
**Returns:** True if initialized, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-runlua"></a>

#### **`Core::RunLua()`**

 Runs the main Lua script

Signature:
```cpp
 static void RunLua();
```
---
<a id="core-isphysicsenabled"></a>

#### **`Core::IsPhysicsEnabled()`**

 Check if physics is enabled in the engine config

Signature:
```cpp
 static bool IsPhysicsEnabled();
```
**Returns:** True if physics is enabled, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-setdebugmode"></a>

#### **`Core::SetDebugMode()`**

 Set the current debug modes

Signature:
```cpp
 static bool SetDebugMode(DebugModes mode);
```
**Parameters:**
- `mode`: DebugModes struct with desired debug settings

**Returns:** True on success, false otherwise

**This function has been available since:** v0.0.1

---
<a id="core-getdebugmode"></a>

#### **`Core::GetDebugMode()`**

 Get the current debug modes

Signature:
```cpp
 static DebugModes GetDebugMode();
```
**Returns:** Current DebugModes struct

**This function has been available since:** v0.0.1

---
<a id="core-context"></a>

#### **`Core::Context()`**

 Struct to hold application state

Signature:
```cpp
 struct Context
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `EngineConfig` | `config` | Engine configuration |
| `std::unique_ptr<Window>` | `window` | Pointer to the window |
| `std::unique_ptr<Renderer>` | `renderer` | Pointer to the render system |
| `std::unique_ptr<ModelLoader>` | `synModels` | Pointer to the model loader |
| `std::unique_ptr<Phys>` | `physicsManager` | Pointer to the physics manager |
| `std::unique_ptr<ZoneManager>` | `zoneManager` | Pointer to the zone manager |
| `std::unique_ptr<LuaManager>` | `luaState` | Pointer to the Lua state |
| `DebugModes` | `debug` | Debug modes flags |
**This function has been available since:** v0.0.1

---
<a id="core-_getcontext"></a>

#### **`Core::_GetContext()`**

 Get the global App instance

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static Context* _GetContext();
```
**Returns:** Pointer to the global Context instance

**This function has been available since:** v0.0.1

---
<a id="core-_getconfig"></a>

#### **`Core::_GetConfig()`**

 Get the global game config

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static inline EngineConfig* _GetConfig();
```
**Returns:** Pointer to the global game config

**This function has been available since:** v0.0.1

---
<a id="core-_framecounter"></a>

#### **`Core::_FrameCounter()`**

 Frame counter for FPS and TPS tracking

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 struct _FrameCounter
```
**This function has been available since:** v0.0.1

---
<a id="core-_handlekeyevent"></a>

#### **`Core::_HandleKeyEvent()`**

~~Handle key events for debug actions
~~
**DEPRECATED**
Signature:
```cpp
 void _HandleKeyEvent(const SDL_Event& event);
```
**Parameters:**
- `event`: SDL_Event to handle

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `Core*` | `m_instance` | Pointer to the global Core instance |
| `Context*` | `m_context` | Pointer to the global Context instance |
| `bool` | `m_shouldClose` | Whether the application should close |
| `_internal` | `m_internal` | Internal state struct |
| `_FrameCounter` | `m_frameCounter` | Frame counter for FPS/TPS tracking |
---
