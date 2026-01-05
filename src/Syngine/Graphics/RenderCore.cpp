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
#include "Syngine/Graphics/DebugRenderer.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/Graphics/Windowing.h"
#include "Syngine/Utils/Version.h"
#include "Syngine/Utils/Profiler.h"
#include "Syngine/Core/Logger.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"

namespace Syngine {

std::unordered_map<std::string, uint16_t> RenderCore::m_defaultUniformIds;

bgfx::TextureHandle       RenderCore::m_shadowDepth = BGFX_INVALID_HANDLE;
bgfx::FrameBufferHandle   RenderCore::m_shadowFB    = BGFX_INVALID_HANDLE;

RendererConfig RenderCore::m_config;
SDL_Window*    RenderCore::win = nullptr;

DebugRender* RenderCore::m_drender = nullptr;
bool         RenderCore::m_isFirstFrame = true;

bgfx::VertexBufferHandle RenderCore::dummy = BGFX_INVALID_HANDLE;
bgfx::VertexBufferHandle RenderCore::m_billboardVbh = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle  RenderCore::m_billboardIbh = BGFX_INVALID_HANDLE;

std::unordered_map<uint16_t, Uniform> Renderer::m_uniformRegistry;
std::unordered_map<uint16_t, std::vector<Program>> Renderer::viewPrograms;

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
    bgInit.platformData.ndt = nullptr; // Only needed on x11
    bgInit.platformData.nwh = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    if (bgInit.platformData.nwh == NULL) {
        Syngine::Logger::Error("Failed to get window properties for Windows");
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
#else
#warning "Platform not supported
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
    Renderer::m_isReady = true; // Teehee. Temporarily set to true to allow shader loading.
    size_t defaultProg = Renderer::AddProgram("shaders/default", "default");
    if (defaultProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create default program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t textureProg = Renderer::AddProgram("shaders/default_texture", "texture");
    if (textureProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create texture program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t debugProg =
        Renderer::AddProgram("shaders/default_debug", "default_debugger", VIEW_DEBUG);
    if (debugProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create debug program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t debugBillboardProg =
        Renderer::AddProgram("shaders/default_billboard", "default_billboard", VIEW_BILL_DBG);
    if (debugBillboardProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create debug billboard program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t billboardProg =
        Renderer::AddProgram("shaders/default_billboard", "billboard", VIEW_BILLBOARD);
    if (billboardProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create debug billboard program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t shadowProg =
        Renderer::AddProgram("shaders/default_shadow", "shadow", VIEW_SHADOW);
    if (shadowProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create shadow program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
    Renderer::m_isReady = false;

    // Create default uniforms
    // Debug program uniforms
    {
    m_defaultUniformIds.insert({ "u_dbg_billboard",
                                 Renderer::RegisterUniform(debugBillboardProg,
                                                 "u_default_billboard",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "s_dbg_bill_albedo",
          Renderer::RegisterUniform(
              debugBillboardProg, "s_albedo", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert({ "u_dbg_billboard_mode",
                                 Renderer::RegisterUniform(debugBillboardProg,
                                                 "u_default_billboard_mode",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_dbg_billboard_lighting",
                                 Renderer::RegisterUniform(debugBillboardProg,
                                                 "u_billboard_lighting",
                                                 UniformType::UNIFORM_VEC4) });

    // Billboard program uniforms
    m_defaultUniformIds.insert({ "u_billboard",
                                 Renderer::RegisterUniform(billboardProg,
                                                 "u_default_billboard",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "s_bill_albedo",
          Renderer::RegisterUniform(
              billboardProg, "s_albedo", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert({ "u_billboard_mode",
                                 Renderer::RegisterUniform(billboardProg,
                                                 "u_default_billboard_mode",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_billboard_lighting",
                                 Renderer::RegisterUniform(billboardProg,
                                                 "u_billboard_lighting",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "s_billboard_shadowMap",
          Renderer::RegisterUniform(
              billboardProg, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });

    // Vertex program uniforms
    m_defaultUniformIds.insert({ "u_baseColor",
                                 Renderer::RegisterUniform(defaultProg,
                                                 "u_baseColor",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_lightDir",
                                 Renderer::RegisterUniform(defaultProg,
                                                 "u_lightDir",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_normalMatrix",
                                 Renderer::RegisterUniform(defaultProg,
                                                 "u_normalMatrix",
                                                 UniformType::UNIFORM_MAT3) });

    m_defaultUniformIds.insert(
        { "u_default_shadowMap",
          Renderer::RegisterUniform(
              defaultProg, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_default_useVertex",
          Renderer::RegisterUniform(
              defaultProg, "u_useVertexColor", UniformType::UNIFORM_VEC4) });

    // Texture program uniforms
    m_defaultUniformIds.insert({ "u_texture_lightDir",
                                 Renderer::RegisterUniform(textureProg,
                                                 "u_lightDir",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_floats",
                                 Renderer::RegisterUniform(textureProg,
                                                 "u_floats",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_normalMatrix",
                                 Renderer::RegisterUniform(textureProg,
                                                 "u_normalMatrix",
                                                 UniformType::UNIFORM_MAT3) });

    m_defaultUniformIds.insert(
        { "u_texture_albedo",
          Renderer::RegisterUniform(
              textureProg, "s_albedo", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_normalMap",
          Renderer::RegisterUniform(
              textureProg, "s_normalMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_heightMap",
          Renderer::RegisterUniform(
              textureProg, "s_heightMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_texture_shadowMap",
          Renderer::RegisterUniform(
              textureProg, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });

    // Shadow program uniforms
    m_defaultUniformIds.insert({ "u_default_csmSplits",
                                 Renderer::RegisterUniform(defaultProg,
                                                 "u_csmSplits",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_csmLightViewProj",
                                 Renderer::RegisterUniform(defaultProg,
                                                 "u_csmLightViewProj",
                                                 UniformType::UNIFORM_MAT4, 4) });
    m_defaultUniformIds.insert({ "u_texture_csmSplits",
                                 Renderer::RegisterUniform(textureProg,
                                                 "u_csmSplits",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_csmLightViewProj",
                                 Renderer::RegisterUniform(textureProg,
                                                 "u_csmLightViewProj",
                                                 UniformType::UNIFORM_MAT4,
                                                 4) });
    m_defaultUniformIds.insert({ "u_texture_viewPos",
                                 Renderer::RegisterUniform(textureProg,
                                                 "u_viewPos",
                                                 UniformType::UNIFORM_VEC4) });;
    m_defaultUniformIds.insert({ "u_default_viewPos",
                                 Renderer::RegisterUniform(defaultProg,
                                                 "u_viewPos",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_shadowParams",
                                 Renderer::RegisterUniform(textureProg,
                                                 "u_shadowParams",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_shadowParams",
                                 Renderer::RegisterUniform(defaultProg,
                                                 "u_shadowParams",
                                                 UniformType::UNIFORM_VEC4) });
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
    }

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

void RenderCore::_CalculateCascadeMatrices(CameraComponent* camera,
                                         float*           outLightView,
                                         float*           outLightProj,
                                         float*           outCascadeSplits) {
    float lightDir[3];
    Renderer::GetSunDirection(lightDir);
    const bx::Vec3 lightDirVec = bx::normalize(bx::Vec3(lightDir[0], lightDir[1], lightDir[2]));
    
    const float* camPos = camera->GetPosition();

    // Currently using fixed cascade sizes based on shadow distance
    float cascadeSizes[NUM_CASCADES] = {
        10, 50, round(m_config.shadowDist / 3), round(m_config.shadowDist)
    };

    float cascadeDistances[NUM_CASCADES];
    float lightViewProj[NUM_CASCADES * 16];
    
    for (uint32_t i = 0; i < NUM_CASCADES; i++) {
        float size = cascadeSizes[i];

        // How far light is from camera
        // Light is always targeting camera, will be some distance away
        float lightDistance = 100.0f;
        
        bx::Vec3 lightPos = {
            camPos[0] + lightDir[0] * lightDistance,
            camPos[1] + lightDir[1] * lightDistance,
            camPos[2] + lightDir[2] * lightDistance
        };
        
        bx::Vec3 target = {camPos[0], camPos[1], camPos[2]};
        bx::Vec3 up     = { 0.0f, 1.0f, 0.0f };

        if (Core::_GetApp()->debug.CSMBounds) { 
            // Draw line from light to target
            float from[3] = { lightPos.x, lightPos.y, lightPos.z };
            float to[3] = { target.x, target.y, target.z };
            Core::_GetApp()->physicsManager->_DrawLine(from, to, JPH::Color::sYellow);
        }
        
        bx::mtxLookAt(&outLightView[i * 16], lightPos, target, up);

        // Try and keep tight near/far planes for good precision
        float lightNear = 20.0f;
        float lightFar = m_config.shadowDist + 50.0f; // Some margin
        
        bx::mtxOrtho(&outLightProj[i * 16],
                     -size, size,
                     -size, size,
                     lightNear, lightFar,
                     0.0f, bgfx::getCaps()->homogeneousDepth);

        cascadeDistances[i] = cascadeSizes[i] / 2.0f;
        outCascadeSplits[i] = cascadeDistances[i];
        bx::mtxMul(&lightViewProj[i * 16], &outLightView[i * 16], &outLightProj[i * 16]);
    }
    
    Renderer::SetUniform(m_defaultUniformIds["u_default_csmLightViewProj"], lightViewProj, NUM_CASCADES);
    Renderer::SetUniform(m_defaultUniformIds["u_texture_csmLightViewProj"], lightViewProj, NUM_CASCADES);
}

/*
--- Drawing functions ---
*/

void RenderCore::_DrawShadows(const Program& program, CameraComponent* camera, uint8_t cascade) {
    const uint64_t renderState =
        BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA | BGFX_STATE_CULL_CCW;

    if (Core::_GetApp()->debug.CSMBounds) {
        float view[16 * NUM_CASCADES], proj[16 * NUM_CASCADES], outCascadeSplits[NUM_CASCADES];
        _CalculateCascadeMatrices(camera, view, proj, outCascadeSplits);
        for (int i = 0; i < NUM_CASCADES; ++i) {
            Core::_GetApp()->physicsManager->_DrawFrustum(view + i * 16, proj + i * 16);
        }

    }

    std::vector<GameObject*> gameObjects = Registry::GetRenderableObjects();
    
    bgfx::setState(renderState);
    for (auto& gameObject : gameObjects) {
        if (!gameObject) continue;

        auto* meshComp = gameObject->GetComponent<MeshComponent>();
        MeshData meshData = meshComp->meshData;

        if (!meshData.valid || !meshComp->isEnabled || !meshComp->castShadows) continue;

        // Get the transform for this object
        float modelMtx[16];
        gameObject->GetComponent<TransformComponent>()->GetModelMatrix(modelMtx);
        bgfx::setTransform(modelMtx);
        
        bgfx::setVertexBuffer(0, meshData.vbh);
        bgfx::setIndexBuffer(meshData.ibh);

        // Shadow shaders are simple, just output depth
        bgfx::submit(program.viewId + cascade, program.program);
    }
}

void RenderCore::_DrawSky(const Program& program) {
    bgfx::setViewClear(program.viewId,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x000000ff,
                       1.0f,
                       0);
    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_DEPTH_TEST_LEQUAL |
                   BGFX_STATE_MSAA);

    // Damn everything about macos
#if BX_PLATFORM_OSX
    bgfx::setVertexBuffer(0, Renderer::dummy);
#else
    bgfx::setVertexCount(3);
#endif
    bgfx::submit(program.viewId, program.program);
}

void RenderCore::_DrawForward(const Program& program, CameraComponent* camera) {
    const uint64_t renderState = BGFX_STATE_DEFAULT | BGFX_STATE_MSAA |
                                 BGFX_STATE_FRONT_CCW | BGFX_STATE_CULL_CW;

    std::vector<GameObject*> gameObjects = Registry::GetRenderableObjects();
    
    // Find every object with the current program to prevent running the same
    // GameObject multiple times
    std::vector<GameObject*> objectsWithProgram;
    
    for (auto& gameObject : gameObjects) {
        if (!gameObject) continue;
        
        if (gameObject->type == program.name) {
            objectsWithProgram.push_back(gameObject);
        }
    }

    const uint64_t samplerFlags =
                BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;

    for (auto& gameObject : objectsWithProgram) {
        auto* meshComp = gameObject->GetComponent<MeshComponent>();
        MeshData meshData = meshComp->meshData;
        if (!meshData.valid || !meshComp->isEnabled) continue;
        
        bgfx::setState(renderState);
        Material& mat = meshData.materials[0];
        // Default vertex color shader
        if (program.name == "default") {
            if (mat.useVertexColor) {
                float useVertex[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
                Renderer::SetUniform(m_defaultUniformIds["u_default_useVertex"], useVertex);
            } else {
                float useVertex[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                Renderer::SetUniform(m_defaultUniformIds["u_default_useVertex"], useVertex);
                Renderer::SetUniform(m_defaultUniformIds["u_baseColor"], mat.baseColor);
            }
        }
        // Default textured shader
        else if (program.name == "texture") {
            float u_floats[4] = {
                mat.heightScale,
                mat.mixFactor,
                mat.ambient,
                mat.tileDetail,
            };
            Renderer::SetUniform(m_defaultUniformIds["u_floats"], u_floats);

            bgfx::setTexture(
                0,
                Renderer::GetUniform(m_defaultUniformIds["u_texture_albedo"])->handle,
                mat.albedo,
                samplerFlags);
            bgfx::setTexture(
                1,
                Renderer::GetUniform(m_defaultUniformIds["u_normalMap"])->handle,
                mat.normalMap,
                samplerFlags);
            bgfx::setTexture(
                2,
                Renderer::GetUniform(m_defaultUniformIds["u_heightMap"])->handle,
                mat.heightMap,
                samplerFlags);
        }
        // Probably a custom shader
        else {
            Program shader = Renderer::GetProgram(gameObject->type.c_str());
            if(!bgfx::isValid(shader.program)) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Program %s not found", gameObject->type.c_str());
                continue;
            }
        }

        if (m_config.useShadows && meshComp->receiveShadows) {
            // Set shadow map for the texture program
            const uint64_t depthSampler =
                BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
            std::string name = "u_" + program.name + "_shadowMap";
            bgfx::setTexture(
                3,
                Renderer::GetUniform(m_defaultUniformIds[name])->handle,
                m_shadowDepth,
                depthSampler);
        }

        bgfx::setVertexBuffer(0, meshData.vbh);
        bgfx::setIndexBuffer(meshData.ibh);

        // Set common uniforms
        float modelMtx[16];
        gameObject->GetComponent<TransformComponent>()->GetModelMatrix(modelMtx);
        bgfx::setTransform(modelMtx);
        
        float sx = bx::length(bx::Vec3(modelMtx[0], modelMtx[1], modelMtx[2]));
        float sy = bx::length(bx::Vec3(modelMtx[4], modelMtx[5], modelMtx[6]));
        float sz = bx::length(bx::Vec3(modelMtx[8], modelMtx[9], modelMtx[10]));

        float normal3x3[9];
        for(int i = 0; i < 3; ++i) {
            normal3x3[i * 3 + 0] = modelMtx[i * 4 + 0] / sx;
            normal3x3[i * 3 + 1] = modelMtx[i * 4 + 1] / sy;
            normal3x3[i * 3 + 2] = modelMtx[i * 4 + 2] / sz;
        }

        std::string type = "u_" + program.name; // Since it's a map, gotta have unique keys
        Renderer::SetUniform(m_defaultUniformIds[type + "_normalMatrix"], normal3x3);

        bgfx::submit(program.viewId, program.program);
    }
}

void RenderCore::_DrawDebug(const Program& program, CameraComponent* camera, DebugModes debug) {
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
}

void RenderCore::_DrawBillboard(const Program& program) {
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
    }
}

void RenderCore::_DrawDbgBillboard(const Program& program) {
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
            Renderer::SetUniform(m_defaultUniformIds["u_dbg_billboard"], billboardData);

            // In addition, send the mode and rot as a vec4
            float billboardExtra[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // No rotation or special mode for gizmos
            Renderer::SetUniform(m_defaultUniformIds["u_dbg_billboard_mode"], billboardExtra);
            float lightingFlags[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // No lighting for gizmos
            Renderer::SetUniform(m_defaultUniformIds["u_dbg_billboard_lighting"], lightingFlags);

            // Dummy model matrix for the billboard
            float modelMtx[16];
            bx::mtxIdentity(modelMtx);

            bgfx::setState(state);
            bgfx::setTransform(modelMtx);
            bgfx::setVertexBuffer(0, m_billboardVbh);
            bgfx::setIndexBuffer(m_billboardIbh);
            bgfx::setTexture(
                0,
                Renderer::GetUniform(m_defaultUniformIds["s_dbg_bill_albedo"])->handle,
                gizmo->_GetTexture()); // Use the texture from the gizmo registry
            bgfx::submit(program.viewId, program.program);
        }
    }
}

void RenderCore::_DrawUIDebug(CameraComponent* camera) {
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

bool RenderCore::_RenderFrame(CameraComponent* camera, DebugModes debug) {
    if (m_isFirstFrame) {
        m_isFirstFrame = false;
        m_drender = Core::_GetApp()->physicsManager->_GetDebugRenderer();
    }

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
            // Remove translation for skybox
            bgfx::setViewRect(view, 0, 0, bgfx::BackbufferRatio::Equal);
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
                if (debug.Enabled && debug.Gizmos) _DrawDbgBillboard(program);
                break;
            case VIEW_BILLBOARD:
                _DrawBillboard(program);
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
