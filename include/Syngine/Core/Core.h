// ╒═════════════════════════════ Core.h ═╕
// │ Syngine                              │
// │ Created 2025-04-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Physics/Physics.h"

namespace Syngine {
// Forward declare Window
class Window;

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

/// @brief Struct to hold engine config
/// @section Core
/// @since v0.0.1
struct EngineConfig {
    std::string windowTitle; //* Title of the game window
    int         windowWidth; //* Width of the game window in pixels
    int         windowHeight; //* Height of the game window in pixels
    bool        vsync; //* Whether vertical sync is enabled
};

/// @brief Struct to hold application state
/// @section Core
/// @since v0.0.1
struct App {
    EngineConfig config; //* Engine configuration
    Window* window; //* Pointer to the window
    Renderer* renderer; //* Pointer to the render system
    SynModelLoader* synModels; //* Pointer to the model loader
    Phys*           physicsManager; //* Pointer to the physics manager
    bool            debug = true;   //* Debug mode flag
};

/// @brief Core class to manage the application
/// @section Core
/// @since v0.0.1
class Core {
  public:
    Core(const EngineConfig config);
    ~Core();

    /// @brief Initialize the core system
    /// @return True on success, false on failure
    /// @since v0.0.1
    static bool Initialize();

    /// @brief Get the global Core instance
    /// @return Pointer to the global Core instance
    /// @since v0.0.1
    /// @internal
    static Core* _Get();

    /// @brief Get the global App instance
    /// @return Pointer to the global App instance
    /// @since v0.0.1
    static App* _GetApp();

    /// @brief Get the global game config
    /// @return Pointer to the global game config
    /// @since v0.0.1
    static EngineConfig* _GetConfig();

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
    /// @pre Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)
    Syngine::HardwareSpecs GetSystemSpecifications();

    /// @brief Get the 

  private:
    static Core* m_instance;
    static App* m_app;
};

} // namespace Syngine