#include "SyngineCore.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SynModelLoader.h"
#include "bx/bx.h"
#include "bx/constants.h"
#include "bx/math.h"
#include "defines.h"

SyngineCore::SyngineCore() {
    //initialize app
    this->app = new SyngineApp();
    this->app->graphics = nullptr; // No graphics attached initially
    this->app->synModels = new AssimpLoader(); // Initialize the model loader
}

SyngineCore::~SyngineCore() {
    //cleanup
    if (this->app) {
        delete this->app;
    }
}

int SyngineCore::AttachGraphics(SyngineGraphics* graphics) {
    //attach graphics to app
    if (!graphics) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No graphics object provided to attach");
        return 1;
    }
    
    this->app->graphics = graphics;
    return 0;
}

int SyngineCore::DetachGraphics() {
    //detach graphics from app
    if (!this->app->graphics) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No graphics object to detach");
        return 1;
    }
    
    this->app->graphics = nullptr;
    return 0;
}

int SyngineCore::SyngineEventLoop() {
    //main loop. this will obviously be replaced with a more complex and modular/multi-threaded loop in the Future
    SDL_Log("Entering event loop");
    bool running = true;
    bool mouseLook = false;
    int frame = 0;

    const float sensitivity = 0.002f; // Adjust sensitivity as needed
    const float maxPitch = bx::kPiHalf - 0.01f; // Limit pitch to avoid gimbal lock

    SDL_Event event;

    while (running) {
        const bool* keystate = SDL_GetKeyboardState(NULL);
        Camera& camera = this->app->graphics->camera;
        bx::Vec3 moveVector = {0,0,0};
        bx::Vec3 newEye = {0,0,0};

        //event handling
        while (SDL_PollEvent(&event)) { //where inputs can also be handled

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                int w, h;
                SDL_Window* resizedWindow = SDL_GetWindowFromID(event.window.windowID);
                if (resizedWindow) {
                    SDL_GetWindowSize(resizedWindow, &w, &h);
                }
                SDL_Log("Window resized to %d x %d", w, h);

                this->app->graphics->width = w;
                this->app->graphics->height = h;
                bgfx::reset(w, h, BGFX_RESET_VSYNC); // reset bgfx with new window size
                bgfx::setViewRect(0, 0, 0, uint16_t(w), uint16_t(h)); // reset view rect
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_F) {
                    SDL_Log("F key pressed");
                    
                    //load model
                    std::string modelPath = resolveOSPath("meshes/horosont.dae"); // Path to your model
                    SynMeshData meshData;
                    if (this->app->synModels->LoadModel(modelPath, meshData)) {
                        SDL_Log("Model loaded successfully");
                    } else {
                        SDL_Log("Failed to load model");
                    }
                } else if (event.key.key == SDLK_ESCAPE) {
                    mouseLook = !mouseLook;
                    SDL_SetWindowRelativeMouseMode(this->app->graphics->win, mouseLook ? true : false);
                }
            } else if (event.type == SDL_EVENT_MOUSE_MOTION && mouseLook) {
                float dx = event.motion.xrel * sensitivity;
                float dy = event.motion.yrel * sensitivity;

                camera.yaw += dx;
                camera.pitch -= dy;
                camera.pitch = bx::clamp(camera.pitch, -maxPitch, maxPitch); // Clamp pitch to avoid gimbal lock
            }
        }

        //camera movement
        if (mouseLook) {
        if (keystate[SDL_SCANCODE_W]) {
            moveVector = {
                cosf(camera.pitch) * sinf(camera.yaw),
                sinf(camera.pitch),
                cosf(camera.pitch) * cosf(camera.yaw)
            };
            moveVector = bx::mul(moveVector, 0.1f);
            newEye = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = newEye.x;
            camera.eye[1] = newEye.y;
            camera.eye[2] = newEye.z;
        }
        if (keystate[SDL_SCANCODE_S]) {
            moveVector = {
                -cosf(camera.pitch) * sinf(camera.yaw),
                sinf(camera.pitch),
                -cosf(camera.pitch) * cosf(camera.yaw)
            };
            moveVector = bx::mul(moveVector, 0.1f);
            newEye = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = newEye.x;
            camera.eye[1] = newEye.y;
            camera.eye[2] = newEye.z;
        }
        if (keystate[SDL_SCANCODE_A]) {
            moveVector = {
                sinf(camera.yaw - bx::kPiHalf),
                0.0f,
                cosf(camera.yaw - bx::kPiHalf)
            };
            moveVector = bx::mul(moveVector, 0.1f);
            newEye = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = newEye.x;
            camera.eye[1] = newEye.y;
            camera.eye[2] = newEye.z;
        }
        if (keystate[SDL_SCANCODE_D]) {
            moveVector = {
                sinf(camera.yaw + bx::kPiHalf),
                0.0f,
                cosf(camera.yaw + bx::kPiHalf)
            };
            moveVector = bx::mul(moveVector, 0.1f);
            newEye = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = newEye.x;
            camera.eye[1] = newEye.y;
            camera.eye[2] = newEye.z;
        }
        if (keystate[SDL_SCANCODE_Q]) {
            moveVector = {
                0.0f,
                -1.0f,
                0.0f
            };
            moveVector = bx::mul(moveVector, 0.1f);
            newEye = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = newEye.x;
            camera.eye[1] = newEye.y;
            camera.eye[2] = newEye.z;
        }
        if (keystate[SDL_SCANCODE_E]) {
            moveVector = {
                0.0f,
                1.0f,
                0.0f
            };
            moveVector = bx::mul(moveVector, 0.1f);
            newEye = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = newEye.x;
            camera.eye[1] = newEye.y;
            camera.eye[2] = newEye.z;
        }
        }
        
        if (this->app && this->app->graphics) {
            this->app->graphics->RenderFrame(*(this->app->synModels)); // render frame
        }
        
        ++frame;
    }
    
    SDL_Log("Exiting event loop");
    return 0;
}