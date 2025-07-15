#include "SyngineGraphics.h"
#include "Components.h"
#include "Components/CameraComponent.h"
#include "ShaderUtils.h"
#include "SynModelLoader.h"
#include "TransformComponent.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/math.h"
#include "SynComponents.h"
#include "helpers.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_properties.h>

#include <bgfx/platform.h>
#include <cstdint>
#include <vector>

#if BX_PLATFORM_OSX
#include "SynOSXMetalBridge.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

using namespace Syngine;

Graphics::Graphics(const char* title, int width, int height) {
    //initialize app
    this->title = title;
    this->width = width;
    this->height = height;
    this->win = nullptr;
};

int Graphics::CreateGameWindow() {
    //bgInit
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return 1;
    }

    //check if app is initialized
    if (this->title == "") {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to create window for");
        return 1;
    }

    //create window
    this->win = SDL_CreateWindow(
        this->title.c_str(),
        this->width, this->height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN
    );
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

        
    //macos requires some kind of renderer to be created before metal is initialized
    #if BX_PLATFORM_OSX
    SDL_Renderer* renderer = SDL_CreateRenderer(this->win, NULL);
    SDL_DestroyRenderer(renderer);
    #endif
    
    //SDL_ShowWindow(win);

    //return window
    return 0;
}

