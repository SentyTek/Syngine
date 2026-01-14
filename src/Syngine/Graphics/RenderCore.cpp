// ╒═════════════════════ RenderCore.cpp ═╕
// │ Syngine                              │
// │ Created 2026-01-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Graphics/RenderCore.h"
#include "Syngine/Core/Core.h"
#include "Syngine/Core/ZoneManager.h"
#include "Syngine/ECS/Components/MeshComponent.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Graphics/DebugRenderer.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/Graphics/Windowing.h"
#include "Syngine/Graphics/TextureHelpers.h"
#include "Syngine/Utils/Version.h"
#include "Syngine/Utils/Profiler.h"
#include "Syngine/Core/Logger.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/math.h"
#include <cstddef>
#include <vector>

#define SYN_INT_RENDEREXIT(program)                                            \
    Syngine::Logger::Error("Failed to create " #program " program");           \
    bgfx::shutdown();                                                          \
    SDL_DestroyWindow(win);                                                    \
    SDL_Quit();                                                                \
    return false;

namespace Syngine {

// I present to you an unholy abomination of static member definitions
std::unordered_map<std::string, uint16_t> RenderCore::m_defaultUniformIds;

bgfx::TextureHandle       RenderCore::m_shadowDepth = BGFX_INVALID_HANDLE;
bgfx::FrameBufferHandle   RenderCore::m_shadowFB    = BGFX_INVALID_HANDLE;

RendererConfig RenderCore::m_config;
SDL_Window*    RenderCore::win = nullptr;

DebugRender* RenderCore::m_drender = nullptr;
bool         RenderCore::m_isFirstFrame = true;
float RenderCore::m_cascadeSizes[RenderCore::NUM_CASCADES] = { 10, 40, 0, 0 };
float RenderCore::m_cascadeTexelSizes[RenderCore::NUM_CASCADES] = { 0, 0, 0, 0 };

bgfx::VertexBufferHandle RenderCore::dummy = BGFX_INVALID_HANDLE;
bgfx::VertexBufferHandle RenderCore::m_billboardVbh = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle  RenderCore::m_billboardIbh = BGFX_INVALID_HANDLE;

std::unordered_map<uint16_t, Uniform> Renderer::m_uniformRegistry;
std::unordered_map<uint16_t, std::vector<Program>> Renderer::viewPrograms;
RenderCore::DrawnObjectCount                       RenderCore::m_drawnCounts;
float RenderCore::m_maxSmallObjDistance =
    50.0f; //* Small objects get culled beyond this distance

RenderCore::internalPrograms RenderCore::m_internalPrograms;
std::vector<RenderCore::RenderPacket> RenderCore::m_renderPackets;
bgfx::FrameBufferHandle               RenderCore::m_sceneFB = BGFX_INVALID_HANDLE;
bgfx::TextureHandle                   RenderCore::m_sceneColor = BGFX_INVALID_HANDLE;
bgfx::TextureHandle                   RenderCore::m_ssaoNoiseTex = BGFX_INVALID_HANDLE;
bgfx::TextureHandle                   RenderCore::m_sceneDepth = BGFX_INVALID_HANDLE;
bgfx::TextureHandle                   RenderCore::m_sceneNormal = BGFX_INVALID_HANDLE;
bgfx::FrameBufferHandle               RenderCore::m_ssaoFB = BGFX_INVALID_HANDLE;
bgfx::TextureHandle                   RenderCore::m_ssaoTex = BGFX_INVALID_HANDLE;
bgfx::VertexBufferHandle              RenderCore::m_fsQuadVbh = BGFX_INVALID_HANDLE;

bool RenderCore::_Initialize(const RendererConfig& config) {
    m_config = config;
    win = Window::_GetSDLWindow();
    if (!win) {
        Syngine::Logger::Fatal("No window to create renderer in");
        return false;
    }
    
    // Initialize bgfx
    bgfx::Init bgInit;
    SDL_PropertiesID sdlProps = SDL_GetWindowProperties(win);
    if(sdlProps == 0) {
        Syngine::Logger::Error("Failed to get window properties");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
    
    // Set platform data
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    // This assumes X11
    bgInit.platformData.ndt = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
    bgInit.platformData.nwh = (void*)SDL_GetNumberProperty(sdlProps, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    if (bgInit.platformData.nwh == NULL || bgInit.platformData.ndt == NULL) {
        Syngine::Logger::Error("Failed to get window properties for Linux X11");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
#elif BX_PLATFORM_OSX
    bgInit.type = bgfx::RendererType::Metal; // Set renderer type to Metal
    bgInit.platformData.ndt = NULL; // Only needed on x11
    bgInit.platformData.nwh = _GetSYNMetalView(win); // Custom obj-c function to get the metal view
    if (bgInit.platformData.nwh == NULL) {
        Syngine::Logger::Error("Failed to get window properties for macOS");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
#elif BX_PLATFORM_WINDOWS
    bgInit.type = bgfx::RendererType::Direct3D12;
    bgInit.platformData.ndt = NULL; // Only needed on x11
    bgInit.platformData.nwh = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
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
    
    bgInit.resolution.width = Renderer::width;
    bgInit.resolution.height = Renderer::height;
    bgInit.resolution.reset |= BGFX_RESET_VSYNC; // Enable vsync
    bgInit.debug = true;

    if(!bgfx::init(bgInit)) {
        Syngine::Logger::Error("Failed to initialize bgfx");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
    
    // Reset view 0 to the dimensions of the window and clear it
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 0, 1, 0);
    bgfx::setViewRect(0, 0, 0, uint16_t(Renderer::width), uint16_t(Renderer::height));
    
    bgfx::touch(0); // touch the view to clear it
    bgfx::frame(); // submit the frame
    SDL_ShowWindow(win); // show the window

    // Create default shaders
    Renderer::m_isReady =
        true; // Teehee. Temporarily set to true to allow shader loading.

    m_internalPrograms.skyProgram =
        Renderer::AddProgram("shaders/default_sky", "skybox", VIEW_SKY);
    if (m_internalPrograms.skyProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("skybox")
    }
    
    m_internalPrograms.defaultProgram = Renderer::AddProgram("shaders/default", "default");
    if (m_internalPrograms.defaultProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("default")
    }

    m_internalPrograms.textureProgram = Renderer::AddProgram("shaders/default_texture", "texture");
    if (m_internalPrograms.textureProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("texture")
    }

    m_internalPrograms.debugProgram =
        Renderer::AddProgram("shaders/default_debug", "default_debugger", VIEW_DEBUG);
    if (m_internalPrograms.debugProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("debugger")
    }
    
    m_internalPrograms.billboardProgram =
        Renderer::AddProgram("shaders/default_billboard", "billboard", VIEW_BILLBOARD);
    if (m_internalPrograms.billboardProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("billboard")
    }

    m_internalPrograms.shadowProgram =
        Renderer::AddProgram("shaders/default_shadow", "shadow", VIEW_SHADOW);
    if (m_internalPrograms.shadowProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("shadow")
    }

    m_internalPrograms.ssaoProgram =
        Renderer::AddProgram("shaders/ssao", "ssao", VIEW_POSTPROCESS);
    if (m_internalPrograms.ssaoProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("ssao")
    }

    m_internalPrograms.tonemapProgram = Renderer::AddProgram(
        "shaders/tonemapping", "tonemapping", VIEW_POSTPROCESS);
    if (m_internalPrograms.tonemapProgram == (size_t)-1) {
        SYN_INT_RENDEREXIT("tonemapping")
    }
    Renderer::m_isReady = false;

    // Create default uniforms
    {
    // Billboard program uniforms
    m_defaultUniformIds.insert({ "u_billboard",
                                 Renderer::RegisterUniform(m_internalPrograms.billboardProgram,
                                                 "u_default_billboard",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "s_bill_albedo",
          Renderer::RegisterUniform(
              m_internalPrograms.billboardProgram, "s_albedo", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert({ "u_billboard_mode",
                                 Renderer::RegisterUniform(m_internalPrograms.billboardProgram,
                                                 "u_default_billboard_mode",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_billboard_lighting",
                                 Renderer::RegisterUniform(m_internalPrograms.billboardProgram,
                                                 "u_billboard_lighting",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "s_billboard_shadowMap",
          Renderer::RegisterUniform(
              m_internalPrograms.billboardProgram, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });

    // Vertex program uniforms
    m_defaultUniformIds.insert({ "u_baseColor",
                                 Renderer::RegisterUniform(m_internalPrograms.defaultProgram,
                                                 "u_baseColor",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_lightDir",
                                 Renderer::RegisterUniform(m_internalPrograms.defaultProgram,
                                                 "u_lightDir",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_default_normalMatrix",
          Renderer::RegisterUniform(
              m_internalPrograms.defaultProgram, "u_normalMatrix", UniformType::UNIFORM_MAT3) });
    m_defaultUniformIds.insert(
        { "u_default_floats",
          Renderer::RegisterUniform(
              m_internalPrograms.defaultProgram, "u_floats", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_default_skyColor",
          Renderer::RegisterUniform(
              m_internalPrograms.skyProgram, "u_skyColor", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_default_sunColor",
          Renderer::RegisterUniform(
              m_internalPrograms.skyProgram, "u_sunColor", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_default_horizonColor",
          Renderer::RegisterUniform(
              m_internalPrograms.skyProgram, "u_horizonColor", UniformType::UNIFORM_VEC4) });
    
    m_defaultUniformIds.insert(
        { "u_default_shadowMap",
          Renderer::RegisterUniform(
              m_internalPrograms.defaultProgram, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_default_useVertex",
          Renderer::RegisterUniform(
              m_internalPrograms.defaultProgram, "u_useVertexColor", UniformType::UNIFORM_VEC4) });
    // Sky program uniforms
    m_defaultUniformIds.insert(
        { "u_skyColorZenith",
          Renderer::RegisterUniform(
              m_internalPrograms.skyProgram, "u_skyColorZenith", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_skyColorMidnight",
          Renderer::RegisterUniform(
              m_internalPrograms.skyProgram, "u_skyColorMidnight", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_sky_sunColor",
          Renderer::RegisterUniform(
              m_internalPrograms.skyProgram, "u_sunColor", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_sky_scatterColor",
          Renderer::RegisterUniform(
              m_internalPrograms.skyProgram, "u_scatterColor", UniformType::UNIFORM_VEC4) });

    // Texture program uniforms
    m_defaultUniformIds.insert({ "u_texture_lightDir",
                                 Renderer::RegisterUniform(m_internalPrograms.textureProgram,
                                                 "u_lightDir",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_floats",
                                 Renderer::RegisterUniform(m_internalPrograms.textureProgram,
                                                 "u_floats",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_normalMatrix",
                                 Renderer::RegisterUniform(m_internalPrograms.textureProgram,
                                                 "u_normalMatrix",
                                                 UniformType::UNIFORM_MAT3) });

    m_defaultUniformIds.insert(
        { "u_texture_albedo",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "s_albedo", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_normalMap",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "s_normalMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_heightMap",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "s_heightMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_texture_shadowMap",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });

    m_defaultUniformIds.insert(
        { "u_texture_skyColor",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "u_skyColor", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_texture_sunColor",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "u_sunColor", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_texture_horizonColor",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "u_horizonColor", UniformType::UNIFORM_VEC4) });

    // Shadow program uniforms
    m_defaultUniformIds.insert({ "u_default_csmSplits",
                                 Renderer::RegisterUniform(m_internalPrograms.defaultProgram,
                                                 "u_csmSplits",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_csmLightViewProj",
                                 Renderer::RegisterUniform(m_internalPrograms.defaultProgram,
                                                 "u_csmLightViewProj",
                                                 UniformType::UNIFORM_MAT4, 4) });
    m_defaultUniformIds.insert({ "u_texture_csmSplits",
                                 Renderer::RegisterUniform(m_internalPrograms.textureProgram,
                                                 "u_csmSplits",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_csmLightViewProj",
                                 Renderer::RegisterUniform(m_internalPrograms.textureProgram,
                                                 "u_csmLightViewProj",
                                                 UniformType::UNIFORM_MAT4,
                                                 4) });
    m_defaultUniformIds.insert({ "u_texture_viewPos",
                                 Renderer::RegisterUniform(m_internalPrograms.textureProgram,
                                                 "u_viewPos",
                                                 UniformType::UNIFORM_VEC4) });;
    m_defaultUniformIds.insert({ "u_default_viewPos",
                                 Renderer::RegisterUniform(m_internalPrograms.defaultProgram,
                                                 "u_viewPos",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_shadowParams",
                                 Renderer::RegisterUniform(m_internalPrograms.textureProgram,
                                                 "u_shadowParams",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_default_shadowParams",
          Renderer::RegisterUniform(
              m_internalPrograms.defaultProgram, "u_shadowParams", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_texture_csmTexelSize",
          Renderer::RegisterUniform(
              m_internalPrograms.textureProgram, "u_csmTexelSize", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "u_default_csmTexelSize",
          Renderer::RegisterUniform(
              m_internalPrograms.defaultProgram, "u_csmTexelSize", UniformType::UNIFORM_VEC4) });

    // SSAO program uniforms
    m_defaultUniformIds.insert(
        { "u_ssao_params",
          Renderer::RegisterUniform(
              m_internalPrograms.ssaoProgram, "u_ssaoParams", UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "s_ssao_normalTex",
          Renderer::RegisterUniform(
              m_internalPrograms.ssaoProgram, "s_normal", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "s_ssao_depthTex",
          Renderer::RegisterUniform(
              m_internalPrograms.ssaoProgram, "s_depth", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "s_ssao_noiseTex",
          Renderer::RegisterUniform(
              m_internalPrograms.ssaoProgram, "s_noise", UniformType::UNIFORM_SAMPLER) });

    // Tonemapping program uniforms
    m_defaultUniformIds.insert(
        { "s_tonemap_sceneTex",
          Renderer::RegisterUniform(
              m_internalPrograms.tonemapProgram, "s_sceneTex", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "s_tonemap_ssaoTex",
          Renderer::RegisterUniform(
              m_internalPrograms.tonemapProgram, "s_ssaoTex", UniformType::UNIFORM_SAMPLER) });
    }

    // create billboard buffers
    static const float billboardVertices[] = { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
                                               0.5f,  -0.5f, 0.0f, 1.0f, 1.0f,
                                               0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
                                               -0.5f, 0.5f,  0.0f, 0.0f, 0.0f };
    static const uint16_t billboardIndices[]  = { 0, 1, 2, 0, 2, 3 };

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
        -1.0f,  3.0f, 0.99999f, 0.0f, -1.0f, // Top-Left (extended above)
         3.0f, -1.0f, 0.99999f, 2.0f,  1.0f, // Bottom-Right (extended right)
        -1.0f, -1.0f, 0.99999f, 0.0f,  1.0f  // Bottom-Left
    };
    m_fsQuadVbh                         = bgfx::createVertexBuffer(
        bgfx::copy(fsQuadVertices, sizeof(fsQuadVertices)), fsQuadLayout);
    
    // Dummy buffer to make metal happy
#if BX_PLATFORM_OSX
    static bgfx::VertexBufferHandle fullscreenDummyVBH = BGFX_INVALID_HANDLE;

    if (!bgfx::isValid(fullscreenDummyVBH)) {
        static float dummyData[3] = {0.0f, 0.0f, 0.0f}; // just 1 vertex's worth

        bgfx::VertexLayout dummyLayout;
        dummyLayout.begin()
            .add(bgfx::Attrib::Position, 1, bgfx::AttribType::Float)
            .end();

        fullscreenDummyVBH = bgfx::createVertexBuffer(
            bgfx::copy(dummyData, sizeof(dummyData)), dummyLayout);

        Renderer::dummy = fullscreenDummyVBH;
    }
#endif

    // If shadows are enabled, initialize shadow mapping resources
    if (m_config.useShadows) {
        RenderCore::m_shadowDepth = bgfx::createTexture2D(
            SHADOW_MAP_SIZE * 2,
            SHADOW_MAP_SIZE * 2,
            false,
            1,
            bgfx::TextureFormat::D16,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                BGFX_SAMPLER_U_CLAMP |
                BGFX_SAMPLER_V_CLAMP); // I wish I knew what the flags meant.

        if (!bgfx::isValid(RenderCore::m_shadowDepth)) {
            Syngine::Logger::Error("Failed to create shadow depth texture");
            return false;
        }

        RenderCore::m_shadowFB = bgfx::createFrameBuffer(1, &RenderCore::m_shadowDepth, true);
        if (!bgfx::isValid(RenderCore::m_shadowFB)) {
            Syngine::Logger::Error("Failed to create shadow framebuffer");
            bgfx::destroy(RenderCore::m_shadowDepth);
            return false;
        }

        m_cascadeSizes[2] = round(m_config.shadowDist / 3.0f);
        m_cascadeSizes[3] = round(m_config.shadowDist);
    }

    // Create scene textures
    const uint64_t tsFlags =
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

    m_sceneColor = bgfx::createTexture2D(uint16_t(Renderer::width),
                                         uint16_t(Renderer::height),
                                         false,
                                         1,
                                         bgfx::TextureFormat::BGRA8,
                                         tsFlags | BGFX_TEXTURE_SRGB);
    m_sceneNormal = bgfx::createTexture2D(uint16_t(Renderer::width),
                                          uint16_t(Renderer::height),
                                          false,
                                          1,
                                          bgfx::TextureFormat::RGBA8,
                                          tsFlags | BGFX_TEXTURE_SRGB);
    m_sceneDepth =
        bgfx::createTexture2D(uint16_t(Renderer::width),
                              uint16_t(Renderer::height),
                              false,
                              1,
                              bgfx::TextureFormat::D24S8,
                              BGFX_TEXTURE_RT);
    m_ssaoTex = bgfx::createTexture2D(uint16_t(Renderer::width),
                                      uint16_t(Renderer::height),
                                      false,
                                      1,
                                      bgfx::TextureFormat::R8,
                                      tsFlags);
    m_ssaoFB  = bgfx::createFrameBuffer(1, &m_ssaoTex, true);

    // Create global scene framebuffer (MRT: 0:Color, 1:Normal, Depth)
    bgfx::TextureHandle screenTextures[] = { m_sceneColor, m_sceneNormal, m_sceneDepth };
    m_sceneFB = bgfx::createFrameBuffer(
        BX_COUNTOF(screenTextures), screenTextures, true);

    m_ssaoNoiseTex = Syngine::CreateNoiseTexture(4, 4);

    if (!bgfx::isValid(m_sceneColor) || !bgfx::isValid(m_sceneDepth) ||
        !bgfx::isValid(m_sceneNormal) || !bgfx::isValid(m_ssaoFB) ||
        !bgfx::isValid(m_ssaoNoiseTex)) {
        Syngine::Logger::Error("Failed to create scene textures");
        return false;
    }

    // Set default sky colors
    float skyColorZenith[4] = { 0.529f, 0.808f, 0.922f, 1.0f };
    float skyColorMidnight[4] = { 0.05f, 0.05f, 0.1f, 1.0f };
    float sunColor[4] = { 1.0f, 0.956f, 0.839f, 1.0f };
    float scatterColor[4]     = { 0.8f, 0.5f, 0.3f, 1.0f };
    
    Renderer::SetUniform(m_defaultUniformIds["u_skyColorZenith"], skyColorZenith);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_skyColor"], skyColorZenith);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_skyColor"], skyColorZenith);
    Renderer::SetUniform(m_defaultUniformIds["u_skyColorMidnight"], skyColorMidnight);
    Renderer::SetUniform(m_defaultUniformIds["u_sky_sunColor"], sunColor);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_sunColor"], sunColor);
    Renderer::SetUniform(m_defaultUniformIds["u_default_sunColor"], sunColor);
    Renderer::SetUniform(m_defaultUniformIds["u_sky_scatterColor"], scatterColor);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_horizonColor"], scatterColor);
    Renderer::SetUniform(m_defaultUniformIds["u_default_horizonColor"], scatterColor);

    Window::_SetContextCreated(true);
    
    return true;
}

void RenderCore::_Shutdown() {
    // Destroy billboard buffers
    if(bgfx::isValid(m_billboardVbh)) {
        bgfx::destroy(m_billboardVbh);
        m_billboardVbh = BGFX_INVALID_HANDLE;
    }
    if(bgfx::isValid(m_billboardIbh)) {
        bgfx::destroy(m_billboardIbh);
        m_billboardIbh = BGFX_INVALID_HANDLE;
    }
    if(bgfx::isValid(RenderCore::dummy)) {
        bgfx::destroy(RenderCore::dummy);
        RenderCore::dummy = BGFX_INVALID_HANDLE;
    }

    // Destroy shadow framebuffer and texture
    if (bgfx::isValid(m_shadowFB)) {
        bgfx::destroy(m_shadowFB);
        m_shadowFB = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_shadowDepth)) {
        bgfx::destroy(m_shadowDepth);
        m_shadowDepth = BGFX_INVALID_HANDLE;
    }
    m_defaultUniformIds.clear();

    bgfx::shutdown();
}

void RenderCore::_SetResolution(int width, int height) {
    bgfx::reset(uint32_t(width), uint32_t(height), m_config.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
}

Uniform* RenderCore::_GetDefaultUniform(const std::string& name) {
    auto it = m_defaultUniformIds.find(name);
    if (it != m_defaultUniformIds.end()) {
        return &Renderer::m_uniformRegistry[it->second];
    }
    return nullptr;
}

/*
--- Drawing helpers ---
*/

void RenderCore::_CalculateCascadeMatrices(CameraComponent* camera,
                                         float*           outLightView,
                                         float*           outLightProj,
                                         float*           outCascadeSplits) {
    float lightDir[3];
    Renderer::GetSunDirection(lightDir);
    const bx::Vec3 lightDirVec = bx::normalize(bx::Vec3(lightDir[0], lightDir[1], lightDir[2]));
    
    const float* camPos = camera->GetPosition();

    float cascadeDistances[NUM_CASCADES];
    float lightViewProj[NUM_CASCADES * 16];

    // How far light is from camera
    // Light is always targeting camera, will be some distance away
    float lightDistance = 50.0f;

    bx::Vec3 target = { camPos[0], camPos[1], camPos[2] };
    bx::Vec3 up     = { 0.0f, 1.0f, 0.0f };

    bx::Vec3 zero = { 0.0f, 0.0f, 0.0f };
    float    lightViewRaw[16];
    bx::mtxLookAt(lightViewRaw, zero, lightDirVec);
    
    for (uint32_t i = 0; i < NUM_CASCADES; i++) {
        float size = m_cascadeSizes[i];

        float camPos4[4] = { camPos[0], camPos[1], camPos[2], 1.0f };
        float camPosLightSpace[4];
        bx::vec4MulMtx(camPosLightSpace, camPos4, lightViewRaw);

        float worldPerTexel = (size * 2.0f) / float(SHADOW_MAP_SIZE);
        m_cascadeTexelSizes[i] = worldPerTexel;
        float xSnapped = floor(camPosLightSpace[0] / worldPerTexel) * worldPerTexel;
        float ySnapped =
            floor(camPosLightSpace[1] / worldPerTexel) * worldPerTexel;

        float yOffset = ySnapped - camPosLightSpace[1];
        float xOffset = xSnapped - camPosLightSpace[0];

        bx::Vec3 lightPosSnapped = {
            camPos[0] + lightDirVec.x * lightDistance,
            camPos[1] + lightDirVec.y * lightDistance,
            camPos[2] + lightDirVec.z * lightDistance
        };

        if (Core::_GetApp()->debug.CSMBounds) { 
            // Draw line from light to target
            float from[3] = { lightPosSnapped.x, lightPosSnapped.y, lightPosSnapped.z };
            float to[3] = { target.x, target.y, target.z };
            Core::_GetApp()->physicsManager->_DrawLine(from, to, JPH::Color::sYellow);
        }

        bx::mtxLookAt(&outLightView[i * 16], lightPosSnapped, target);

        // Try and keep tight near/far planes for good precision
        float lightNear = 20.0f;
        float lightFar = m_config.shadowDist + 50.0f; // Some margin

        bx::mtxOrtho(&outLightProj[i * 16],
                     -size + xOffset,
                     size + xOffset,
                     -size + yOffset,
                     size + yOffset,
                     lightNear,
                     lightFar,
                     0.0f,
                     bgfx::getCaps()->homogeneousDepth);

        cascadeDistances[i] = m_cascadeSizes[i] / 2.0f;
        outCascadeSplits[i] = cascadeDistances[i];
        bx::mtxMul(&lightViewProj[i * 16], &outLightView[i * 16], &outLightProj[i * 16]);
    }
    
    Renderer::SetUniform(m_defaultUniformIds["u_default_csmLightViewProj"], lightViewProj, NUM_CASCADES);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_csmLightViewProj"], lightViewProj, NUM_CASCADES);
}

CameraComponent::Frustum RenderCore::_GetCascadeFrustum(uint8_t cascade, CameraComponent* camera) {
    CameraComponent::Frustum cascadeFrustum;
    float lightView[16 * NUM_CASCADES], lightProj[16 * NUM_CASCADES], outCascadeSplits[NUM_CASCADES];
    _CalculateCascadeMatrices(camera, lightView, lightProj, outCascadeSplits);
    
    // Get the view-projection matrix for this cascade
    float* cascadeView = &lightView[cascade * 16];
    float* cascadeProj = &lightProj[cascade * 16];
    float cascadeViewProj[16];
    bx::mtxMul(cascadeViewProj, cascadeView, cascadeProj);
    
    // Extract frustum planes from view-projection matrix
    // For orthographic projection, we extract planes differently than perspective
    
    // Left plane: row4 + row1
    cascadeFrustum.left.normal[0] = cascadeViewProj[3] + cascadeViewProj[0];
    cascadeFrustum.left.normal[1] = cascadeViewProj[7] + cascadeViewProj[4];
    cascadeFrustum.left.normal[2] = cascadeViewProj[11] + cascadeViewProj[8];
    cascadeFrustum.left.distance = cascadeViewProj[15] + cascadeViewProj[12];
    CameraComponent::_normalizePlane(cascadeFrustum.left);
    
    // Right plane: row4 - row1
    cascadeFrustum.right.normal[0] = cascadeViewProj[3] - cascadeViewProj[0];
    cascadeFrustum.right.normal[1] = cascadeViewProj[7] - cascadeViewProj[4];
    cascadeFrustum.right.normal[2] = cascadeViewProj[11] - cascadeViewProj[8];
    cascadeFrustum.right.distance = cascadeViewProj[15] - cascadeViewProj[12];
    CameraComponent::_normalizePlane(cascadeFrustum.right);
    
    // Bottom plane: row4 + row2
    cascadeFrustum.bottom.normal[0] = cascadeViewProj[3] + cascadeViewProj[1];
    cascadeFrustum.bottom.normal[1] = cascadeViewProj[7] + cascadeViewProj[5];
    cascadeFrustum.bottom.normal[2] = cascadeViewProj[11] + cascadeViewProj[9];
    cascadeFrustum.bottom.distance = cascadeViewProj[15] + cascadeViewProj[13];
    CameraComponent::_normalizePlane(cascadeFrustum.bottom);
    
    // Top plane: row4 - row2
    cascadeFrustum.top.normal[0] = cascadeViewProj[3] - cascadeViewProj[1];
    cascadeFrustum.top.normal[1] = cascadeViewProj[7] - cascadeViewProj[5];
    cascadeFrustum.top.normal[2] = cascadeViewProj[11] - cascadeViewProj[9];
    cascadeFrustum.top.distance = cascadeViewProj[15] - cascadeViewProj[13];
    CameraComponent::_normalizePlane(cascadeFrustum.top);
    
    // Near plane: row4 + row3
    cascadeFrustum.n.normal[0] = cascadeViewProj[3] + cascadeViewProj[2];
    cascadeFrustum.n.normal[1] = cascadeViewProj[7] + cascadeViewProj[6];
    cascadeFrustum.n.normal[2] = cascadeViewProj[11] + cascadeViewProj[10];
    cascadeFrustum.n.distance = cascadeViewProj[15] + cascadeViewProj[14];
    CameraComponent::_normalizePlane(cascadeFrustum.n);
    
    // Far plane: row4 - row3
    cascadeFrustum.f.normal[0] = cascadeViewProj[3] - cascadeViewProj[2];
    cascadeFrustum.f.normal[1] = cascadeViewProj[7] - cascadeViewProj[6];
    cascadeFrustum.f.normal[2] = cascadeViewProj[11] - cascadeViewProj[10];
    cascadeFrustum.f.distance = cascadeViewProj[15] - cascadeViewProj[14];
    CameraComponent::_normalizePlane(cascadeFrustum.f);
    return cascadeFrustum;
}

float RenderCore::_CalculateScreenSize(const MeshAABB& aabb,
                                       const float*    cameraPos,
                                       const Camera&   camera,
                                       float           distance) {
    // Very close == huge on screen
    if (distance <= 0.01f) return 1000.0f;

    float maxExtent =
        std::max(
            { aabb.halfExtents[0], aabb.halfExtents[1], aabb.halfExtents[2] }) *
        2.0f;

    // Project to screen size
    // Screen size = (objSize / dist) * (screenHeight / (2 * tan(fov/2)))
    float fovRad = camera.fov * (3.14159265f / 180.0f);
    float projectedSize = (maxExtent / distance) *
                          (Renderer::height / (2.0f * tanf(fovRad / 2.0f)));
    return projectedSize;
}

bool RenderCore::_ShouldCullBySize(GameObject* go, CameraComponent* camera) {
    auto* meshComp = go->GetComponent<MeshComponent>();
    if (!meshComp || !meshComp->isEnabled) return false;

    MeshAABB aabb = meshComp->GetAABB();
    const float* camPos = camera->GetPosition();
    Camera       cam    = camera->GetCamera();

    // Calculate distance to object
    float dx = aabb.center[0] - camPos[0];
    float dy = aabb.center[1] - camPos[1];
    float dz = aabb.center[2] - camPos[2];
    float distance = sqrtf(dx * dx + dy * dy + dz * dz);

    // Don't cull close objects
    if (distance < 20.0f) return false;
    if (distance > m_maxSmallObjDistance) {
        float screenSize = _CalculateScreenSize(aabb, camPos, cam, distance);
        return screenSize < 4.0f; // Cull if smaller than 4 pixels
    }
    return false;
}

bool RenderCore::_ShouldCullBySizeShadow(GameObject* go, CameraComponent* camera, uint8_t cascade) {
    auto* meshComp = go->GetComponent<MeshComponent>();
    if (!meshComp || !meshComp->isEnabled) return false;

    MeshAABB aabb = meshComp->GetAABB();
    
    // Get light position for this cascade
    float lightDir[3];
    Renderer::GetSunDirection(lightDir);
    const float* camPos = camera->GetPosition();
    float lightDistance = 100.0f; // Same as in _CalculateCascadeMatrices
    
    float lightPos[3] = {
        camPos[0] + lightDir[0] * lightDistance,
        camPos[1] + lightDir[1] * lightDistance,
        camPos[2] + lightDir[2] * lightDistance
    };

    // Calculate distance from light to object
    float dx = aabb.center[0] - lightPos[0];
    float dy = aabb.center[1] - lightPos[1];
    float dz = aabb.center[2] - lightPos[2];
    float distance = sqrtf(dx * dx + dy * dy + dz * dz);

    float cdx = aabb.center[0] - camPos[0];
    float cdy = aabb.center[1] - camPos[1];
    float cdz = aabb.center[2] - camPos[2];
    float camDistance = sqrtf(cdx * cdx + cdy * cdy + cdz * cdz);

    // Don't cull very close objects to light, nor close to the camera
    if (distance < 10.0f || camDistance < 20.0f) return false;
    
    // For orthographic projection, objects far from light contribute less to shadows
    // Get object size
    float maxExtent = std::max({
        aabb.halfExtents[0], aabb.halfExtents[1], aabb.halfExtents[2]
    }) * 2.0f;
    
    // For ortho shadows, cull based on shadow contribution
    // Objects that are very small relative to the cascade size won't contribute meaningful shadows
    float cascadeSize = m_cascadeSizes[cascade];
    
    // If object is smaller than 1.5% of cascade size, cull it
    return maxExtent < (cascadeSize * 0.015f);
}

void RenderCore::_CollectRenderPackets(CameraComponent* camera) {
    m_renderPackets.clear();

    // Iterate registry
    auto gameObjects = Registry::GetRenderableObjects();
    for (auto& go : gameObjects) {
        if (!go) continue;
        auto* meshComp = go->GetComponent<MeshComponent>();
        if (!meshComp || !meshComp->isEnabled || !meshComp->meshData.valid)
            continue;

        if (_ShouldCullBySize(go, camera)) {
            m_drawnCounts.culledSize++;
            continue;
        }

        MeshAABB aabb = meshComp->GetAABB();
        bx::Vec3 min = { aabb.min[0], aabb.min[1], aabb.min[2] };
        bx::Vec3 max = { aabb.max[0], aabb.max[1], aabb.max[2] };
        if (!camera->_aabbInsideFrustum(camera->_extractFrustum(), min, max)) {
            m_drawnCounts.culledFrustum++;
            continue;
        }

        RenderPacket packet;
        packet.vbh = meshComp->meshData.vbh;
        packet.ibh = meshComp->meshData.ibh;
        go->GetComponent<TransformComponent>()->GetModelMatrix(packet.modelMtx);

        Material& mat = meshComp->meshData.materials[0];
        packet.program = Renderer::GetProgram(go->type.c_str());

        MaterialInstance matInst;
        matInst.renderState = BGFX_STATE_DEFAULT | BGFX_STATE_DEPTH_TEST_LESS |
                              BGFX_STATE_MSAA | BGFX_STATE_CULL_CW;
        if (go->type == "default") {
            // Uniform: u_default_floats
            size_t handle = m_defaultUniformIds.at("u_default_floats");
            Uniform* u = Renderer::GetUniform(handle);
            static float floatData[4] = { 0.f, 0.f, 0.2f, 0.f };
            matInst.uniforms.push_back({ handle,  &floatData, u->num });

            // Uniform: u_default_useVertex / u_baseColor
            if (mat.useVertexColor) {
                size_t handle = m_defaultUniformIds.at("u_default_useVertex");
                Uniform* u      = Renderer::GetUniform(handle);
                static float useVertexData[4] = { 1.f, 0.f, 0.f, 0.f };
                matInst.uniforms.push_back({ handle, &useVertexData, u->num });
            } else {
                size_t handle = m_defaultUniformIds.at("u_default_useVertex");
                Uniform* u = Renderer::GetUniform(handle);
                static float useVertexData[4] = { 0.f, 0.f, 0.f, 0.f };
                matInst.uniforms.push_back({ handle, &useVertexData, u->num });

                size_t handle2 = m_defaultUniformIds.at("u_baseColor");
                Uniform* uBaseColor = Renderer::GetUniform(handle2);
                matInst.uniforms.push_back({ handle2, mat.baseColor, uBaseColor->num });
            }
        } else if (go->type == "texture") {
            uint32_t samplerFlags =
                BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
            // Uniform: u_texture_floats
            size_t handle = m_defaultUniformIds.at("u_texture_floats");
            Uniform* u = Renderer::GetUniform(handle);
            static float floatData[4] = { 0.f, 0.f, 0.2f, 0.f };
            matInst.uniforms.push_back({ handle, &floatData, u->num });
            // Textures
            matInst.textures.push_back({
                0, 
                Renderer::GetUniform(m_defaultUniformIds.at("u_texture_albedo"))->handle, 
                mat.albedo, 
                samplerFlags
            });

            matInst.textures.push_back({
                1, 
                Renderer::GetUniform(m_defaultUniformIds.at("u_normalMap"))->handle, 
                mat.normalMap, 
                samplerFlags
            });

            matInst.textures.push_back({
                2, 
                Renderer::GetUniform(m_defaultUniformIds.at("u_heightMap"))->handle, 
                mat.heightMap, 
                samplerFlags
            });
        }

        packet.material = matInst;
        m_renderPackets.push_back(packet);
    }
}

void RenderCore::_ScreenSpaceQuad(ViewID view, Program program) {
    SYN_PROFILE_FUNCTION();
    uint16_t flags = view == VIEW_AO ? (BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH) : (BGFX_CLEAR_NONE);
    bgfx::setViewClear(view, flags, 0x000000ff, 1.0f, 0);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CCW);

    bgfx::setVertexBuffer(0, m_fsQuadVbh);
    bgfx::submit(view, program.program);
}

/*
--- Drawing functions ---
*/

void RenderCore::_DrawShadows(const Program&   program,
                              CameraComponent* camera,
                              uint8_t          cascade) {
    SYN_PROFILE_FUNCTION();
    const uint64_t renderState =
        BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA | BGFX_STATE_CULL_CW;

    if (Core::_GetApp()->debug.CSMBounds) {
        if (Renderer::m_pseudoCamera) camera = Renderer::m_pseudoCamera;
        float view[16 * NUM_CASCADES], proj[16 * NUM_CASCADES], outCascadeSplits[NUM_CASCADES];
        _CalculateCascadeMatrices(camera, view, proj, outCascadeSplits);
        for (int i = 0; i < NUM_CASCADES; ++i) {
            Core::_GetApp()->physicsManager->_DrawFrustum(view + i * 16, proj + i * 16);
        }

    }

    std::vector<GameObject*> gameObjects = Registry::GetRenderableObjects();
    {
        SYN_PROFILE_SCOPE("Set Shadow View");
        bgfx::setState(renderState);
    }
    {
        SYN_PROFILE_SCOPE("Draw Shadow Cascade");
    for (auto& gameObject : gameObjects) {
        if (!gameObject) continue;

        auto* meshComp = gameObject->GetComponent<MeshComponent>();
        MeshData meshData = meshComp->meshData;

        if (!meshData.valid || !meshComp->isEnabled || !meshComp->castShadows)
            continue;

        // Size-based shadow culling from light's perspective
        if (_ShouldCullBySizeShadow(gameObject, camera, cascade)) {
            m_drawnCounts.culledShadowSize++;
            continue;
        }

        MeshAABB aabb = meshComp->GetAABB();
        bx::Vec3 min = { aabb.min[0], aabb.min[1], aabb.min[2] };
        bx::Vec3 max = { aabb.max[0], aabb.max[1], aabb.max[2] };
        if (!camera->_aabbInsideFrustum(_GetCascadeFrustum(cascade, camera), min, max)) {
            m_drawnCounts.culledShadowFrustum++;
            continue;
        }

        // Get the transform for this object
        {
            SYN_PROFILE_SCOPE("Set transform");
        float modelMtx[16];
        gameObject->GetComponent<TransformComponent>()->GetModelMatrix(modelMtx);
        bgfx::setTransform(modelMtx);
        }

        {
            SYN_PROFILE_SCOPE("Set buffers");
        bgfx::setVertexBuffer(0, meshData.vbh);
        bgfx::setIndexBuffer(meshData.ibh);
        }

        {
            SYN_PROFILE_SCOPE("Submit shadow");
        // Shadow shaders are simple, just output depth
        bgfx::submit(program.viewId + cascade, program.program);
        m_drawnCounts.shadows++;
        }
    }
    }
}

void RenderCore::_DrawSky(const Program& program) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewFrameBuffer(program.viewId, m_sceneFB);
    bgfx::setViewClear(program.viewId,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x000000ff,
                       1.0f,
                       0);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_ALWAYS |
                   BGFX_STATE_MSAA | BGFX_STATE_CULL_CCW);

    bgfx::setVertexBuffer(0, m_fsQuadVbh);
    bgfx::submit(program.viewId, program.program);
}

void RenderCore::_DrawForward(const Program& program, CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    bgfx::setViewFrameBuffer(VIEW_FORWARD, m_sceneFB);
    const uint64_t renderState = BGFX_STATE_DEFAULT | BGFX_STATE_MSAA |
                                 BGFX_STATE_FRONT_CCW | BGFX_STATE_CULL_CW | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z;

    for (auto& packet : m_renderPackets) {
        if (program.program.idx != packet.program.program.idx) continue; // Skip if not matching program
        bgfx::setState(renderState | packet.material.renderState);

        packet.material.Bind(); // Set uniforms and textures

        bgfx::setTransform(packet.modelMtx);
        bgfx::setVertexBuffer(0, packet.vbh);
        bgfx::setIndexBuffer(packet.ibh);
        bgfx::submit(program.viewId, program.program);
        m_drawnCounts.forward++;
    }
}

void RenderCore::_DrawDebug(const Program&   program,
                            CameraComponent* camera,
                            DebugModes       debug) {
    SYN_PROFILE_FUNCTION();
    GameObject *p = Registry::GetGameObjectByName("player");
    if (p && Core::IsPhysicsEnabled()) {
        Core::_GetApp()->physicsManager->_DrawDebug(
            Renderer::width, Renderer::height, program.program,
            p->GetComponent<CameraComponent>()->GetCamera(), camera->GetCamera(),
            debug);
    }

    // Draw various debug overlays
    if (debug.Gizmos) {
        // Draw zone boundaries
        std::vector<ZoneComponent*> zones = Core::_GetApp()->zoneManager.get()->GetZones();
        for (auto zone : zones) {
            if (!zone || !zone->isEnabled || !m_drender) continue;
            switch (zone->GetShape()) {
                case ZoneShape::BOX: {
                    float min[3], max[3], pos[3], size[3];
                    zone->GetPosition(pos);
                    zone->GetSize(size);
                    for (int i = 0; i < 3; ++i) {
                        min[i] = pos[i] - size[i] * 0.5f;
                        max[i] = pos[i] + size[i] * 0.5f;
                    }

                    m_drender->DrawBox(min, max, JPH::Color::sRed);
                    break;
                }
                case ZoneShape::SPHERE: {
                    float pos[3];
                    float size[3];
                    zone->GetPosition(pos);
                    zone->GetSize(size); // size.x is radius
                    m_drender->DrawSphere(JPH::Vec3(pos[0], pos[1], pos[2]),
                                        size[0],
                                        JPH::Color::sRed);
                    break;
                }
            }
        }
    }

    if (debug.DrawBoundingBoxes) {
        std::vector<GameObject*> meshObjects =
            Registry::GetGameObjectsWithComponent(SYN_COMPONENT_MESH);
        for (auto go : meshObjects) {
            MeshAABB aabb = go->GetComponent<MeshComponent>()->GetAABB();
            m_drender->DrawBox(aabb.min, aabb.max, JPH::Color::sGreen);
        }
    }
}

void RenderCore::_DrawBillboard(const Program& program) {
    SYN_PROFILE_FUNCTION();
    const uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                           BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LEQUAL |
                           BGFX_STATE_MSAA;

    // Draw regular billboards
    std::vector<GameObject*> billboards =
        Registry::GetGameObjectsWithComponent(SYN_COMPONENT_BILLBOARD);
    for (auto go : billboards) {
        auto* comp = go->GetComponent<BillboardComponent>();
        if (!comp || !comp->isEnabled) continue;
        
        const float* pos = go->GetComponent<TransformComponent>()->GetPosition();
        
        // Pack center position and size into a vec4 and send it off
        float billboardData[4] = { pos[0], pos[1], pos[2], comp->size };
        Renderer::SetUniform(m_defaultUniformIds["u_billboard"], billboardData);
        
        // In addition, send the mode and rot as a vec4
        float* rot = comp->GetRot();
        float  billboardExtra[4] = {
            rot[0], rot[1], rot[2], static_cast<float>(comp->GetMode())
        };
        Renderer::SetUniform(m_defaultUniformIds["u_billboard_mode"], billboardExtra);

        float lightingFlags[4] = { comp->receiveSunLight ? 1.0f : 0.0f,
                                   comp->receiveShadows ? 1.0f : 0.0f,
                                   0.0f,
                                   0.0f };
        Renderer::SetUniform(m_defaultUniformIds["u_billboard_lighting"], lightingFlags);
        
        // Dummy model matrix for the billboard
        float modelMtx[16];
        bx::mtxIdentity(modelMtx);

        bgfx::setState(state);
        bgfx::setTransform(modelMtx);
        bgfx::setVertexBuffer(0, m_billboardVbh);
        bgfx::setIndexBuffer(m_billboardIbh);

        // Set shadow if applicable
        if (m_config.useShadows && comp->receiveShadows) {
            const uint64_t depthSampler =
                BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
            bgfx::setTexture(
                3,
                Renderer::GetUniform(m_defaultUniformIds["s_billboard_shadowMap"])->handle,
                m_shadowDepth,
                depthSampler);
        }

        bgfx::setTexture(
                0,
                Renderer::GetUniform(m_defaultUniformIds["s_bill_albedo"])->handle,
                comp->_GetTexture());
        bgfx::submit(program.viewId, program.program);
        m_drawnCounts.billboard++;
    }
}

void RenderCore::_DrawPostProcess(const Program& program) {
    SYN_PROFILE_FUNCTION();

    if (program.id == m_internalPrograms.ssaoProgram) {
        bgfx::setViewFrameBuffer(VIEW_AO, m_ssaoFB);
        bgfx::setTexture(
            0,
            Renderer::GetUniform(m_defaultUniformIds.at("s_ssao_depthTex"))
                ->handle,
            m_sceneDepth);
        bgfx::setTexture(
            1,
            Renderer::GetUniform(m_defaultUniformIds.at("s_ssao_normalTex"))
                ->handle,
            m_sceneNormal);
        bgfx::setTexture(
            2,
            Renderer::GetUniform(m_defaultUniformIds.at("s_ssao_noiseTex"))
                ->handle,
            m_ssaoNoiseTex);
        _ScreenSpaceQuad(VIEW_AO, program);
    } else if (program.id == m_internalPrograms.tonemapProgram) {
        bgfx::setViewFrameBuffer(VIEW_POSTPROCESS,
                                 BGFX_INVALID_HANDLE); // Backbuffer
        bgfx::setTexture(
            0,
            Renderer::GetUniform(m_defaultUniformIds.at("s_tonemap_sceneTex"))
                ->handle,
            m_sceneColor);
        bgfx::setTexture(
            1,
            Renderer::GetUniform(m_defaultUniformIds.at("s_tonemap_ssaoTex"))
                ->handle,
            m_ssaoTex);
         _ScreenSpaceQuad(VIEW_POSTPROCESS, program);
    }

}

void RenderCore::_DrawDbgBillboard(const Program& program) {
    SYN_PROFILE_FUNCTION();
    // Debug billboards (gizmos) are always drawn on top. Regular billboards (forward pass ig) are depth-tested normally.
    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                    BGFX_STATE_BLEND_ALPHA | BGFX_STATE_DEPTH_TEST_ALWAYS;

    // Draw all gizmos
    std::vector<GameObject*> gizmos = Registry::GetGizmos();
    for (auto go : gizmos) {
        auto it = Renderer::m_gizmoRegistry.find(go->gizmo);
        if (it != Renderer::m_gizmoRegistry.end()) {
            auto* comp = go->GetComponent<Syngine::CameraComponent>();
            if (!comp) continue;

            const BillboardComponent* gizmo = it->second;
            const float* pos   = comp->GetPosition();

            // Pack center position and size into a vec4 and send it off
            float billboardData[4] = { pos[0], pos[1], pos[2], gizmo->size };
            Renderer::SetUniform(m_defaultUniformIds["u_billboard"], billboardData);

            // In addition, send the mode and rot as a vec4
            float billboardExtra[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // No rotation or special mode for gizmos
            Renderer::SetUniform(m_defaultUniformIds["u_billboard_mode"], billboardExtra);
            float lightingFlags[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // No lighting for gizmos
            Renderer::SetUniform(m_defaultUniformIds["u_billboard_lighting"], lightingFlags);

            // Dummy model matrix for the billboard
            float modelMtx[16];
            bx::mtxIdentity(modelMtx);

            bgfx::setState(state);
            bgfx::setTransform(modelMtx);
            bgfx::setVertexBuffer(0, m_billboardVbh);
            bgfx::setIndexBuffer(m_billboardIbh);
            bgfx::setTexture(
                0,
                Renderer::GetUniform(m_defaultUniformIds["s_bill_albedo"])->handle,
                gizmo->_GetTexture()); // Use the texture from the gizmo registry
            bgfx::submit(program.viewId, program.program);
        }
    }
}

void RenderCore::_DrawUIDebug(CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::dbgTextClear();

    int width = Renderer::width;
    int height = Renderer::height;
    int maxCols = width / 8; // Assuming each character is 8 pixels wide
    int maxRows = height / 16; // Assuming each character is 16 pixels tall

    bgfx::dbgTextPrintf(
        1, maxRows - 2, 0x0C, "Syngine v%s", SYN_VERSION_STRING);
    bgfx::dbgTextPrintf(
        1, maxRows - 1, 0x0C, "FOR INTERNAL USE ONLY - NOT FOR PUBLIC RELEASE");
}

bool RenderCore::_PrepareRenderViews(CameraComponent* camera) {
    SYN_PROFILE_FUNCTION();
    // Prepare camera and light information
    if (!camera) {
        Syngine::Logger::Fatal("No camera provided to render frame");
        return false;
    }

    const float* camPos = camera->GetPosition();
    float        viewPos[4] = { camPos[0], camPos[1], camPos[2], 1.0f };
    Renderer::SetUniform(m_defaultUniformIds["u_default_viewPos"], viewPos);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_viewPos"], viewPos);

    float sunDir[3];
    Renderer::GetSunDirection(sunDir); // returns normalized world-space direction TO the light (direction-to-light)
    float lightDirUniform[4] = { sunDir[0], sunDir[1], sunDir[2], 0.0f };
    Renderer::SetUniform(m_defaultUniformIds["u_default_lightDir"], lightDirUniform);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_lightDir"], lightDirUniform);
    
    float lightView[NUM_CASCADES * 16];
    float lightProj[NUM_CASCADES * 16];
    if (m_config.useShadows) {
        // Calculate the cascade matrices for shadow mapping and send to GPU
        float cascadeSplits[NUM_CASCADES];
        _CalculateCascadeMatrices(camera, lightView, lightProj, cascadeSplits);
        
        float farClip = m_config.shadowDist;
        float splits[4] = { cascadeSplits[0],
                            cascadeSplits[1],
                            cascadeSplits[2],
                            cascadeSplits[3] };
        Renderer::SetUniform(m_defaultUniformIds["u_default_csmSplits"], splits);
        Renderer::SetUniform(m_defaultUniformIds["u_texture_csmSplits"], splits);

        float shadowParams[4] = {
            bgfx::getCaps()->homogeneousDepth ? 1.0f
                                              : 0.0f, // Homogeneous depth
            1.0f / (float)(SHADOW_MAP_SIZE * 2),      // Inv shadow map size
            m_config.shadowDist, // Light far plane
            0.0f // Debug value, not used
        };
        Renderer::SetUniform(m_defaultUniformIds["u_default_shadowParams"], shadowParams);
        Renderer::SetUniform(m_defaultUniformIds["u_texture_shadowParams"], shadowParams);
    }

    // Update main camera matrices
    for (Syngine::ViewID view : _allViews) {
        switch (view) {
        case ViewID::VIEW_SHADOW: {
            if (!m_config.useShadows) break; // Skip if shadows are disabled
            
            for (uint8_t i = 0; i < NUM_CASCADES; ++i) {
                bgfx::ViewId cascadeViewId = view + i;
                bgfx::setViewFrameBuffer(cascadeViewId, m_shadowFB);
                uint16_t x = (i % 2) * SHADOW_MAP_SIZE;
                uint16_t y = (i / 2) * SHADOW_MAP_SIZE;
                bgfx::setViewRect(
                    cascadeViewId, x, y, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
                bgfx::setViewScissor(
                    cascadeViewId, x, y, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
                bgfx::setViewClear(
                    cascadeViewId, BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
                bgfx::setViewTransform(
                    cascadeViewId, &lightView[i * 16], &lightProj[i * 16]);
            }
            
            break;
        }
        case ViewID::VIEW_SKY: {
            bgfx::setViewRect(view, 0, 0, uint16_t(Renderer::width), uint16_t(Renderer::height));
            camera->Update(view, Renderer::width, Renderer::height);
            Camera cam = camera->GetCamera();
            float  skyView[16];
            bx::memCopy(skyView, cam.view, sizeof(skyView));
            skyView[12] = skyView[13] = skyView[14] = 0.0f;
            bgfx::setViewTransform(view, skyView, cam.proj);
            break;
        }
        default: {
            bgfx::setViewRect(view, 0, 0, uint16_t(Renderer::width), uint16_t(Renderer::height));
            Camera cam = camera->GetCamera();
            bgfx::setViewTransform(view, cam.view, cam.proj);
            break;
        }
        }
    }
    m_drawnCounts = DrawnObjectCount(); // Reset counts for this frame
    return true;
}

bool RenderCore::_RenderFrame(CameraComponent* camera, DebugModes debug) {
    SYN_PROFILE_FUNCTION();
    if (m_isFirstFrame) {
        m_isFirstFrame = false;
        if (Core::IsPhysicsEnabled()) 
            m_drender = Core::_GetApp()->physicsManager->_GetDebugRenderer();
    }

    if (!_PrepareRenderViews(camera)) return false;
    _CollectRenderPackets(camera);
    
    // Main render loop
    for (auto view : _allViews) {
        auto progListIt = Renderer::viewPrograms.find(view);
        if (progListIt == Renderer::viewPrograms.end()) continue;
        
        for (auto& program : progListIt->second) {
            if (!bgfx::isValid(program.program)) continue;
            
            // Draw logic based on view type
            switch (view) {
            case VIEW_SHADOW:
                if (m_config.useShadows) {
                    for (uint8_t i = 0; i < NUM_CASCADES; ++i) {
                        _DrawShadows(program, camera, i);
                    }
                }
                break;
            case VIEW_SKY:
                _DrawSky(program);
                break;
            case VIEW_FORWARD:
                _DrawForward(program, camera);
                break;
            case VIEW_DEBUG:
                if (debug.Enabled) _DrawDebug(program, camera, debug);
                break;
            case VIEW_BILL_DBG:
            case VIEW_BILLBOARD:
                if (debug.Enabled && debug.Gizmos) _DrawDbgBillboard(program);
                _DrawBillboard(program);
                break;
            case VIEW_POSTPROCESS:
            case VIEW_AO:
                _DrawPostProcess(program);
                break;
            default:
                break;
            }
        }
    }

    _DrawUIDebug(camera);

    bgfx::frame();
    return true;
}

} // namespace Syngine
