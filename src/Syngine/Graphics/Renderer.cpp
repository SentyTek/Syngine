// ╒═══════════════════════ Renderer.cpp ═╕
// │ Syngine                              │
// │ Created 2025-04-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Registry.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/Graphics/Renderer.h"
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
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/math.h"

#include <cstdint>
#include <string_view>
#include <vector>

#if BX_PLATFORM_OSX
#include "MetalBridge.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Syngine {

std::string Renderer::m_title;
bool        Renderer::m_isReady = false;

SDL_Window* Renderer::win = nullptr;
Syngine::Handles Renderer::handles;

std::map<std::string, Gizmo> Renderer::m_gizmoRegistry;
bgfx::VertexBufferHandle     Renderer::m_billboardVbh = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle      Renderer::m_billboardIbh = BGFX_INVALID_HANDLE;

int                          Renderer::width        = 0;
int                          Renderer::height       = 0;

Renderer::Renderer(int width, int height) {
    Renderer::width = width;
    Renderer::height = height;
    Renderer::win = Window::_GetSDLWindow();

    if (!_CreateRenderer()) {
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
    if(bgfx::isValid(handles.dummy)) {
        bgfx::destroy(handles.dummy);
        handles.dummy = BGFX_INVALID_HANDLE;
    }

    // Destroy all programs and uniforms
    for (auto& program : handles.programs) {
        bgfx::destroy(program.program);
        program.program = BGFX_INVALID_HANDLE;
    }
    handles.programs.clear();

    for (auto& uniform : handles.uniforms) {
        if(bgfx::isValid(uniform.second) && uniform.second.idx != 0) {
            bgfx::destroy(uniform.second);
        }
    }
    handles.uniforms.clear();

    // Clear gizmos
    for (auto& gizmo : m_gizmoRegistry) {
        if (bgfx::isValid(gizmo.second.texture)) {
            bgfx::destroy(gizmo.second.texture);
        }
    }
    m_gizmoRegistry.clear();

    bgfx::shutdown(); // Shut down bgfx BEFORE destroying the window
}

bool Renderer::_CreateRenderer() {
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

    // Create uniforms and store them in a map by name
    handles.uniforms["s_albedo"]        = bgfx::createUniform("s_albedo", bgfx::UniformType::Sampler);
    handles.uniforms["s_normalMap"]     = bgfx::createUniform("s_normalMap", bgfx::UniformType::Sampler);
    handles.uniforms["s_heightMap"]     = bgfx::createUniform("s_heightMap", bgfx::UniformType::Sampler);

    handles.uniforms["u_normalMatrix"]  = bgfx::createUniform("u_normalMatrix", bgfx::UniformType::Mat3);
    handles.uniforms["u_lightDir"]      = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    handles.uniforms["u_floats"]        = bgfx::createUniform("u_floats", bgfx::UniformType::Vec4);
    handles.uniforms["u_baseColor"]     = bgfx::createUniform("u_baseColor", bgfx::UniformType::Vec4);

    handles.uniforms["u_skyColorDay"]   = bgfx::createUniform("u_skyColorDay", bgfx::UniformType::Vec4);
    handles.uniforms["u_skyColorNight"] = bgfx::createUniform("u_skyColorNight", bgfx::UniformType::Vec4);
    handles.uniforms["u_sunColorDay"]   = bgfx::createUniform("u_sunColorDay", bgfx::UniformType::Vec4);
    handles.uniforms["u_sunColorRise"]  = bgfx::createUniform("u_sunColorRise", bgfx::UniformType::Vec4);

    handles.uniforms["u_billboard"] = bgfx::createUniform("u_billboard", bgfx::UniformType::Vec4);
    
    bgfx::touch(0); // touch the view to clear it
    bgfx::frame(); // submit the frame
    SDL_ShowWindow(win); // show the window
    
    // Create default shaders
    m_isReady = true; // set true for now just to create default shaders
    size_t defaultProg = AddProgram("shaders/default.vert.sc.bin",
                                    "shaders/default.frag.sc.bin",
                                    "default");
    if (defaultProg == (size_t)-1) {
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t debugProg = AddProgram("shaders/debug.vert.sc.bin",
                                  "shaders/debug.frag.sc.bin",
                                  "debugger");
    if (debugProg == (size_t)-1) {
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }

    size_t billboardProg = AddProgram("shaders/billboard.vert.sc.bin",
                                      "shaders/billboard.frag.sc.bin",
                                      "billboard");
    if (billboardProg == (size_t)-1) {
        bgfx::shutdown();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return false;
    }
    m_isReady = false;

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

        handles.dummy = fullscreenDummyVBH;
    }
#endif

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

    handles.programs.push_back(prog);
    Syngine::Logger::LogF(Syngine::LogLevel::INFO, "Program %s created successfully", name.c_str());
    return handles.programs.size() - 1; // return the index of the new program
}

size_t Renderer::AddProgram(const std::string& path,
                            const std::string& name,
                            Syngine::ViewID    viewId) {
    if (!Renderer::IsReady()) Syngine::Logger::Fatal("Cannot add program before renderer is ready");

    // Load the shader from the given path
    bgfx::ShaderHandle vs = _LoadShader((path + ".vert.sc.bin").c_str());
    bgfx::ShaderHandle fs = _LoadShader((path + ".frag.sc.bin").c_str());

    if (!bgfx::isValid(vs) || !bgfx::isValid(fs)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to load shaders for program %s", name.c_str());
        return -1;
    }

    bgfx::ProgramHandle programHandle = bgfx::createProgram(vs, fs, true);
    if (!bgfx::isValid(programHandle)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to create program %s", name.c_str());
        bgfx::destroy(vs);
        bgfx::destroy(fs);
        return -1;
    }

    Program prog;
    prog.program = programHandle;
    prog.name = name;
    prog.viewId  = viewId;
    prog.vsPath  = std::string(path) + ".vert.sc.bin";
    prog.fsPath  = std::string(path) + ".frag.sc.bin";

    handles.programs.push_back(prog);
    Syngine::Logger::LogF(Syngine::LogLevel::INFO, "Program %s created successfully", name.c_str());
    return handles.programs.size() - 1; // return the index of the new program
}

Program Renderer::GetProgram(const std::string_view& name) {
    for (const auto& program : handles.programs) {
        if (program.name == name && bgfx::isValid(program.program)) {
            return program;
        }
    }
    Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Program %s not found", name.data());
    return Program();
}

bool Renderer::RemoveProgram(int index) {
    if (index < 0 || index >= handles.programs.size()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Invalid program index: %zu when attempting to remove shader program", index);
        return false;
    }
    bgfx::destroy(handles.programs[index].program);
    handles.programs.erase(handles.programs.begin() + index);
    return true;
}
bool Renderer::RemoveProgram(const std::string_view& name) {
    for (int i = 0; i < handles.programs.size(); ++i) {
        if (handles.programs[i].name == name) {
            return RemoveProgram(i);
        }
    }
    Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Program %s not found when attempting to remove shader program", name);
    return false;
}
bool Renderer::RemoveAllPrograms() {
    for (auto& program : handles.programs) {
        if (!bgfx::isValid(program.program)) {
            continue; // Skip invalid programs
        }
        bgfx::destroy(program.program);
    }
    handles.programs.clear();
    return true;
}

bool Renderer::ReloadProgram(const std::string_view& name) {
    for (auto& prog : handles.programs) {
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
            Syngine::Logger::LogF(Syngine::LogLevel::INFO, "Program %s reloaded successfully", name.data());
            return true;
        }
    }
    return false;
}

