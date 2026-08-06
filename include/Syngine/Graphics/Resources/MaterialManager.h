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
#include <memory>
#include <optional>
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

class MaterialInstance;

/// Immutable material asset. A material owns the shader and its default values;
/// per-renderer changes belong on a MaterialInstance.
class Material {
    std::vector<MaterialParameter> m_parameters; // Material parameters
    std::vector<TextureParameter>
        m_textures; // Textures associated with the material

    friend class RenderCore;
    friend class MaterialManager;
    friend class MaterialInstance;

    /// @note Highly recommended to NOT use this constructor directly.
    Material(const std::string& name) : name(name), shader(nullptr) {}

    void
    _SetDefault(const std::string& paramName, const void* data, size_t size) {
        auto it = std::find_if(m_parameters.begin(),
                               m_parameters.end(),
                               [&](const MaterialParameter& param) {
                                   return param.name == paramName;
                               });
        if (it == m_parameters.end()) {
            Logger::Error("Material parameter not found: " + paramName);
            return;
        }
        std::memcpy(
            it->storage.data(), data, std::min(size, it->storage.size()));
    }

    void _SetDefaultTexture(const std::string&  paramName,
                            bgfx::TextureHandle texture,
                            uint32_t            samplerFlags,
                            uint8_t             stage) {
        auto it = std::find_if(m_textures.begin(),
                               m_textures.end(),
                               [&](const TextureParameter& textureParam) {
                                   return textureParam.name == paramName;
                               });
        if (it == m_textures.end()) {
            Logger::Error("Material texture parameter not found: " + paramName);
            return;
        }
        it->handle       = texture;
        it->samplerFlags = samplerFlags;
        it->stage        = stage;
    }

  public:
    const std::string name;   // Name of the material
    Shader* const     shader; // Pointer to the associated shader

    Material(const Material& other) = delete;

    Material& operator=(const Material&) = delete;

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
        for (const auto& shaderTexture : shader->m_textureParams) {
            m_textures.push_back({ shaderTexture.name,
                                   BGFX_INVALID_HANDLE,
                                   0,
                                   shaderTexture.stage });
        }
    }

    MaterialInstance CreateInstance() const;
    MaterialInstance Clone() const;
};

/// Mutable values used by a single renderer. Unset values resolve to the
/// immutable defaults held by Material.
class MaterialInstance {
    const Material*                               m_material = nullptr;
    std::vector<std::optional<MaterialParameter>> m_parameterOverrides;
    std::vector<std::optional<TextureParameter>>  m_textureOverrides;

    friend class RenderCore;

    const MaterialParameter*
    _FindParameter(const std::string& paramName) const {
        if (!m_material) return nullptr;
        auto it = std::find_if(m_material->m_parameters.begin(),
                               m_material->m_parameters.end(),
                               [&](const MaterialParameter& param) {
                                   return param.name == paramName;
                               });
        return it == m_material->m_parameters.end() ? nullptr : &*it;
    }

    const TextureParameter* _FindTexture(const std::string& paramName) const {
        if (!m_material) return nullptr;
        auto it = std::find_if(
            m_material->m_textures.begin(),
            m_material->m_textures.end(),
            [&](const TextureParameter& tex) { return tex.name == paramName; });
        return it == m_material->m_textures.end() ? nullptr : &*it;
    }

  public:
    MaterialInstance() = default;
    explicit MaterialInstance(const Material& material)
        : m_material(&material) {
        m_parameterOverrides.resize(material.m_parameters.size());
        m_textureOverrides.resize(material.m_textures.size());
    }

    const Material* GetMaterial() const { return m_material; }
    Shader*         GetShader() const {
        return m_material ? m_material->shader : nullptr;
    }

    inline void
    Set(const std::string& paramName, const void* data, size_t size) {
        if (!m_material) return;
        auto it = std::find_if(m_material->m_parameters.begin(),
                               m_material->m_parameters.end(),
                               [&](const MaterialParameter& param) {
                                   return param.name == paramName;
                               });
        if (it != m_material->m_parameters.end()) {
            const size_t index =
                static_cast<size_t>(it - m_material->m_parameters.begin());
            m_parameterOverrides[index] = *it;
            std::memcpy(
                m_parameterOverrides[index]->storage.data(),
                data,
                std::min(size, m_parameterOverrides[index]->storage.size()));
        } else {
            Syngine::Logger::Error(
                "Material parameter not found: " + paramName +
                ". Note that only parameters defined in the shader can be "
                "set.");
        }
    };

    inline void Set(uint16_t paramIndex, const void* data, size_t size) {
        if (m_material && paramIndex < m_material->m_parameters.size()) {
            m_parameterOverrides[paramIndex] =
                m_material->m_parameters[paramIndex];
            std::memcpy(
                m_parameterOverrides[paramIndex]->storage.data(),
                data,
                std::min(size,
                         m_parameterOverrides[paramIndex]->storage.size()));
        } else {
            Syngine::Logger::Error("Material parameter index out of range: " +
                                   std::to_string(paramIndex));
        }
    };

