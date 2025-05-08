#include "SyngineGraphics.h"
#include "bgfx/bgfx.h"
#include "bgfx/defines.h"
#include "helpers.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_properties.h>

#include <bx/math.h>
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
    bx::Vec3 targetVec = { target[0], target[1], target[2] };
    bx::Vec3 upVec = { up[0], up[1], up[2] };
    float aspect = float(width) / float(height);

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
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    this->u_mvp = bgfx::createUniform("u_mvp", bgfx::UniformType::Mat4); //u_modelViewProjection. it's a mat4 that is used to transform the vertices from model space to clip space

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
    bgfx::destroy(this->u_mvp);
    bgfx::shutdown(); //shut down bgfx BEFORE destroying the window
    SDL_Log("goodbye renderer");
    return 0;
}

void SyngineGraphics::SetProgram(bgfx::ProgramHandle program) {
    if (bgfx::isValid(this->program)) {
        bgfx::destroy(this->program);
        SDL_Log("Replacing program");
    }
    this->program = program;

    // Add diagnostic info
    if (bgfx::isValid(this->program)) {
        SDL_Log("Program successfully set");
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set program - invalid handle");
    }
}
bgfx::ProgramHandle SyngineGraphics::GetProgram() const {
    return this->program;
}
int SyngineGraphics::DestroyProgram() {
    if (!bgfx::isValid(this->program)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No program to destroy");
        return 1;
    }
    bgfx::destroy(this->program);
    this->program = BGFX_INVALID_HANDLE;
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

int SyngineGraphics::RenderFrame(SynModelLoader& modelLoader) {
    int viewId = 0;
    camera.Update(viewId, this->width, this->height); //update camera view and projection matrices

    /*float view[16], proj[16];

    bx::Vec3 eyeVec = { camera.eye[0], camera.eye[1], camera.eye[2] };
    bx::Vec3 targetVec = { camera.target[0], camera.target[1], camera.target[2] };
    bx::Vec3 upVec = { camera.up[0], camera.up[1], camera.up[2] };
    bx::mtxLookAt(view, eyeVec, targetVec, upVec);

    bx::mtxProj(
        proj,
        camera.fov,
        float(this->width) / float(this->height),
        camera.near,
        camera.far,
        bgfx::getCaps()->homogeneousDepth
    );

    float I[16];
    bx::mtxIdentity(I); //identity matrix

    bgfx::setViewTransform(viewId, I, I); //set view transform for the view*/

    float model[16];
    bx::mtxIdentity(model);
    //bx::mtxScale(model, 0.1f, 0.1f, 0.1f);

    float viewProj[16];
    bx::mtxMul(viewProj, camera.proj, camera.view);

    float mvp[16];
    bx::mtxMul(mvp, viewProj, model);
    
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
    uint64_t renderState = BGFX_STATE_DEFAULT;

    bgfx::setState(renderState);

    //SDL_Log("Mesh count: %zu", modelLoader.getMeshes().size());
    //SDL_Log("Eye: %.2f, %.2f, %.2f", camera.eye[0], camera.eye[1], camera.eye[2]);

    //hardcoded tri test
    /*{
        struct PosColorVertex {
            float x,y,z;
        };

        static PosColorVertex triangleVertices[] = {
            { -1.0f, -1.0f, 0.0f },  //top left
            {  1.0f, -1.0f, 0.0f }, //top right
            {  0.0f,  1.0f, 0.0f } //bottom center
        };

        static uint16_t triangleIndices[] = {
            0, 1, 2
        };

        bgfx::VertexLayout layout;
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, false, false)
            .end();
        
        bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
            bgfx::makeRef(triangleVertices, sizeof(triangleVertices)),
            layout
        );
        bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
            bgfx::makeRef(triangleIndices, sizeof(triangleIndices))
        );
        
        if(bgfx::isValid(vbh) && bgfx::isValid(ibh)) {
            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);
            bgfx::setUniform(u_mvp, mvp);
            bgfx::submit(viewId, this->program);
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create triangle buffers");
        }

        bgfx::destroy(vbh);
        bgfx::destroy(ibh);
    }*/

    for (auto& mesh : modelLoader.getMeshes()) {
        /*SDL_Log("Mesh %s has %zu vertices and %zu indices", mesh.material.name.c_str(), mesh.vertices.size(), mesh.indices.size());
        SDL_Log("Vert 1 pos: %.2f, %.2f, %.2f", mesh.vertices[0].pos[0], mesh.vertices[0].pos[1], mesh.vertices[0].pos[2]);
        SDL_Log("The VBH is %s", bgfx::isValid(mesh.vbh) ? "valid" : "invalid");
        SDL_Log("The IBH is %s", bgfx::isValid(mesh.ibh) ? "valid" : "invalid");*/
        bgfx::setVertexBuffer(0, mesh.vbh);
        bgfx::setIndexBuffer(mesh.ibh);
        bgfx::setUniform(u_mvp, mvp);
        bgfx::submit(viewId, this->program);
    }

    bgfx::frame(); // submit the frame
    return 0;
}