// ╒═════════════════════════ Renderer.h ═╕
// │ Syngine                              │
// │ Created 2025-05-06                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include <string>
#include <vector>
#include <map>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>
#include "bx/math.h"

namespace Syngine {
class CameraComponent; // Forward declaration

/// @brief Collection of view IDs for rendering. Rendered in the order they are
/// defined here.
/// @section Renderer
/// @since v0.0.1
enum ViewID : bgfx::ViewId {
    VIEW_SHADOW   = 0, //* Shadow map rendering
    VIEW_SKY      = 1, //* Skybox rendering
    VIEW_GBUFFER  = 2, //* G-Buffer rendering for deferred shading
    VIEW_LIGHTING = 3, //* Lighting pass for deferred shading
    VIEW_FORWARD  = 4, //* Forward rendering pass for translucent objects
    VIEW_DEBUG    = 5, //* Debug rendering pass for debug rendering
    VIEW_BILLBOARD = 6, //* Billboard rendering
    VIEW_UI        = 7, //* UI rendering
    VIEW_UI_DEBUG  = 8, //* UI debug rendering
};

/// @brief Program structure to hold shader program information
/// @section Renderer
struct Program {
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE; //* Shader program handle
    unsigned short      viewId  = -1; //* View ID for rendering
    std::string         name    = "empty"; //* Name of the shader program
    std::string         vsPath  = ""; //* Vertex shader path
    std::string         fsPath  = ""; //* Fragment shader path
    int                 id      = 0; //* Unique ID of the shader program
};

/// @brief Handles structure to manage shader programs and uniforms
/// @section Renderer
/// @since v0.0.1
struct Handles {
    std::vector<Program> programs; //* List of shader programs
    std::map<std::string, bgfx::UniformHandle> uniforms; //* Map of uniform names to handles
    bgfx::VertexBufferHandle dummy = BGFX_INVALID_HANDLE; //* Dummy vertex buffer handle (needed on some platforms)
};

/// @brief Gizmo structure to hold information about gizmos
/// @section Renderer
/// @since v0.0.1
struct Gizmo {
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE; //* Texture handle for the gizmo
    float size = 1.0f; //* Size of the gizmo. 1.0f is the default size, roughly 1 unit in world space
};

/// @brief Renderer class to manage rendering and shader programs
/// @section Renderer
/// @since v0.0.1
class Renderer {
  public:
    static int width; //* Width of the game window in pixels
    static int height; //* Height of the game window in pixels

    Renderer(int width, int height);
    ~Renderer();

    /// @brief Load a shader from vertex and fragment shader file paths and
    /// create a shader program
    /// @param vsPath Path to the vertex shader file
    /// @param fsPath Path to the fragment shader file
    /// @param name Name of the shader program
    /// @param viewId View ID for the shader program, defaults to VIEW_FORWARD
    /// @return The index of the newly created program on success, -1 on failure
    /// @threadsafety not-safe
    /// @pre Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)
    /// @since v0.0.1
    static size_t AddProgram(const std::string& vsPath,
                          const std::string& fsPath,
                          const std::string& name,
                          Syngine::ViewID viewId = Syngine::VIEW_FORWARD);

    /// @brief Load a shader from a single file path and create a shader program
    /// @param path Path to the shader file
    /// @param name Name of the shader program
    /// @param viewId View ID for the shader program, defaults to VIEW_FORWARD
    /// @return The index of the newly created program on success, -1 on failure
    /// @note Assumes the vertex shader is named "{name}.vert.bin" and fragment
    /// shader is named "{name}.frag.bin"
    /// @pre Renderer must be initialized (Core::Initialize() called or Renderer::IsReady() == true)
    /// @threadsafety not-safe
    /// @since v0.0.1
    static size_t AddProgram(const std::string& path,
                          const std::string& name,
                          Syngine::ViewID viewId = Syngine::VIEW_FORWARD);

    /// @brief Get a shader program by name. Returns first matching program
    /// @param name Name of the shader program
    /// @return Program with the shader program handle, or an invalid handle if
    /// not found
    /// @threadsafety read-only
    /// @since v0.0.1
    static Program GetProgram(const std::string_view& name);

    /// @brief Get a shader program by index
    /// @param index Index of the shader program in the list
    /// @return Program with the shader program handle, or an invalid handle if
    /// index is out of bounds
    /// @threadsafety read-only
    /// @since v0.0.1
    static Program GetProgram(int index);

    /// @brief Remove all shader programs
    /// @return True if all programs were removed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool RemoveAllPrograms();

    /// @brief Remove a shader program by index
    /// @param index Index of the shader program to remove
    /// @return True if the program was removed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool RemoveProgram(int index);

    /// @brief Remove a shader program by name
    /// @param name Name of the shader program to remove
    /// @return True if the program was removed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool RemoveProgram(const std::string_view& name);

    /// @brief Reload a shader program by name
    /// @param name Name of the shader program to reload
    /// @return True if the program was reloaded successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    static bool ReloadProgram(const std::string_view& name);

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

    /// @brief Get a uniform handle by name
    /// @param name Name of the uniform
    /// @return bgfx::UniformHandle The handle to the uniform, or
    /// BGFX_INVALID_HANDLE if not found
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    bgfx::UniformHandle _GetUniform(const std::string_view& name) const;

    /// @brief Render a frame
    /// @param lightDir Direction of the light for lighting calculations
    /// @param camera Pointer to the camera component for rendering
    /// @param debug Whether to render debug information
    /// @return 0 on success, non-zero on failure
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    int _RenderFrame(bx::Vec3&                lightDir,
                    CameraComponent*         camera,
                    bool                     debug = false);

    /// @brief Register a gizmo with a tag and optional size
    /// @param tag Name of the gizmo
    /// @param size Size of the gizmo
    /// @note Loads texture from "default/gizmos/{tag}.png"
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _RegisterGizmo(const std::string& tag, float size = 1.0f);

    /// @brief Render all nearby gizmos
    /// @param camera Pointer to the camera component for rendering
    /// @note This will render all registered gizmos that are within 1000 units
    /// of the camera's position.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _RenderGizmos(CameraComponent* camera);

  private:
    static std::string m_title;
    static bool        m_isReady;

    static std::map<std::string, Gizmo> m_gizmoRegistry;
    static bgfx::VertexBufferHandle     m_billboardVbh;
    static bgfx::IndexBufferHandle      m_billboardIbh;

    static SDL_Window* win; //* SDL window handle
    static Handles handles; //* Shader program and uniform handles

    /// @brief Initialize the graphics system
    /// @return true on success, false on failure
    /// @note This should be called after the game window is created.
    /// @throws std::runtime_error if initialization fails (e.g., bgfx::init()
    /// fails or missing files)
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static bool _CreateRenderer();
};

} // namespace Syngine