    inline void SetTexture(const std::string&  paramName,
                           bgfx::TextureHandle texture,
                           uint32_t            samplerFlags,
                           uint8_t             stage) {
        if (!m_material) return;
        auto it = std::find_if(
            m_material->m_textures.begin(),
            m_material->m_textures.end(),
            [&](const TextureParameter& tex) { return tex.name == paramName; });
        if (it == m_material->m_textures.end()) {
            Syngine::Logger::Error("Texture parameter not found: " + paramName);
            return;
        }
        const size_t index =
            static_cast<size_t>(it - m_material->m_textures.begin());
        m_textureOverrides[index]               = *it;
        m_textureOverrides[index]->handle       = texture;
        m_textureOverrides[index]->samplerFlags = samplerFlags;
        m_textureOverrides[index]->stage        = stage;
    };

    inline void SetTexture(uint16_t            paramIndex,
                           bgfx::TextureHandle texture,
                           uint32_t            samplerFlags,
                           uint8_t             stage) {
        if (m_material && paramIndex < m_material->m_textures.size()) {
            m_textureOverrides[paramIndex] = m_material->m_textures[paramIndex];
            m_textureOverrides[paramIndex]->handle       = texture;
            m_textureOverrides[paramIndex]->samplerFlags = samplerFlags;
            m_textureOverrides[paramIndex]->stage        = stage;
        } else {
            Syngine::Logger::Error("Texture parameter index out of range: " +
                                   std::to_string(paramIndex));
        }
    };

    inline void Destroy() {
        for (auto& tex : m_textureOverrides) {
            if (tex && bgfx::isValid(tex->handle)) {
                bgfx::destroy(tex->handle);
                tex->handle = BGFX_INVALID_HANDLE;
            }
        }
    };

    template <typename T>
    inline T Get(const std::string& paramName) const = delete;
};

template <>
inline Math::Vector4
MaterialInstance::Get<Math::Vector4>(const std::string& paramName) const {
    const MaterialParameter* param = _FindParameter(paramName);
    if (!param) return Math::Vector4();
    const size_t index =
        static_cast<size_t>(param - m_material->m_parameters.data());
    const auto&          storage = m_parameterOverrides[index]
                                       ? m_parameterOverrides[index]->storage
                                       : param->storage;
    std::array<float, 4> values;
    std::memcpy(values.data(), storage.data(), sizeof(values));
    return Math::Vector4(values[0], values[1], values[2], values[3]);
}

template <>
inline Math::Mat3
MaterialInstance::Get<Math::Mat3>(const std::string& paramName) const {
    const MaterialParameter* param = _FindParameter(paramName);
    if (!param) return Math::Mat3();
    const size_t index =
        static_cast<size_t>(param - m_material->m_parameters.data());
    const auto&          storage = m_parameterOverrides[index]
                                       ? m_parameterOverrides[index]->storage
                                       : param->storage;
    std::array<float, 9> values;
    std::memcpy(values.data(), storage.data(), sizeof(values));
    return Math::Mat3(values.data());
}

template <>
inline Math::Mat4
MaterialInstance::Get<Math::Mat4>(const std::string& paramName) const {
    const MaterialParameter* param = _FindParameter(paramName);
    if (!param) return Math::Mat4();
    const size_t index =
        static_cast<size_t>(param - m_material->m_parameters.data());
    const auto&           storage = m_parameterOverrides[index]
                                        ? m_parameterOverrides[index]->storage
                                        : param->storage;
    std::array<float, 16> values;
    std::memcpy(values.data(), storage.data(), sizeof(values));
    return Math::Mat4(values.data());
}

inline MaterialInstance Material::CreateInstance() const {
    return MaterialInstance(*this);
}

inline MaterialInstance Material::Clone() const { return CreateInstance(); }

class MaterialManager {
    static std::vector<std::unique_ptr<Material>> m_materials;
    static Material& _DeserializeMaterial(scl::stream& xmlStream);

  public:
    static Material& GetMaterialFromFile(const std::string& filePath);
    static Material& GetMaterialFromBundle(const std::string& bundlePath,
                                           const std::string& materialName);

    static Material& GetDefaultMaterialPBR(bool textured = true);
    static bgfx::TextureHandle GetFallbackAlbedoTexture();
    static bgfx::TextureHandle GetFallbackNormalTexture();

    static bool MaterialExists(const std::string& name) {
        auto it = std::find_if(m_materials.begin(),
                               m_materials.end(),
                               [&](const std::unique_ptr<Material>& mat) {
                                   return mat->name == name;
                               });
        return it != m_materials.end();
    };

    static Material& GetMaterialByName(const std::string& name) {
        auto it = std::find_if(m_materials.begin(),
                               m_materials.end(),
                               [&](const std::unique_ptr<Material>& mat) {
                                   return mat->name == name;
                               });
        if (it != m_materials.end()) {
            return *it->get();
        } else {
            Syngine::Logger::Error(
                "Material not found: " + name +
                ". Note that only materials loaded through the MaterialManager "
                "can be retrieved.");
            static Material dummy("dummy");
            return dummy;
        }
    };

    static Material& CreateMaterial(const std::string& name, Shader* shader) {
        if (MaterialExists(name)) {
            return GetMaterialByName(name);
        }
        std::unique_ptr<Material> mat =
            std::make_unique<Material>(name, shader);
        m_materials.push_back(std::move(mat));
        return *m_materials.back().get();
    };
};

} // namespace Syngine
