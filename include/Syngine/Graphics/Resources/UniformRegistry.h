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

class UniformRegistry {
    std::unordered_map<std::string, UniformDataProvider> m_UniformProviders;
    std::vector<bgfx::UniformHandle>
        m_UniformHandles; // for bgfx uniform handles

    std::unordered_map<std::string, UniformType>
        m_uniformTypeMap; // for shader metadata parsing
    std::unordered_map<std::string, UniformFrequency>
        m_uniformFreqMap; // for shader metadata parsing

    static UniformRegistry& GetInstance() {
        static UniformRegistry instance;
        return instance;
    }

    static bgfx::UniformHandle _GetUniformHandle(const std::string& name) {
        auto& instance = GetInstance();
        for (size_t i = 0; i < instance.m_UniformHandles.size(); ++i) {
            auto it = instance.m_UniformProviders.begin();
            std::advance(it, i);
            if (it->second.name == name) {
                return instance.m_UniformHandles[i];
            }
        }
        return BGFX_INVALID_HANDLE;
    }

    friend class ShaderManager; // for shader metadata parsing
    friend class RenderCore;    // for uniform updates during rendering
  public:
    static void RegisterProvider(const std::string&  name,
                                 UniformDataProvider provider) {
        if (provider.name.empty()) {
            provider.name = name; // Set the name if not provided
        }
        GetInstance().m_UniformProviders[name] = provider;
    }

    static UniformDataProvider* FindProvider(const std::string& name) {
        auto& providers = GetInstance().m_UniformProviders;
        auto  it        = providers.find(name);
        if (it != providers.end()) {
            return &it->second;
        }
        return nullptr;
    }

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

    template <typename T> static const T& GetContext(const void* ptr) {
        return *static_cast<const T*>(ptr);
    }
};

} // namespace Syngine
