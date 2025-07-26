#pragma once
#include <string>
#include <vector>
#include <map>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>
#include "SyngineGameobject.h"
#include "bx/math.h"

namespace Syngine {
class CameraComponent; // Forward declaration

// Collection of view IDs for rendering.
// Rendered in the order they are defined here.
enum ViewID : bgfx::ViewId {
    VIEW_SHADOW    = 0,
    VIEW_SKY       = 1,
    VIEW_GBUFFER   = 2,
    VIEW_LIGHTING  = 3,
    VIEW_FOWARD    = 4,
    VIEW_DEBUG     = 5,
    VIEW_BILLBOARD = 6,
    VIEW_UI        = 7,
    VIEW_UI_DEBUG  = 8,
};

// Program structure to hold shader program information
struct Program {
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    unsigned short      viewId  = -1;
    std::string         name    = "empty";
    std::string         vsPath  = "";
    std::string         fsPath  = "";
    int                 id      = 0;
};

// Handles structure to manage shader programs and uniforms
struct Handles {
    std::vector<Program> programs;
    std::map<std::string, bgfx::UniformHandle> uniforms;
    bgfx::VertexBufferHandle dummy = BGFX_INVALID_HANDLE;
};

// Gizmo structure to hold information about gizmos
struct Gizmo {
    bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
    float size = 1.0f;
};

class Graphics {
  public:
    int width;
    int height;
    SDL_Window* win;
    Handles handles;

    Graphics(const char* title, int width, int height);

    // Create a window for rendering, returns 0 on success, non-zero on failure
    int CreateGameWindow();
    // Create a renderer, returns 0 on success, non-zero on failure
    int CreateRenderer();

    int DestroyRenderer();
    // Destroy the window, cleans up resources. Returns nothing.
    void DestroyWindow();

    // Load a shader from a file path, returns index of the shader or -1 on failure.
    // Optional param `viewId` can be used to specify the view ID for the shader (e.g. VIEW_FOWARD, VIEW_DEBUG, etc.)
    int AddProgram(const char*     vsPath,
                   const char*     fsPath,
                   const char*     name,
                   Syngine::ViewID viewId = Syngine::VIEW_FOWARD);
    // Load a shader from a file path, returns index of the shader or -1 on failure. Assumes both vs and fs have same name.
    // Optional param `viewId` can be used to specify the view ID for the shader (e.g. VIEW_FOWARD, VIEW_DEBUG, etc.)
    int AddProgram(const char*     path,
                   const char*     name,
                   Syngine::ViewID viewId = Syngine::VIEW_FOWARD);

    // Get program by name, returns Program with invalid handle if not found
    Program GetProgram(const char* name) const;
    // Get program by index, returns Program with invalid handle if index is out of bounds
    Program GetProgram(int index) const;

    // Remove all shader programs, returns true if successful
    bool RemoveAllPrograms();
    // Removes a program by index, returns true if successful
    bool     RemoveProgram(int index);
    // Removes a program by name, returns true if successful
    bool     RemoveProgram(const char* name);

    // Reloads a program by name, returns true if successful
    bool ReloadProgram(const char* name);
    // Reloads all programs, returns true if successful
    bool ReloadAllPrograms();

    // Get a uniform handle by name, returns BGFX_INVALID_HANDLE if not found
    bgfx::UniformHandle GetUniform(const char* name) const;

    // Render frame
    int RenderFrame(std::vector<GameObject*> modelLoader,
                    bx::Vec3&                lightDir,
                    CameraComponent*         camera,
                    bool                     debug = false);

    // Register a gizmo with a tag and optional size, loads texture from "default/gizmos/{tag}.png".
    void RegisterGizmo(const std::string& tag, float size = 1.0f);
    // Render gizmos for the given game objects, using the camera component for positioning.
    void RenderGizmos(std::vector<GameObject*> gameObjects, CameraComponent* camera);

  private:
    std::string title;

    std::map<std::string, Gizmo> gizmoRegistry;
    bgfx::VertexBufferHandle     billboardVbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle      billboardIbh = BGFX_INVALID_HANDLE;
};

} // namespace Syngine