bool Renderer::ReloadAllPrograms() {
    Syngine::Logger::Info("Reloading all shader programs...");
    for (const auto& prog : handles.programs) { // Are we sure looping through every program several times is okay?
        if (!ReloadProgram(prog.name.data())) {
            Syngine::Logger::Error("Failed to reload all programs, see previous errors for details");
            return false;
        }
    }
    return true;
}

bool Renderer::IsReady() {
    return m_isReady;
}

bgfx::UniformHandle Renderer::_GetUniform(const std::string_view& name) const {
    auto it = this->handles.uniforms.find(name.data());
    if (it != this->handles.uniforms.end()) {
        return it->second;
    }
    return BGFX_INVALID_HANDLE;
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

void Renderer::_RenderGizmos(CameraComponent* camera) {
    unsigned short viewId = 26; // View ID for gizmos
    bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);
    bgfx::setViewTransform(
        viewId, camera->GetCamera().view, camera->GetCamera().proj);
    bgfx::touch(viewId);

    Program billboardProgram = GetProgram("billboard");
    if(!bgfx::isValid(billboardProgram.program)) {
        Syngine::Logger::Error("Billboard program not found");
        return;
    }

    bgfx::UniformHandle billboardUniform = _GetUniform("u_billboard");
    if (!bgfx::isValid(billboardUniform)) {
        Syngine::Logger::Error("Billboard uniform not found");
        return;
    }

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_BLEND_ALPHA | BGFX_STATE_DEPTH_TEST_LEQUAL);

    // Get all gizmos from the registry
    std::vector<GameObject*> gizmos = Registry::GetGizmos();

    for (auto go : gizmos) {
        auto it = m_gizmoRegistry.find(go->gizmo);
        if (it != m_gizmoRegistry.end()) {
            auto* comp =
                go->GetComponent<Syngine::CameraComponent>();
            if (comp) {
                const Gizmo& gizmo = it->second;
                const float* pos   = comp->GetPosition();

                // Pack center position and size into a vec4
                float billboardData[4] = { pos[0], pos[1], pos[2], gizmo.size };
                bgfx::setUniform(billboardUniform, billboardData);

                // Dummy model matrix for billboard
                float modelMtx[16];
                bx::mtxIdentity(modelMtx);

                // And finally render
                bgfx::setTransform(modelMtx);
                bgfx::setVertexBuffer(0, this->m_billboardVbh);
                bgfx::setIndexBuffer(this->m_billboardIbh);
                bgfx::setTexture(0, _GetUniform("s_albedo"), gizmo.texture);
                bgfx::submit(viewId, billboardProgram.program);
            }
        }
    }
}

