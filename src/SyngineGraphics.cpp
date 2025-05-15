#include "SyngineGraphics.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"

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
    bgInit.platformData.ndt = NULL; //only needed on x11
    bgInit.platformData.nwh = SDL_GetPointerProperty(sdlProps, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
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

    if(!bgfx::init(bgInit)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize bgfx");
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return 1;
    }
    
    //reset view 0 to the dimentions of the window and clear it
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 0, 1, 0);
    bgfx::setViewRect(0, 0, 0, uint16_t(this->width), uint16_t(this->height));

    this->handles.u_mvp = bgfx::createUniform("u_mvp", bgfx::UniformType::Mat4); //u_modelViewProjection. it's a mat4 that is used to transform the vertices from model space to clip space
    
    this->handles.u_albedoSampler = bgfx::createUniform("s_albedo", bgfx::UniformType::Sampler);
    this->handles.u_normalMapSampler = bgfx::createUniform("s_normalMap", bgfx::UniformType::Sampler);
    this->handles.u_heightMapSampler = bgfx::createUniform("s_heightMap", bgfx::UniformType::Sampler);
    
    this->handles.u_normalMatrix = bgfx::createUniform("u_normalMatrix", bgfx::UniformType::Mat3);
    this->handles.u_lightDir = bgfx::createUniform("u_lightDir", bgfx::UniformType::Vec4);
    this->handles.u_floats = bgfx::createUniform("u_floats", bgfx::UniformType::Vec4);

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

    SDL_Log("hello world, this is bakerman coming at you live from CNN");

    return 0;
}
int SyngineGraphics::DestroyRenderer() {
    if (!this->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No app to destroy renderer for");
        return 1;
    }
    bgfx::destroy(this->handles.u_mvp);
    bgfx::shutdown(); //shut down bgfx BEFORE destroying the window
    SDL_Log("goodbye renderer");
    return 0;
}

void SyngineGraphics::SetProgram(bgfx::ProgramHandle program) {
    if (bgfx::isValid(this->handles.program)) {
        bgfx::destroy(this->handles.program);
        SDL_Log("Replacing program");
    }
    this->handles.program = program;

    // Add diagnostic info
    if (bgfx::isValid(this->handles.program)) {
        SDL_Log("Program successfully set");
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set program - invalid handle");
    }
}
bgfx::ProgramHandle SyngineGraphics::GetProgram() const {
    return this->handles.program;
}
int SyngineGraphics::DestroyProgram() {
    if (!bgfx::isValid(this->handles.program)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No program to destroy");
        return 1;
    }
    bgfx::destroy(this->handles.program);
    this->handles.program = BGFX_INVALID_HANDLE;
    return 0;
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

int SyngineGraphics::RenderFrame(SynModelLoader& modelLoader, bx::Vec3& lightDir) {
    int viewId = 0;
    camera.Update(viewId, this->width, this->height); //update camera view and projection matrices

    float model[16];
    bx::mtxSRT(model, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    //once again, hardcoded above until the transform component is implemented

    float viewProj[16];
    bx::mtxMul(viewProj, model, camera.view);

    float mvp[16];
    bx::mtxMul(mvp, viewProj, camera.proj);
    
    //background slate
    uint32_t bgColor = 0x6495EDff;

    bgfx::setViewClear(
        viewId,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        bgColor,
        1.0f, //depth clear
        0     //stencil clear
    );

    bgfx::setViewRect(viewId, 0, 0, uint16_t(this->width), uint16_t(this->height));
    bgfx::touch(viewId);

    //prepare render
    uint64_t renderState = BGFX_STATE_DEFAULT | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;

    bgfx::setState(renderState);

    constexpr uint32_t samplerFlags =
        BGFX_SAMPLER_MIN_ANISOTROPIC |
        BGFX_SAMPLER_MAG_ANISOTROPIC;

    for (auto& mesh : modelLoader.getMeshes()) {
        //TODO: add support for multiple materials
        bgfx::setTexture(0, handles.u_albedoSampler, mesh.materials[0].albedo, samplerFlags);
        bgfx::setTexture(1, handles.u_normalMapSampler, mesh.materials[0].normalMap, samplerFlags);
        bgfx::setTexture(2, handles.u_heightMapSampler, mesh.materials[0].heightMap, samplerFlags);

        float sx = bx::length(bx::Vec3(model[0], model[1], model[2]));
        float sy = bx::length(bx::Vec3(model[4], model[5], model[6]));
        float sz = bx::length(bx::Vec3(model[8], model[9], model[10]));

        float normal3x3[9];
        for(int i = 0; i < 3; ++i) {
            normal3x3[i * 3 + 0] = model[i * 4 + 0] / sx;
            normal3x3[i * 3 + 1] = model[i * 4 + 1] / sy;
            normal3x3[i * 3 + 2] = model[i * 4 + 2] / sz;
        }

        float u_floats[4] = {
            mesh.materials[0].heightScale,
            mesh.materials[0].mixFactor,
            mesh.materials[0].ambient,
            mesh.materials[0].tileDetail
        };
        bgfx::setUniform(handles.u_normalMatrix, normal3x3);
        bgfx::setUniform(handles.u_mvp, mvp);
        bgfx::setUniform(handles.u_lightDir, &lightDir);
        bgfx::setUniform(handles.u_floats, u_floats);

        bgfx::setVertexBuffer(0, mesh.vbh);
        bgfx::setIndexBuffer(mesh.ibh);
        bgfx::submit(viewId, this->handles.program);
    }

    bgfx::frame(); // submit the frame
    return 0;
}