int Graphics::CreateRenderer() {
    //create renderer
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to create renderer for");
        return 1;
    }

    // Check if required folders exist (shaders, meshes)
    if (!CheckRequiredFolders()) {
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }

    //initialize bgfx
    bgfx::Init bgInit;
    SDL_PropertiesID sdlProps = SDL_GetWindowProperties(this->win);
    if(sdlProps == 0) { //return 0 on failure
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    
    //set platform data
    #if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    //this assumes X11
    bgInit.platformData.ndt = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
    bgInit.platformData.nwh = (void*)SDL_GetNumberProperty(sdlProps, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    if (bgInit.platformData.nwh == NULL || bgInit.platformData.ndt == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties Linux");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    #elif BX_PLATFORM_OSX
    bgInit.type = bgfx::RendererType::Metal; // set renderer type to Metal
    bgInit.platformData.ndt = NULL; //only needed on x11
    bgInit.platformData.nwh = GetSYNMetalView(this->win); //custom obj-c function to get the metal view
    if (bgInit.platformData.nwh == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties macOSX");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    #elif BX_PLATFORM_WINDOWS
    bgInit.type = bgfx::RendererType::Direct3D12;
    bgInit.platformData.ndt = nullptr; //only needed on x11
    bgInit.platformData.nwh = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    if (bgInit.platformData.nwh == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get window properties windows");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    #else
    #warning "Platform not supported
    bgInit.platformData.ndt = NULL;
    bgInit.platformData.nwh = NULL;
    #endif
    
    bgInit.resolution.width = this->width;
    bgInit.resolution.height = this->height;
    bgInit.resolution.reset |= BGFX_RESET_VSYNC; // enable vsync
    bgInit.debug = true;

    if(!bgfx::init(bgInit)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize bgfx");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    
    //reset view 0 to the dimentions of the window and clear it
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 0, 1, 0);
    bgfx::setViewRect(0, 0, 0, uint16_t(this->width), uint16_t(this->height));
    
    // Create uniforms and store them in a map by name
    this->handles.uniforms["s_albedo"]        = bgfx::createUniform("s_albedo", bgfx::UniformType::Sampler);
    this->handles.uniforms["s_normalMap"]     = bgfx::createUniform("s_normalMap", bgfx::UniformType::Sampler);
    this->handles.uniforms["s_heightMap"]     = bgfx::createUniform("s_heightMap", bgfx::UniformType::Sampler);

    this->handles.uniforms["u_normalMatrix"]  = bgfx::createUniform("u_normalMatrix", bgfx::UniformType::Mat3);
    this->handles.uniforms["u_lightDir"]      = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    this->handles.uniforms["u_floats"]        = bgfx::createUniform("u_floats", bgfx::UniformType::Vec4);
    this->handles.uniforms["u_baseColor"]     = bgfx::createUniform("u_baseColor", bgfx::UniformType::Vec4);

    this->handles.uniforms["u_skyColorDay"]   = bgfx::createUniform("u_skyColorDay", bgfx::UniformType::Vec4);
    this->handles.uniforms["u_skyColorNight"] = bgfx::createUniform("u_skyColorNight", bgfx::UniformType::Vec4);
    this->handles.uniforms["u_sunColorDay"]   = bgfx::createUniform("u_sunColorDay", bgfx::UniformType::Vec4);
    this->handles.uniforms["u_sunColorRise"]  = bgfx::createUniform("u_sunColorRise", bgfx::UniformType::Vec4);

    this->handles.uniforms["u_billboard"] = bgfx::createUniform("u_billboard", bgfx::UniformType::Vec4);
    
    bgfx::touch(0); // touch the view to clear it
    bgfx::frame(); // submit the frame
    SDL_ShowWindow(this->win); //show the window
    
    //print window info
    {
        int w, h, bufw, bufh;
        SDL_GetWindowSize(win, &w, &h);
        SDL_GetWindowSizeInPixels(win, &bufw, &bufh);
        SDL_Log("Window size: %d x %d, Buffer size: %d x %d", w, h, bufw, bufh);
        if(w != bufw) {
            SDL_Log("This is a high DPI display");
        }
    }

    //create default shaders
    size_t defaultProg = AddProgram("shaders/default.vert.sc.bin",
                                    "shaders/default.frag.sc.bin",
                                    "default");
    if (defaultProg == (size_t)-1) {
        bgfx::shutdown();
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }

    size_t debugProg = AddProgram("shaders/debug.vert.sc.bin",
                                  "shaders/debug.frag.sc.bin",
                                  "debugger");
    if (debugProg == (size_t)-1) {
        bgfx::shutdown();
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }

    size_t billboardProg = AddProgram("shaders/billboard.vert.sc.bin",
                                      "shaders/billboard.frag.sc.bin",
                                      "billboard");
    if (billboardProg == (size_t)-1) {
        bgfx::shutdown();
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
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
    this->billboardVbh = bgfx::createVertexBuffer(
        bgfx::copy(billboardVertices, sizeof(billboardVertices)),
        billboardLayout);
    this->billboardIbh = bgfx::createIndexBuffer(
        bgfx::copy(billboardIndices, sizeof(billboardIndices)));
    
    if (!bgfx::isValid(this->billboardVbh) || !bgfx::isValid(this->billboardIbh)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create billboard buffers");
        bgfx::shutdown();
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
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

    SDL_Log("hello world, this is bakerman coming at you live from CNN");

    return 0;
}

int Graphics::DestroyRenderer() {
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to destroy renderer for");
        return 1;
    }

    // Default cleanup
    if(bgfx::isValid(this->billboardVbh)) {
        bgfx::destroy(this->billboardVbh);
        this->billboardVbh = BGFX_INVALID_HANDLE;
    }
    if(bgfx::isValid(this->billboardIbh)) {
        bgfx::destroy(this->billboardIbh);
        this->billboardIbh = BGFX_INVALID_HANDLE;
    }
    if(bgfx::isValid(this->handles.dummy)) {
        bgfx::destroy(this->handles.dummy);
        this->handles.dummy = BGFX_INVALID_HANDLE;
    }

    // Destroy all programs and uniforms
    for (auto& program : this->handles.programs) {
        bgfx::destroy(program.program);
        program.program = BGFX_INVALID_HANDLE;
    }
    this->handles.programs.clear();

    for (auto& uniform : this->handles.uniforms) {
        if(bgfx::isValid(uniform.second) && uniform.second.idx != 0) {
            bgfx::destroy(uniform.second);
        }
    }
    this->handles.uniforms.clear();

    // Clear gizmos
    for (auto& gizmo : this->gizmoRegistry) {
        if (bgfx::isValid(gizmo.second.texture)) {
            bgfx::destroy(gizmo.second.texture);
        }
    }
    this->gizmoRegistry.clear();

    bgfx::shutdown(); //shut down bgfx BEFORE destroying the window
    SDL_Log("goodbye renderer");
    return 0;
}

int Graphics::AddProgram(const char* vsPath, const char* fsPath, const char* name, Syngine::ViewID viewId) {
    bgfx::ShaderHandle vs = LoadShader(vsPath);
    bgfx::ShaderHandle fs = LoadShader(fsPath);
    bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (bgfx::isValid(vs) && bgfx::isValid(fs)) {
        programHandle = bgfx::createProgram(vs, fs, true);
    }

    if (!bgfx::isValid(programHandle)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create program %s", name);
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

    this->handles.programs.push_back(prog);
    SDL_Log("Program %s created successfully", name);
    return (int)this->handles.programs.size() - 1; // return the index of the new program
}

int Graphics::AddProgram(const char* path, const char* name, Syngine::ViewID viewId) {
    // Load the shader from the given path
    bgfx::ShaderHandle vs = LoadShader((std::string(path) + ".vert.sc.bin").c_str());
    bgfx::ShaderHandle fs = LoadShader((std::string(path) + ".frag.sc.bin").c_str());
    
    if (!bgfx::isValid(vs) || !bgfx::isValid(fs)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load shaders for program %s", name);
        return -1;
    }

    bgfx::ProgramHandle programHandle = bgfx::createProgram(vs, fs, true);
    if (!bgfx::isValid(programHandle)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create program %s", name);
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

    this->handles.programs.push_back(prog);
    SDL_Log("Program %s created successfully", name);
    return (int)this->handles.programs.size() - 1; // return the index of the new program
}

Program Graphics::GetProgram(const char* name) const {
    for (const auto& program : this->handles.programs) {
        if (program.name == name && bgfx::isValid(program.program)) {
            return program;
        }
    }
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program %s not found", name);
    return Program();
}

bool Graphics::RemoveProgram(int index) {
    if (index < 0 || index >= this->handles.programs.size()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid program index %zu", index);
        return false;
    }
    bgfx::destroy(this->handles.programs[index].program);
    this->handles.programs.erase(this->handles.programs.begin() + index);
    return true;
}
bool Graphics::RemoveProgram(const char* name) {
    for (int i = 0; i < this->handles.programs.size(); ++i) {
        if (this->handles.programs[i].name == name) {
            return RemoveProgram(i);
        }
    }
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program %s not found", name);
    return false;
}
bool Graphics::RemoveAllPrograms() {
    for (auto& program : this->handles.programs) {
        if (!bgfx::isValid(program.program)) {
            continue; // Skip invalid programs
        }
        bgfx::destroy(program.program);
    }
    this->handles.programs.clear();
    return true;
}

bool Graphics::ReloadProgram(const char* name) {
    for (auto& prog : this->handles.programs) {
        if (prog.name == name) {
            bgfx::ShaderHandle vs = LoadShader(prog.vsPath.c_str());
            bgfx::ShaderHandle fs = LoadShader(prog.fsPath.c_str());
            bgfx::ProgramHandle newProgram = bgfx::createProgram(vs, fs, true);

            if (!bgfx::isValid(newProgram)) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to reload program %s", name);
                bgfx::destroy(vs);
                bgfx::destroy(fs);
                return false;
            }

            bgfx::destroy(prog.program); // Destroy old program
            prog.program = newProgram;   // Update to new program
            SDL_Log("Program %s reloaded successfully", name);
            return true;
        }
    }
    return false;
}

bool Graphics::ReloadAllPrograms() {
    SDL_Log("Reloading all shader programs...");
    for (const auto& prog : this->handles.programs) { // Are we sure looping through every program several times is okay?
        if (!ReloadProgram(prog.name.c_str())) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to reload program %s", prog.name.c_str());
            return false;
        }
    }
    return true;
}

bgfx::UniformHandle Graphics::GetUniform(const char* name) const {
    auto it = this->handles.uniforms.find(name);
    if (it != this->handles.uniforms.end()) {
        return it->second;
    }
    return BGFX_INVALID_HANDLE;
}

void Graphics::DestroyWindow() { //dw this is effectively the destructor
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to destroy");
        return;
    }
    //cleanup
    SDL_Log("app shutting down");
    SDL_DestroyWindow(this->win);
    this->win = nullptr;
    SDL_Log("goodbye world");
}

void Graphics::RegisterGizmo(const std::string& tag, float size) {
    
    std::string resolvedPath = resolveOSPath((std::string("default/gizmos/") + tag + ".png").c_str());
    const char* path = resolvedPath.c_str();
    bgfx::TextureHandle texture = SynLoadTextureFromFile(path);
    if (bgfx::isValid(texture)) {
        gizmoRegistry[tag] = { texture, size };
    }
}

void Graphics::RenderGizmos(std::vector<GameObject*> gameObjects,
                            CameraComponent*         camera) {
    unsigned short viewId = 26; // View ID for gizmos
    bgfx::setViewRect(viewId, 0, 0, bgfx::BackbufferRatio::Equal);
    bgfx::setViewTransform(
        viewId, camera->GetCamera().view, camera->GetCamera().proj);
    bgfx::touch(viewId);

    Program billboardProgram = GetProgram("billboard");
    if(!bgfx::isValid(billboardProgram.program)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Billboard program not found");
        return;
    }

    bgfx::UniformHandle billboardUniform = GetUniform("u_billboard");
    if (!bgfx::isValid(billboardUniform)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Billboard uniform not found");
        return;
    }

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_BLEND_ALPHA | BGFX_STATE_DEPTH_TEST_LEQUAL);

    for (auto go : gameObjects) {
        auto it = gizmoRegistry.find(go->gizmo);
        if (it != gizmoRegistry.end()) {
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
                bgfx::setVertexBuffer(0, this->billboardVbh);
                bgfx::setIndexBuffer(this->billboardIbh);
                bgfx::setTexture(0, GetUniform("s_albedo"), gizmo.texture);
                bgfx::submit(viewId, billboardProgram.program);
            }
        }
    }
}

int Graphics::RenderFrame(std::vector<GameObject*> gameObjects, bx::Vec3& lightDir, CameraComponent* camera, bool debug) {
    const Program& terrainProgram = GetProgram("terrain");
    const Program& skyProgram = GetProgram("sky");
    const Program& defaultProgram = GetProgram("default");
    if (!bgfx::isValid(terrainProgram.program) || !bgfx::isValid(skyProgram.program) || !bgfx::isValid(defaultProgram.program)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid program");
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
    BGFX_SAMPLER_MIN_ANISOTROPIC |
    BGFX_SAMPLER_MAG_ANISOTROPIC;
    
    for (auto& gameObject : gameObjects) {
        if (!gameObject) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GameObject is null");
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
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program %s not found", gameObject->type.c_str());
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
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid mesh");
                continue;
            }
            
            if(mesh.materials.empty()) {
                SDL_Log("GameObject %s has no materials, using default material", gameObject->name.c_str());
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
    if(debug) {
        RenderGizmos(gameObjects, camera);
    }

    bgfx::frame(); // submit the frame
    return 0;
}