int Renderer::_RenderFrame(bx::Vec3& lightDir, CameraComponent* camera, bool debug) {
    const Program& terrainProgram = GetProgram("terrain");
    const Program& skyProgram = GetProgram("sky");
    const Program& defaultProgram = GetProgram("default");
    if (!bgfx::isValid(terrainProgram.program) || !bgfx::isValid(skyProgram.program) || !bgfx::isValid(defaultProgram.program)) {
        Syngine::Logger::Error("Invalid program");
        return -1;
    }

    const bgfx::ViewId SKY_VIEW = skyProgram.viewId;
    const bgfx::ViewId MAIN_VIEW = terrainProgram.viewId;
    camera->Update(SKY_VIEW,
                   this->width,
                   this->height); // update camera view and projection matrices

    Camera camObj = camera->GetCamera();
    
    //sky pass
    bgfx::setViewRect(SKY_VIEW, 0, 0, uint16_t(this->width), uint16_t(this->height));
    bgfx::setViewClear(SKY_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.0f, 0, 1, 0);
    bgfx::touch(SKY_VIEW);
    bgfx::setViewTransform(MAIN_VIEW, camObj.view, camObj.proj);
    bgfx::touch(MAIN_VIEW);

    float skyView[16];
    bx::memCopy(skyView, camObj.view, sizeof(skyView));
    skyView[12] = 0.0f; //remove translation for skybox
    skyView[13] = 0.0f;
    skyView[14] = 0.0f;

    bgfx::setViewTransform(SKY_VIEW, skyView, camObj.proj);

    float skyColorDay[4] = { 0.5f, 0.7f, 1.0f, 1.0f };
    float skyColorNight[4] = { 0.0f, 0.0f, 0.1f, 1.0f };
    float sunColorDay[4] = { 1.0f, 1.0f, 0.9f, 1.0f };
    float sunColorRise[4] = { 0.77f, 0.39f, 0.14f, 1.0f };
    bgfx::setUniform(handles.uniforms["u_skyColorDay"], skyColorDay);
    bgfx::setUniform(handles.uniforms["u_skyColorNight"], skyColorNight);
    bgfx::setUniform(handles.uniforms["u_sunColorDay"], sunColorDay);
    bgfx::setUniform(handles.uniforms["u_sunColorRise"], sunColorRise);

    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_DEPTH_TEST_LEQUAL | BGFX_STATE_MSAA);

