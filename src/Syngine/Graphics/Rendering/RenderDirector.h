// ╒═══════════════════ RenderDirector.h ═╕
// │ Syngine                              │
// │ Created 2026-01-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once
#include <Syngine/GameObjects/Components/CameraComponent.h>
#include <Syngine/Graphics/Resources/UniformRegistry.h>
#include <Syngine/Graphics/Resources/ShaderManager.h>
#include <Syngine/Graphics/Rendering/Renderer.h>
#include <Syngine/GameObjects/AllComponents.h>
#include <Syngine/Core/Memory/ArenaAlloc.h>
#include <Syngine/Math/Matrix4x4.hpp>

#include <bgfx/bgfx.h>

#include <cstdint>
#include <array>

#define SYNINT_DEFAULT_SHADERBUNDLE_NAME "shaders/default_shaders.spk"

namespace Syngine {

class RenderDirector {
  public:
    /// @brief Render a single frame. Calls several internal rendering
    /// functions.
    /// @param camera Pointer to the camera component for rendering
    /// @param debug Debug modes for rendering
    /// @return true on success, false on failure
    /// @internal
    static bool _RenderFrame(CameraComponent* camera, DebugModes debug);

    /// @brief Initialize the RenderDirector system
    /// @param config Renderer configuration options
    /// @return true on success, false on failure
    /// @internal
    static bool _Initialize(const RendererConfig& config);

    /// @brief Shutdown the RenderDirector system
    /// @internal
    static void _Shutdown();

    static bool _SetVsync();
    static bool _SetResolution(int width, int height);

    /// @brief Request a resolution change for the next frame
    /// @param reqW Requested width in pixels
    /// @param reqH Requested height in pixels
    /// @internal
    /// @since v0.0.2
    static void SetResolutionFlag(int reqW, int reqH) {
        m_changeResolutionThisFrame = true;
        m_requestedWidth            = reqW;
        m_requestedHeight           = reqH;
    }

    /// @brief Sets the Vsync status for the application
    /// @param enabled Whether to enable or disable Vsync
    /// @return true on success, false on failure
    /// @note Will take into effect on the next frame.
    /// @internal
    /// @since v0.0.2
    static void SetVsyncFlag(bool enabled) {
        if (enabled == m_config.vsync) return; // No change needed
        m_config.vsync         = enabled;
        m_changeVsyncThisFrame = true;
    }

  private:
    static constexpr uint16_t SHADOW_MAP_SIZE = 2048;
    static constexpr uint8_t  NUM_CASCADES    = 4;

    static bool m_changeResolutionThisFrame;
    static bool m_changeVsyncThisFrame;
    static int  m_requestedWidth;
    static int  m_requestedHeight;

    // Called by _DrawShadows when CSM debug is enabled
    static void
    _CalculateCascadeMatrices(CameraComponent*                camera,
                              DirectionalLightComponent*      lightSrc,
                              std::array<Math::Matrix4x4, 4>& outLightView,
                              std::array<Math::Matrix4x4, 4>& outLightProj,
                              Math::Vector4&                  outCascadeSplits);

    static constexpr std::array<Syngine::ViewID, 11> _allViews = {
        Syngine::VIEW_SHADOW,      Syngine::VIEW_LIGHT_SHADOWS,
        Syngine::VIEW_SKY,         Syngine::VIEW_FORWARD,
        Syngine::VIEW_BILLBOARD,   Syngine::VIEW_DEBUG,
        Syngine::VIEW_BILL_DBG,    Syngine::VIEW_AO,
        Syngine::VIEW_POSTPROCESS, Syngine::VIEW_UI,
        Syngine::VIEW_UI_DEBUG
    };

    static bool _CreateSceneBuffers();

    static Memory::ArenaAlloc
        m_renderPackets; //* All forward render packets for the current frame
    static Memory::ArenaAlloc
        m_billboardRenderPackets; //* All billboard render packets for the
                                  // current frame
    static std::tuple<std::span<RenderPacket>, std::span<RenderPacket>>
    _CollectRenderPackets(CameraComponent* camera); //* Collect render packets
                                                    // for the current frame

    static bool _PrepareRenderViews(CameraComponent* camera,
                                    bool&            outHasAvailGlobalLight);
    static CameraComponent::Frustum
    _GetCascadeFrustum(uint8_t                    cascade,
                       CameraComponent*           camera,
                       DirectionalLightComponent* lightSrc);

    static void _DrawShadows(const Shader* program, CameraComponent* camera);
    static void _DrawSky(const Shader* program, const CameraComponent* camera);
    static void _DrawForward(const Shader*           program,
                             CameraComponent*        camera,
                             std::span<RenderPacket> packets);
    static void _DrawDebug(const Shader*    program,
                           CameraComponent* camera,
                           DebugModes       debug);
    static void _DrawBillboard(const Shader*           program,
                               CameraComponent*        camera,
                               std::span<RenderPacket> packets);
    static void _DrawSSAO(const Shader* program);
    static void _DrawPostProcess(const Shader* program);
    static void _DrawDbgBillboard(Shader* program);
    static void _DrawUIDebug(CameraComponent* camera);
    static void _DrawUI(const Shader* program);

