// ╒═════════════════ RenderDirector.cpp ═╕
// │ Syngine                              │
// │ Created 2026-01-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Core/Core.h>
#include <Syngine/Core/Logger.h>
#include <Syngine/Scene/ZoneSystem.h>
#include <Syngine/Graphics/Rendering/RenderDirector.h>
#include "Syngine/GameObjects/Components/BillboardComponent.h"
#include "Syngine/Graphics/Resources/ModelLoader.h"
#include "Syngine/Graphics/Resources/UniformRegistry.h"
#include "Syngine/Scene/GameObjectRegistry.h"
#include <Syngine/Graphics/Resources/TextureHelpers.h>
#include <Syngine/Graphics/Rendering/DebugRenderer.h>
#include <Syngine/Graphics/Rendering/Renderer.h>
#include <Syngine/Graphics/Windowing.h>
#include <Syngine/GameObjects/Component.h>
#include <Syngine/GameObjects/Components/CameraComponent.h>
#include <Syngine/GameObjects/Components/MeshComponent.h>
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/Math/Math.hpp>
#include <Syngine/Utils/Serializer.h>
#include <Syngine/Utils/Version.h>
#include <Syngine/Utils/Profiler.h>

#include <bgfx/bgfx.h>
#include <bgfx/defines.h>
#include <bx/math.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#if BX_PLATFORM_OSX
#include "Syngine/Graphics/Rendering/MetalBridge.h"
#endif

#define SYNINT_DEFAULT_SHADERBUNDLE_NAME "shaders/default_shaders.spk"

