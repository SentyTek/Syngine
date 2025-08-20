// ╒═══════════════════════ Renderer.cpp ═╕
// │ Syngine                              │
// │ Created 2025-04-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Jolt/Core/Core.h"
#include "Syngine/Core/Registry.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/ECS/Components/MeshComponent.h"
#include "Syngine/Graphics/Shaders.h"
#include "Syngine/Graphics/TextureHelpers.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/Components/CameraComponent.h"
#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/Graphics/Windowing.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_properties.h>

#include <bgfx/platform.h>
#include "Syngine/Utils/Version.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/math.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <set>
#include <string_view>
#include <unordered_map>
#include <vector>

#if BX_PLATFORM_OSX
#include "MetalBridge.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Syngine {

std::string Renderer::m_title;
bool        Renderer::m_isReady = false;

SDL_Window* Renderer::win = nullptr;

std::map<std::string, Renderer::Gizmo> Renderer::m_gizmoRegistry;
bgfx::VertexBufferHandle     Renderer::m_billboardVbh = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle      Renderer::m_billboardIbh = BGFX_INVALID_HANDLE;

bgfx::VertexBufferHandle Renderer::dummy = BGFX_INVALID_HANDLE;
std::unordered_map<bgfx::ViewId, std::vector<Program>> Renderer::viewPrograms;
std::unordered_map<uint16_t, Uniform> Renderer::m_uniformRegistry;
std::unordered_map<std::string, uint16_t> Renderer::m_defaultUniformIds;

bgfx::TextureHandle       Renderer::m_shadowDepth = BGFX_INVALID_HANDLE;
bgfx::FrameBufferHandle   Renderer::m_shadowFB = BGFX_INVALID_HANDLE;

int                          Renderer::width        = 0;
int                          Renderer::height       = 0;
RendererConfig                Renderer::m_config;

Renderer::Renderer(int width, int height, const RendererConfig& config) {
    Renderer::width = width;
    Renderer::height = height;
    Renderer::win = Window::_GetSDLWindow();

    if (!_CreateRenderer(config)) {
        Syngine::Logger::Fatal("Failed to create renderer");
    }
};

Renderer::~Renderer() {
    if (!win) {
        Syngine::Logger::Error("No app to destroy renderer for");
        return;
    }

    // Default cleanup
    if(bgfx::isValid(m_billboardVbh)) {
        bgfx::destroy(m_billboardVbh);
        m_billboardVbh = BGFX_INVALID_HANDLE;
    }
    if(bgfx::isValid(m_billboardIbh)) {
        bgfx::destroy(m_billboardIbh);
        m_billboardIbh = BGFX_INVALID_HANDLE;
    }
    if(bgfx::isValid(Renderer::dummy)) {
        bgfx::destroy(Renderer::dummy);
        Renderer::dummy = BGFX_INVALID_HANDLE;
    }

    // Destroy all programs and uniforms (if not already destroyed)
    for (auto& view : Renderer::viewPrograms) {
        for (auto& prog : view.second) {
            for (auto& uniform : prog.uniforms) {
                if (bgfx::isValid(uniform.handle)) {
                    bgfx::destroy(uniform.handle);
                }
                if (uniform.data) {
                    free(uniform.data);
                    uniform.data = nullptr;
                }
            }
            bgfx::destroy(prog.program);
        }
    }
    Renderer::viewPrograms.clear();
    Renderer::m_defaultUniformIds.clear();
    Renderer::m_uniformRegistry.clear();

    // TODO: loop through renderer::viewprograms effectively (note each program
    // stores uniforms)

    // Clear gizmos
    for (auto& gizmo : m_gizmoRegistry) {
        if (bgfx::isValid(gizmo.second.texture)) {
            bgfx::destroy(gizmo.second.texture);
        }
    }
    m_gizmoRegistry.clear();

    bgfx::shutdown(); // Shut down bgfx BEFORE destroying the window
}