    static float
        m_maxSmallObjDistance; //* Small objects get culled beyond this distance
    static float _CalculateScreenSize(const MeshAABB& aabb,
                                      const Camera&   camera,
                                      float           distance);
    static bool  _ShouldCullBySize(GameObject* go, CameraComponent* camera);
    static bool  _ShouldCullBySizeShadow(GameObject*                go,
                                         CameraComponent*           camera,
                                         uint8_t                    cascade,
                                         DirectionalLightComponent* lightSrc);

    static void _ScreenSpaceQuad(ViewID view, const Shader* program);

    static void _SetFrameUniforms(const Shader* shader,
                                  const void*   ctx = nullptr);
    static void _SetViewUniforms(const Shader* shader,
                                 const void*   ctx = nullptr);
    static void _SetObjectUniforms(const Shader*                 shader,
                                   const Renderer::RenderPacket& packet);
    static void _SetMaterialUniforms(const Shader*                 shader,
                                     const Renderer::RenderPacket& packet,
                                     uint32_t                      flags = 0);

    // Static members
    struct RenderCoreBuffers {
        bgfx::FrameBufferHandle sceneFB; //* Framebuffer for scene rendering
        bgfx::TextureHandle
            sceneColor; //* Color texture for scene rendering (RGBA16F)
        bgfx::TextureHandle
            sceneDepth; //* Depth texture for scene rendering (D24S8)
        bgfx::TextureHandle
            sceneNormal; //* Normal texture for scene rendering (RGBA8)
        bgfx::FrameBufferHandle ssaoFB; //* Framebuffer for SSAO rendering
        bgfx::FrameBufferHandle
            ssaoBlurHFB; //* Temp framebuffer for SSAO blurring (horizontal)
        bgfx::FrameBufferHandle
            ssaoBlurVFB; //* Temp framebuffer for SSAO blurring (vertical)
        bgfx::TextureHandle ssaoTex;   //* SSAO texture (R8)
        bgfx::TextureHandle ssaoBlurH; //* SSAO texture mid-blur (R8)
        bgfx::TextureHandle
            ssaoBlurFinal; //* SSAO texture post-blur (Use this one) (R8)
        bgfx::TextureHandle     shadowDepth; //* Shadow map depth texture handle
        bgfx::FrameBufferHandle shadowFB;    //* Shadow map framebuffer

        template <class T> void ForEachTexture(T&& t) {
            t(sceneColor);
            t(sceneDepth);
            t(sceneNormal);
            t(ssaoTex);
            t(ssaoBlurH);
            t(ssaoBlurFinal);
            t(shadowDepth);
        }
        template <class T> void ForEachFrameBuffer(T&& t) {
            t(sceneFB);
            t(ssaoFB);
            t(ssaoBlurHFB);
            t(ssaoBlurVFB);
            t(shadowFB);
        }
    };
    static RenderCoreBuffers m_buffers;

    static Shader*             m_ssaoProgram;
    static bgfx::UniformHandle m_ssao_depthTex;
    static bgfx::UniformHandle m_ssao_normalTex;
    static bgfx::UniformHandle m_ssaob_ssaoTex;
    static bgfx::UniformHandle m_tonemap_sceneTex;
    static bgfx::UniformHandle m_tonemap_ssaoTex;
    static bgfx::UniformHandle m_defaultShadowMap;

    static std::array<Math::Matrix4x4, NUM_CASCADES> m_csmLightViewProj;
    static Math::Vec4                                m_csmCascadeSplits;
    static float m_cascadeSizes[NUM_CASCADES];
    static float m_cascadeTexelSizes[NUM_CASCADES];

    static RendererConfig m_config; //* Render configuration

    static bgfx::VertexBufferHandle
        m_billboardVbh; //* Vertex buffer handle for billboards
    static bgfx::IndexBufferHandle
        m_billboardIbh; //* Index buffer handle for billboards

    static bgfx::VertexBufferHandle
        m_fsQuadVbh; //* Vertex buffer handle for fullscreen quad

    static SDL_Window* win; //* SDL window handle

    static bgfx::VertexBufferHandle
        dummyVbh; //* Dummy vertex buffer handle for rendering

    static DebugRender* m_drender; //* Debug renderer instance

    static bool
        m_isFirstFrame; //* Whether this is the first frame being rendered

    /// @brief Struct to hold counts of drawn objects per rendering pass
    struct DrawnObjectCount {
        uint32_t shadows             = 0;
        uint32_t sky                 = 0;
        uint32_t forward             = 0;
        uint32_t debug               = 0;
        uint32_t billboard           = 0;
        uint32_t ui                  = 0;
        uint32_t culledFrustum       = 0;
        uint32_t culledSize          = 0;
        uint32_t culledShadowFrustum = 0;
        uint32_t culledShadowSize    = 0;
    };

    static DrawnObjectCount
        m_drawnCounts; //* Counts of drawn objects per rendering pass

    friend class Core;
    friend class Renderer;
    friend class UniformRegistry;

    static void _GetRenderResources();
    static bool
        m_collectedresources; //* Whether render resources have been collected

#ifdef SYN_IS_EDITOR
    static UI::Debug::ImGui_ImplBgfx m_uiDebug;
#endif
};

} // namespace Syngine
