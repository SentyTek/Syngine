// ╒══════════════════ MaterialManager.h ═╕
// │ Syngine                              │
// │ Created 2026-07-23                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <Syngine/Core/Logger.h>
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Graphics/Resources/ShaderManager.h>
#include <Syngine/Math/Math.hpp>

#include <bgfx/bgfx.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

namespace scl {
class stream;
}

namespace Syngine {

struct MaterialParameter {
    std::string               name;
    std::array<std::byte, 64> storage;
};

struct TextureParameter {
    std::string         name;
    bgfx::TextureHandle handle;
    uint32_t            samplerFlags;
    uint8_t             stage;
};

class Material {
    std::vector<MaterialParameter> m_parameters; // Material parameters
    std::vector<TextureParameter>
        m_textures; // Textures associated with the material

    friend class RenderCore;
    friend class MaterialManager;

  public:
    std::string name;   // Name of the material
    Shader*     shader; // Pointer to the associated shader

    /// @note Using this constructor is NOT recommended. It is highly
    /// recommended to supply a shader.
    Material(const std::string& name) : name(name), shader(nullptr) {}

    Material(const std::string& name, Shader* shader)
        : name(name), shader(shader) {
        for (const auto& shaderParam : shader->m_materialParams) {
            MaterialParameter param;
            param.name = shaderParam.name;
            std::memcpy(param.storage.data(),
                        shaderParam.defaultStorage.data(),
                        sizeof(shaderParam.defaultStorage));
            m_parameters.push_back(param);
        }
    }

    inline void
    Set(const std::string& paramName, const void* data, size_t size) {
        auto it = std::find_if(m_parameters.begin(),
                               m_parameters.end(),
                               [&](const MaterialParameter& param) {
                                   return param.name == paramName;
                               });
        if (it != m_parameters.end()) {
            std::memcpy(it->storage.data(), data, size);
        } else {
            Syngine::Logger::Error(
                "Material parameter not found: " + paramName +
                ". Note that only parameters defined in the shader can be "
                "set.");
        }
    };

    inline void Set(uint16_t paramIndex, const void* data, size_t size) {
        if (paramIndex < m_parameters.size()) {
            std::memcpy(m_parameters[paramIndex].storage.data(), data, size);
        } else {
            Syngine::Logger::Error("Material parameter index out of range: " +
                                   std::to_string(paramIndex));
        }
    };

    inline void SetTexture(const std::string&  paramName,
                           bgfx::TextureHandle texture,
                           uint32_t            samplerFlags,
                           uint8_t             stage) {
        // Make sure we dont add a texture with the same name twice
        auto it = std::find_if(
            m_textures.begin(),
            m_textures.end(),
            [&](const TextureParameter& tex) { return tex.name == paramName; });
        if (it != m_textures.end()) {
            Syngine::Logger::Error("Texture parameter already exists: " +
                                   paramName);
            return;
        }

        TextureParameter texParam;
        texParam.name         = paramName;
        texParam.handle       = texture;
        texParam.samplerFlags = samplerFlags;
        texParam.stage        = stage;
        m_textures.push_back(texParam);
    };

    inline void SetTexture(uint16_t            paramIndex,
                           bgfx::TextureHandle texture,
                           uint32_t            samplerFlags,
                           uint8_t             stage) {
        if (paramIndex < m_textures.size()) {
            m_textures[paramIndex].handle       = texture;
            m_textures[paramIndex].samplerFlags = samplerFlags;
            m_textures[paramIndex].stage        = stage;
        } else {
            Syngine::Logger::Error("Texture parameter index out of range: " +
                                   std::to_string(paramIndex));
        }
    };

    inline void Destroy() {
        for (auto& tex : m_textures) {
            if (bgfx::isValid(tex.handle)) {
                bgfx::destroy(tex.handle);
            }
        }
    };

    template <typename T>
    inline T Get(const std::string& paramName) const = delete;
};

template <>
inline Math::Vector4
Material::Get<Math::Vector4>(const std::string& paramName) const {
    auto it = std::find_if(m_parameters.begin(),
                           m_parameters.end(),
                           [&](const MaterialParameter& param) {
                               return param.name == paramName;
                           });
    if (it != m_parameters.end()) {
        std::array<float, 4> values;
        std::memcpy(values.data(), it->storage.data(), sizeof(values));
        return Math::Vector4(values[0], values[1], values[2], values[3]);
    }

    Syngine::Logger::Error(
        "Material parameter not found: " + paramName +
        ". Note that only parameters defined in the shader can be "
        "retrieved.");
    return Math::Vector4();
}

template <>
inline Math::Mat3
Material::Get<Math::Mat3>(const std::string& paramName) const {
    auto it = std::find_if(m_parameters.begin(),
                           m_parameters.end(),
                           [&](const MaterialParameter& param) {
                               return param.name == paramName;
                           });
    if (it != m_parameters.end()) {
        std::array<float, 9> values;
        std::memcpy(values.data(), it->storage.data(), sizeof(values));
        return Math::Mat3(values.data());
    }

    Syngine::Logger::Error(
        "Material parameter not found: " + paramName +
        ". Note that only parameters defined in the shader can be "
        "retrieved.");
    return Math::Mat3();
}

template <>
inline Math::Mat4
Material::Get<Math::Mat4>(const std::string& paramName) const {
    auto it = std::find_if(m_parameters.begin(),
                           m_parameters.end(),
                           [&](const MaterialParameter& param) {
                               return param.name == paramName;
                           });
    if (it != m_parameters.end()) {
        std::array<float, 16> values;
        std::memcpy(values.data(), it->storage.data(), sizeof(values));
        return Math::Mat4(values.data());
    }

    Syngine::Logger::Error(
        "Material parameter not found: " + paramName +
        ". Note that only parameters defined in the shader can be "
        "retrieved.");
    return Math::Mat4();
}

class MaterialManager {
    static Material _DeserializeMaterial(scl::stream& xmlStream);

  public:
    static Material GetMaterialFromFile(const std::string& filePath);
    static Material GetMaterialFromBundle(const std::string& bundlePath,
                                          const std::string& materialName);

    static Material GetDefaultMaterialPBR(bool textured = true);
};

} // namespace Syngine
