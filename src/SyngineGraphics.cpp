#include "SyngineGraphics.h"
#include "Components.h"
#include "ShaderUtils.h"
#include "SynModelLoader.h"
#include "TransformComponent.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "bx/math.h"
#include "SynComponents.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_properties.h>

#include <bgfx/platform.h>
#include <cstdint>

#if BX_PLATFORM_OSX
#include "SynOSXMetalBridge.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

SyngineGraphics::SyngineGraphics(const char* title, int width, int height) {
    //initialize app
    this->title = title;
    this->width = width;
    this->height = height;
    this->win = nullptr;
};

void Camera::Update(int viewId, int width, int height) {
    //update view and projection matrices
    bx::Vec3 eyeVec = { eye[0], eye[1], eye[2] };
    float aspect = float(width) / float(height);

    bx::Vec3 forward = {
        cosf(pitch) * sinf(yaw),
        sinf(pitch),
        cosf(pitch) * cosf(yaw)
    };
    bx::Vec3 right = {
        sinf(yaw - bx::kPiHalf),
        0.0f,
        cosf(yaw - bx::kPiHalf)
    };

    bx::Vec3 targetVec = bx::add(eyeVec, forward);
    bx::Vec3 upVec = bx::cross(right, forward);

    bx::mtxLookAt(view, eyeVec, targetVec, upVec);

    bx::mtxLookAt(view, eyeVec, targetVec, upVec);
    bx::mtxProj(proj, fov, aspect, near, far, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(viewId, view, proj);
}

int SyngineGraphics::CreateWindow() {
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

int SyngineGraphics::CreateRenderer() {
    //create renderer
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to create renderer for");
        return 1;
    }

    //initialize bgfx
    bgfx::Init bgInit;
    bgInit.debug = true;
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
    bgInit.debug = true; // enable debug mode

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

    //create default shader
    size_t defaultProg = AddProgram("shaders/default.vert.sc.bin", "shaders/default.frag.sc.bin", "default");
    if (defaultProg == (size_t)-1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create default program");
        bgfx::shutdown();
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }

    SDL_Log("hello world, this is bakerman coming at you live from CNN");

    return 0;
}

int SyngineGraphics::DestroyRenderer() {
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to destroy renderer for");
        return 1;
    }

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

    bgfx::shutdown(); //shut down bgfx BEFORE destroying the window
    SDL_Log("goodbye renderer");
    return 0;
}

size_t SyngineGraphics::AddProgram(const char* vsPath, const char* fsPath, const char* name) {
    bgfx::ShaderHandle vs = LoadShader(vsPath);
    bgfx::ShaderHandle fs = LoadShader(fsPath);
    bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (bgfx::isValid(vs) && bgfx::isValid(fs)) {
        programHandle = bgfx::createProgram(vs, fs, true);
    }

    if (bgfx::isValid(programHandle)) {
        SynProgram prog;
        prog.program = programHandle;
        prog.name = name;
        prog.viewId = this->handles.programs.size();
        this->handles.programs.push_back(prog);
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create program %s", name);
        if (bgfx::isValid(vs)) {
            bgfx::destroy(vs);
        }
        if (bgfx::isValid(fs)) {
            bgfx::destroy(fs);
        }
        return -1;
    }
    return this->handles.programs.size() - 1;
}

SynProgram SyngineGraphics::GetProgram(size_t index) const {
    if (index < 0 || index >= this->handles.programs.size()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid program index %zu", index);
        return SynProgram();
    }
    if (!bgfx::isValid(this->handles.programs[index].program)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program %zu is not valid", index);
        return SynProgram();
    } 
    return this->handles.programs[index];
}
SynProgram SyngineGraphics::GetProgram(const char* name) const {
    for (const auto& program : this->handles.programs) {
        if (program.name == name && bgfx::isValid(program.program)) {
            return program;
        }
    }
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program %s not found", name);
    return SynProgram();
}

