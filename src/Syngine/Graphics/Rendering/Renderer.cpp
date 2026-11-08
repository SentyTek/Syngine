// ╒═══════════════════════ Renderer.cpp ═╕
// │ Syngine                              │
// │ Created 2025-04-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Graphics/Rendering/Renderer.h>
#include <Syngine/Core/Core.h>
#include <Syngine/Core/Logger.h>
#include <Syngine/GameObjects/AllComponents.h>
#include <Syngine/Graphics/Resources/TextureHelpers.h>
#include <Syngine/Graphics/Rendering/RenderDirector.h>
#include <Syngine/Graphics/Resources/ShaderManager.h>
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Graphics/Resources/RegisterBuiltinUniformProviders.inl>
#include <Syngine/Math/Math.hpp>
#include <Syngine/Math/Vector3.hpp>
#include <Syngine/Utils/FsUtils.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_properties.h>

#include "Syngine/GameObjects/Components/CameraComponent.h"
#include "Syngine/Graphics/Resources/MaterialManager.h"
#include "Syngine/Graphics/Resources/UniformRegistry.h"

#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <vector>

#if BX_PLATFORM_OSX
#include "MetalBridge.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Syngine {

// I present to you an absurd amount of static member definitions.
std::string Renderer::m_title;
bool        Renderer::m_isReady     = false;
bool        Renderer::m_isRendering = false;

float Renderer::m_gizmoSize = 1.0f;
std::unordered_map<std::string, Syngine::BillboardComponent*>
                                         Renderer::m_gizmoRegistry;
std::vector<Renderer::UniformCacheEntry> Renderer::m_uniformCache;
Renderer::UniformProviderData            Renderer::m_uniformProviderData;

int      Renderer::width         = 0;
int      Renderer::height        = 0;
uint64_t Renderer::currentDrawId = 1;

CameraComponent* Renderer::m_camera       = nullptr;
CameraComponent* Renderer::m_pseudoCamera = nullptr;
Math::Vector3    Renderer::m_sunDir       = Math::Vector3(0.0f, 1.0f, 0.0f);

Renderer::Renderer(int width, int height, const RendererConfig& config) {
    Renderer::width  = width;
    Renderer::height = height;
    Renderer::m_uniformCache.resize(256);

    if (!_CreateRenderer(config)) {
        Syngine::Logger::Fatal("Failed to create renderer");
    }
};

Renderer::~Renderer() {
    // Destroy all registered uniforms
    MaterialManager::DestroyAllMaterials();
    UniformRegistry::DestroyAllUniforms();
    ShaderManager::UnloadAllShaders();

    // Clear gizmos
    for (auto& [tag, gizmo] : m_gizmoRegistry) {
        delete gizmo;
    }
    m_gizmoRegistry.clear();

    RenderDirector::_Shutdown(); // Destroys RenderDirector buffers/textures/VBs
                                 // and calls bgfx::shutdown()
}

bool Renderer::_CreateRenderer(const RendererConfig& config) {
    _RegisterBuiltinUniformProviders();
    RenderDirector::_Initialize(config);

    m_isReady = true;
    Syngine::Logger::Info("Renderer created successfully");
    return true;
}

void Renderer::_RegisterGizmo(const std::string& tag) {
    for (auto& [existingTag, gizmo] : m_gizmoRegistry) {
        if (existingTag == tag) {
            // Gizmo already registered
            return;
        }
    }

    Syngine::BillboardComponent* gizmo =
        new BillboardComponent(nullptr,
                               "gizmos/default_gizmos.spk",
                               tag + ".png",
                               BillboardMode::CAMERA_ALIGNED,
                               m_gizmoSize);

    m_gizmoRegistry[tag] = gizmo;
}

void Renderer::_RenderFrame(DebugModes debug) {
    m_isRendering = true;
    RenderDirector::_RenderFrame(m_camera, debug);
    m_isRendering = false;
}

void Renderer::_UpdateDrawID() { currentDrawId++; }

void Renderer::SetPseudoCamera(CameraComponent* camera) {
    m_pseudoCamera = camera;
}

#ifdef SYN_IS_EDITOR
bgfx::TextureHandle Renderer::GetSceneTexture() {
    return RenderDirector::m_buffers.sceneColor;
}
#endif

} // namespace Syngine
