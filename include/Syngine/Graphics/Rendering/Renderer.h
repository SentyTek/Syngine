// ╒═════════════════════════ Renderer.h ═╕
// │ Syngine                              │
// │ Created 2025-05-06                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once

#include <Syngine/Math/Math.hpp>
#include <Syngine/Graphics/Rendering/RenderPacket.h>
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Graphics/Resources/MaterialManager.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>

namespace Syngine {
class DebugRender;        // Forward declaration
class CameraComponent;    // Forward declaration
struct DebugModes;        // Forward declaration
class BillboardComponent; // Forward declaration

/// @brief Collection of view IDs for rendering. Rendered in the order they are
/// defined here.
/// @section Renderer
/// @since v0.0.1
enum ViewID : bgfx::ViewId {
    VIEW_SHADOW      = 0,  //* Shadow map rendering
    VIEW_SKY         = 4,  //* Skybox rendering
    VIEW_GBUFFER     = 5,  //* G-Buffer rendering for deferred shading
    VIEW_LIGHTING    = 6,  //* Lighting pass for deferred shading
    VIEW_FORWARD     = 7,  //* Forward rendering pass for translucent objects
    VIEW_BILLBOARD   = 8,  //* Billboard rendering
    VIEW_DEBUG       = 9,  //* Debug rendering pass for debug rendering
    VIEW_BILL_DBG    = 10, //* Billboard debug rendering
    VIEW_AO          = 11, //* Ambient occlusion passes (3 passes)
    VIEW_POSTPROCESS = 14, //* Post-processing effects passes (Max 8 passes)
    VIEW_UI          = 18, //* UI rendering
    VIEW_UI_DEBUG    = 19, //* UI debug rendering
};

/// @brief To manage renderer configuration
struct RendererConfig {
    bool loadFromFile =
        true; //* Whether to load settings from file on initialization. This
              // will ignore all other settings in this struct if true. But if
              // file is not found, it will use the other settings and create a
              // new file with them. Specially for width and height, which are
              // set by EngineConfig instead.
    bool useShadows = true; //* Whether to use shadow mapping
    int  shadowDist = 500;  //* Distance for shadow rendering
    bool vsync      = true; //* Whether to enable vertical sync
    bool usePseudoCamera =
        false; //* (only if DebugModes.Enabled == true) Pseudo camera is a
               // separate camera that all rendering will use, but the main
               // camera will still be the one drawn to the screen
    bool useSSAO      = true; //* Whether to enable SSAO
    int shadowMapSize = 2048; //* Size of the shadow map texture (default: 2048)
};

/// @brief Renderer class to manage rendering and shader programs
/// @section Renderer
/// @since v0.0.1
class Renderer {
  public:
    using RenderPacket = RenderPacket;

    static int width;  //* Width of the game window in pixels
    static int height; //* Height of the game window in pixels
    static uint64_t
        currentDrawId; //* Current frame number for tracking uniform updates

    /// @brief Constructor for the Renderer class
    /// @param width Width of the game window in pixels
    /// @param height Height of the game window in pixels
    /// @param config Renderer configuration options
    /// @throws std::runtime_error if initialization fails (e.g., bgfx::init()
    /// fails or missing files)
    /// @threadsafety not-safe
    /// @since v0.0.1
    Renderer(int width, int height, const RendererConfig& config);
    ~Renderer();

    /// @brief Check if the renderer is ready
    /// @return True if the renderer is ready, false otherwise
    /// @threadsafety read-only
    /// @since v0.0.1
    static bool IsReady() { return m_isReady; };

    /// @brief Set a uniform variable
    /// @param id The ID of the uniform variable to set, returned from
    /// RegisterUniform
    /// @param data Pointer to the data to set the uniform to
    /// @param num The number of elements to set (default is 1)
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetUniform(size_t id, const void* data, uint16_t num = 1);

