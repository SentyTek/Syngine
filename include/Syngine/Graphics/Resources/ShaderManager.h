// ╒════════════════════ ShaderManager.h ═╕
// │ Syngine                              │
// │ Created 2026-07-19                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Core/Logger.h>
#include <Syngine/Utils/Serializer.h>

#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>
#include <cstdint>
#include <sys/stat.h>
#include <vector>
#include <string>

#include <miniscl.hpp>

#define SYNINT_SHADER_METADATA_VERSION "1.0"

namespace Syngine {
enum ViewID : bgfx::ViewId;


struct MaterialParameterDesc {
    std::string               name;
    UniformType               type;
    uint16_t                  count;
    bgfx::UniformHandle       handle;
    std::array<std::byte, 64> defaultStorage;
};

struct TextureParameterDesc {
    std::string         name;
    uint8_t             stage;
    bgfx::UniformHandle handle;
};

class Shader {
    struct EngineUniform {
        bgfx::UniformHandle handle = BGFX_INVALID_HANDLE;
        UniformGetter       getter = nullptr;
        uint16_t            count  = 1;
    };

    std::vector<EngineUniform>         m_frameUniforms;
    std::vector<EngineUniform>         m_viewUniforms;
    std::vector<EngineUniform>         m_drawUniforms;
    std::vector<MaterialParameterDesc> m_materialParams;
    std::vector<TextureParameterDesc>  m_textureParams;
    bgfx::ProgramHandle                m_program = BGFX_INVALID_HANDLE;

    std::string bundlePath; // Path to the bundle from which this shader
                            // was loaded
    std::string shaderName; // Name of the shader

    ViewID m_viewId;
    Shader(bgfx::ProgramHandle               program,
           const std::string&                bundlePath,
           const std::string&                shaderName,
           const std::vector<EngineUniform>& frameUniforms,
           const std::vector<EngineUniform>& viewUniforms,
           const std::vector<EngineUniform>& drawUniforms,
           ViewID                   viewId)
        : m_program(program), bundlePath(bundlePath), shaderName(shaderName),
          m_frameUniforms(frameUniforms), m_viewUniforms(viewUniforms),
          m_drawUniforms(drawUniforms), m_viewId(viewId) {}

    friend class ShaderManager;
    friend class RenderCore;

  public:
    Shader(Shader&&) noexcept            = default;
    Shader& operator=(Shader&&) noexcept = default;
};

class ShaderManager {
    enum _UniformStage {
        ENGINE,
        MATERIAL,
        SAMPLER,
    };
    struct _UniformMeta {
        std::string      name;
        UniformType      type;
        UniformFrequency frequency;
        uint16_t         num;
        std::string      src;
        _UniformStage    stage;
        UniformGetter    getter;
    };

    static std::vector<_UniformMeta>
    _parseShaderMetadata(scl::pack::Packager& packager,
                         const std::string&   shaderName);

    static bool _resolveBindings(std::vector<_UniformMeta>& requestedBindings);

    static bool
    _createUniforms(const std::vector<_UniformMeta>&    requestedBindings,
                    std::vector<Shader::EngineUniform>& outFrameUniforms,
                    std::vector<Shader::EngineUniform>& outViewUniforms,
                    std::vector<Shader::EngineUniform>& outDrawUniforms,
                    std::vector<MaterialParameterDesc>& outMaterialParams,
                    std::vector<TextureParameterDesc>&  outTextureParams);

    static bgfx::ShaderHandle _LoadShaderFromMemory(const void* data,
                                                    size_t      size);

    ShaderManager() = delete; // Prevent instantiation of this class

    static std::vector<Shader> m_loadedShaders; // List of loaded shaders
  public:
    static size_t LoadShader(const std::string& bundlePath,
                             const std::string& shaderName,
                             const ViewID       viewId);

    static bool UnloadShader(size_t shaderId);
    static bool ReloadShader(size_t shaderId);
    static bool UnloadAllShaders();
    static bool ReloadAllShaders();

    static std::vector<Shader*> GetProgramsByViewID(ViewID viewId);

    static Shader* Get(size_t shaderId);
    static Shader* Get(const std::string& shaderName);
};

} // namespace Syngine
