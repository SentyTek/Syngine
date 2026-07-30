// ╒══ RegisterBuiltinUniformProviders.h ═╕
// │ Syngine                              │
// │ Created 2026-07-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include "bgfx/bgfx.h"
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Core/Core.h>
#include <Syngine/Graphics/Rendering/Renderer.h>
#include <Syngine/Graphics/Rendering/RenderCore.h>
#include <Syngine/ECS/AllComponents.h>

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
        UniformDataProvider{ UniformType::VEC4,
                             UniformFrequency::FRAME,
                             1,
                             [](const void* rawPacket) -> const void* {
                                 static float frameCount[4] = {
                                     static_cast<float>(
                                         Core::m_frameCounter.frameCount),
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                 };
                                 return frameCount;
                             } });

    // Camera ones
    // Register the "Camera.ViewProjection" uniform provider, which provides the
    // view-projection matrix of the active camera.
    UniformRegistry::RegisterProvider(
        "Camera.ViewProjection",
        UniformDataProvider{ UniformType::MAT4,
                             UniformFrequency::VIEW,
                             1,
                             [](const void* rawPacket) -> const void* {
                                 return Renderer::GetActiveCamera()
                                            ? Renderer::GetActiveCamera()
                                                  ->GetViewProjMatrix()
                                                  .data()
                                            : nullptr;
                             } });

    UniformRegistry::RegisterProvider(
        "Camera.Position",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* rawPacket) -> const void* {
                return Math::Vec4(Renderer::GetActiveCamera()->GetPosition(),
                                  1.0f)
                    .data();
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
            [](const void* rawPacket) -> const void* {
                Math::Vector3 dir       = Renderer::GetSunDirection();
                static float  dirArr[4] = { dir.x(), dir.y(), dir.z(), 0.0f };
                return dirArr;
            } });

    /* Set default sky colors
     * Math::Vector3 skyColorZenith   = { 0.529f, 0.808f, 0.922f };
     * Math::Vector3 skyColorMidnight = { 0.05f, 0.05f, 0.1f };
     * Math::Vector3 sunColor         = { 1.0f, 0.956f, 0.839f };
     * Math::Vector3 scatterColor     = { 0.8f, 0.5f, 0.3f };
     */
    UniformRegistry::RegisterProvider(
        "Renderer.SkyColorZenith",
        UniformDataProvider{ UniformType::VEC4,
                             UniformFrequency::FRAME,
                             1,
                             [](const void* rawPacket) -> const void* {
                                 static float skyColorZenith[4] = {
                                     0.529f, 0.808f, 0.922f, 1.0f
                                 };
                                 return skyColorZenith;
                             } });
    UniformRegistry::RegisterProvider(
        "Renderer.SkyColorMidnight",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* rawPacket) -> const void* {
                static float skyColorMidnight[4] = { 0.05f, 0.05f, 0.1f, 1.0f };
                return skyColorMidnight;
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.SunColor",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* rawPacket) -> const void* {
                static float sunColor[4] = { 1.0f, 0.956f, 0.839f, 1.0f };
                return sunColor;
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.HorizonColor",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* rawPacket) -> const void* {
                static float horizonColor[4] = { 0.8f, 0.5f, 0.3f, 1.0f };
                return horizonColor;
            } });

    // Debug uniforms
    UniformRegistry::RegisterProvider(
        "Renderer.Debug.CSMLightViewProj",
        UniformDataProvider{ UniformType::MAT4,
                             UniformFrequency::FRAME,
                             4,
                             [](const void* rawPacket) -> const void* {
                                 return RenderCore::m_csmLightViewProj.data();
                             } });
    UniformRegistry::RegisterProvider(
        "Renderer.Debug.CSMCascadeSplits",
        UniformDataProvider{ UniformType::VEC4,
                             UniformFrequency::FRAME,
                             1,
                             [](const void* rawPacket) -> const void* {
                                 return RenderCore::m_csmCascadeSplits.data();
                             } });
    UniformRegistry::RegisterProvider(
        "Renderer.ShadowParams",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* rawPacket) -> const void* {
                static float shadowParams[4] = {
                    bgfx::getCaps()->homogeneousDepth ? 1.0f : 0.0f,
                    1.0f / (float)(RenderCore::m_config.shadowMapSize),
                    0.0f,
                    0.0f
                };
                return shadowParams;
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
              static Math::Mat4 normalMatrix;
              normalMatrix = transform->GetModelMatrix().inverse().transposed();
              return normalMatrix.data();
          } });

    // Default texture providers
    UniformRegistry::RegisterProvider(
        "Renderer.ShadowMap",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              static bgfx::TextureHandle shadowMap =
                  RenderCore::m_buffers.shadowDepth;
              return &shadowMap;
          } });
    UniformRegistry::RegisterProvider("Renderer.ssaoTex",
                                      { UniformType::SAMPLER,
                                        UniformFrequency::DRAW,
                                        1,
                                        [](const void* context) -> const void* {
                                            static bgfx::TextureHandle ssaoTex =
                                                RenderCore::m_buffers.ssaoTex;
                                            return &ssaoTex;
                                        } });
    UniformRegistry::RegisterProvider(
        "Renderer.sceneColor",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              static bgfx::TextureHandle sceneColor =
                  RenderCore::m_buffers.sceneColor;
              return &sceneColor;
          } });
    UniformRegistry::RegisterProvider(
        "Renderer.sceneNormal",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              static bgfx::TextureHandle sceneNormal =
                  RenderCore::m_buffers.sceneNormal;
              return &sceneNormal;
          } });
    UniformRegistry::RegisterProvider(
        "Renderer.sceneDepth",
        { UniformType::SAMPLER,
          UniformFrequency::DRAW,
          1,
          [](const void* context) -> const void* {
              static bgfx::TextureHandle sceneDepth =
                  RenderCore::m_buffers.sceneDepth;
              return &sceneDepth;
          } });

    // SSAO providers
    UniformRegistry::RegisterProvider(
        "Renderer.SSAO.Params",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::FRAME,
            1,
            [](const void* rawPacket) -> const void* {
                static float ssaoParams[4] = {
                    0.5f, 0.1f, 1.0f, static_cast<float>(Renderer::width / 2.0f)
                };
                return ssaoParams;
            } });
    UniformRegistry::RegisterProvider(
        "Renderer.SSAO.Resolution",
        UniformDataProvider{ UniformType::VEC4,
                             UniformFrequency::FRAME,
                             1,
                             [](const void* rawPacket) -> const void* {
                                 static float ssaoResolution[4] = {
                                     static_cast<float>(Renderer::width),
                                     static_cast<float>(Renderer::height),
                                     1.0f / static_cast<float>(Renderer::width),
                                     1.0f / static_cast<float>(Renderer::height)
                                 };
                                 return ssaoResolution;
                             } });

    // Billboard uniforms
    UniformRegistry::RegisterProvider(
        "Billboard.Position",
        UniformDataProvider{
            UniformType::VEC4,
            UniformFrequency::DRAW,
            1,
            [](const void* rawPacket) -> const void* {
                const auto& packet =
                    UniformRegistry::GetContext<const Renderer::RenderPacket>(
                        rawPacket);
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
            [](const void* rawPacket) -> const void* {
                const auto& packet =
                    UniformRegistry::GetContext<const Renderer::RenderPacket>(
                        rawPacket);
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
            [](const void* rawPacket) -> const void* {
                const auto& packet =
                    UniformRegistry::GetContext<const Renderer::RenderPacket>(
                        rawPacket);
                auto* billboard = packet.go->GetComponent<BillboardComponent>();

                thread_local Math::Vec4 data;
                if (billboard) {
                    data = { billboard->receiveSunLight ? 1.0f : 0.0f,
                             (billboard->receiveShadows &&
                              RenderCore::m_config.useShadows)
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
