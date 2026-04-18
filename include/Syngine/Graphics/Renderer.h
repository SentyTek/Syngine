// ╒═════════════════════════ Renderer.h ═╕
// │ Syngine                              │
// │ Created 2025-05-06                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>

namespace Syngine {
class DebugRender; // Forward declaration
class CameraComponent; // Forward declaration
struct DebugModes;     // Forward declaration
class BillboardComponent; // Forward declaration

/// @brief Collection of view IDs for rendering. Rendered in the order they are
/// defined here.
/// @section Renderer
/// @since v0.0.1
enum ViewID : bgfx::ViewId {
    VIEW_SHADOW    = 0, //* Shadow map rendering
    VIEW_SKY       = 4, //* Skybox rendering
    VIEW_GBUFFER   = 5, //* G-Buffer rendering for deferred shading
    VIEW_LIGHTING  = 6, //* Lighting pass for deferred shading
    VIEW_FORWARD   = 7, //* Forward rendering pass for translucent objects
    VIEW_BILLBOARD = 8, //* Billboard rendering
    VIEW_AO        = 9, //* Ambient occlusion passes (3 passes)
    VIEW_POSTPROCESS = 12, //* Post-processing effects passes (Max 8 passes)
    VIEW_DEBUG       = 16,  //* Debug rendering pass for debug rendering
    VIEW_BILL_DBG    = 17, //* Billboard debug rendering
    VIEW_UI          = 18, //* UI rendering
    VIEW_UI_DEBUG    = 19, //* UI debug rendering
};

/// @brief Different types of shader uniforms
/// @section Renderer
/// @since v0.0.1
enum UniformType {
    UNIFORM_SAMPLER = bgfx::UniformType::Sampler, //* Texture sampler uniform
    UNIFORM_VEC4    = bgfx::UniformType::Vec4,    //* Vec4 uniform
    UNIFORM_MAT4    = bgfx::UniformType::Mat4,    //* Mat4 uniform
    UNIFORM_MAT3    = bgfx::UniformType::Mat3,    //* Mat3 uniform
    UNIFORM_UNKNOWN = bgfx::UniformType::Count,   //* Unknown uniform type
};

/// @brief Struct to hold shader uniform information
/// @section Renderer
/// @since v0.0.1
struct Uniform {
    bgfx::UniformHandle handle = BGFX_INVALID_HANDLE; //* Handle for the uniform
    UniformType type = UniformType::UNIFORM_UNKNOWN;  //* Type of the uniform
    void*       data = nullptr; //* Pointer to the uniform data
    std::string name;           //* Name of the uniform
    uint16_t    num = 1;       //* Number of elements in the uniform (for arrays)
};

/// @brief Program structure to hold shader program information
/// @section Renderer
struct Program {
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE; //* Shader program handle
    unsigned short      viewId  = UINT16_MAX; //* View ID for rendering
    std::string         name    = "empty"; //* Name of the shader program
    std::string         vsPath  = ""; //* Vertex shader path
    std::string         fsPath  = ""; //* Fragment shader path
    std::string         bundlePath = ""; //* Bundle path if loaded from a bundle
    int                 id      = 0;  //* Unique ID of the shader program
    std::vector<Uniform> uniforms;     //* List of shader uniforms
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
    int  shadowDist = 500; //* Distance for shadow rendering
    bool  vsync     = true;   //* Whether to enable vertical sync
    bool  usePseudoCamera =
        false; //* (only if DebugModes.Enabled == true) Pseudo camera is a
               //separate camera that all rendering will use, but the main
               //camera will still be the one drawn to the screen
    bool useSSAO = true; //* Whether to enable SSAO
};

/// @brief Renderer class to manage rendering and shader programs
/// @section Renderer
/// @since v0.0.1
class Renderer {
  public:

    static int width; //* Width of the game window in pixels
    static int height; //* Height of the game window in pixels

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

    /// @brief Load a shader from a single file path and create a shader program
    /// @param path Path to the shader file
    /// @param name Name of the shader program
    /// @param viewId View ID for the shader program, defaults to VIEW_FORWARD
    /// @return The index of the newly created program on success, -1 on failure
    /// @note Assumes the vertex shader is named "{name}.vert.bin" and fragment
    /// shader "{name}.frag.bin"
    /// @pre Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)
    /// @threadsafety not-safe
    /// @since v0.0.1
    static size_t AddProgram(const std::string& path,
                             const std::string& name,
                             Syngine::ViewID    viewId = Syngine::VIEW_FORWARD);

    /// @brief Load a shader from a bundle file and create a shader program
    /// @param bundlePath Path to the bundle file containing the shader
    /// @param path Path to the shader file within the bundle (without .vert.bin or .frag.bin suffix)
    /// @param name Name of the shader program
    /// @param viewId View ID for the shader program, defaults to VIEW_FORWARD
    /// @return The index of the newly created program on success, -1 on failure
    /// @note Assumes the vertex shader is named "{path}.vert.bin" and fragment
    /// shader "{path}.frag.bin" within the bundle
    /// @pre Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)
    /// @threadsafety not-safe
    /// @since v0.0.1
    static size_t AddProgram(const std::string& bundlePath,
                             const std::string& path,
                             const std::string& name,
                             Syngine::ViewID    viewId = Syngine::VIEW_FORWARD);

