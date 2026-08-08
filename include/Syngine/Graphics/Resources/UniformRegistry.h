// ╒══════════════════ UniformRegistry.h ═╕
// │ Syngine                              │
// │ Created 2026-07-18                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

#include <bgfx/bgfx.h>

namespace Syngine {

using UniformGetter =
    const void* (*)(const void* context); // Function pointer type for uniform
                                          // data getters

/// @brief Different types of shader uniforms
/// @section Uniforms
/// @since v0.0.2
enum class UniformType {
    SAMPLER = bgfx::UniformType::Sampler, //* Sampler uniform
    VEC4    = bgfx::UniformType::Vec4,    //* Vec4 uniform
    MAT4    = bgfx::UniformType::Mat4,    //* Mat4 uniform
    MAT3    = bgfx::UniformType::Mat3,    //* Mat3 uniform
    UNKNOWN = bgfx::UniformType::Count,   //* Unknown uniform type
};

/// @brief Different frequencies at which shader uniforms can be updated
/// @section Uniforms
/// @since v0.0.2
enum class UniformFrequency {
    DRAW    = bgfx::UniformFreq::Draw,  //* Uniform updated per draw call
    VIEW    = bgfx::UniformFreq::View,  //* Uniform updated per view
    FRAME   = bgfx::UniformFreq::Frame, //* Uniform updated per frame
    UNKNOWN = bgfx::UniformFreq::Count, //* Count of uniform frequencies
};

/// @brief Struct to hold information about a uniform data provider
/// @section Uniforms
/// @since v0.0.2
struct UniformDataProvider {
    UniformType      type;      //* Type of the uniform
    UniformFrequency frequency; //* Frequency of uniform updates
    uint16_t         num = 1; //* Number of elements in the uniform (for arrays)
    UniformGetter    getter;  //* Function pointer to retrieve uniform data
    std::string      name;    //* Name of the uniform
};

/// @brief Class to manage uniform data providers and their associated bgfx
/// uniform handles
/// @section Uniforms
/// @since v0.0.2
class UniformRegistry {
    std::unordered_map<std::string, UniformDataProvider> m_UniformProviders;
    std::unordered_map<std::string, bgfx::UniformHandle> m_UniformHandlesMap;
    std::vector<bgfx::UniformHandle>                     m_UniformHandles;

    std::unordered_map<std::string, UniformType> m_uniformTypeMap = {
        { "sampler", UniformType::SAMPLER },
        { "vec4", UniformType::VEC4 },
        { "mat4", UniformType::MAT4 },
        { "mat3", UniformType::MAT3 },
    }; // for shader metadata parsing

    std::unordered_map<std::string, UniformFrequency> m_uniformFreqMap = {
        { "draw", UniformFrequency::DRAW },
        { "view", UniformFrequency::VIEW },
        { "frame", UniformFrequency::FRAME },
    }; // for shader metadata parsing

    /// @brief Get the singleton instance of the UniformRegistry
    /// @return UniformRegistry& Reference to the singleton instance
    /// @threadsafety thread-safe
    /// @internal
    /// @since v0.0.2
    static UniformRegistry& GetInstance() {
        static UniformRegistry instance;
        return instance;
    }

    /// @brief Get the bgfx uniform handle for a given uniform name
    /// @param name Name of the uniform
    /// @return bgfx::UniformHandle Handle to the bgfx uniform
    /// @threadsafety thread-safe
    /// @internal
    /// @since v0.0.2
    static bgfx::UniformHandle _GetUniformHandle(const std::string& name) {
        auto& instance = GetInstance();
        auto  it       = instance.m_UniformHandlesMap.find(name);
        if (it != instance.m_UniformHandlesMap.end()) {
            return it->second;
        }
        return BGFX_INVALID_HANDLE;
    }

    friend class ShaderManager; // for shader metadata parsing
    friend class RenderCore;    // for uniform updates during rendering
  public:
    /// @brief Register a uniform data provider with the registry
    /// @param name Name of the uniform
    /// @param provider UniformDataProvider struct containing the provider
    /// @threadsafety thread-safe
    /// @since v0.0.2
    static void RegisterProvider(const std::string&  name,
                                 UniformDataProvider provider) {
        if (provider.name.empty()) {
            provider.name = name; // Set the name if not provided
        }
        GetInstance().m_UniformProviders[name] = provider;
    }

    /// @brief Find a uniform data provider by name
    /// @param name Name of the uniform
    /// @return UniformDataProvider* Pointer to the provider, nullptr if not
    /// found
    /// @threadsafety thread-safe
    /// @since v0.0.2
    static UniformDataProvider* FindProvider(const std::string& name) {
        auto& providers = GetInstance().m_UniformProviders;
        auto  it        = providers.find(name);
        if (it != providers.end()) {
            return &it->second;
        }
        return nullptr;
    }

    /// @brief Destroys all registered uniform data providers and their
    /// associated bgfx uniform handles
    /// @threadsafety thread-safe
    /// @since v0.0.2
    static void DestroyAllUniforms() {
        // Destroy bgfx uniform handle if it exists
        for (auto& handle : GetInstance().m_UniformHandles) {
            if (bgfx::isValid(handle)) {
                bgfx::destroy(handle);
            }
        }
        GetInstance().m_UniformHandles.clear();
        GetInstance().m_UniformProviders.clear();
    }

    /// @brief Convert a void pointer to a specific type T and return a
    /// reference to it
    /// @tparam T The type to convert the pointer to
    /// @param ptr The void pointer to convert
    /// @return const T& Reference to the converted type
    /// @threadsafety thread-safe
    /// @since v0.0.2
    template <typename T> static const T& GetContext(const void* ptr) {
        return *static_cast<const T*>(ptr);
    }
};

} // namespace Syngine
