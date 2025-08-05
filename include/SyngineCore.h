// ╒══════════════════════ SyngineCore.h ═╕
// │ Syngine                              │
// │ Created 2025-04-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "SyngineGraphics.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"

namespace Syngine {

/// @brief Struct to hold hardware specifications
/// @section Core
/// @note This should only be called after window creation, as it relies on SDL for some information.
/// @since v0.0.1
struct HardwareSpecs {
    std::string osName; //* Operating system name
    std::string cpuModel; //* CPU model name
    std::string cpuArch; //* CPU architecture (e.g., x86_64, ARM)
    int         cpuCores; //* Number of CPU cores
    int         ramMB; //* Amount of RAM in MB
    int         screenWidth; //* Width of the screen in pixels
    int         screenHeight; //* Height of the screen in pixels
    int         winWidth; //* Width of the game window in pixels
    int         winHeight; //* Height of the game window in pixels
    int         gpuVendorID; //* GPU vendor ID
    int         gpuDeviceID; //* GPU device ID
    int         maxTextureSize; //* Maximum texture size supported by the GPU
    bool        supportsCompute; //* Whether the GPU supports compute shaders
    bool        supports3DTextures; //* Whether the GPU supports 3D textures
};

/// @brief Struct to hold application state
/// @section Core
/// @since v0.0.1
struct App {
    Graphics* graphics; //* Pointer to the graphics system
    SynModelLoader* synModels; //* Pointer to the model loader
    Phys*           physicsManager; //* Pointer to the physics manager
    bool            debug = true;   //* Debug mode flag
    std::string     appName;      //* Name of the application
};

/// @brief Core class to manage the application
/// @section Core
/// @since v0.0.1
class Core {
  public:
    /// @brief Constructor for the Core class
    /// @param appName Name of the application
    /// @since v0.0.1
    Core(const std::string& appName = "SyngineGame");

    /// Destructor for the Core class
    ~Core();

    /// @brief Get the global Core instance
    /// @return Pointer to the global Core instance
    /// @since v0.0.1
    /// @internal
    static Core* _Get();

    /// @brief Get the global App instance
    /// @return Pointer to the global App instance
    /// @since v0.0.1
    static App* _GetApp();

    /// @brief Attach the graphics system to the core.
    /// @param graphics Pointer to the graphics system
    /// @return 0 on success, non-zero on failure
    int AttachGraphics(Graphics* graphics);

    /// @brief Detach the graphics system from the core.
    /// @return 0 on success, non-zero on failure.
    int DetachGraphics();

    /// @brief Main event loop, blocks until the application is closed.
    /// @return 0 on clean exit, non-zero on error.
    int SyngineEventLoop();

    /// @brief Add a new GameObject to the scene
    /// @param gameobject Pointer to the GameObject to add
    /// @return 0 on success, non-zero on failure
    int CreateGameobject(GameObject* gameobject);

    /// @brief Delete a GameObject from the scene and free its resources
    /// @param gameobject Pointer to the GameObject to delete
    /// @return 0 on success, non-zero on failure
    int DeleteGameobject(GameObject* gameobject);

    /// @brief Get the physics manager.
    /// @return Pointer to the physics manager, or nullptr if not initialized
    Syngine::Phys * GetPhysicsManager();

    /// @brief Get system specifications
    /// @return Hardware specifications of the system
    Syngine::HardwareSpecs GetSystemSpecifications();

  private:
    static Core* s_instance;
    App* app;
};

} // namespace Syngine