    /// @brief Get a shader program by name. Returns first matching program
    /// @param name Name of the shader program
    /// @return Program with the shader program handle, or an invalid handle if
    /// not found
    /// @threadsafety read-only
    /// @since v0.0.1
    static Program GetProgram(const std::string_view& name);

    /// @brief Get a shader program by ID.
    /// @param id ID of the shader program
    /// @return Program with the shader program handle, or an invalid handle if
    /// not found
    /// @threadsafety read-only
    /// @since v0.0.1
    static Program GetProgram(size_t id);

    /// @brief Remove all shader programs
    /// @return True if all programs were removed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool RemoveAllPrograms();

    /// @brief Remove a shader program by name
    /// @param viewId The view to remove the program from
    /// @param name Name of the shader program to remove
    /// @return True if the program was removed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool RemoveProgram(Syngine::ViewID         viewId,
                              const std::string_view& name);

    /// @brief Remove a shader program by id
    /// @param viewId The view to remove the program from
    /// @param id ID of the shader program to remove
    /// @return True if the program was removed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool RemoveProgram(Syngine::ViewID viewId, size_t id);

    /// @brief Reload a shader program by name
    /// @param name Name of the shader program to reload
    /// @return True if the program was reloaded successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool ReloadProgram(Syngine::ViewID         viewId,
                              const std::string_view& name);

    /// @brief Reload all shader programs
    /// @return True if all programs were reloaded successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool ReloadAllPrograms();

    /// @brief Check if the renderer is ready
    /// @return True if the renderer is ready, false otherwise
    /// @threadsafety read-only
    /// @since v0.0.1
    static bool IsReady();

    /// @brief Register a uniform variable
    /// @param program The ID of the shader program to register the uniform with
    /// @param name The name of the uniform variable
    /// @param type The type of the uniform variable
    /// @param num The number of elements for the uniform variable (default is 1)
    /// @pre The shader program must exist
    /// @return The ID of the registered uniform variable to use when
    /// setting, 0 if failure
    /// @threadsafety not-safe
    /// @since v0.0.1
    static size_t
    RegisterUniform(size_t program, const std::string& name, UniformType type, uint16_t num = 1);

    /// @brief Set a uniform variable
    /// @param id The ID of the uniform variable to set, returned from
    /// RegisterUniform
    /// @param data Pointer to the data to set the uniform to
    /// @param num The number of elements to set (default is 1)
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetUniform(size_t id, const void* data, uint16_t num = 1);

    /// @brief Get the global sun light direction
    /// @param outLightDir A 3 float array to store the light direction.
    ///        The direction is a normalized vector in world space. X is
    ///        east-west, Y is pitch, Z is north-south.
    /// @threadsafety not-safe
    static void GetSunDirection(float* outLightDir);

    /// @brief Set the global sun light direction
    /// @param lightDir A 3 float array representing the light direction.
    ///        Normalized direction vector in world space is expected.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetSunDirection(const float* lightDir);

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

    /// @brief Get a uniform by ID
    /// @param id ID of the uniform (Returned by RegisterUniform)
    /// @return Pointer to the Uniform struct, or nullptr if not found
    /// @threadsafety read-only
    /// @since v0.0.1
    static Uniform* GetUniform(size_t id);

    /// @brief Set which CameraComponent to use as the pseudo camera for
    /// rendering
    /// @param camera Pointer to the CameraComponent to use as the pseudo camera
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetPseudoCamera(CameraComponent* camera);
  private:

    static std::string m_title; //* Title of the game window
    static bool m_isReady; //* Whether the renderer is initialized and ready
    static std::unordered_map<uint16_t, Uniform> m_uniformRegistry; //* Registry of shader uniforms

    static std::unordered_map<std::string, Syngine::BillboardComponent*> m_gizmoRegistry; //* Registry of gizmos
    static float m_gizmoSize;      //* Default size for gizmos
    static std::unordered_map<bgfx::ViewId, std::vector<Program>> viewPrograms; //* Shader programs organized by view ID

    static CameraComponent* m_pseudoCamera; //* Pseudo camera for rendering when enabled in debug mode

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

    static bool m_isFirstFrame; //* Whether this is the first frame being rendered

    /// @brief Render all nearby gizmos
    /// @param camera Pointer to the camera component for rendering
    /// @note This will render all registered gizmos that are within 1000 units
    /// of the camera's position.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _RenderGizmos(CameraComponent* camera);

    /// @brief Internal helper for GetProgram.
    /// @param id ID of the shader program
    /// @return Pointer to the Program struct, or nullptr if not found
    /// @threadsafety read-only
    /// @internal
    static Program* _GetProgram(size_t id);

    // Wrapper to call RenderCore's _RenderFrame
    static void _RenderFrame(CameraComponent* camera, DebugModes debug);

    friend class Core;
    friend class RenderCore;
};

} // namespace Syngine
