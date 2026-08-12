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

/// @brief Struct to hold information about a material parameter
/// @section MaterialManager
/// @since v0.0.2
struct MaterialParameter {
    std::string               name;    //* Name of the material parameter
    std::array<std::byte, 64> storage; //* Storage for the parameter data (used
                                       // for initializing MaterialParameter)
};

/// @brief Struct to hold information about a texture parameter
/// @section MaterialManager
/// @since v0.0.2
struct TextureParameter {
    std::string name; //* Name of the texture parameter
    bgfx::TextureHandle
        handle; //* Handle to the bgfx texture associated with this parameter
    uint32_t samplerFlags; //* Sampler flags for the texture parameter
    uint8_t  stage;        //* Stage of the texture parameter
};

class MaterialInstance;

/// @brief Immutable material asset. A material owns the shader and its default
/// values; per-renderer changes belong on a MaterialInstance.
/// @section MaterialManager
/// @since v0.0.2
class Material {
    std::vector<MaterialParameter> m_parameters; // Material parameters
    std::vector<TextureParameter>
        m_textures; // Textures associated with the material

    friend class RenderDirector;
    friend class MaterialManager;
    friend class MaterialInstance;

    /// @brief Private constructor to create a Material with a given name. Use
    /// MaterialManager::CreateMaterial to create a new material.
    /// @note Highly recommended to NOT use this constructor directly.
    /// @internal
    Material(const std::string& name) : name(name), shader(nullptr) {}

    /// @brief Set the default value for a material parameter
    /// @param paramName Name of the material parameter
    /// @param data Pointer to the data to set as the default value
    /// @param size Size of the data in bytes
    /// @threadsafety not-safe
    /// @internal
    /// @since v0.0.2
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

    /// @brief Set the default texture for a material parameter
    /// @param paramName Name of the material parameter
    /// @param texture Handle to the texture to set as the default value
    /// @param samplerFlags Sampler flags for the texture
    /// @param stage Stage of the texture parameter
    /// @threadsafety not-safe
    /// @internal
    /// @since v0.0.2
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
    const std::string name;   //* Name of the material
    Shader* const     shader; //* Pointer to the associated shader

    Material(const Material& other) = delete;

    Material& operator=(const Material&) = delete;

    /// @brief Construct a Material with a given name and shader
    /// @param name Name of the material
    /// @param shader Pointer to the associated shader
    /// @note Generally recommended to use MaterialManager::CreateMaterial to
    /// create a new material.
    /// @threadsafety not-safe
    /// @internal
    /// @since v0.0.2
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

    /// @brief Create a MaterialInstance from this Material
    /// @return MaterialInstance A new MaterialInstance with default values
    /// @threadsafety not-safe
    /// @since v0.0.2
    MaterialInstance CreateInstance() const;

    /// @brief Clone this Material into a new MaterialInstance
    /// @return MaterialInstance A new MaterialInstance with default values
    /// @threadsafety not-safe
    /// @since v0.0.2
    MaterialInstance Clone() const;
};

/// Mutable values used by a single renderer. Unset values resolve to the
/// immutable defaults held by Material.
class MaterialInstance {
    const Material*                               m_material = nullptr;
    std::vector<std::optional<MaterialParameter>> m_parameterOverrides;
    std::vector<std::optional<TextureParameter>>  m_textureOverrides;

    friend class RenderDirector;

    /// @brief Find a material parameter by name
    /// @param paramName Name of the material parameter
    /// @return const MaterialParameter* Pointer to the material parameter, or
    /// nullptr if not found
    /// @threadsafety read-only
    /// @internal
    /// @since v0.0.2
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

    /// @brief Find a texture parameter by name
    /// @param paramName Name of the texture parameter
    /// @return const TextureParameter* Pointer to the texture parameter, or
    /// nullptr if not found
    /// @threadsafety read-only
    /// @internal
    /// @since v0.0.2
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

    /// @brief Construct a MaterialInstance from a given Material
    /// @param material Reference to the Material to create the instance from
    /// @threadsafety not-safe
    /// @since v0.0.2
    explicit MaterialInstance(const Material& material)
        : m_material(&material) {
        m_parameterOverrides.resize(material.m_parameters.size());
        m_textureOverrides.resize(material.m_textures.size());
    }

    /// @brief Get the associated Material of this MaterialInstance
    /// @return const Material* Pointer to the associated Material
    /// @threadsafety read-only
    /// @since v0.0.2
    const Material* GetMaterial() const { return m_material; }

    /// @brief Get the associated Shader of this MaterialInstance
    /// @return Shader* Pointer to the associated Shader
    /// @threadsafety read-only
    /// @since v0.0.2
    Shader* GetShader() const {
        return m_material ? m_material->shader : nullptr;
    }

    /// @brief Set the value of a material parameter by name
    /// @param paramName Name of the material parameter
    /// @param data Pointer to the data to set
    /// @param size Size of the data in bytes
    /// @threadsafety not-safe
    /// @since v0.0.2
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