#if BX_PLATFORM_OSX
    bgfx::setVertexBuffer(0, handles.dummy);
#else
    bgfx::setVertexCount(3);
#endif
    bgfx::submit(SKY_VIEW, skyProgram.program);

    //main scene pass
    //bgfx::setViewClear(MAIN_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);
    bgfx::setViewRect(MAIN_VIEW, 0, 0, uint16_t(this->width), uint16_t(this->height));
    //prepare render
    uint64_t renderState = BGFX_STATE_DEFAULT | BGFX_STATE_MSAA | BGFX_STATE_FRONT_CCW | BGFX_STATE_CULL_CW;


    const uint32_t samplerFlags =
        BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;

    std::vector<GameObject*> gameObjects = Registry::GetRenderableObjects();

    for (auto& gameObject : gameObjects) {
        if (!gameObject) {
            Syngine::Logger::Error("GameObject is null");
            continue;
        }
        if (!gameObject->HasComponent(Syngine::SYN_COMPONENT_TRANSFORM) || (!gameObject->HasComponent(Syngine::SYN_COMPONENT_MESH)) || !(gameObject->GetComponent<TransformComponent>())->isEnabled) {
            continue;
        }
        MeshData mesh = gameObject->GetComponent<MeshComponent>()->meshData;
        if (!mesh.valid) continue; // Skip invalid meshes
        
        bool isDefault = false;
        Program currentProg;
        if (gameObject->type != "default") { 
            currentProg = GetProgram(gameObject->type.c_str());
            if (!bgfx::isValid(currentProg.program)) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Program %s not found", gameObject->type.c_str());
                continue;
            }
        } else {
            isDefault = true;
            currentProg = defaultProgram;
        }
        
        bgfx::setState(renderState);

        if(gameObject->HasComponent(Syngine::SYN_COMPONENT_MESH) && gameObject->GetComponent<MeshComponent>()->isEnabled) {
            //TODO: add support for multiple materials
            if(!bgfx::isValid(mesh.ibh) || !bgfx::isValid(mesh.vbh)) {
                Syngine::Logger::Error("Invalid mesh");
                continue;
            }
            
            if(mesh.materials.empty()) {
                Syngine::Logger::LogF(Syngine::LogLevel::WARN, "GameObject %s has no materials, using default material", gameObject->name.c_str());
                if (gameObject->type == "default") {
                    float color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
                    bgfx::setUniform(handles.uniforms["u_baseColor"], color);
                } else {
                    continue; // Other types absolutely require materials
                }
            } else {
                Material& currentMat = mesh.materials[0]; // TODO: support multiple materials
                if (!isDefault) {
                    //Set textures
                    if (bgfx::isValid(currentMat.albedo)) 
                        bgfx::setTexture(0, handles.uniforms["s_albedo"], currentMat.albedo, samplerFlags);
                    if (bgfx::isValid(currentMat.normalMap))
                        bgfx::setTexture(1, handles.uniforms["s_normalMap"], currentMat.normalMap, samplerFlags);
                    if (bgfx::isValid(currentMat.heightMap))
                        bgfx::setTexture(2, handles.uniforms["s_heightMap"], currentMat.heightMap, samplerFlags);

                    //Set floats
                    float u_floats[4] = {
                        currentMat.heightScale,
                        currentMat.mixFactor,
                        currentMat.ambient,
                        currentMat.tileDetail
                    };
                    bgfx::setUniform(handles.uniforms["u_floats"], u_floats);
                } else {
                    // Set base color for default shader
                    bgfx::setUniform(handles.uniforms["u_baseColor"], currentMat.baseColor);
                }
            }
            
            bgfx::setVertexBuffer(0, mesh.vbh);
            bgfx::setIndexBuffer(mesh.ibh);
        }
        
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

        bgfx::setUniform(handles.uniforms["u_normalMatrix"], normal3x3);
        bgfx::setUniform(handles.uniforms["u_lightDir"], &lightDir);

        bgfx::submit(MAIN_VIEW, currentProg.program);
    }

    // Render gizmos
    if (debug) {
        _RenderGizmos(camera);
    }

    bgfx::frame(); // submit the frame
    return 0;
}

} // namespace Syngine