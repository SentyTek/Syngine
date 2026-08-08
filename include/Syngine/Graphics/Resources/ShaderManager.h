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

#include <array>
#include <cstdint>
#include <optional>
#include <sys/stat.h>
#include <vector>
#include <string>

#define SYNINT_SHADER_METADATA_VERSION "1.0"

namespace Syngine {
enum ViewID : bgfx::ViewId;

/// @brief Struct to hold information about a material parameter
/// @section ShaderManager
/// @since v0.0.2
struct MaterialParameterDesc {
    std::string name;  //* Name of the material parameter
    UniformType type;  //* Type of the material parameter (e.g., VEC4, MAT4)
    uint16_t    count; //* Number of elements in the parameter (for arrays)
    bgfx::UniformHandle
        handle; //* Handle to the bgfx uniform associated with this parameter
    std::array<std::byte, 64>
        defaultStorage; //* Default storage for the parameter (used for
                        // initializing MaterialParameter)
};

/// @brief Struct to hold information about a texture parameter
/// @section ShaderManager
/// @since v0.0.2
struct TextureParameterDesc {
    std::string         name;   //* Name of the texture parameter
    uint8_t             stage;  //* Stage of the texture parameter
    bgfx::UniformHandle handle; //* Handle to the bgfx uniform associated with
                                // this texture parameter
};

/// @brief Class representing a shader program and its associated metadata
/// @section ShaderManager
/// @internal
/// @since v0.0.2
class Shader {
    /// @brief Struct to hold information about an engine uniform
    /// @section ShaderManager
    /// @internal
    /// @since v0.0.2
    struct EngineUniform {
        bgfx::UniformHandle handle =
            BGFX_INVALID_HANDLE; //* Handle to the bgfx uniform
        UniformGetter getter =
            nullptr;        //* Function pointer to retrieve uniform data
        uint16_t count = 1; //* Number of elements in the uniform (for arrays)
    };

    /// @brief Struct to hold information about an engine sampler
    /// @section ShaderManager
    /// @internal
    /// @since v0.0.2
    struct EngineSampler {
        std::string         name; //* Name of the engine sampler
        bgfx::UniformHandle handle =
            BGFX_INVALID_HANDLE; //* Handle to the bgfx uniform
        uint8_t       stage = 0; //* Stage of the engine sampler
        UniformGetter getter =
            nullptr; //* Function pointer to retrieve uniform data
    };

    std::vector<EngineUniform>         m_frameUniforms;
    std::vector<EngineUniform>         m_viewUniforms;
    std::vector<EngineUniform>         m_drawUniforms;
    std::vector<EngineSampler>         m_engineSamplers;
    std::vector<MaterialParameterDesc> m_materialParams;
    std::vector<TextureParameterDesc>  m_textureParams;
    bgfx::ProgramHandle                m_program = BGFX_INVALID_HANDLE;

    std::string bundlePath; // Path to the bundle from which this shader
                            // was loaded
    std::string shaderName; // Name of the shader

    ViewID m_viewId;
    Shader(bgfx::ProgramHandle                       program,
           const std::string&                        bundlePath,
           const std::string&                        shaderName,
           const std::vector<EngineUniform>&         frameUniforms,
           const std::vector<EngineUniform>&         viewUniforms,
           const std::vector<EngineUniform>&         drawUniforms,
           const std::vector<EngineSampler>&         engineSamplers,
           const std::vector<MaterialParameterDesc>& materialParams,
           const std::vector<TextureParameterDesc>&  textureParams,
           ViewID                                    viewId)
        : m_program(program), bundlePath(bundlePath), shaderName(shaderName),
          m_frameUniforms(frameUniforms), m_viewUniforms(viewUniforms),
          m_drawUniforms(drawUniforms), m_engineSamplers(engineSamplers),
          m_materialParams(materialParams), m_textureParams(textureParams),
          m_viewId(viewId) {}

    friend class ShaderManager;
    friend class RenderCore;
    friend class Material;

  public:
    Shader(Shader&&) noexcept            = default;
    Shader& operator=(Shader&&) noexcept = default;
};

/// @brief Class to manage shaders and their associated metadata
/// @section ShaderManager
/// @internal
/// @since v0.0.2
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
        uint8_t          texStage;
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
                    std::vector<Shader::EngineSampler>& outEngineSamplers,
                    std::vector<MaterialParameterDesc>& outMaterialParams,
                    std::vector<TextureParameterDesc>&  outTextureParams);

    static bgfx::ShaderHandle _LoadShaderFromMemory(const void* data,
                                                    size_t      size);

    static std::optional<Shader> _BuildShader(const std::string& bundlePath,
                                              const std::string& shaderName,
                                              const ViewID       viewId);

    ShaderManager() = delete; // Prevent instantiation of this class

    static std::vector<Shader> m_loadedShaders; // List of loaded shaders
  public:
    /// @brief Load a shader from a bundle and return its ID
    /// @param bundlePath Path to the bundle containing the shader
    /// @param shaderName Name of the shader to load
    /// @param viewId View ID associated with the shader
    /// @return size_t ID of the loaded shader, or BGFX_INVALID_HANDLE if
    /// loading failed
    /// @threadsafety not-safe
    /// @since v0.0.2
    static size_t LoadShader(const std::string& bundlePath,
                             const std::string& shaderName,
                             const ViewID       viewId);

    /// @brief Unload a shader by its ID
    /// @param shaderId ID of the shader to unload
    /// @return true if the shader was successfully unloaded, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.2
    static bool UnloadShader(size_t shaderId);

    /// @brief Reload a shader by its ID
    /// @param shaderId ID of the shader to reload
    /// @return true if the shader was successfully reloaded, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.2
    static bool ReloadShader(size_t shaderId);

    /// @brief Unload all loaded shaders
    /// @return true if all shaders were successfully unloaded, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.2
    static bool UnloadAllShaders();

    /// @brief Reload all loaded shaders
    /// @return true if all shaders were successfully reloaded, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.2
    static bool ReloadAllShaders();

    /// @brief Get all loaded shaders for a specific view ID
    /// @param viewId View ID to filter shaders by
    /// @return std::vector<Shader*> Vector of pointers to shaders associated
    /// with the specified view ID
    /// @threadsafety read-only
    /// @since v0.0.2
    static std::vector<Shader*> GetProgramsByViewID(ViewID viewId);

    /// @brief Get a shader by its ID
    /// @param shaderId ID of the shader to retrieve
    /// @return Shader* Pointer to the shader with the specified ID, or nullptr
    /// if not found
    /// @threadsafety read-only
    /// @since v0.0.2
    static Shader* Get(size_t shaderId);

    /// @brief Get a shader by its name
    /// @param shaderName Name of the shader to retrieve
    /// @return Shader* Pointer to the shader with the specified name, or
    /// nullptr if not found
    /// @note It is generally recommened to use ID for shader retrieval, as
    /// names are not guaranteed to be unique and considerably slower to search
    /// for
    /// @threadsafety read-only
    /// @since v0.0.2
    static Shader* Get(const std::string& shaderName);
};

} // namespace Syngine