    /// @brief Set the value of a material parameter by index
    /// @param paramIndex Index of the material parameter
    /// @param data Pointer to the data to set
    /// @param size Size of the data in bytes
    /// @threadsafety not-safe
    /// @since v0.0.2
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

    /// @brief Set the value of a texture parameter by name
    /// @param paramName Name of the texture parameter
    /// @param texture Handle to the texture to set
    /// @param samplerFlags Sampler flags for the texture
    /// @param stage Stage of the texture parameter
    /// @threadsafety not-safe
    /// @since v0.0.2
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

    /// @brief Set the value of a texture parameter by index
    /// @param paramIndex Index of the texture parameter
    /// @param texture Handle to the texture to set
    /// @param samplerFlags Sampler flags for the texture
    /// @param stage Stage of the texture parameter
    /// @threadsafety not-safe
    /// @since v0.0.2
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

    /// @brief Destroy the material instance and release any associated
    /// resources
    /// @threadsafety not-safe
    /// @since v0.0.2
    inline void Destroy() {
        for (auto& tex : m_textureOverrides) {
            if (tex && bgfx::isValid(tex->handle)) {
                bgfx::destroy(tex->handle);
                tex->handle = BGFX_INVALID_HANDLE;
            }
        }
    };

    /// @brief Get the value of a material parameter by name
    /// @tparam T Type of the material parameter (e.g., Math::Vector4,
    /// Math::Mat4)
    /// @param paramName Name of the material parameter
    /// @return T Value of the material parameter, or default value if not found
    /// @threadsafety read-only
    /// @since v0.0.2
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

/// @brief Create a MaterialInstance from this Material
/// @return MaterialInstance A new MaterialInstance with default values
/// @threadsafety not-safe
/// @since v0.0.2
inline MaterialInstance Material::CreateInstance() const {
    return MaterialInstance(*this);
}

/// @brief Clone this Material into a new MaterialInstance
/// @return MaterialInstance A new MaterialInstance with default values
/// @threadsafety not-safe
/// @since v0.0.2
inline MaterialInstance Material::Clone() const { return CreateInstance(); }

/// @brief Class to manage materials and their instances
/// @section MaterialManager
/// @since v0.0.2
class MaterialManager {
    static std::vector<std::unique_ptr<Material>>
                     m_materials; //* Vector of all loaded materials;
    static Material& _DeserializeMaterial(scl::stream& xmlStream);

  public:
    /// @brief Get a material by its name
    /// @param name Name of the material to get
    /// @return Material& Reference to the material with the given name or a
    /// dummy material if not found
    /// @threadsafety read-only
    /// @since v0.0.2
    static Material& GetMaterialFromFile(const std::string& filePath);

    /// @brief Get a material from a bundle by its name
    /// @param bundlePath Path to the bundle
    /// @param materialName Name of the material to get
    /// @return Material& Reference to the material with the given name or a
    /// dummy material if not found
    /// @threadsafety read-only
    /// @since v0.0.2
    static Material& GetMaterialFromBundle(const std::string& bundlePath,
                                           const std::string& materialName);

    /// @brief Get the default PBR material
    /// @return Material& Reference to the default PBR material
    /// @threadsafety read-only
    /// @since v0.0.2
    static Material& GetDefaultMaterialPBR();

    /// @brief Get the fallback albedo texture
    /// @return bgfx::TextureHandle Handle to the fallback albedo texture
    /// @threadsafety read-only
    /// @since v0.0.2
    static bgfx::TextureHandle GetFallbackAlbedoTexture();

    /// @brief Get the fallback normal texture
    /// @return bgfx::TextureHandle Handle to the fallback normal texture
    /// @threadsafety read-only
    /// @since v0.0.2
    static bgfx::TextureHandle GetFallbackNormalTexture();

    /// @brief Check if a material with the given name exists
    /// @param name Name of the material to check
    /// @return true if the material exists, false otherwise
    /// @threadsafety read-only
    /// @since v0.0.2
    static bool MaterialExists(const std::string& name) {
        auto it = std::find_if(m_materials.begin(),
                               m_materials.end(),
                               [&](const std::unique_ptr<Material>& mat) {
                                   return mat->name == name;
                               });
        return it != m_materials.end();
    };

    /// @brief Get a material by its name
    /// @param name Name of the material to get
    /// @return Material& Reference to the material with the given name or a
    /// dummy material if not found
    /// @threadsafety read-only
    /// @since v0.0.2
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

    /// @brief Create a new material with the given name and shader
    /// @param name Name of the material to create
    /// @param shader Pointer to the shader to associate with the material
    /// @return Material& Reference to the newly created material
    /// @threadsafety not-safe
    /// @since v0.0.2
    static Material& CreateMaterial(const std::string& name, Shader* shader) {
        if (MaterialExists(name)) {
            return GetMaterialByName(name);
        }
        std::unique_ptr<Material> mat =
            std::make_unique<Material>(name, shader);
        m_materials.push_back(std::move(mat));
        return *m_materials.back().get();
    };

    /// @brief Destroy all materials
    /// @threadsafety not-safe
    /// @since v0.0.2
    static void DestroyAllMaterials();
};

} // namespace Syngine
