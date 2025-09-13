# Syngine API Documentation


## Core.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Core.h)

---

## Goto: 


- [HardwareSpecs](#synginehardwarespecs)
- [EngineConfig](#syngineengineconfig)
- [App](#syngineapp)
- [Core](#corecore)
- [Initialize](#coreinitialize)
- [Get](#coreget)
- [_GetApp](#core_getapp)
- [_GetConfig](#core_getconfig)
- [IsRunning](#coreisrunning)
- [HandleEvents](#corehandleevents)
- [Update](#coreupdate)
- [Render](#corerender)
- [GetPhysicsManager](#coregetphysicsmanager)
- [GetSystemSpecifications](#coregetsystemspecifications)
- [SetSimulationState](#coresetsimulationstate)
- [GetSimulationState](#coregetsimulationstate)

---

#### **`Syngine::HardwareSpecs`**


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
| `int` | `gpuVendorID` | GPU vendor ID |
| `int` | `gpuDeviceID` | GPU device ID |
| `int` | `maxTextureSize` | Maximum texture size supported by the GPU |
| `bool` | `supportsCompute` | Whether the GPU supports compute shaders |
| `bool` | `supports3DTextures` | Whether the GPU supports 3D textures |

**This function has been available since:** v0.0.1

---

#### **`Syngine::EngineConfig`**


 Struct to hold engine config

Signature:

```cpp
struct EngineConfig 
```

**Members:**

| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `windowTitle` | Title of the game window |
| `int` | `windowWidth` | Width of the game window in pixels |
| `int` | `windowHeight` | Height of the game window in pixels |
| `bool` | `vsync` | Whether vertical sync is enabled |

**This function has been available since:** v0.0.1

---

#### **`Syngine::App`**


 Struct to hold application state

Signature:

```cpp
struct App 
```

**Members:**

| Type | Name | Description |
| --- | --- | --- | 
| `EngineConfig` | `config` | Engine configuration |
| `std::unique_ptr<Window>` | `window` | Pointer to the window |
| `std::unique_ptr<Renderer>` | `renderer` | Pointer to the render system |
| `std::unique_ptr<SynModelLoader>` | `synModels` | Pointer to the model loader |
| `std::unique_ptr<Phys>` | `physicsManager` | Pointer to the physics manager |
| `bool` | `debug` | Debug mode flag |

**This function has been available since:** v0.0.1

---

#### **`Core::Core`**


 Core class to manage the application

Signature:

```cpp
 public: Core(const EngineConfig config);
```

**This function has been available since:** v0.0.1

---

#### **`Core::Initialize`**


 Initialize the core system. Creates a window, renderer, and other subsystems.

Signature:

```cpp
 static bool Initialize();
```

**Returns:** True on success, false on failure

**This function has been available since:** v0.0.1

---

#### **`Core::Get`**


 Get the global Core instance

Signature:

```cpp
 static Core* Get();
```

**Returns:** Pointer to the global Core instance

**This function has been available since:** v0.0.1

---

#### **`Core::_GetApp`**


 Get the global App instance

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static App* _GetApp();
```

**Returns:** Pointer to the global App instance

**This function has been available since:** v0.0.1

---

#### **`Core::_GetConfig`**


 Get the global game config

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static EngineConfig* _GetConfig();
```

**Returns:** Pointer to the global game config

**This function has been available since:** v0.0.1

---

#### **`Core::IsRunning`**


 Check if the application is running

**Note:** Used to determine if the main loop should continue

Signature:

```cpp
 bool IsRunning();
```

**Returns:** True if the application is running, false otherwise

**This function has been available since:** v0.0.1

---

#### **`Core::HandleEvents`**


 Handle events for the application

Signature:

```cpp
 bool HandleEvents();
```

**Returns:** True if events were handled, false otherwise

**This function has been available since:** v0.0.1

---

#### **`Core::Update`**


 Update the application state

Signature:

```cpp
 bool Update();
```

**Returns:** True if the update was successful, false otherwise

**This function has been available since:** v0.0.1

---

#### **`Core::Render`**


 Render the application

Signature:

```cpp
 bool Render(CameraComponent* camera);
```

**Returns:** True if the render was successful, false otherwise

**This function has been available since:** v0.0.1

---

#### **`Core::GetPhysicsManager`**


 Get the physics manager.

Signature:

```cpp
 Syngine::Phys* GetPhysicsManager();
```

**Returns:** Pointer to the physics manager, or nullptr if not initialized

---

#### **`Core::GetSystemSpecifications`**


 Get system specifications

**Preconditions:** Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)

Signature:

```cpp
 Syngine::HardwareSpecs GetSystemSpecifications();
```

**Returns:** Hardware specifications of the system

---

#### **`Core::SetSimulationState`**


 Set the simulation state

Signature:

```cpp
 void SetSimulationState(bool simulate);
```

**Parameters:**

- `simulate`: True to enable simulation, false to disable

---

#### **`Core::GetSimulationState`**


 Get the simulation state

Signature:

```cpp
 bool GetSimulationState();
```

**Returns:** True if simulation is enabled, false otherwise

---

