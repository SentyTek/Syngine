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

struct Program {
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    std::string name = "empty";
    uint8_t viewId = 0;
};

struct Handles {
    std::vector<Program> programs;
    std::map<std::string, bgfx::UniformHandle> uniforms;
    bgfx::VertexBufferHandle dummy = BGFX_INVALID_HANDLE;
};

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
    
    int CreateWindow();
    int CreateRenderer();

    int DestroyRenderer();
    void DestroyWindow();

    size_t AddProgram(const char* vsPath, const char* fsPath, const char* name);
    Program GetProgram(const char* name) const;
    Program GetProgram(size_t index) const;
    int RemoveProgram(bool all);
    int RemoveProgram(size_t index);
    int RemoveProgram(const char* name);

    bgfx::UniformHandle GetUniform(const char* name) const;

    int RenderFrame(std::vector<GameObject*> modelLoader,
                    bx::Vec3&                lightDir,
                    CameraComponent*         camera,
                    bool                     debug = false);

    void RegisterGizmo(const std::string& tag, float size = 1.0f);
    void RenderGizmos(std::vector<GameObject*> gameObjects, CameraComponent* camera);

  private:
    std::string title;

    std::map<std::string, Gizmo> gizmoRegistry;
    bgfx::VertexBufferHandle     billboardVbh = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle      billboardIbh = BGFX_INVALID_HANDLE;
};

} // namespace Syngine