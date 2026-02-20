// ╒═════════════════════════════ Core.h ═╕
// │ Syngine                              │
// │ Created 2025-04-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

#include "Syngine/Core/Registry.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/Components/CameraComponent.h"
#include "Syngine/ECS/Components/PlayerComponent.h"
#include "Syngine/ECS/Components/RigidbodyComponent.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/Physics/Physics.h"
#include "Syngine/Utils/Profiler.h"

#include <cstdint>
#include <memory>

namespace Syngine {
// Forward declare
class ZoneManager;
class Window;

/// @brief Struct to hold hardware specifications
/// @section Core
/// @note This should only be called after window creation, as it relies on SDL
/// for some information.
/// @since v0.0.1
struct HardwareSpecs {
    std::string osName;          //* Operating system name
    std::string cpuModel;        //* CPU model name
    std::string cpuArch;         //* CPU architecture (e.g., x86_64, ARM)
    int         cpuCores;        //* Number of CPU cores
    int         ramMB;           //* Amount of RAM in MB
    int         screenWidth;     //* Width of the screen in pixels
    int         screenHeight;    //* Height of the screen in pixels
    int         winWidth;        //* Width of the game window in pixels
    int         winHeight;       //* Height of the game window in pixels
    int         gpuVendorID;     //* GPU vendor ID
    int         gpuDeviceID;     //* GPU device ID
    int         maxTextureSize;  //* Maximum texture size supported by the GPU
    bool        supportsCompute; //* Whether the GPU supports compute shaders
    bool        supports3DTextures; //* Whether the GPU supports 3D textures
};

/// @brief Struct to hold engine config
/// @section Core
/// @since v0.0.1
struct EngineConfig {
    std::string windowTitle = "SyngineGame";  //* Title of the game window
    int         windowWidth = 800;  //* Width of the game window in pixels
    int         windowHeight = 600; //* Height of the game window in pixels
    bool        usePhysics = true; //* Whether to initialize the physics system
};

/// @brief the various debug modes possible
/// @section Core
/// @since v0.0.1
struct DebugModes {
    bool Enabled = true; //* Global debug toggle
    bool PhysWireframes = false; //* Whether to draw physics wireframes
    bool Gizmos = true; //* Gizmos such as cameras, lights, and audio sources
    bool CSMBounds = false; //* Cascading Shadow Map zone bounds.
    bool DrawBoundingBoxes = false; //* Whether to draw mesh bounding boxes
};

/// @brief Core class to manage the application
/// @section Core
/// @since v0.0.1
class Core {
  public:
    /// @brief Constructor for the Core class
    /// @param config Engine configuration (Syngine::EngineConfig)
    /// @throws std::runtime_error if initialization fails (e.g., SDL_Init()
    /// fails or missing files)
    /// @since v0.0.1
    Core(const EngineConfig config);
    ~Core();

    static float deltaTime;

    /// @brief Initialize the core system. Creates a window, renderer, and other
    /// subsystems.
    /// @param rendererConfig Renderer configuration options (Syngine::RendererConfig)
    /// @return True on success, false on failure
    /// @since v0.0.1
    static bool Initialize(const RendererConfig rendererConfig);

    /// @brief Get the global Core instance
    /// @return Pointer to the global Core instance
    /// @since v0.0.1
    static Core* Get();

    /// @brief Check if the application is running
    /// @return True if the application is running, false otherwise
    /// @note Used to determine if the main loop should continue
    /// @since v0.0.1
    bool IsRunning();

    /// @brief Handle events for the application
    /// @return True if events were handled, false otherwise
    /// @since v0.0.1
    bool HandleEvents();

    /// @brief Update the application state
    /// @return True if the update was successful, false otherwise
    /// @since v0.0.1
    bool Update();

    /// @brief Render the application
    /// @return True if the render was successful, false otherwise
    /// @since v0.0.1
    bool Render(CameraComponent* camera);

    /// @brief Get the physics manager.
    /// @return Pointer to the physics manager, or nullptr if not initialized
    Syngine::Phys* GetPhysicsManager();

    /// @brief Get system specifications
    /// @return Hardware specifications of the system
    /// @pre Renderer must be initialized (Core::Initialize() called or
    /// Renderer::IsReady() == true)
    /// @since v0.0.1
    Syngine::HardwareSpecs GetSystemSpecifications();

    /// @brief Set the simulation state
    /// @param simulate True to enable simulation, false to disable
    /// @since v0.0.1
    void SetSimulationState(bool simulate);

    /// @brief Get the simulation state
    /// @return True if simulation is enabled, false otherwise
    /// @since v0.0.1
    bool GetSimulationState();

    /// @brief Check if the Core is initialized
    /// @return True if initialized, false otherwise
    /// @since v0.0.1
    static bool IsInitialized() noexcept { return m_instance != nullptr; };

    /// @brief Check if physics is enabled in the engine config
    /// @return True if physics is enabled, false otherwise
    /// @since v0.0.1
    static bool IsPhysicsEnabled() {
        return m_instance && m_instance->m_app &&
            m_instance->m_app->config.usePhysics;
    }