#define SYN_INT_RENDEREXIT(program)                                            \
    Syngine::Logger::Error("Failed to create " #program " program");           \
    bgfx::shutdown();                                                          \
    SDL_DestroyWindow(win);                                                    \
    SDL_Quit();                                                                \
    return false;

namespace Syngine {

namespace {

bgfx::TextureHandle s_fallbackAlbedo = BGFX_INVALID_HANDLE;
bgfx::TextureHandle s_fallbackNormal = BGFX_INVALID_HANDLE;
bgfx::TextureHandle s_fallbackHeight = BGFX_INVALID_HANDLE;

bgfx::TextureHandle
_CreateSolidRGBA8Texture(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    const uint8_t pixel[4] = { r, g, b, a };
    return bgfx::createTexture2D(1,
                                 1,
                                 false,
                                 1,
                                 bgfx::TextureFormat::RGBA8,
                                 0,
                                 bgfx::copy(pixel, sizeof(pixel)));
}

} // namespace

// I present to you an unholy abomination of static member definitions
RenderDirector::RenderCoreBuffers RenderDirector::m_buffers = {
    .sceneFB = BGFX_INVALID_HANDLE, //* Framebuffer for scene rendering
    .sceneColor =
        BGFX_INVALID_HANDLE, //* Color texture for scene rendering (RGBA16F)
    .sceneDepth =
        BGFX_INVALID_HANDLE, //* Depth texture for scene rendering (D24S8)
    .sceneNormal =
        BGFX_INVALID_HANDLE, //* Normal texture for scene rendering (RGBA8)
    .ssaoFB = BGFX_INVALID_HANDLE, //* Framebuffer for SSAO rendering
    .ssaoBlurHFB =
        BGFX_INVALID_HANDLE, //* Temp framebuffer for SSAO blurring (horizontal)
    .ssaoBlurVFB =
        BGFX_INVALID_HANDLE, //* Temp framebuffer for SSAO blurring (vertical)
    .ssaoTex   = BGFX_INVALID_HANDLE, //* SSAO texture (R8)
    .ssaoBlurH = BGFX_INVALID_HANDLE, //* SSAO texture mid-blur (R8)
    .ssaoBlurFinal =
        BGFX_INVALID_HANDLE, //* SSAO texture post-blur (Use this one) (R8)
    .shadowDepth = BGFX_INVALID_HANDLE, //* Shadow map depth texture handle
    .shadowFB    = BGFX_INVALID_HANDLE, //* Shadow map framebuffer handle
};

bool RenderDirector::m_changeVsyncThisFrame      = false;
bool RenderDirector::m_changeResolutionThisFrame = false;
int  RenderDirector::m_requestedWidth            = 0;
int  RenderDirector::m_requestedHeight           = 0;

RendererConfig RenderDirector::m_config;
SDL_Window*    RenderDirector::win = nullptr;

DebugRender* RenderDirector::m_drender      = nullptr;
bool         RenderDirector::m_isFirstFrame = true;
float        RenderDirector::m_cascadeSizes[RenderDirector::NUM_CASCADES] = {
    20, 40, 0, 0
};
float RenderDirector::m_cascadeTexelSizes[RenderDirector::NUM_CASCADES] = {
    0, 0, 0, 0
};

bgfx::VertexBufferHandle RenderDirector::dummyVbh       = BGFX_INVALID_HANDLE;
bgfx::VertexBufferHandle RenderDirector::m_billboardVbh = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle  RenderDirector::m_billboardIbh = BGFX_INVALID_HANDLE;
bgfx::VertexBufferHandle RenderDirector::m_fsQuadVbh    = BGFX_INVALID_HANDLE;

Shader*             RenderDirector::m_ssaoProgram      = nullptr;
bgfx::UniformHandle RenderDirector::m_defaultShadowMap = BGFX_INVALID_HANDLE;
bgfx::UniformHandle RenderDirector::m_ssao_depthTex    = BGFX_INVALID_HANDLE;
bgfx::UniformHandle RenderDirector::m_ssao_normalTex   = BGFX_INVALID_HANDLE;
bgfx::UniformHandle RenderDirector::m_ssaob_ssaoTex    = BGFX_INVALID_HANDLE;
bgfx::UniformHandle RenderDirector::m_tonemap_sceneTex = BGFX_INVALID_HANDLE;
bgfx::UniformHandle RenderDirector::m_tonemap_ssaoTex  = BGFX_INVALID_HANDLE;

RenderDirector::DrawnObjectCount RenderDirector::m_drawnCounts;
float                            RenderDirector::m_maxSmallObjDistance =
    50.0f; //* Small objects get culled beyond this distance

std::vector<Renderer::RenderPacket> RenderDirector::m_renderPackets;
std::vector<Renderer::RenderPacket> RenderDirector::m_billboardRenderPackets;
std::array<Math::Matrix4x4, RenderDirector::NUM_CASCADES>
           RenderDirector::m_csmLightViewProj;
Math::Vec4 RenderDirector::m_csmCascadeSplits;

bool RenderDirector::_Initialize(const RendererConfig& config) {
    m_config = config;
    if (m_config.loadFromFile) {
        auto videoSettings = Serializer::_LoadCoreSettingsCategory<
            Serializer::CoreSettings::Video>();

        if (videoSettings) {
            m_config.useShadows = videoSettings->useShadows;
            m_config.shadowDist = videoSettings->shadowDist;
            m_config.vsync      = videoSettings->vSync;
            m_config.useSSAO    = videoSettings->useSSAO;
        } else {
            Syngine::Logger::Warn(
                "Failed to load video settings from file, using defaults");
        }
    }

    win = Window::_GetSDLWindow();
    if (!win) {
        Syngine::Logger::Fatal("No window to create renderer in");
        return false;
    }

    // Initialize bgfx
    bgfx::Init       bgInit;
    SDL_PropertiesID sdlProps = SDL_GetWindowProperties(win);
    if (sdlProps == 0) {
        Syngine::Logger::Error("Failed to get window properties");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    // Set platform data
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    // Check whether sdl is using x11 or wayland
    const char* _sdldriver = SDL_GetCurrentVideoDriver();
    if (!_sdldriver) {
        Syngine::Logger::Error("Failed to query SDL video driver");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
    bgInit.type = bgfx::RendererType::Vulkan; // Set renderer type to Vulkan
    if (!strcmp(_sdldriver, "x11")) {
        // Init for x11
        bgInit.platformData.ndt = SDL_GetPointerProperty(
            sdlProps, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        bgInit.platformData.nwh = (void*)SDL_GetNumberProperty(
            sdlProps, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        if (bgInit.platformData.nwh == NULL ||
            bgInit.platformData.ndt == NULL) {
            Syngine::Logger::Error(
                "Failed to get window properties for Linux X11");
            SDL_DestroyWindow(win);
            SDL_Quit();
            return false;
        }
    } else if (!strcmp(_sdldriver, "wayland")) {
        // Init for wayland
        bgInit.platformData.ndt = SDL_GetPointerProperty(
            sdlProps, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        bgInit.platformData.nwh = (void*)SDL_GetPointerProperty(
            sdlProps, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);

        bgInit.platformData.type = bgfx::NativeWindowHandleType::Wayland;

        if (bgInit.platformData.nwh == NULL ||
            bgInit.platformData.ndt == NULL) {
            Syngine::Logger::Error(
                "Failed to get window properties for Linux Wayland");
            SDL_DestroyWindow(win);
            SDL_Quit();
            return false;
        }
    } else {
        Syngine::Logger::Error("Unknown SDL video backend");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
#elif BX_PLATFORM_OSX
    bgInit.type = bgfx::RendererType::Metal; // Set renderer type to Metal
    bgInit.platformData.ndt = NULL;          // Only needed on x11 and wayland
    bgInit.platformData.nwh =
        _GetSYNMetalView(win); // Custom obj-c function to get the metal view
    if (bgInit.platformData.nwh == NULL) {
        Syngine::Logger::Error("Failed to get window properties for macOS");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
#elif BX_PLATFORM_WINDOWS
    bgInit.type             = bgfx::RendererType::Direct3D12;
    bgInit.platformData.ndt = NULL; // Only needed on x11
    bgInit.platformData.nwh = SDL_GetPointerProperty(
        sdlProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    if (bgInit.platformData.nwh == NULL) {
        Syngine::Logger::Error("Failed to get window properties for Windows");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
#else
#warning "Platform not supported"
    bgInit.platformData.ndt = NULL;
    bgInit.platformData.nwh = NULL;
#endif

    bgInit.resolution.width  = Renderer::width;
    bgInit.resolution.height = Renderer::height;
    bgInit.resolution.reset |= BGFX_RESET_VSYNC; // Enable vsync
    bgInit.debug = true;

    if (!bgfx::init(bgInit)) {
        Syngine::Logger::Error("Failed to initialize bgfx");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    // Reset view 0 to the dimensions of the window and clear it
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 0, 1, 0);
    bgfx::setViewRect(
        0, 0, 0, uint16_t(Renderer::width), uint16_t(Renderer::height));

    bgfx::touch(0);      // touch the view to clear it
    bgfx::frame();       // submit the frame
    SDL_ShowWindow(win); // show the window

    // Create default shaders
    Renderer::m_isReady =
        true; // Teehee. Temporarily set to true to allow shader loading.

    {
        ShaderManager::LoadShader(
            SYNINT_DEFAULT_SHADERBUNDLE_NAME, "default_sky", VIEW_SKY);
        ShaderManager::LoadShader(
            SYNINT_DEFAULT_SHADERBUNDLE_NAME, "default_pbr", VIEW_FORWARD);
        ShaderManager::LoadShader(
            SYNINT_DEFAULT_SHADERBUNDLE_NAME, "default_debug", VIEW_DEBUG);
        ShaderManager::LoadShader(SYNINT_DEFAULT_SHADERBUNDLE_NAME,
                                  "default_billboard",
                                  VIEW_BILLBOARD);
        ShaderManager::LoadShader(
            SYNINT_DEFAULT_SHADERBUNDLE_NAME, "default_shadow", VIEW_SHADOW);
        size_t ssao = ShaderManager::LoadShader(
            SYNINT_DEFAULT_SHADERBUNDLE_NAME, "ssao", VIEW_AO);
        ShaderManager::LoadShader(
            SYNINT_DEFAULT_SHADERBUNDLE_NAME, "ssao_blur", VIEW_AO);
        ShaderManager::LoadShader(
            SYNINT_DEFAULT_SHADERBUNDLE_NAME, "tonemapping", VIEW_POSTPROCESS);

        m_ssaoProgram      = ShaderManager::Get(ssao);
        m_defaultShadowMap = UniformRegistry::_GetUniformHandle("s_shadowMap");
        m_ssao_depthTex    = UniformRegistry::_GetUniformHandle("s_depth");
        m_ssao_normalTex   = UniformRegistry::_GetUniformHandle("s_normal");
        m_ssaob_ssaoTex    = UniformRegistry::_GetUniformHandle("s_ssao");
        m_tonemap_sceneTex = UniformRegistry::_GetUniformHandle("s_sceneColor");
        m_tonemap_ssaoTex  = UniformRegistry::_GetUniformHandle("s_ssao");
    }

    if (!bgfx::isValid(s_fallbackAlbedo)) {
        s_fallbackAlbedo = _CreateSolidRGBA8Texture(255, 255, 255, 255);
    }
    if (!bgfx::isValid(s_fallbackNormal)) {
        s_fallbackNormal = _CreateSolidRGBA8Texture(128, 128, 255, 255);
    }
    if (!bgfx::isValid(s_fallbackHeight)) {
        s_fallbackHeight = Syngine::CreateFlatTexture();
    }

    if (!bgfx::isValid(s_fallbackAlbedo) || !bgfx::isValid(s_fallbackNormal) ||
        !bgfx::isValid(s_fallbackHeight)) {
        Syngine::Logger::Error(
            "Failed to create one or more fallback textures");
        return false;
    }

    m_config.shadowMapSize = SHADOW_MAP_SIZE; // Ensure shadow map size is set

    // create billboard buffers
    static const float billboardVertices[]   = { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
                                                 0.5f,  -0.5f, 0.0f, 1.0f, 1.0f,
                                                 0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                                                 -0.5f, 0.5f,  0.0f, 0.0f, 0.0f };
    static const uint16_t billboardIndices[] = { 0, 1, 2, 0, 2, 3 };

    bgfx::VertexLayout billboardLayout;
    billboardLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
    m_billboardVbh = bgfx::createVertexBuffer(
        bgfx::copy(billboardVertices, sizeof(billboardVertices)),
        billboardLayout);
    m_billboardIbh = bgfx::createIndexBuffer(
        bgfx::copy(billboardIndices, sizeof(billboardIndices)));

    if (!bgfx::isValid(m_billboardVbh) || !bgfx::isValid(m_billboardIbh)) {
        Syngine::Logger::Error("Failed to create billboard buffers");
        bgfx::shutdown();
        SDL_Quit();
        return false;
    }

    // Create fullscreen quad buffer for post-processing
    bgfx::VertexLayout fsQuadLayout;
    fsQuadLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    // CCW Winding: (-1, 3), (3, -1), (-1, -1)
    static const float fsQuadVertices[] = {
        -1.0f, 3.0f,  0.99999f, 0.0f, -1.0f, // Top-Left (extended above)
        3.0f,  -1.0f, 0.99999f, 2.0f, 1.0f,  // Bottom-Right (extended right)
        -1.0f, -1.0f, 0.99999f, 0.0f, 1.0f   // Bottom-Left
    };
    m_fsQuadVbh = bgfx::createVertexBuffer(
        bgfx::copy(fsQuadVertices, sizeof(fsQuadVertices)), fsQuadLayout);

    // Dummy buffer to make metal happy
#if BX_PLATFORM_OSX
    static bgfx::VertexBufferHandle fullscreenDummyVBH = BGFX_INVALID_HANDLE;

    if (!bgfx::isValid(fullscreenDummyVBH)) {
        static Vector3 dummyData = { Vector3(0.0f, 0.0f, 0.0f) };

        bgfx::VertexLayout dummyLayout;
        dummyLayout.begin()
            .add(bgfx::Attrib::Position, 1, bgfx::AttribType::Float)
            .end();

        fullscreenDummyVBH = bgfx::createVertexBuffer(
            bgfx::copy(&dummyData, sizeof(dummyData)), dummyLayout);

        dummyVbh = fullscreenDummyVBH;
    }
#endif

    if (!_CreateSceneBuffers()) {
        return false;
    }

    Window::_SetContextCreated(true);

    return true;
}

void RenderDirector::_Shutdown() {
    // Destroy billboard buffers
    if (bgfx::isValid(m_billboardVbh)) {
        bgfx::destroy(m_billboardVbh);
        m_billboardVbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_billboardIbh)) {
        bgfx::destroy(m_billboardIbh);
        m_billboardIbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(dummyVbh)) {
        bgfx::destroy(dummyVbh);
        dummyVbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_fsQuadVbh)) {
        bgfx::destroy(m_fsQuadVbh);
        m_fsQuadVbh = BGFX_INVALID_HANDLE;
    }

    // Framebuffers are created with destroyTextures=true, so they own and
    // release attached textures on destroy.
    m_buffers.ForEachFrameBuffer([](auto& fb) {
        if (bgfx::isValid(fb)) {
            bgfx::destroy(fb);
        }
        fb = BGFX_INVALID_HANDLE;
    });

    // When SSAO is disabled, these are standalone dummy textures and are not
    // owned by any framebuffer.
    if (!m_config.useSSAO) {
        if (bgfx::isValid(m_buffers.ssaoTex)) {
            bgfx::destroy(m_buffers.ssaoTex);
        }
        if (bgfx::isValid(m_buffers.ssaoBlurH)) {
            bgfx::destroy(m_buffers.ssaoBlurH);
        }
        if (bgfx::isValid(m_buffers.ssaoBlurFinal)) {
            bgfx::destroy(m_buffers.ssaoBlurFinal);
        }
    }

    m_buffers.ForEachTexture([](auto& tex) { tex = BGFX_INVALID_HANDLE; });

    bgfx::shutdown();
}

bool RenderDirector::_CreateSceneBuffers() {
    // If shadows are enabled, initialize shadow mapping resources
    if (m_config.useShadows) {
        m_buffers.shadowDepth = bgfx::createTexture2D(
            SHADOW_MAP_SIZE * 2,
            SHADOW_MAP_SIZE * 2,
            false,
            1,
            bgfx::TextureFormat::D32,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

        if (!bgfx::isValid(m_buffers.shadowDepth)) {
            Syngine::Logger::Error("Failed to create shadow depth texture");
            return false;
        }

        m_buffers.shadowFB =
            bgfx::createFrameBuffer(1, &m_buffers.shadowDepth, true);
        if (!bgfx::isValid(m_buffers.shadowFB)) {
            Syngine::Logger::Error("Failed to create shadow framebuffer");
            bgfx::destroy(m_buffers.shadowDepth);
            return false;
        }

        m_cascadeSizes[2] =
            round(static_cast<float>(m_config.shadowDist) / 3.0f);
        m_cascadeSizes[3] = round(static_cast<float>(m_config.shadowDist));
    }

    // Create scene textures
    const uint64_t tsFlags =
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

    m_buffers.sceneColor  = bgfx::createTexture2D(uint16_t(Renderer::width),
                                                 uint16_t(Renderer::height),
                                                 false,
                                                 1,
                                                 bgfx::TextureFormat::RGBA16F,
                                                 tsFlags);
    m_buffers.sceneNormal = bgfx::createTexture2D(uint16_t(Renderer::width),
                                                  uint16_t(Renderer::height),
                                                  false,
                                                  1,
                                                  bgfx::TextureFormat::RGBA16F,
                                                  tsFlags);
    m_buffers.sceneDepth  = bgfx::createTexture2D(uint16_t(Renderer::width),
                                                 uint16_t(Renderer::height),
                                                 false,
                                                 1,
                                                 bgfx::TextureFormat::D24S8,
                                                 BGFX_TEXTURE_RT);

    if (m_config.useSSAO) {
        const uint16_t ssaoWidth  = uint16_t(std::max(1, Renderer::width / 2));
        const uint16_t ssaoHeight = uint16_t(std::max(1, Renderer::height / 2));

        m_buffers.ssaoTex = bgfx::createTexture2D(
            ssaoWidth, ssaoHeight, false, 1, bgfx::TextureFormat::R16, tsFlags);
        m_buffers.ssaoBlurH = bgfx::createTexture2D(
            ssaoWidth, ssaoHeight, false, 1, bgfx::TextureFormat::R16, tsFlags);
        m_buffers.ssaoBlurFinal = bgfx::createTexture2D(
            ssaoWidth, ssaoHeight, false, 1, bgfx::TextureFormat::R16, tsFlags);
        m_buffers.ssaoFB = bgfx::createFrameBuffer(1, &m_buffers.ssaoTex, true);
        m_buffers.ssaoBlurHFB =
            bgfx::createFrameBuffer(1, &m_buffers.ssaoBlurH, true);
        m_buffers.ssaoBlurVFB =
            bgfx::createFrameBuffer(1, &m_buffers.ssaoBlurFinal, true);
    } else {
        // Create dummy textures if needed
        m_buffers.ssaoTex       = _CreateSolidRGBA8Texture(255, 255, 255, 255);
        m_buffers.ssaoBlurH     = _CreateSolidRGBA8Texture(255, 255, 255, 255);
        m_buffers.ssaoBlurFinal = _CreateSolidRGBA8Texture(255, 255, 255, 255);

        if (!bgfx::isValid(m_buffers.ssaoTex) ||
            !bgfx::isValid(m_buffers.ssaoBlurH) ||
            !bgfx::isValid(m_buffers.ssaoBlurFinal)) {
            Syngine::Logger::Error("Failed to create dummy SSAO textures");
            return false;
        }
    }

    // Create global scene framebuffer (MRT: 0:Color, 1:Normal, Depth)
    bgfx::TextureHandle screenTextures[] = { m_buffers.sceneColor,
                                             m_buffers.sceneNormal,
                                             m_buffers.sceneDepth };
    m_buffers.sceneFB                    = bgfx::createFrameBuffer(
        BX_COUNTOF(screenTextures), screenTextures, true);

    if (!bgfx::isValid(m_buffers.sceneColor) ||
        !bgfx::isValid(m_buffers.sceneDepth) ||
        !bgfx::isValid(m_buffers.sceneNormal) ||
        (m_config.useSSAO && !bgfx::isValid(m_buffers.ssaoFB)) ||
        (m_config.useShadows && !bgfx::isValid(m_buffers.shadowFB))) {
        Syngine::Logger::Error("Failed to create scene textures");
        return false;
    }
    return true;
}

bool RenderDirector::_SetResolution(int width, int height) {
    if (width <= 0 || height <= 0) {
        Syngine::Logger::LogF(LogLevel::WARN,
                              true,
                              "Ignoring invalid resolution: %dx%d",
                              width,
                              height);
        return false;
    }

    if (width == Renderer::width && height == Renderer::height) {
        return true;
    }

    // Destroy current render targets before reset/recreate to keep backend
    // resource lifetime transitions clean during live resize.
    m_buffers.ForEachFrameBuffer([](auto& fb) {
        if (bgfx::isValid(fb)) {
            bgfx::destroy(fb);
        }
        fb = BGFX_INVALID_HANDLE;
    });

    // When SSAO is disabled, these are standalone dummy textures and are not
    // owned by any framebuffer.
    if (!m_config.useSSAO) {
        if (bgfx::isValid(m_buffers.ssaoTex)) {
            bgfx::destroy(m_buffers.ssaoTex);
        }
        if (bgfx::isValid(m_buffers.ssaoBlurH)) {
            bgfx::destroy(m_buffers.ssaoBlurH);
        }
        if (bgfx::isValid(m_buffers.ssaoBlurFinal)) {
            bgfx::destroy(m_buffers.ssaoBlurFinal);
        }
    }

    m_buffers.ForEachTexture([](auto& tex) { tex = BGFX_INVALID_HANDLE; });

    Renderer::height                        = height;
    Renderer::width                         = width;
    Serializer::m_coreSettings.video.width  = width;
    Serializer::m_coreSettings.video.height = height;

    bgfx::reset(uint32_t(width),
                uint32_t(height),
                m_config.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));

    if (!_CreateSceneBuffers()) return false;
    return true;
}

bool RenderDirector::_SetVsync() {
    Serializer::m_coreSettings.video.vSync = m_config.vsync;

    bgfx::reset(uint32_t(Renderer::width),
                uint32_t(Renderer::height),
                m_config.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
    bgfx::setViewRect(
        0, 0, 0, uint16_t(Renderer::width), uint16_t(Renderer::height));

    return true;
}

/*
--- Drawing helpers ---
*/

void RenderDirector::_CalculateCascadeMatrices(
    CameraComponent*                           camera,
    DirectionalLightComponent*                 lightSrc,
    std::array<Math::Matrix4x4, NUM_CASCADES>& outLightView,
    std::array<Math::Matrix4x4, NUM_CASCADES>& outLightProj,
    Math::Vector4&                             outCascadeSplits) {

    const Math::Vector3 lightDirVec =
        lightSrc->GetDirectionVector().normalized();

    const Math::Vector3 target = camera->GetPosition();
    const Math::Vector3 up(0.0f, 1.0f, 0.0f); // Up vector for light view matrix

    Math::Vector4                             cascadeDistances;
    std::array<Math::Matrix4x4, NUM_CASCADES> lightViewProj;

    Math::Vector3 zero;
    Math::Mat4    lightViewRaw;
    bx::mtxLookAt(lightViewRaw.data(), zero.toBxVec3(), lightDirVec.toBxVec3());

    for (uint32_t i = 0; i < NUM_CASCADES; i++) {
        float size = m_cascadeSizes[i];

        Math::Vector4 camPos4(target);
        Math::Vector4 camPosLightSpace = camPos4 * lightViewRaw;

        float worldPerTexel    = (size * 2.0f) / float(SHADOW_MAP_SIZE);
        m_cascadeTexelSizes[i] = worldPerTexel;
        float xSnapped =
            floor(camPosLightSpace.x() / worldPerTexel) * worldPerTexel;
        float ySnapped =
            floor(camPosLightSpace.y() / worldPerTexel) * worldPerTexel;

        float xOffset = xSnapped - camPosLightSpace.x();
        float yOffset = ySnapped - camPosLightSpace.y();

        // Keep the light camera's depth range proportional to this cascade.
        // The previous fixed 20..(shadowDist + 50) interval was used for all
        // cascades, leaving the near cascades with only a tiny fraction of the
        // D16 depth range and visibly quantized depth values.
        const float   lightDistance   = size * 2.0f;
        const float   depthPadding    = size * 1.5f;
        Math::Vector3 lightPosSnapped = target + lightDirVec * lightDistance;

        if (Core::_GetContext()->debug.CSMBounds) {
            // Draw line from light to target
            Math::Vector3 from = lightPosSnapped;
            Math::Vector3 to   = target;
            Core::_GetContext()->physicsManager->_DrawLine(
                from, to, JPH::Color::sYellow);
        }

        bx::mtxLookAt(outLightView[i].data(),
                      lightPosSnapped.toBxVec3(),
                      target.toBxVec3());

        const float lightNear = std::max(0.1f, lightDistance - depthPadding);
        const float lightFar  = lightDistance + depthPadding;

        bx::mtxOrtho(outLightProj[i].data(),
                     -size + xOffset,
                     size + xOffset,
                     -size + yOffset,
                     size + yOffset,
                     lightNear,
                     lightFar,
                     0.0f,
                     bgfx::getCaps()->homogeneousDepth);

        cascadeDistances.set(i, m_cascadeSizes[i] / 2.0f);
        bx::mtxMul(lightViewProj[i].data(),
                   outLightView[i].data(),
                   outLightProj[i].data());
    }

    outCascadeSplits   = cascadeDistances;
    m_csmLightViewProj = lightViewProj;
}

CameraComponent::Frustum
RenderDirector::_GetCascadeFrustum(uint8_t                    cascade,
                                   CameraComponent*           camera,
                                   DirectionalLightComponent* lightSrc) {
    SYN_PROFILE_FUNCTION();
    CameraComponent::Frustum                  cascadeFrustum;
    std::array<Math::Matrix4x4, NUM_CASCADES> lightView;
    std::array<Math::Matrix4x4, NUM_CASCADES> lightProj;
    Math::Vector4                             outCascadeSplits;
    _CalculateCascadeMatrices(
        camera, lightSrc, lightView, lightProj, outCascadeSplits);

    Math::Matrix4x4 cascadeViewProj = lightView[cascade] * lightProj[cascade];

    const auto extractPlane = [&cascadeViewProj](int axis, float sign) {
        CameraComponent::Plane plane;
        plane.normal = Math::Vector3(
            cascadeViewProj.m(0, 3) + sign * cascadeViewProj.m(0, axis),
            cascadeViewProj.m(1, 3) + sign * cascadeViewProj.m(1, axis),
            cascadeViewProj.m(2, 3) + sign * cascadeViewProj.m(2, axis));
        plane.distance =
            cascadeViewProj.m(3, 3) + sign * cascadeViewProj.m(3, axis);
        return plane;
    };

    cascadeFrustum.left = extractPlane(0, 1.0f);
    CameraComponent::_normalizePlane(cascadeFrustum.left);

    cascadeFrustum.right = extractPlane(0, -1.0f);
    CameraComponent::_normalizePlane(cascadeFrustum.right);

    cascadeFrustum.bottom = extractPlane(1, 1.0f);
    CameraComponent::_normalizePlane(cascadeFrustum.bottom);

    cascadeFrustum.top = extractPlane(1, -1.0f);
    CameraComponent::_normalizePlane(cascadeFrustum.top);

    cascadeFrustum.n = extractPlane(2, 1.0f);
    CameraComponent::_normalizePlane(cascadeFrustum.n);

    cascadeFrustum.f = extractPlane(2, -1.0f);
    CameraComponent::_normalizePlane(cascadeFrustum.f);
    return cascadeFrustum;
}

float RenderDirector::_CalculateScreenSize(const MeshAABB& aabb,
                                           const Camera&   camera,
                                           float           distance) {
    SYN_PROFILE_FUNCTION();
    if (distance <= 0.01f) return 1000.0f;

    // Fast max without initializer_list
    float maxExtent =
        std::max(aabb.halfExtents[0],
                 std::max(aabb.halfExtents[1], aabb.halfExtents[2])) *
        2.0f;

    // Use a precomputed focal length off the camera if you can!
    // e.g., camera.GetFocalLength() or pass down precomputed (Renderer::height
    // / (2.0f * tanf(fov / 2)))
    float projectedSize = (maxExtent / distance) * camera.screenFocalLength;

    return projectedSize;
}

bool RenderDirector::_ShouldCullBySize(GameObject*      go,
                                       CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    auto* meshComp = go->GetComponent<MeshComponent>();
    if (!meshComp || !meshComp->IsEnabled()) return false;

    const MeshAABB&      aabb   = meshComp->GetAABB();
    const Math::Vector3& camPos = camera->GetPosition();

    // 1. Quick distance check using SQUARED distance (no sqrt)
    // Vector3 length/distance squared is way cheaper
    float distSq = camPos.distanceSquared(aabb.center);

    // 20.0f squared is 400.0f
    if (distSq < 400.0f) return false;

    // Compare against max distance squared to avoid sqrt if it's too close
    // anyway
    float maxDistSq = m_maxSmallObjDistance * m_maxSmallObjDistance;
    if (distSq > maxDistSq) {
        float distance =
            std::sqrt(distSq); // Only pay for sqrt when actually needed
        float screenSize =
            _CalculateScreenSize(aabb, camera->GetCamera(), distance);
        return screenSize < 4.0f;
    }

    return false;
}

bool RenderDirector::_ShouldCullBySizeShadow(
    GameObject*                go,
    CameraComponent*           camera,
    uint8_t                    cascade,
    DirectionalLightComponent* lightSrc) {
    SYN_PROFILE_FUNCTION();
    auto* meshComp = go->GetComponent<MeshComponent>();
    if (!meshComp || !meshComp->IsEnabled()) return false;

    const MeshAABB& aabb = meshComp->GetAABB();

    // 1. CHEAPEST CHECK FIRST: Size relative to cascade
    float maxExtent =
        std::max(aabb.halfExtents[0],
                 std::max(aabb.halfExtents[1], aabb.halfExtents[2])) *
        2.0f;

    float cascadeThreshold = m_cascadeSizes[cascade] * 0.01f;
    if (maxExtent < cascadeThreshold) {
        return true; // Cull small objects immediately
    }

    // 2. EXPENSIVE CHECK SECOND: Distances
    const Math::Vector3& camPos    = camera->GetPosition();
    float                camDistSq = camPos.distanceSquared(aabb.center);
    if (camDistSq < 400.0f)
        return false; // Within 20 units of camera? Don't cull

    Math::Vector3 lightDir    = lightSrc->GetDirectionVector().normalized();
    Math::Vector3 lightPos    = camPos + lightDir * 100.0f;
    float         lightDistSq = lightPos.distanceSquared(aabb.center);
    if (lightDistSq < 100.0f)
        return false; // Within 10 units of light? Don't cull

    return false; // Otherwise, don't cull by size for shadows
}

void RenderDirector::_CollectRenderPackets(CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    m_renderPackets.clear();
    m_billboardRenderPackets.clear();

    // Iterate registry
    const auto& gameObjects = GameObjectRegistry::GetRenderableObjects();
    m_renderPackets.reserve(gameObjects.size());
    for (auto& go : gameObjects) {
        if (!go || !go->IsActive()) continue;

        auto meshComp = go->GetComponent<MeshComponent>();
        if (!meshComp || !meshComp->IsEnabled()) continue;

        MeshAABB      aabb = meshComp->GetAABB();
        Math::Vector3 min  = aabb.min;
        Math::Vector3 max  = aabb.max;
        if (!camera->_aabbInsideFrustum(camera->_extractFrustum(), min, max)) {
            m_drawnCounts.culledFrustum++;
            continue;
        }

        ModelData& meshData = meshComp->modelData;
        if (!meshData.valid) continue;
        if (_ShouldCullBySize(go, camera)) {
            m_drawnCounts.culledSize++;
            continue;
        }

        Mat4 modelMtx =
            go->GetComponent<TransformComponent>()->GetModelMatrix();
        float det       = modelMtx.determinant();
        bool  mirrored  = det < 0.0f;
        Mat4  normalMtx = modelMtx.inverse();
        normalMtx.transpose();

        // Emit one packet per submesh
        for (size_t submeshIdx = 0; submeshIdx < meshData.subMeshes.size();
             ++submeshIdx) {
            const auto& subMesh = meshData.subMeshes[submeshIdx];

            m_renderPackets.push_back(
                { .vbh        = meshData.vbh,
                  .ibh        = meshData.ibh,
                  .modelMtx   = modelMtx,
                  .mirror     = mirrored,
                  .indexStart = subMesh.indexStart,
                  .indexCount = subMesh.indexCount,
                  .material   = &meshData.materials[subMesh.materialIndex],
                  .shader =
                      meshData.materials[subMesh.materialIndex].GetShader(),
                  .go = go });
        }
    }

    // Sort forward packets by shader and material
    std::sort(m_renderPackets.begin(),
              m_renderPackets.end(),
              [](const RenderPacket& a, const RenderPacket& b) {
                  if (a.shader != b.shader) {
                      return std::less<Shader*>{}(a.shader, b.shader);
                  }
                  return std::less<MaterialInstance*>{}(a.material, b.material);
              });

    // Include billboards too
    auto billboardGameObjects = GameObjectRegistry::GetGameObjectsWithComponent(
        SYN_COMPONENT_BILLBOARD);
    m_billboardRenderPackets.reserve(billboardGameObjects.size());
    for (auto& go : billboardGameObjects) {
        if (!go || !go->IsActive()) continue;

        auto billboardComp = go->GetComponent<BillboardComponent>();
        if (!billboardComp || !billboardComp->IsEnabled()) continue;

        // Since we can't use _ShouldCullBySize for billboards (they don't have
        // mesh data), we do a simple distance check here and skip if they're
        // too far away to be visible
        const Vector3 camPos = camera->GetPosition();
        const Vector3 goPos =
            go->GetComponent<TransformComponent>()->GetWorldPosition();
        const float distance = (goPos - camPos).length();
        if (distance > m_maxSmallObjDistance) {
            m_drawnCounts.culledSize++;
            continue;
        }

        const Vector3 min = billboardComp->GetMinBounds();
        const Vector3 max = billboardComp->GetMaxBounds();
        if (!camera->_aabbInsideFrustum(camera->_extractFrustum(), min, max)) {
            m_drawnCounts.culledFrustum++;
            continue;
        }

        Mat4 modelMtx =
            go->GetComponent<TransformComponent>()->GetModelMatrix();

        m_billboardRenderPackets.push_back(
            { .vbh        = m_billboardVbh,
              .ibh        = m_billboardIbh,
              .modelMtx   = modelMtx,
              .mirror     = false,
              .indexStart = 0,
              .indexCount = 6,
              .material   = &billboardComp->_GetMaterial(),
              .shader     = billboardComp->_GetMaterial().GetShader(),
              .go         = go });
    }
}

void RenderDirector::_ScreenSpaceQuad(ViewID view, const Shader* program) {
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_CULL_CW);

    bgfx::setVertexBuffer(0, m_fsQuadVbh);
    Renderer::_UpdateDrawID();
    bgfx::submit(view, program->m_program);
}

/*
--- Drawing functions ---
*/

void RenderDirector::_DrawShadows(const Shader*    program,
                                  CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    const uint64_t renderState =
        BGFX_STATE_DEFAULT &
        ~BGFX_STATE_WRITE_RGB; // Don't write color, only depth

    DirectionalLightComponent* lightSrc =
        GameObjectRegistry::GetFirstActiveDirectionalLight();
    if (!lightSrc) return;

    if (Core::_GetContext()->debug.CSMBounds) {
        if (Renderer::m_pseudoCamera) camera = Renderer::m_pseudoCamera;
        std::array<Math::Matrix4x4, NUM_CASCADES> view;
        std::array<Math::Matrix4x4, NUM_CASCADES> proj;
        Math::Vector4                             outCascadeSplits;
        _CalculateCascadeMatrices(
            camera, lightSrc, view, proj, outCascadeSplits);
        for (int i = 0; i < NUM_CASCADES; ++i) {
            Core::_GetContext()->physicsManager->_DrawFrustum(view[i], proj[i]);
        }
    }

    const auto& gameObjects = GameObjectRegistry::GetRenderableObjects();

    for (uint8_t cascade = 0; cascade < NUM_CASCADES; ++cascade) {
        auto cascadeFrustum = _GetCascadeFrustum(cascade, camera, lightSrc);

        bgfx::setViewName(program->m_viewId + cascade, "Shadow Cascade");
        bgfx::touch(program->m_viewId + cascade);
        for (const auto& gameObject : gameObjects) {
            if (!gameObject) continue;

            auto* meshComp = gameObject->GetComponent<MeshComponent>();
            const ModelData& modelData = meshComp->modelData;

            if (!modelData.valid || !meshComp->IsEnabled() ||
                !meshComp->castShadows)
                continue;

            const MeshAABB& aabb = meshComp->GetAABB();
            Math::Vector3   min  = aabb.min;
            Math::Vector3   max  = aabb.max;
            if (!camera->_aabbInsideFrustum(cascadeFrustum, min, max)) {
                m_drawnCounts.culledShadowFrustum++;
                continue;
            }

            // Size-based shadow culling from light's perspective
            if (_ShouldCullBySizeShadow(
                    gameObject, camera, cascade, lightSrc)) {
                m_drawnCounts.culledShadowSize++;
                continue;
            }

            bgfx::setState(renderState);

            // Get the transform for this object
            Mat4 modelMtx = gameObject->GetComponent<TransformComponent>()
                                ->GetModelMatrix();
            bgfx::setTransform(modelMtx.data());

            bgfx::setVertexBuffer(0, modelData.vbh);
            bgfx::setIndexBuffer(modelData.ibh);

            // Shadow shaders are simple, just output depth
            Renderer::_UpdateDrawID();
            bgfx::submit(program->m_viewId + cascade, program->m_program);
            m_drawnCounts.shadows++;
        }
    }
}

void RenderDirector::_DrawSky(const Shader*          program,
                              const CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewName(program->m_viewId, "Sky");
    bgfx::setViewFrameBuffer(program->m_viewId, m_buffers.sceneFB);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_ALWAYS |
                   BGFX_STATE_MSAA | BGFX_STATE_CULL_CW);

    _SetFrameUniforms(program);

    bgfx::setVertexBuffer(0, m_fsQuadVbh);
    Renderer::_UpdateDrawID();
    bgfx::submit(program->m_viewId, program->m_program);
}

void RenderDirector::_DrawForward(const Shader*    program,
                                  CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewName(program->m_viewId, "Forward");
    bgfx::setViewFrameBuffer(program->m_viewId, m_buffers.sceneFB);

    const uint64_t renderState = BGFX_STATE_DEFAULT | BGFX_STATE_MSAA;

    _SetFrameUniforms(program);
    _SetViewUniforms(program);

    for (auto& packet : m_renderPackets) {
        if (program->m_program.idx != packet.shader->m_program.idx)
            continue; // Skip if not matching program

        bgfx::setState(renderState | (packet.mirror ? BGFX_STATE_CULL_CW
                                                    : BGFX_STATE_FRONT_CCW));

        _SetObjectUniforms(program, packet);
        _SetMaterialUniforms(program, packet);

        bgfx::setTransform(packet.modelMtx.data());
        bgfx::setVertexBuffer(0, packet.vbh);
        bgfx::setIndexBuffer(packet.ibh, packet.indexStart, packet.indexCount);

        Renderer::_UpdateDrawID();
        bgfx::submit(program->m_viewId, program->m_program);
        m_drawnCounts.forward++;
    }
}

void RenderDirector::_DrawDebug(const Shader*    program,
                                CameraComponent* camera,
                                DebugModes       debug) {
    SYN_PROFILE_FUNCTION();

    bgfx::setViewName(program->m_viewId, "Debug");
    bgfx::setViewFrameBuffer(program->m_viewId, m_buffers.sceneFB);

    // Draw various debug overlays
    if (debug.Gizmos && m_drender) {
        // Draw zone boundaries

        SYN_PROFILE_SCOPE("Draw Zones");
        for (std::vector<ZoneComponent*> zones =
                 Core::_GetContext()->ZoneSystem->GetZones();
             auto zone : zones) {
            if (!zone || !zone->IsEnabled()) continue;
            switch (zone->GetShape()) {
            case ZoneShape::BOX: {
                Vector3 pos  = zone->GetPosition();
                Vector3 size = zone->GetSize();
                Vector3 min, max;
                for (int i = 0; i < 3; ++i) {
                    min.set(i, pos[i] - size[i] * 0.5f);
                    max.set(i, pos[i] + size[i] * 0.5f);
                }

                m_drender->DrawBox(min, max, JPH::Color::sRed);
                break;
            }
            case ZoneShape::SPHERE: {
                Vector3 pos  = zone->GetPosition();
                Vector3 size = zone->GetSize(); // size.x is radius
                m_drender->DrawSphere(
                    pos.toJoltRVec3(), size[0], JPH::Color::sRed);
                break;
            }
            }
        }
    }

    if (debug.DrawBoundingBoxes && m_drender) {
        SYN_PROFILE_SCOPE("Draw AABBs");
        std::vector<GameObject*> meshObjects =
            GameObjectRegistry::GetGameObjectsWithComponent(SYN_COMPONENT_MESH);
        for (auto go : meshObjects) {
            MeshAABB aabb = go->GetComponent<MeshComponent>()->GetAABB();
            m_drender->DrawBox(aabb.min, aabb.max, JPH::Color::sGreen);
        }

        std::vector<GameObject*> billboards =
            GameObjectRegistry::GetGameObjectsWithComponent(
                SYN_COMPONENT_BILLBOARD);
        for (auto go : billboards) {
            BillboardComponent* comp = go->GetComponent<BillboardComponent>();
            if (!comp) continue;
            Vector3 goPos =
                go->GetComponent<TransformComponent>()->GetWorldPosition();
            Vector3 min = comp->GetMinBounds();
            Vector3 max = comp->GetMaxBounds();
            m_drender->DrawBox(min, max, JPH::Color::sBlue);
        }
    }

    // Flush all queued debug lines (physics wireframes, frustums, CSM
    // lines, zone bounds, and AABBs) in a single pass.
    // TODO: Rework this to draw camera frustums as gizmos instead of
    // hardcoded
    GameObject* p = GameObjectRegistry::GetGameObjectByName("player");
    if (p && Core::IsPhysicsEnabled()) {
        CameraComponent* playerCamera = p->GetComponent<CameraComponent>();
        if (!playerCamera) {
            return;
        }

        // Keep player camera matrices valid even when simulation is paused.
        const int safeWidth  = std::max(Renderer::width, 1);
        const int safeHeight = std::max(Renderer::height, 1);
        playerCamera->Update(VIEW_DEBUG, safeWidth, safeHeight);

        Core::_GetContext()->physicsManager->_DrawDebug(
            Renderer::width,
            Renderer::height,
            program->m_program,
            playerCamera->GetCamera(),
            camera->GetCamera(),
            debug);
    }
}

void RenderDirector::_DrawBillboard(const Shader*    program,
                                    CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewName(program->m_viewId, "Billboards");
    bgfx::setViewFrameBuffer(program->m_viewId, m_buffers.sceneFB);
    const uint64_t renderState = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                                 BGFX_STATE_WRITE_Z |
                                 BGFX_STATE_DEPTH_TEST_LEQUAL | BGFX_STATE_MSAA;

    _SetFrameUniforms(program);
    _SetViewUniforms(program);

    // Draw billboards
    for (auto packet : m_billboardRenderPackets) {
        bgfx::setState(renderState);
        _SetObjectUniforms(program, packet);
        _SetMaterialUniforms(program, packet);

        bgfx::setTransform(packet.modelMtx.data());
        bgfx::setVertexBuffer(0, packet.vbh);
        bgfx::setIndexBuffer(packet.ibh, packet.indexStart, packet.indexCount);
        Renderer::_UpdateDrawID();
        bgfx::submit(program->m_viewId, program->m_program);
        m_drawnCounts.billboard++;
    }
}

void RenderDirector::_DrawSSAO(const Shader* program) {
    SYN_PROFILE_FUNCTION();
    if (!m_config.useSSAO) return;
    uint64_t samplerFlags = BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                            BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

    if (program->m_program.idx == m_ssaoProgram->m_program.idx) {
        _SetFrameUniforms(program);
        _SetViewUniforms(program);
        bgfx::setViewName(VIEW_AO, "SSAO Main");
        bgfx::setViewFrameBuffer(VIEW_AO, m_buffers.ssaoFB);
        bgfx::setTexture(
            0, m_ssao_depthTex, m_buffers.sceneDepth, samplerFlags);
        bgfx::setTexture(
            1, m_ssao_normalTex, m_buffers.sceneNormal, samplerFlags);

        _ScreenSpaceQuad(VIEW_AO, program);
    } else {
        for (int i = 0; i < 2; ++i) {
            _SetFrameUniforms(program, &i);
            _SetViewUniforms(program);
            if (i == 0) { // Horizontal blur
                bgfx::setViewName(ViewID(VIEW_AO + 1), "SSAO Blur H");
                bgfx::setViewFrameBuffer(VIEW_AO + 1, m_buffers.ssaoBlurHFB);
                bgfx::setTexture(
                    0, m_ssaob_ssaoTex, m_buffers.ssaoTex, samplerFlags);
            } else { // Vertical blur
                bgfx::setViewName(ViewID(VIEW_AO + 2), "SSAO Blur V");
                bgfx::setViewFrameBuffer(VIEW_AO + 2, m_buffers.ssaoBlurVFB);
                bgfx::setTexture(
                    0, m_ssaob_ssaoTex, m_buffers.ssaoBlurH, samplerFlags);
            }

            bgfx::setTexture(
                1, m_ssao_depthTex, m_buffers.sceneDepth, samplerFlags);
            bgfx::setTexture(
                2, m_ssao_normalTex, m_buffers.sceneNormal, samplerFlags);
            _ScreenSpaceQuad(ViewID(VIEW_AO + i + 1), program);
        }
    }
}

void RenderDirector::_DrawPostProcess(const Shader* program) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewName(program->m_viewId, "PostProcess");
    bgfx::setViewFrameBuffer(VIEW_POSTPROCESS,
                             BGFX_INVALID_HANDLE); // Backbuffer
    bgfx::setTexture(0, m_tonemap_sceneTex, m_buffers.sceneColor);
    bgfx::setTexture(1, m_tonemap_ssaoTex, m_buffers.ssaoBlurFinal);
    _ScreenSpaceQuad(VIEW_POSTPROCESS, program);
}

void RenderDirector::_DrawDbgBillboard(Shader* program) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewName(VIEW_BILL_DBG, "Gizmos");
    bgfx::setViewFrameBuffer(VIEW_BILL_DBG, m_buffers.sceneFB);
    // Debug billboards (gizmos) are always drawn on top. Regular billboards
    // (forward pass ig) are depth-tested normally.
    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_ALPHA |
                     BGFX_STATE_DEPTH_TEST_ALWAYS;

    _SetFrameUniforms(program);
    _SetViewUniforms(program);

    // Draw all gizmos
    std::vector<GameObject*> gizmos = GameObjectRegistry::GetGizmos();

    // Build render packets for gizmos and draw them
    std::vector<Renderer::RenderPacket> packets;
    for (auto go : gizmos) {
        auto it = Renderer::m_gizmoRegistry.find(go->gizmo);
        if (it == Renderer::m_gizmoRegistry.end()) continue;
        Renderer::RenderPacket packet;
        BillboardComponent*    gizmo = it->second;
        packet.vbh                   = m_billboardVbh;
        packet.ibh                   = m_billboardIbh;
        packet.material              = &gizmo->_GetMaterial();
        packet.shader                = packet.material->GetShader();
        packet.modelMtx =
            go->GetComponent<TransformComponent>()->GetModelMatrix();
        packet.go = go;
        packets.push_back(packet);
    }

    for (auto& packet : packets) {
        _SetObjectUniforms(program, packet);
        _SetMaterialUniforms(program, packet);
        bgfx::setState(state);

        bgfx::setTransform(packet.modelMtx.data());
        bgfx::setVertexBuffer(0, m_billboardVbh);
        bgfx::setIndexBuffer(m_billboardIbh);
        Renderer::_UpdateDrawID();
        bgfx::submit(VIEW_BILL_DBG, program->m_program);
    }
}

void RenderDirector::_DrawUIDebug(CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewName(VIEW_UI_DEBUG, "UI Debug");
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::dbgTextClear();

    int width   = Renderer::width;
    int height  = Renderer::height;
    int maxCols = width / 8;   // Assuming each character is 8 pixels wide
    int maxRows = height / 16; // Assuming each character is 16 pixels tall

    bgfx::dbgTextPrintf(
        1, maxRows - 2, 0x0C, "Syngine v%s", SYN_VERSION_STRING);
    bgfx::dbgTextPrintf(
        1, maxRows - 1, 0x0C, "FOR INTERNAL USE ONLY - NOT FOR PUBLIC RELEASE");
}

bool RenderDirector::_PrepareRenderViews(CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    // Prepare camera and light information
    if (!camera) {
        Syngine::Logger::Fatal("No camera provided to render frame");
        return false;
    }

    const int safeWidth  = std::max(Renderer::width, 1);
    const int safeHeight = std::max(Renderer::height, 1);
    camera->Update(VIEW_FORWARD, safeWidth, safeHeight);
    Camera cam = camera->GetCamera();

    std::array<Math::Matrix4x4, NUM_CASCADES> lightView;
    std::array<Math::Matrix4x4, NUM_CASCADES> lightProj;
    if (m_config.useShadows) {
        // Calculate the cascade matrices for shadow mapping and send to GPU
        Math::Vector4              cascadeSplits;
        DirectionalLightComponent* lightSrc =
            GameObjectRegistry::GetFirstActiveDirectionalLight();
        _CalculateCascadeMatrices(
            camera, lightSrc, lightView, lightProj, cascadeSplits);
        m_csmCascadeSplits = cascadeSplits;
    }

    // Update main camera matrices
    for (Syngine::ViewID view : _allViews) {
        switch (view) {
        case ViewID::VIEW_SHADOW: {
            if (!m_config.useShadows) break; // Skip if shadows are disabled

            for (uint8_t i = 0; i < NUM_CASCADES; ++i) {
                bgfx::ViewId cascadeViewId = view + i;
                bgfx::setViewFrameBuffer(cascadeViewId, m_buffers.shadowFB);
                uint16_t x = (i % 2) * SHADOW_MAP_SIZE;
                uint16_t y = (i / 2) * SHADOW_MAP_SIZE;
                bgfx::setViewRect(
                    cascadeViewId, x, y, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
                bgfx::setViewScissor(
                    cascadeViewId, x, y, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
                bgfx::setViewClear(
                    cascadeViewId, BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
                bgfx::setViewTransform(
                    cascadeViewId, lightView[i].data(), lightProj[i].data());
            }

            break;
        }
        case ViewID::VIEW_SKY: {
            bgfx::setViewRect(view,
                              0,
                              0,
                              uint16_t(Renderer::width),
                              uint16_t(Renderer::height));
            bgfx::setViewClear(
                view, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);

            camera->Update(view, Renderer::width, Renderer::height);
            Camera cam = camera->GetCamera();
            bgfx::setViewTransform(view, cam.view.data(), cam.proj.data());
            break;
        }
        case VIEW_AO:
        case VIEW_POSTPROCESS: {
            bgfx::setViewRect(view,
                              0,
                              0,
                              uint16_t(view == VIEW_AO ? Renderer::width / 2
                                                       : Renderer::width),
                              uint16_t(view == VIEW_AO ? Renderer::height / 2
                                                       : Renderer::height));
            uint16_t flags = view == VIEW_AO
                                 ? (BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH)
                                 : (BGFX_CLEAR_NONE);
            bgfx::setViewClear(view, flags, 0x000000ff, 1.0f, 0);
            if (view == VIEW_AO) {
                bgfx::setViewTransform(view, cam.view.data(), cam.proj.data());
            } else {
                Math::Mat4 identity, orthoProj;
                bx::mtxOrtho(orthoProj.data(),
                             0.0f,
                             1.0f,
                             1.0f,
                             0.0f,
                             0.0f,
                             1.0f,
                             0.0f,
                             bgfx::getCaps()->homogeneousDepth);
                bgfx::setViewTransform(view, identity.data(), orthoProj.data());
            }
            break;
        }
        default: {
            bgfx::setViewRect(view,
                              0,
                              0,
                              uint16_t(Renderer::width),
                              uint16_t(Renderer::height));
            // bgfx::setViewClear(
            //     view, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
            //     0x000000ff, 1.0f, 0);
            bgfx::setViewTransform(view, cam.view.data(), cam.proj.data());
            break;
        }
        }
    }

    // Update additional AO views
    uint16_t flags = BGFX_CLEAR_NONE;
    for (int i = 0; i < 2; ++i) {
        bgfx::setViewRect(ViewID(VIEW_AO + i + 1),
                          0,
                          0,
                          uint16_t(Renderer::width / 2),
                          uint16_t(Renderer::height / 2));
        bgfx::setViewClear(ViewID(VIEW_AO + i + 1), flags, 0x000000ff, 1.0f, 0);
        bgfx::setViewTransform(
            ViewID(VIEW_AO + i + 1), cam.view.data(), cam.proj.data());
    }

    m_drawnCounts = DrawnObjectCount(); // Reset counts for this frame
    return true;
}

// MARK: Uniform Uploading

void RenderDirector::_SetFrameUniforms(const Shader* shader, const void* ctx) {
    SYN_PROFILE_FUNCTION();
    for (const auto& uniform : shader->m_frameUniforms) {
        bgfx::setUniform(uniform.handle, uniform.getter(ctx), uniform.count);
    }
}

void RenderDirector::_SetViewUniforms(const Shader* shader, const void* ctx) {
    SYN_PROFILE_FUNCTION();
    for (const auto& uniform : shader->m_viewUniforms) {
        bgfx::setUniform(uniform.handle, uniform.getter(ctx), uniform.count);
    }
}

void RenderDirector::_SetObjectUniforms(const Shader*                 shader,
                                        const Renderer::RenderPacket& packet) {
    SYN_PROFILE_FUNCTION();
    for (const auto& uniform : shader->m_drawUniforms) {
        bgfx::setUniform(
            uniform.handle, uniform.getter(&packet), uniform.count);
    }
}

void RenderDirector::_SetMaterialUniforms(const Shader*                 shader,
                                          const Renderer::RenderPacket& packet,
                                          uint32_t                      flags) {
    SYN_PROFILE_FUNCTION();

    // Bind material paramters
    if (!packet.material || !packet.material->m_material) return;
    for (size_t i = 0; i < shader->m_materialParams.size(); ++i) {
        const auto& shaderDesc = shader->m_materialParams[i];
        const auto& override   = packet.material->m_parameterOverrides[i];
        const auto& matData =
            override ? override->storage
                     : packet.material->m_material->m_parameters[i].storage;
        bgfx::setUniform(shaderDesc.handle, matData.data(), shaderDesc.count);
    }

    for (const auto& uniform : shader->m_engineSamplers) {
        const bgfx::TextureHandle* texHandle =
            static_cast<const bgfx::TextureHandle*>(uniform.getter(nullptr));
        bgfx::setTexture(uniform.stage,
                         uniform.handle,
                         *texHandle,
                         BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                             BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_U_CLAMP |
                             BGFX_SAMPLER_V_CLAMP);
    }

    // Bind material textures
    for (const auto& uniform : shader->m_textureParams) {
        auto it = std::find_if(packet.material->m_material->m_textures.begin(),
                               packet.material->m_material->m_textures.end(),
                               [&](const TextureParameter& texture) {
                                   return texture.name == uniform.name;
                               });
        if (it == packet.material->m_material->m_textures.end()) continue;
        const size_t index = static_cast<size_t>(
            it - packet.material->m_material->m_textures.begin());
        const auto& texture = packet.material->m_textureOverrides[index]
                                  ? *packet.material->m_textureOverrides[index]
                                  : *it;
        const bgfx::TextureHandle texHandle = texture.handle;
        const uint32_t            realFlags = flags | texture.samplerFlags;
        bgfx::setTexture(uniform.stage, uniform.handle, texHandle, realFlags);
    }
}

// MARK: Main Render Loop

bool RenderDirector::_RenderFrame(CameraComponent* camera, DebugModes debug) {
    SYN_PROFILE_FUNCTION();
    if (m_isFirstFrame) {
        m_isFirstFrame = false;
        if (Core::IsPhysicsEnabled())
            m_drender =
                Core::_GetContext()->physicsManager->_GetDebugRenderer();
    }

    if (!camera) {
        Syngine::Logger::Fatal("No camera provided to render frame");
        return false;
    }

    if (m_changeResolutionThisFrame) {
        m_changeResolutionThisFrame = false;
        if (!_SetResolution(m_requestedWidth, m_requestedHeight)) {
            Renderer::_UpdateDrawID();
            return false;
        }
    }

    if (m_changeVsyncThisFrame) {
        m_changeVsyncThisFrame = false;
        if (!_SetVsync()) {
            Renderer::_UpdateDrawID();
            return false;
        }
    }

    if (!_PrepareRenderViews(camera)) {
        Renderer::_UpdateDrawID();
        return false;
    }
    _CollectRenderPackets(camera);

    // Main render loop
    for (auto view : _allViews) {
        auto progList = ShaderManager::GetProgramsByViewID(view);

        for (auto& program : progList) {
            if (!bgfx::isValid(program->m_program)) continue;

            // Draw logic based on view type
            switch (view) {
            case VIEW_SHADOW:
                if (m_config.useShadows) {
                    _DrawShadows(program, camera);
                }
                break;
            case VIEW_SKY: _DrawSky(program, camera); break;
            case VIEW_FORWARD: _DrawForward(program, camera); break;
            case VIEW_DEBUG:
                if (debug.Enabled) _DrawDebug(program, camera, debug);
                break;
            case VIEW_BILL_DBG:
                // Debug billboard draws are submitted from VIEW_BILLBOARD
                // using this view ID.
                break;
            case VIEW_BILLBOARD:
                if (debug.Enabled && debug.Gizmos) _DrawDbgBillboard(program);
                _DrawBillboard(program, camera);
                break;
            case VIEW_POSTPROCESS: _DrawPostProcess(program); break;
            case VIEW_AO: _DrawSSAO(program); break;
            default: break;
            }
        }
    }

#ifndef NDEBUG
    _DrawUIDebug(camera);
#endif

    bgfx::frame();
    Renderer::_UpdateDrawID();
    return true;
}

} // namespace Syngine