int SyngineGraphics::RemoveProgram(size_t index) {
    if (index < 0 || index >= this->handles.programs.size()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid program index %zu", index);
        return -1;
    }
    bgfx::destroy(this->handles.programs[index].program);
    this->handles.programs.erase(this->handles.programs.begin() + index);
    return 0;
}
int SyngineGraphics::RemoveProgram(const char* name) {
    for (size_t i = 0; i < this->handles.programs.size(); ++i) {
        if (this->handles.programs[i].name == name) {
            return RemoveProgram(i);
        }
    }
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Program %s not found", name);
    return -1;
}
int SyngineGraphics::RemoveProgram(bool all) {
    if (all) {
        for (auto& program : this->handles.programs) {
            bgfx::destroy(program.program);
        }
        this->handles.programs.clear();
        return 0;
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No program to remove");
        return -1;
    }
}

void SyngineGraphics::DestroyWindow() { //dw this is effectively the destructor
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

int SyngineGraphics::RenderFrame(std::vector<GameObject*> gameObjects, bx::Vec3& lightDir) {
    const SynProgram& terrainProgram = GetProgram("terrain");
    const SynProgram& skyProgram = GetProgram("sky");
    const SynProgram& defaultProgram = GetProgram("default");
    if (!bgfx::isValid(terrainProgram.program) || !bgfx::isValid(skyProgram.program) || !bgfx::isValid(defaultProgram.program)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid program");
        return -1;
    }

    const bgfx::ViewId SKY_VIEW = skyProgram.viewId;
    const bgfx::ViewId MAIN_VIEW = terrainProgram.viewId;
    camera.Update(SKY_VIEW, this->width, this->height); //update camera view and projection matrices
    
    //sky pass
    bgfx::setViewRect(SKY_VIEW, 0, 0, uint16_t(this->width), uint16_t(this->height));
    bgfx::setViewClear(SKY_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 1.0f, 0, 1, 0);
    bgfx::touch(SKY_VIEW);
    bgfx::setViewTransform(MAIN_VIEW, camera.view, camera.proj);
    bgfx::touch(MAIN_VIEW);

    float skyView[16];
    bx::memCopy(skyView, camera.view, sizeof(skyView));
    skyView[12] = 0.0f; //remove translation for skybox
    skyView[13] = 0.0f;
    skyView[14] = 0.0f;

    bgfx::setViewTransform(SKY_VIEW, skyView, camera.proj);

    float skyColorDay[4] = { 0.5f, 0.7f, 1.0f, 1.0f };
    float skyColorNight[4] = { 0.0f, 0.0f, 0.1f, 1.0f };
    float sunColorDay[4] = { 1.0f, 1.0f, 0.9f, 1.0f };
    float sunColorRise[4] = { 0.77f, 0.39f, 0.14f, 1.0f };
    bgfx::setUniform(handles.uniforms["u_skyColorDay"], skyColorDay);
    bgfx::setUniform(handles.uniforms["u_skyColorNight"], skyColorNight);
    bgfx::setUniform(handles.uniforms["u_sunColorDay"], sunColorDay);
    bgfx::setUniform(handles.uniforms["u_sunColorRise"], sunColorRise);

    bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_DEPTH_TEST_LEQUAL | BGFX_STATE_MSAA);

    //bgfx::setTransform(nullptr);
    bgfx::setVertexCount(3);
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
        if (!gameObject->HasComponent(SYN_COMPONENT_TRANSFORM) || (!gameObject->HasComponent(SYN_COMPONENT_MESH)) || !(gameObject->GetComponent<TransformComponent>())->isEnabled) {
            continue;
        }
        SynMeshData mesh = gameObject->GetComponent<MeshComponent>()->meshData;
        
        bool isDefault = false;
        SynProgram currentProg;
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

        if(gameObject->HasComponent(SYN_COMPONENT_MESH) && gameObject->GetComponent<MeshComponent>()->isEnabled) {
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

    bgfx::frame(); // submit the frame
    return 0;
}