    /// @brief Set the current debug modes
    /// @param mode DebugModes struct with desired debug settings
    /// @return True on success, false otherwise
    /// @since v0.0.1
    static bool SetDebugMode(DebugModes mode) {
        if (m_instance && m_instance->m_app) {
            m_instance->m_app->debug = mode;
            return true;
        }
        return false;
    }

    /// @brief Get the current debug modes
    /// @return Current DebugModes struct
    /// @since v0.0.1
    static DebugModes GetDebugMode() {
        if (m_instance && m_instance->m_app) {
            return m_instance->m_app->debug;
        }
        return DebugModes();
    }

  private:
    struct _internal {
        // Mouse sensitivity
        static constexpr float DEFAULT_SENSITIVITY = 0.002f;
        // Highest camera can go up
        static constexpr float DEFAULT_MAX_PITCH = 3.14f / 2 - 0.01f;
        // Sprint speed multiplier
        static constexpr float DEFAULT_SPRINT_MULT = 2.0f;
        // Crouch speed multiplier
        static constexpr float DEFAULT_CROUCH_MULT = 1.0f;
        // Physics update timestep
        static constexpr float DEFAULT_PHYSICS_TIMESTEP = 1.0f / 60.0f;
        // Iterations per physics update
        static constexpr int DEFAULT_PHYSICS_STEPS = 1;

        float accumulator = 0.0f; // Accumulator for physics updates

        uint64_t now  = 0;
        uint64_t last = 0;

        bool simulate   = false; // Toggles the physics simulation
        bool mouseState = false;
    };

    /// @brief Struct to hold application state
    /// @section Core
    /// @since v0.0.1
    struct App {
        EngineConfig                    config;         //* Engine configuration
        std::unique_ptr<Window>         window;         //* Pointer to the window
        std::unique_ptr<Renderer>       renderer;       //* Pointer to the render system
        std::unique_ptr<SynModelLoader> synModels;      //* Pointer to the model loader
        std::unique_ptr<Phys>           physicsManager; //* Pointer to the physics manager
        std::unique_ptr<ZoneManager>    zoneManager;    //* Pointer to the zone manager
        DebugModes                      debug;          //* Debug modes flags
    };

    /// @brief Get the global App instance
    /// @return Pointer to the global App instance
    /// @since v0.0.1
    /// @internal
    static App* _GetApp();

    /// @brief Get the global game config
    /// @return Pointer to the global game config
    /// @since v0.0.1
    /// @internal
    static EngineConfig* _GetConfig();

    struct FrameCounts {
        struct DrawnObjectCount {
            uint32_t shadows = 0;
            uint32_t sky    = 0;
            uint32_t forward = 0;
            uint32_t debug   = 0;
            uint32_t billboard = 0;
            uint32_t ui        = 0;
            uint32_t culledFrustum = 0;
            uint32_t culledSize    = 0;
        } drawnObjects;
        uint32_t updates = 0;
    };

    static FrameCounts m_frameCounts;

    /// @brief Frame counter for FPS and TPS tracking
    /// @internal
    /// @since v0.0.1
    struct _FrameCounter {
        float oneSecond    = 0.0f;
        int   frameCount   = 0;
        int   frameDisplay = 0;
        int   physCounter  = 0;
        int   lastFPS      = 0;
        int   lastTPS      = 0;

        void Update(float deltaTime, bool simulate, size_t gameObjectCount) {
            oneSecond += deltaTime;

            if (oneSecond >= 1.0f) {
                lastFPS      = frameDisplay;
                lastTPS      = physCounter;
                frameDisplay = 0;
                oneSecond    = 0.0f;
                physCounter  = 0;

                SDL_Log("Frame: %d, GameObjects: %zu, Sim: %s, FPS/TPS: %d/%d, "
                        "Updates: %d, Drawn Objects - Shadows: %d, Forward: "
                        "%d, Billboards: %d - Culled (Frustum/Size): %d/%d",
                        frameCount,
                        gameObjectCount,
                        simulate ? "ON" : "OFF",
                        lastFPS,
                        lastTPS,
                        m_frameCounts.updates,
                        m_frameCounts.drawnObjects.shadows,
                        m_frameCounts.drawnObjects.forward,
                        m_frameCounts.drawnObjects.billboard,
                        m_frameCounts.drawnObjects.culledFrustum,
                        m_frameCounts.drawnObjects.culledSize);

                m_frameCounts.updates = 0;
                m_frameCounts.drawnObjects = FrameCounts::DrawnObjectCount();
            }
        }
    };

    // Debug keybind actions
    static void _ReloadChangedAssets();
    static void _ReloadShaders();

    static Core*         m_instance;     //* Pointer to the global Core instance
    static App*          m_app;          //* Pointer to the global App instance
    static bool          m_shouldClose;  //* Whether the application should close
    static _internal     m_internal;     //* Internal state struct
    static _FrameCounter m_frameCounter; //* Frame counter for FPS/TPS tracking

    /// @deprecated In favor of the new input system
    void _HandleKeyEvent(const SDL_Event& event);

    friend class Renderer;
    friend class RenderCore;
    friend class GameObject;
    friend class RigidbodyComponent;
    friend class PlayerComponent;
    friend class Registry;
#ifndef SYN_DEBUG_GRAPHICS
    friend class Syngine::Profiler;
#endif
};

} // namespace Syngine