bool Renderer::_CreateRenderer(const RendererConfig& config) {
    m_config = config;
    if (!win) {
        Syngine::Logger::Fatal("No app to create renderer for");
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
    
    bgInit.resolution.width = width;
    bgInit.resolution.height = height;
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
    bgfx::setViewRect(0, 0, 0, uint16_t(width), uint16_t(height));
    
    bgfx::touch(0); // touch the view to clear it
    bgfx::frame(); // submit the frame
    SDL_ShowWindow(win); // show the window
    
    // Create default shaders
    m_isReady = true; // set true for now just to create default shaders
    size_t defaultProg = AddProgram("shaders/default", "default");
    if (defaultProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create default program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t textureProg = AddProgram("shaders/default_texture", "texture");
    if (textureProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create texture program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t debugProg =
        AddProgram("shaders/default_debug", "default_debugger", VIEW_DEBUG);
    if (debugProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create debug program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t debugBillboardProg =
        AddProgram("shaders/default_debug_billboard", "default_billboard", VIEW_BILL_DBG);
    if (debugBillboardProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create debug billboard program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t shadowProg =
        AddProgram("shaders/default_shadow", "shadow", VIEW_SHADOW);
    if (shadowProg == (size_t)-1) {
        Syngine::Logger::Error("Failed to create shadow program");
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
    m_isReady = false;

    // Create default uniforms
    m_defaultUniformIds.insert({ "u_billboard",
                                 RegisterUniform(debugBillboardProg,
                                                 "u_default_billboard",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert(
        { "s_bill_albedo",
          RegisterUniform(
              debugBillboardProg, "s_albedo", UniformType::UNIFORM_SAMPLER) });

    // Vertex program uniforms
    m_defaultUniformIds.insert({ "u_baseColor",
                                 RegisterUniform(defaultProg,
                                                 "u_baseColor",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_lightDir",
                                 RegisterUniform(defaultProg,
                                                 "u_lightDir",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_normalMatrix",
                                 RegisterUniform(defaultProg,
                                                 "u_normalMatrix",
                                                 UniformType::UNIFORM_MAT3) });

    m_defaultUniformIds.insert(
        { "u_default_shadowMap",
          RegisterUniform(
              textureProg, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });

    // Texture program uniforms
    m_defaultUniformIds.insert({ "u_texture_lightDir",
                                 RegisterUniform(textureProg,
                                                 "u_lightDir",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_floats",
                                 RegisterUniform(textureProg,
                                                 "u_floats",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_normalMatrix",
                                 RegisterUniform(textureProg,
                                                 "u_normalMatrix",
                                                 UniformType::UNIFORM_MAT3) });

    m_defaultUniformIds.insert(
        { "u_texture_albedo",
          RegisterUniform(
              textureProg, "s_albedo", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_normalMap",
          RegisterUniform(
              textureProg, "s_normalMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_heightMap",
          RegisterUniform(
              textureProg, "s_heightMap", UniformType::UNIFORM_SAMPLER) });
    m_defaultUniformIds.insert(
        { "u_texture_shadowMap",
          RegisterUniform(
              textureProg, "s_shadowMap", UniformType::UNIFORM_SAMPLER) });

    // Shadow program uniforms
    m_defaultUniformIds.insert({ "u_default_csmSplits",
                                 RegisterUniform(defaultProg,
                                                 "u_csmSplits",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_csmLightViewProj",
                                 RegisterUniform(defaultProg,
                                                 "u_csmLightViewProj",
                                                 UniformType::UNIFORM_MAT4, 4) });
    m_defaultUniformIds.insert({ "u_texture_csmSplits",
                                 RegisterUniform(textureProg,
                                                 "u_csmSplits",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_csmLightViewProj",
                                 RegisterUniform(textureProg,
                                                 "u_csmLightViewProj",
                                                 UniformType::UNIFORM_MAT4,
                                                 4) });
    m_defaultUniformIds.insert({ "u_texture_viewPos",
                                 RegisterUniform(textureProg,
                                                 "u_viewPos",
                                                 UniformType::UNIFORM_VEC4) });;
    m_defaultUniformIds.insert({ "u_default_viewPos",
                                 RegisterUniform(defaultProg,
                                                 "u_viewPos",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_texture_shadowParams",
                                 RegisterUniform(textureProg,
                                                 "u_shadowParams",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_default_shadowParams",
                                 RegisterUniform(defaultProg,
                                                 "u_shadowParams",
                                                 UniformType::UNIFORM_VEC4) });
    m_defaultUniformIds.insert({ "u_dbg",
                                 RegisterUniform(defaultProg,
                                                 "u_dbg",
                                                 UniformType::UNIFORM_VEC4) });

    // Initial sun direction in degrees (yaw, pitch, roll)
    // Stored as (yaw, pitch, roll) with pitch = degrees above horizon (positive = up).
    const float initialSunDir[3] = { 0.0f, 45.0f, 0.0f };
    float pitch = bx::toRad(initialSunDir[1]);
    float yaw   = bx::toRad(initialSunDir[0]);
    float cp    = cosf(pitch);
    float sp    = sinf(pitch);
    float cy    = cosf(yaw);
    float sy    = sinf(yaw);

    // y = +sin(pitch) when pitch is above horizon. (Ensure convention matches UI)
    bx::Vec3 dirVec = { cp * sy, sp, cp * cy };
    
    dirVec = bx::normalize(dirVec);

    float final[3] = { dirVec.x, dirVec.y, dirVec.z };
    SetSunDirection(final);

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
        SDL_DestroyWindow(win);
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
        Renderer::m_shadowDepth = bgfx::createTexture2D(
            SHADOW_MAP_SIZE * 2,
            SHADOW_MAP_SIZE * 2,
            false,
            1,
            bgfx::TextureFormat::D32,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                BGFX_SAMPLER_U_CLAMP |
                BGFX_SAMPLER_V_CLAMP); // I wish I knew what the flags meant.

        if (!bgfx::isValid(Renderer::m_shadowDepth)) {
            Syngine::Logger::Error("Failed to create shadow depth texture");
            return false;
        }

        Renderer::m_shadowFB = bgfx::createFrameBuffer(1, &Renderer::m_shadowDepth, true);
        if (!bgfx::isValid(Renderer::m_shadowFB)) {
            Syngine::Logger::Error("Failed to create shadow framebuffer");
            bgfx::destroy(Renderer::m_shadowDepth);
            return false;
        }
    }

    m_isReady = true;
    Window::_SetContextCreated(true);
    Syngine::Logger::Info("Renderer created successfully");
    return true;
}

size_t Renderer::AddProgram(const std::string& vsPath,
                            const std::string& fsPath,
                            const std::string& name,
                            Syngine::ViewID    viewId) {
    if (!Renderer::IsReady()) Syngine::Logger::Fatal("Cannot add program before renderer is ready");

    bgfx::ShaderHandle vs = _LoadShader(vsPath.c_str());
    bgfx::ShaderHandle fs = _LoadShader(fsPath.c_str());
    bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (bgfx::isValid(vs) && bgfx::isValid(fs)) {
        programHandle = bgfx::createProgram(vs, fs, true);
    }

    if (!bgfx::isValid(programHandle)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to create program %s", name.c_str());
        bgfx::destroy(vs);
        bgfx::destroy(fs);
        return -1;
    }

    Program prog;
    prog.program = programHandle;
    prog.name    = name;
    prog.viewId  = viewId;
    prog.vsPath  = vsPath;
    prog.fsPath  = fsPath;
    prog.id = programHandle.idx;

    viewPrograms[viewId].push_back(prog); // Store program by viewId
    Syngine::Logger::LogF(Syngine::LogLevel::INFO, "Program %s created successfully", name.c_str());
    return prog.id;
}

size_t Renderer::AddProgram(const std::string& path,
                            const std::string& name,
                            Syngine::ViewID    viewId) {
    return AddProgram(path + ".vert.sc.bin", path + ".frag.sc.bin", name, viewId);
}

Program Renderer::GetProgram(const std::string_view& name) {
    for (const auto& programs : viewPrograms) {
        for (const auto& program : programs.second) {
            if (program.name == name && bgfx::isValid(program.program)) {
                return program;
            }
        }
    }
    return Program();
}
Program Renderer::GetProgram(size_t id) {
    Program* prog = _GetProgram(id);
    if (prog) {
        return *prog;
    }
    return Program();
}
Program* Renderer::_GetProgram(size_t id) {
    for (auto& pair : viewPrograms) {
        for (auto& prog : pair.second) {
            if (prog.id == id) {
                return &prog;
            }
        }
    }
    return nullptr;
}

bool Renderer::RemoveProgram(Syngine::ViewID viewId, const std::string_view& name) {
    for (int i = 0; i < viewPrograms[viewId].size(); ++i) {
        if (viewPrograms[viewId][i].name == name) {
            // Destroy all uniforms associated with this program
            for (auto& uniform : viewPrograms[viewId][i].uniforms) {
                bgfx::destroy(uniform.handle);
                if (uniform.data) {
                    free(uniform.data);
                }
            }
            bgfx::destroy(viewPrograms[viewId][i].program);
            viewPrograms[viewId].erase(viewPrograms[viewId].begin() + i);
            return true;
        }
    }
    return false;
}
bool Renderer::RemoveProgram(Syngine::ViewID viewId, size_t id) {
    if (id < viewPrograms[viewId].size()) {
        // Destroy all uniforms associated with this program
        for (auto& uniform : viewPrograms[viewId][id].uniforms) {
            bgfx::destroy(uniform.handle);
            if (uniform.data) {
                free(uniform.data);
            }
        }
        bgfx::destroy(viewPrograms[viewId][id].program);
        viewPrograms[viewId].erase(viewPrograms[viewId].begin() + id);
        return true;
    }
    return false;
}

bool Renderer::RemoveAllPrograms() {
    for (auto& programs : viewPrograms) {
        for (auto& program : programs.second) {
            // Destroy all uniforms associated with this program
            for (auto& uniform : program.uniforms) {
                bgfx::destroy(uniform.handle);
                if (uniform.data) {
                    free(uniform.data);
                }
            }
            bgfx::destroy(program.program);
        }
        programs.second.clear();
    }
    return true;
}

bool Renderer::ReloadProgram(Syngine::ViewID viewId, const std::string_view& name) {
    for (auto& prog : viewPrograms[viewId]) {
        if (prog.name == name) {
            bgfx::ShaderHandle vs = _LoadShader(prog.vsPath.c_str());
            bgfx::ShaderHandle fs = _LoadShader(prog.fsPath.c_str());
            bgfx::ProgramHandle newProgram = bgfx::createProgram(vs, fs, true);

            if (!bgfx::isValid(newProgram)) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to reload program %s", name.data());
                bgfx::destroy(vs);
                bgfx::destroy(fs);
                return false;
            }

            bgfx::destroy(prog.program); // Destroy old program
            prog.program = newProgram;   // Update to new program
            return true;
        }
    }
    return false;
}

bool Renderer::ReloadAllPrograms() {
    for (auto& programs : viewPrograms) {
        for (auto& prog : programs.second) {
            bgfx::ShaderHandle vs = _LoadShader(prog.vsPath.c_str());
            bgfx::ShaderHandle fs = _LoadShader(prog.fsPath.c_str());
            bgfx::ProgramHandle newProgram = bgfx::createProgram(vs, fs, true);

            if (!bgfx::isValid(newProgram)) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to reload program %s", prog.name.c_str());
                bgfx::destroy(vs);
                bgfx::destroy(fs);
                return false;
            }

            bgfx::destroy(prog.program); // Destroy old program
            prog.program = newProgram;   // Update to new program
        }
    }
    return true;
}

bool Renderer::IsReady() {
    return m_isReady;
}

size_t Renderer::RegisterUniform(int                program,
                                 const std::string& name,
                                 UniformType        type,
                                 uint16_t           num) {
    Program* prog = _GetProgram(program);
    if (prog) {
        Uniform u = {
            .handle = bgfx::createUniform(
                name.c_str(), static_cast<bgfx::UniformType::Enum>(type), num),
                      .type = type,
                      .name = name,
                      .num  = num };

        // Allocate memory based on uniform type
        size_t size = 0;
        switch (type) {
        case UNIFORM_SAMPLER:
            u.data = nullptr; // Samplers don't need data storage
            break;
        case UNIFORM_VEC4:
            size = sizeof(float) * 4;
            break;
        case UNIFORM_MAT3:
            size = sizeof(float) * 9;
            break;
        case UNIFORM_MAT4:
            size = sizeof(float) * 16;
            break;
        default:
            u.data = nullptr;
            break;
        }

        if (size > 0) {
            u.data = malloc(size * num);
            memset(u.data, 0, size * num);
        }

        prog->uniforms.push_back(u);
        // Store in uniform registry for fast lookup
        m_uniformRegistry[u.handle.idx] = u;
        return u.handle.idx;
    }
    return 0;
}

Uniform* Renderer::_GetUniform(uint16_t id) {
    auto it = m_uniformRegistry.find(id);
    if (it != m_uniformRegistry.end()) {
        return &it->second;
    }
    return nullptr;
}

void Renderer::SetUniform(uint16_t id, const void* data, uint16_t num) {
    Uniform* u = _GetUniform(id);
    if (u && bgfx::isValid(u->handle)) {
        size_t size = 0;
        switch (u->type) {
        case UNIFORM_SAMPLER:
            SDL_Log("Cannot set texture uniform. Use bgfx::setTexture instead.");
            break;
        case UNIFORM_VEC4:
            size = sizeof(float) * 4;
            bgfx::setUniform(u->handle, static_cast<const float*>(data), num);
            break;
        case UNIFORM_MAT3:
            size = sizeof(float) * 9;
            bgfx::setUniform(u->handle, static_cast<const float*>(data), num);
            break;
        case UNIFORM_MAT4:
            size = sizeof(float) * 16;
            bgfx::setUniform(u->handle, static_cast<const float*>(data), num);
            break;
        default:
            Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Unknown uniform type");
            break;
        }

        if (size > 0) {
            memcpy(u->data, data, size * num);
        }
    }
}

void Renderer::_RegisterGizmo(const std::string& tag, float size) {
    if (this->m_gizmoRegistry.find(tag) != this->m_gizmoRegistry.end())
        return; // Gizmo already registered

    std::string resolvedPath = Syngine::_ResolveOSPath((std::string("default/gizmos/") + tag + ".png").c_str());
    const char* path = resolvedPath.c_str();
    bgfx::TextureHandle texture = Syngine::LoadTextureFromFile(path);
    
    if (bgfx::isValid(texture)) {
        m_gizmoRegistry[tag] = { texture, size };
    }
}

void Renderer::GetSunDirection(float* outDir) {
    Uniform* u = _GetUniform(m_defaultUniformIds["u_default_lightDir"]);
    if (u && u->data) {
        float* dir = static_cast<float*>(u->data);
        outDir[0] = dir[0];
        outDir[1] = dir[1];
        outDir[2] = dir[2];
    } else {
        // If the uniform is not found, default to a downward light direction
        outDir[0] = 0.0f;
        outDir[1] = -1.0f;
        outDir[2] = 0.0f;
    }
}

void Renderer::SetSunDirection(const float* lightDir) {
    float dir[4] = { lightDir[0], lightDir[1], lightDir[2], 0.0f };
    // Loop over every uniform with "u_lightDir" as the name and set its value
    for (auto& pair : m_uniformRegistry) {
        Uniform& uniform = pair.second;
        const std::string& name = uniform.name;
        if (name.find("u_lightDir") != std::string::npos && bgfx::isValid(uniform.handle)) {
            SetUniform(pair.first, dir);
        }
    }
}

void Renderer::_CalculateCascadeMatrices(CameraComponent* camera,
                                         float*           outLightView,
                                         float*           outLightProj,
                                         float*           outCascadeSplits) {
    float lightDir[3];
    GetSunDirection(lightDir);
    const bx::Vec3 lightDirVec = bx::normalize(bx::Vec3(lightDir[0], lightDir[1], lightDir[2]));
    
    const float* camPos = camera->GetPosition();

    // Currently using fixed cascade sizes based on shadow distance
    float cascadeSizes[NUM_CASCADES] = {
        10, 25, round(m_config.shadowDist / 3), round(m_config.shadowDist)
    };

    float cascadeDistances[NUM_CASCADES];
    float lightViewProj[NUM_CASCADES * 16];
    
    for (uint32_t i = 0; i < NUM_CASCADES; i++) {
        float size = cascadeSizes[i];

        // How far light is from camera
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
    
    SetUniform(m_defaultUniformIds["u_default_csmLightViewProj"], lightViewProj, NUM_CASCADES);
    SetUniform(m_defaultUniformIds["u_texture_csmLightViewProj"], lightViewProj, NUM_CASCADES);
}

/*
--- Drawing functions ---
*/

void Renderer::_DrawShadows(const Program& program, CameraComponent* camera, uint8_t cascade) {
    const uint64_t renderState =
        BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA | BGFX_STATE_CULL_CCW;

    if (Core::_GetApp()->debug.CSMBounds) {
        float view[16 * NUM_CASCADES], proj[16 * NUM_CASCADES], outCascadeSplits[NUM_CASCADES];
        _CalculateCascadeMatrices(camera, view, proj, outCascadeSplits);
        for (int i = 0; i < NUM_CASCADES; ++i) {
            Core::_GetApp()->physicsManager->_DrawOtherFrustum(view + i * 16, proj + i * 16);
        }

    }

    std::vector<GameObject*> gameObjects = Registry::GetRenderableObjects();

    for (auto& gameObject : gameObjects) {
        if (!gameObject) continue;

        MeshData meshData = gameObject->GetComponent<MeshComponent>()->meshData;
        if (!meshData.valid || !gameObject->GetComponent<MeshComponent>()->isEnabled) continue;

        // Get the transform for this object
        float modelMtx[16];
        gameObject->GetComponent<TransformComponent>()->GetModelMatrix(modelMtx);
        bgfx::setTransform(modelMtx);

        bgfx::setState(renderState);
        bgfx::setVertexBuffer(0, meshData.vbh);
        bgfx::setIndexBuffer(meshData.ibh);

        // Shadow shaders are simple, just output depth
        bgfx::submit(program.viewId + cascade, program.program);
    }
}

void Renderer::_DrawSky(const Program& program) {
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

void Renderer::_DrawForward(const Program& program, CameraComponent* camera) {
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
        MeshData meshData = gameObject->GetComponent<MeshComponent>()->meshData;
        if (!meshData.valid || !gameObject->GetComponent<MeshComponent>()->isEnabled) continue;
        
        bgfx::setState(renderState);
        Material& mat = meshData.materials[0];
        // Default vertex color shader
        if (program.name == "default") {
            SetUniform(m_defaultUniformIds["u_baseColor"], mat.baseColor);
        }
        // Default textured shader
        else if (program.name == "texture") {
            float u_floats[4] = {
                mat.heightScale,
                mat.mixFactor,
                mat.ambient,
                mat.tileDetail,
            };
            SetUniform(m_defaultUniformIds["u_floats"], u_floats);

            bgfx::setTexture(
                0,
                _GetUniform(m_defaultUniformIds["u_texture_albedo"])->handle,
                mat.albedo,
                samplerFlags);
            bgfx::setTexture(
                1,
                _GetUniform(m_defaultUniformIds["u_normalMap"])->handle,
                mat.normalMap,
                samplerFlags);
            bgfx::setTexture(
                2,
                _GetUniform(m_defaultUniformIds["u_heightMap"])->handle,
                mat.heightMap,
                samplerFlags);
        }
        // Probably a custom shader
        else {
            Program shader = GetProgram(gameObject->type.c_str());
            if(!bgfx::isValid(shader.program)) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Program %s not found", gameObject->type.c_str());
                continue;
            }
        }

        if (m_config.useShadows) {
            // Set shadow map for the texture program
            const uint64_t depthSampler =
                BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT |
                BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
            std::string name = "u_" + program.name + "_shadowMap";
            bgfx::setTexture(
                3,
                _GetUniform(m_defaultUniformIds[name])->handle,
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
        SetUniform(m_defaultUniformIds[type + "_normalMatrix"], normal3x3);

        bgfx::submit(program.viewId, program.program);
    }
}

void Renderer::_DrawDebug(const Program& program, CameraComponent* camera, DebugModes debug) {
    Core::_GetApp()->physicsManager->_DrawDebug(
        width,
        height,
        program.program,
        Registry::GetGameObjectByName("player")
            ->GetComponent<CameraComponent>()
            ->GetCamera(),
        camera->GetCamera(), debug);
}

void Renderer::_DrawBillboard(const Program& program) {
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_BLEND_ALPHA | BGFX_STATE_DEPTH_TEST_LEQUAL);

    std::vector<GameObject*> gizmos = Registry::GetGizmos();
    for (auto go : gizmos) {
        auto it = m_gizmoRegistry.find(go->gizmo);
        if (it != m_gizmoRegistry.end()) {
            auto* comp = go->GetComponent<Syngine::CameraComponent>();
            if (!comp) continue;

            const Gizmo& gizmo = it->second;
            const float* pos   = comp->GetPosition();

            // Pack center position and size into a vec4 and send it off
            float billboardData[4] = { pos[0], pos[1], pos[2], gizmo.size };
            SetUniform(m_defaultUniformIds["u_billboard"], billboardData);

            // Dummy model matrix for the billboard
            float modelMtx[16];
            bx::mtxIdentity(modelMtx);

            bgfx::setTransform(modelMtx);
            bgfx::setVertexBuffer(0, m_billboardVbh);
            bgfx::setIndexBuffer(m_billboardIbh);
            bgfx::setTexture(
                0,
                _GetUniform(m_defaultUniformIds["s_bill_albedo"])->handle,
                gizmo.texture); // Use the texture from the gizmo registry
            bgfx::submit(program.viewId, program.program);
        }
    }
}

void Renderer::_DrawUIDebug(CameraComponent* camera) {
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

bool Renderer::_RenderFrame(CameraComponent* camera, DebugModes debug) {
    float lightView[NUM_CASCADES * 16];
    float lightProj[NUM_CASCADES * 16];
    float cascadeSplits[NUM_CASCADES];

    CameraComponent* pcam = Registry::GetGameObjectByName("player")
                                ->GetComponent<CameraComponent>();

    const float* camPos = camera->GetPosition();
    float        viewPos[4] = { camPos[0], camPos[1], camPos[2], 1.0f };
    SetUniform(m_defaultUniformIds["u_default_viewPos"], viewPos);
    SetUniform(m_defaultUniformIds["u_texture_viewPos"], viewPos);

    float sunDir[3];
    GetSunDirection(sunDir); // returns normalized world-space direction TO the light (direction-to-light)
    float lightDirUniform[4] = { sunDir[0], sunDir[1], sunDir[2], 0.0f };
    SetUniform(m_defaultUniformIds["u_default_lightDir"], lightDirUniform);
    SetUniform(m_defaultUniformIds["u_texture_lightDir"], lightDirUniform);

    if (m_config.useShadows) {
        _CalculateCascadeMatrices(camera, lightView, lightProj, cascadeSplits);

        float farClip = m_config.shadowDist;
        float splits[4] = { cascadeSplits[0],
                            cascadeSplits[1],
                            cascadeSplits[2],
                            cascadeSplits[3] };
        SetUniform(m_defaultUniformIds["u_default_csmSplits"], splits);
        SetUniform(m_defaultUniformIds["u_texture_csmSplits"], splits);

        float shadowParams[4] = {
            bgfx::getCaps()->homogeneousDepth ? 1.0f
                                              : 0.0f, // Homogeneous depth
            1.0f / (float)(SHADOW_MAP_SIZE * 2),      // Inv shadow map size
            m_config.shadowDist, // Light far plane
            0.0f // Debug value, not used
        };
        SetUniform(m_defaultUniformIds["u_default_shadowParams"], shadowParams);
        SetUniform(m_defaultUniformIds["u_texture_shadowParams"], shadowParams);
    }

    // Update main camera matrices
    for (Syngine::ViewID view : _allViews) {
        switch (view) {
        case ViewID::VIEW_SHADOW: {
            if (!m_config.useShadows) break; // Skip if shadows are disabled
            
            for (uint8_t i = 0; i < NUM_CASCADES; ++i) {
                bgfx::ViewId cascadeViewId = view + i;
                bgfx::setViewFrameBuffer(cascadeViewId, Renderer::m_shadowFB);
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
            camera->Update(view, width, height);
            Camera cam = camera->GetCamera();
            float  skyView[16];
            bx::memCopy(skyView, cam.view, sizeof(skyView));
            skyView[12] = skyView[13] = skyView[14] = 0.0f;
            bgfx::setViewTransform(view, skyView, cam.proj);
            break;
        }
        default: {
            bgfx::setViewRect(view, 0, 0, uint16_t(width), uint16_t(height));
            Camera cam = camera->GetCamera();
            bgfx::setViewTransform(view, cam.view, cam.proj);
            break;
        }
        }
    }
    
    // Main render loop
    for (auto view : _allViews) {
        auto progListIt = viewPrograms.find(view);
        if (progListIt == viewPrograms.end()) continue;
        
        for (auto& program : progListIt->second) {
            if (!bgfx::isValid(program.program)) continue;
            
            // Draw logic based on view type
            switch (view) {
            case VIEW_SHADOW:
                if (m_config.useShadows) {
                    for (uint8_t i = 0; i < NUM_CASCADES; ++i) {
                        _DrawShadows(program, pcam, i);
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
                if (debug.Enabled &&debug.Gizmos) _DrawBillboard(program);
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
