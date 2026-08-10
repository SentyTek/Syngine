// ╒══ RegisterBuiltinUniformProviders.h ═╕
// │ Syngine                              │
// │ Created 2026-07-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include "Syngine/Scene/GameObjectRegistry.h"
#include "bgfx/bgfx.h"
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Core/Core.h>
#include <Syngine/Graphics/Rendering/Renderer.h>
#include <Syngine/Graphics/Rendering/RenderDirector.h>
#include <Syngine/GameObjects/AllComponents.h>

// This file registers built-in uniform providers for the renderer. These
// providers supply data for commonly used uniforms, such as camera matrices and
// lighting parameters.

namespace Syngine {

// The getter function should be as fast and flat as possible, as it will be
// called frequently during rendering.
inline void Renderer::_RegisterBuiltinUniformProviders() {
    // Engine uniforms
    // Register the "Engine.FrameCount" uniform provider, which provides the
    // current frame count of the engine.
    UniformRegistry::RegisterProvider(
        "Engine.FrameCount",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                Renderer::m_uniformProviderData.frameCount = {
                    static_cast<float>(Core::m_frameCounter.frameCount),
                    0.0f,
                    0.0f,
                    0.0f
                };
                return Renderer::m_uniformProviderData.frameCount.data();
            } });

    // Camera ones
    // Register the "Camera.ViewProjection" uniform provider, which provides the
    // view-projection matrix of the active camera.
    UniformRegistry::RegisterProvider(
        "Camera.ViewProjection",
        UniformDataProvider{
            UniformType::MAT4,
            UniformFrequency::VIEW,
            1,
            [](const void* ctx) -> const void* {
                auto* camera = Renderer::GetActiveCamera();
                if (!camera) {
                    return nullptr;
                }

                Renderer::m_uniformProviderData.cameraViewProjection =
                    camera->GetViewProjMatrix();
                return Renderer::m_uniformProviderData.cameraViewProjection
                    .data();
            } });

    UniformRegistry::RegisterProvider(
        "Camera.Position",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                auto* camera = Renderer::GetActiveCamera();
                if (!camera) {
                    return nullptr;
                }

                Renderer::m_uniformProviderData.cameraPosition =
                    Math::Vec4(camera->GetPosition(), 1.0f);
                return Renderer::m_uniformProviderData.cameraPosition.data();
            } });

    // Renderer stuff
    // Register the "Renderer.SunDirection" uniform provider, which provides the
    // direction of the sun light in world space.
    UniformRegistry::RegisterProvider(
        "Renderer.SunDirection",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                if (!GameObjectRegistry::GetFirstActiveDirectionalLight()) {
                    return nullptr;
                }
                return GameObjectRegistry::GetFirstActiveDirectionalLight()
                    ->GetDirectionVector()
                    .data();
            } });

    /* Set default sky colors
     * Math::Vector3 skyColorZenith   = { 0.529f, 0.808f, 0.922f };
     * Math::Vector3 skyColorMidnight = { 0.05f, 0.05f, 0.1f };
     * Math::Vector3 sunColor         = { 1.0f, 0.956f, 0.839f };
     * Math::Vector3 scatterColor     = { 0.8f, 0.5f, 0.3f };
     */
    UniformRegistry::RegisterProvider(
        "Renderer.SkyColorZenith",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                return Renderer::m_uniformProviderData.skyColorZenith.data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.SkyColorMidnight",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                return Renderer::m_uniformProviderData.skyColorMidnight.data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.SunColor",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                if (!GameObjectRegistry::GetFirstActiveDirectionalLight()) {
                    return nullptr;
                }
                return GameObjectRegistry::GetFirstActiveDirectionalLight()
                    ->GetColor()
                    .data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.HorizonColor",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                return Renderer::m_uniformProviderData.horizonColor.data();
            } });

    // Debug uniforms
    UniformRegistry::RegisterProvider(
        "Renderer.Debug.CSMLightViewProj",
        UniformDataProvider{
            UniformType::MAT4,
            UniformFrequency::FRAME,
            4,
            [](const void* ctx) -> const void* {
                return RenderDirector::m_csmLightViewProj.data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.Debug.CSMCascadeSplits",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                return RenderDirector::m_csmCascadeSplits.data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.ShadowParams",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                Renderer::m_uniformProviderData.shadowParams = {
                    bgfx::getCaps()->homogeneousDepth ? 1.0f : 0.0f,
                    1.0f / (float)(RenderDirector::m_config.shadowMapSize),
                    0.0f,
                    0.0f
                };
                return Renderer::m_uniformProviderData.shadowParams.data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.NormalMatrix",
        { UniformType::MAT4,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              const auto& packet =
                  UniformRegistry::GetContext<const Renderer::RenderPacket>(
                      context);
              auto* transform = packet.go->GetComponent<TransformComponent>();
              if (!transform) {
                  return nullptr;
              }
              Renderer::m_uniformProviderData.normalMatrix =
                  transform->GetModelMatrix().inverse().transposed();
              return Renderer::m_uniformProviderData.normalMatrix.data();
          } });

    // Default texture providers
    UniformRegistry::RegisterProvider(
        "Renderer.ShadowMap",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              return &RenderDirector::m_buffers.shadowDepth;
          } });
    UniformRegistry::RegisterProvider(
        "Renderer.ssaoTex",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              return &RenderDirector::m_buffers.ssaoTex;
          } });
    UniformRegistry::RegisterProvider(
        "Renderer.sceneColor",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              return &RenderDirector::m_buffers.sceneColor;
          } });
    UniformRegistry::RegisterProvider(
        "Renderer.sceneNormal",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              return &RenderDirector::m_buffers.sceneNormal;
          } });
    UniformRegistry::RegisterProvider(
        "Renderer.sceneDepth",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              return &RenderDirector::m_buffers.sceneDepth;
          } });

    // SSAO providers
    UniformRegistry::RegisterProvider(
        "Renderer.SSAO.Params",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                /*
                 * x = radius
                 * y = bias
                 * z = intensity
                 * w = unused
                 */
                return Renderer::m_uniformProviderData.ssaoParams.data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.SSAO.Resolution",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                Renderer::m_uniformProviderData.ssaoResolution = {
                    static_cast<float>(Renderer::width),
                    static_cast<float>(Renderer::height),
                    1.0f / static_cast<float>(Renderer::width),
                    1.0f / static_cast<float>(Renderer::height)
                };
                return Renderer::m_uniformProviderData.ssaoResolution.data();
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.SSAOBlur.Params",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* ctx) -> const void* {
                const auto& direction =
                    UniformRegistry::GetContext<const int>(ctx);
                /*
                 * x = blur radius
                 * y = blur direction (0 = horizontal, 1 = vertical)
                 * z = half resolution width
                 * w = half resolution height
                 */
                Renderer::m_uniformProviderData.ssaoBlurParams = {
                    4.0f,
                    static_cast<float>(direction),
                    Renderer::width / 2.0f,
                    Renderer::height / 2.0f
                };
                return Renderer::m_uniformProviderData.ssaoBlurParams.data();
            } });

    // Billboard uniforms
    UniformRegistry::RegisterProvider(
        "Billboard.Position",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::DRAW,
            1,
            [](const void* ctx) -> const void* {
                const auto& packet =
                    UniformRegistry::GetContext<const Renderer::RenderPacket>(
                        ctx);
                auto* billboard = packet.go->GetComponent<BillboardComponent>();

                thread_local Math::Vec4 data;
                if (billboard) {
                    auto* transform =
                        packet.go->GetComponent<TransformComponent>();
                    data = { transform->GetWorldPosition(), billboard->size };
                } else {
                    auto* camera = packet.go->GetComponent<CameraComponent>();
                    data         = { camera->GetPosition(), 1.0f };
                }

                return data.data();
            } });

    UniformRegistry::RegisterProvider(
        "Billboard.Mode",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::DRAW,
            1,
            [](const void* ctx) -> const void* {
                const auto& packet =
                    UniformRegistry::GetContext<const Renderer::RenderPacket>(
                        ctx);
                auto* billboard = packet.go->GetComponent<BillboardComponent>();

                thread_local Math::Vec4 data;
                if (billboard) {
                    data = { billboard->GetRot(),
                             static_cast<float>(billboard->GetMode()) };
                } else {
                    data = { 0.0f, 0.0f, 0.0f, 0.0f };
                }

                return data.data();
            } });

    UniformRegistry::RegisterProvider(
        "Billboard.Lighting",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::DRAW,
            1,
            [](const void* ctx) -> const void* {
                const auto& packet =
                    UniformRegistry::GetContext<const Renderer::RenderPacket>(
                        ctx);
                auto* billboard = packet.go->GetComponent<BillboardComponent>();

                thread_local Math::Vec4 data;
                if (billboard) {
                    data = { billboard->receiveSunLight ? 1.0f : 0.0f,
                             (billboard->receiveShadows &&
                              RenderDirector::m_config.useShadows)
                                 ? 1.0f
                                 : 0.0f,
                             0.0f,
                             0.0f };
                } else {
                    data = { 0.0f, 0.0f, 0.0f, 0.0f };
                }

                return data.data();
            } });
}

} // namespace Syngine