    /// @brief Get the global sun light direction
    /// @return A vector3 representing the normalized sun light direction in
    /// world space
    /// @threadsafety not-safe
    static Math::Vector3 GetSunDirection();

    /// @brief Set the global sun light direction
    /// @param lightDir A Vector3 representing the normalized sun light
    /// direction in world space
    /// @note The direction should be normalized and in world space coordinates.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetSunDirection(const Math::Vector3& lightDir);

    /// @brief Set the default gizmo size
    /// @param size Size of the gizmo
    /// @since v0.0.1
    static void SetGizmoSize(float size) { m_gizmoSize = size; }

    /// @brief Get the default gizmo size
    /// @return Size of the gizmo
    /// @since v0.0.1
    static float GetGizmoSize() { return m_gizmoSize; }

    /// @brief Register a gizmo with a tag and optional size
    /// @param tag Name of the gizmo
    /// @param size Size of the gizmo
    /// @note Loads texture from "default/gizmos/{tag}.png"
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _RegisterGizmo(const std::string& tag);

    /// @brief Set which CameraComponent to use as the pseudo camera for
    /// rendering
    /// @param camera Pointer to the CameraComponent to use as the pseudo camera
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetPseudoCamera(CameraComponent* camera);

    /// @brief Get the active pseudo camera used for rendering.
    /// @return Pointer to the active CameraComponent used for rendering
    /// @threadsafety read-only
    /// @since v0.0.2
    static CameraComponent* GetPseudoCamera() { return m_pseudoCamera; }

    /// @brief Set which CameraComponent to use as the active camera for
    /// rendering
    /// @param camera Pointer to the CameraComponent to use as the active camera
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetActiveCamera(CameraComponent* camera) { m_camera = camera; }

    /// @brief Get the active camera used for rendering.
    /// @return Pointer to the active CameraComponent used for rendering
    /// @threadsafety read-only
    /// @since v0.0.2
    static CameraComponent* GetActiveCamera() { return m_camera; }

  private:
    struct UniformCacheEntry {
        uint16_t             idx;
        std::vector<uint8_t> lastData;
        uint64_t             drawId = 0;
    };

    static std::string m_title; //* Title of the game window
    static bool m_isReady; //* Whether the renderer is initialized and ready
    static std::vector<UniformCacheEntry>
        m_uniformCache; //* Cache of uniform data for the current frame

    static std::unordered_map<std::string, Syngine::BillboardComponent*>
                 m_gizmoRegistry; //* Registry of gizmos
    static float m_gizmoSize;     //* Default size for gizmos

    // Active camera is the camera that's actually used for rendering. Pseudo
    // camera is a separate camera that things like the shadow map debug boxes
    // will use when enabled in debug mode, and is not used for perspective when
    // rendering the scene.
    static CameraComponent* m_camera;       //* Active camera used for rendering
    static CameraComponent* m_pseudoCamera; //* Pseudo camera for rendering when
                                            // enabled in debug mode

    /// @brief Initialize the graphics system
    /// @return true on success, false on failure
    /// @note This should be called after the game window is created.
    /// @throws std::runtime_error if initialization fails (e.g., bgfx::init()
    /// fails or missing files)
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static bool _CreateRenderer(const RendererConfig& config);

    static DebugRender* m_drender; //* Debug renderer instance

    static bool
        m_isFirstFrame; //* Whether this is the first frame being rendered
    static Math::Vector3 m_sunDir; //* Direction of the sun light in world space

    /// @brief Render all nearby gizmos
    /// @param camera Pointer to the camera component for rendering
    /// @note This will render all registered gizmos that are within 1000 units
    /// of the camera's position.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _RenderGizmos(CameraComponent* camera);

    // Wrapper to call RenderCore's _RenderFrame
    static void _RenderFrame(DebugModes debug);
    static void _UpdateDrawID();
    static void _RegisterBuiltinUniformProviders();

    friend class Core;
    friend class RenderCore;
};

} // namespace Syngine
