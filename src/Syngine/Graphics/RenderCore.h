// ╒═══════════════════════ RenderCore.h ═╕
// │ Syngine                              │
// │ Created 2026-01-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Components/CameraComponent.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/ECS/AllComponents.h"

#include <cstdint>
#include <unordered_map>
#include <string>
#include <array>

#include <bgfx/bgfx.h>

namespace Syngine {

class RenderCore {
  public:
    /// @brief Render a single frame. Calls several internal rendering functions.
    /// @param camera Pointer to the camera component for rendering
    /// @param debug Debug modes for rendering
    /// @return true on success, false on failure
    /// @internal
    static bool _RenderFrame(CameraComponent* camera, DebugModes debug);

    /// @brief Initialize the RenderCore system
    /// @param config Renderer configuration options
    /// @return true on success, false on failure
    /// @internal
    static bool _Initialize(const RendererConfig& config);

    /// @brief Shutdown the RenderCore system
    /// @internal
    static void _Shutdown();

    /// @brief Set the resolution of the renderer
    /// @param width Width in pixels
    /// @param height Height in pixels
    /// @internal
    static void _SetResolution(int width, int height);

    /// @brief Get default uniform data by name
    /// @param name Name of the uniform
    /// @return Pointer to the Uniform struct, or nullptr if not found
    /// @internal
    static Uniform* _GetDefaultUniform(const std::string& name);

  private:
    // Called by _DrawShadows when CSM debug is enabled
    static void _CalculateCascadeMatrices(CameraComponent* camera,
                                          float*           outLightView,
                                          float*           outLightProj,
                                          float*           outCascadeSplits);

    static constexpr std::array<Syngine::ViewID, 10> _allViews = {
        Syngine::VIEW_SHADOW,   Syngine::VIEW_SKY,      Syngine::VIEW_GBUFFER,
        Syngine::VIEW_LIGHTING, Syngine::VIEW_FORWARD,  Syngine::VIEW_BILLBOARD,
        Syngine::VIEW_DEBUG,    Syngine::VIEW_BILL_DBG, Syngine::VIEW_UI,
        Syngine::VIEW_UI_DEBUG
    };

    static bool _PrepareRenderViews(CameraComponent* camera);
    static CameraComponent::Frustum _GetCascadeFrustum(uint8_t          cascade,
                                                      CameraComponent* camera);

    static void _DrawShadows(const Program&   program,
                             CameraComponent* camera,
                             uint8_t          cascade);
    static void _DrawSky(const Program& program);
    static void _DrawForward(const Program& program, CameraComponent* camera);
    static void _DrawDebug(const Program&   program,
                           CameraComponent* camera,
                           DebugModes       debug);
    static void _DrawBillboard(const Program& program);
    static void _DrawDbgBillboard(const Program& program);
    static void _DrawUIDebug(CameraComponent* camera);

    static float m_maxSmallObjDistance; //* Small objects get culled beyond this distance
    static float _CalculateScreenSize(const MeshAABB& aabb,
                                      const float*    cameraPos,
                                      const Camera&   camera,
                                      float           distance);
    static bool  _ShouldCullBySize(GameObject* go, CameraComponent* camera);
    static bool  _ShouldCullBySizeShadow(GameObject*      go,
                                         CameraComponent* camera,
                                         uint8_t          cascade);

    // Static members
    static std::unordered_map<std::string, uint16_t> m_defaultUniformIds; //* Default uniform IDs

    static bgfx::TextureHandle m_shadowDepth; //* Shadow map depth texture handle
    static bgfx::FrameBufferHandle m_shadowFB; //* Shadow map framebuffer handle
    static constexpr uint16_t      SHADOW_MAP_SIZE = 2048;
    static constexpr uint8_t       NUM_CASCADES    = 4;
    static float                   m_cascadeSizes[NUM_CASCADES];
    static float                   m_cascadeTexelSizes[NUM_CASCADES];

    static RendererConfig m_config; //* Render configuration

    static bgfx::VertexBufferHandle
        m_billboardVbh; //* Vertex buffer handle for billboards
    static bgfx::IndexBufferHandle
        m_billboardIbh; //* Index buffer handle for billboards

    static SDL_Window* win; //* SDL window handle

    static bgfx::VertexBufferHandle
        dummy; //* Dummy vertex buffer handle for rendering

    static DebugRender* m_drender; //* Debug renderer instance

    static bool
        m_isFirstFrame; //* Whether this is the first frame being rendered

    /// @brief Struct to hold counts of drawn objects per rendering pass
    struct DrawnObjectCount {
        uint32_t shadows = 0;
        uint32_t sky    = 0;
        uint32_t forward = 0;
        uint32_t debug   = 0;
        uint32_t billboard = 0;
        uint32_t ui        = 0;
        uint32_t culledFrustum = 0;
        uint32_t culledSize    = 0;
        uint32_t culledShadowFrustum = 0;
        uint32_t culledShadowSize    = 0;
    };

    static DrawnObjectCount
        m_drawnCounts; //* Counts of drawn objects per rendering pass

    friend class Core;
};

} // namespace Syngine
