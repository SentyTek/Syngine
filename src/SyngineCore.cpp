#include "SyngineCore.h"
#include "Components.h"
#include "MeshComponent.h"
#include "PhysComponent.h"
#include "SynginePhys.h"
#include "TransformComponent.h"
#include "bgfx/defines.h"
#include "defines.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_timer.h"

#include "bx/bx.h"
#include "bx/constants.h"
#include "bx/math.h"

#include <Jolt/Physics/Body/BodyInterface.h>

SyngineCore::SyngineCore() {
    //initialize app
    this->app = new SyngineApp();
    this->app->graphics = nullptr; // No graphics attached initially
    this->app->synModels = new AssimpLoader(); // Initialize the model loader

    this->app->physicsManager = new Syngine::SynginePhys(); // Initialize the physics manager
    this->app->physicsManager->Init(); // Initialize the physics system
}

SyngineCore::~SyngineCore() {
    //cleanup
    if (this->app) {
        if (this->app->graphics) {
            delete this->app->graphics;
            this->app->graphics = nullptr;
        }
        if (this->app->synModels) {
            this->app->synModels->UnloadAll();
            delete this->app->synModels;
            this->app->synModels = nullptr;
        }
        if (this->app->physicsManager) {
            this->app->physicsManager->Shutdown();
            delete this->app->physicsManager;
            this->app->physicsManager = nullptr;
        }
        delete this->app;
        this->app = nullptr;
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
    bool simulate = false;
    bool rmb = false;
    bool mouseState = false; //relative mouse mode state
    float mouseX, mouseY;

    int frame = 0;
    int count = 0;
    
    bx::Vec3 startDir = {75.0f, 0.0f, 90.0f};

    const float sensitivity = 0.002f; // Adjust sensitivity as needed
    const float maxPitch =
        bx::kPiHalf - 0.01f; // Limit pitch to avoid gimbal lock
    float moveSpeed = 2.0f; // Speed of camera movement

    SDL_Event event;

    uint64 NOW = SDL_GetPerformanceCounter();
    uint64 LAST = 0;

    while (running) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        float deltaTime = (float)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

        const bool* keystate = SDL_GetKeyboardState(NULL);
        Camera& camera = this->app->graphics->camera;
        bx::Vec3 moveVector = {0,0,0};

        //event handling
        while (SDL_PollEvent(&event)) { //where inputs can also be handled

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                int w, h;
                SDL_Window* resizedWindow = SDL_GetWindowFromID(event.window.windowID);
                SDL_GetWindowSize(resizedWindow, &w, &h);

                this->app->graphics->width = w;
                this->app->graphics->height = h;
                bgfx::reset(w, h, BGFX_RESET_VSYNC); // reset bgfx with new window size
                bgfx::setViewRect(0, 0, 0, uint16_t(w), uint16_t(h)); // reset view rect
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_F) {
                    //load model
                    std::string modelPath = resolveOSPath("meshes/ground.glb");
                    GameObject* model = new GameObject("ground", "terrain");
                    model->AddComponent(SYN_COMPONENT_TRANSFORM);
                    model->AddComponent(SYN_COMPONENT_MESH);
                    model->AddComponent(SYN_COMPONENT_PHYSICS);
                    MeshComponent* meshComp = model->GetComponent<MeshComponent>();
                    if (meshComp) meshComp->LoadMesh(modelPath);
                    Syngine::PhysicsComponent* physComp = model->GetComponent<Syngine::PhysicsComponent>();
                    if (physComp) {
                            physComp->Init(
                                model->GetComponent<TransformComponent>(),
                                this->app->physicsManager,
                                Syngine::PhysicsShapes::MESH,
                                0.0f, // no mass for static objects
                                0.5f, // friction
                                JPH::EMotionType::Static,
                                Syngine::Layers::NON_MOVING,
                                {}
                            );
                    }
                    this->app->gameObjects.push_back(model);
                } else if (event.key.key == SDLK_ESCAPE) {
                    simulate = !simulate;
                } else if (event.key.key == SDLK_LSHIFT) {
                    moveSpeed = 10.0f; // Increase speed when holding shift
                } else if (event.key.key == SDLK_LCTRL) {
                    moveSpeed = 1.0f; // Decrease speed when holding ctrl
                }
            } else if (event.type == SDL_EVENT_KEY_UP) {
                if (event.key.key == SDLK_LSHIFT || event.key.key == SDLK_LCTRL) {
                    moveSpeed = 2.0f; // Reset speed when releasing shift or ctrl
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    rmb = true;
                    mouseX = event.button.x;
                    mouseY = event.button.y;
                } else if (event.button.button == SDL_BUTTON_LEFT) {
                    string modelPath = resolveOSPath("meshes/cube.glb");
                    GameObject* cube = new GameObject("cube", "default");
                    cube->AddComponent(SYN_COMPONENT_TRANSFORM);
                    cube->AddComponent(SYN_COMPONENT_MESH);
                    cube->AddComponent(SYN_COMPONENT_PHYSICS);
                    MeshComponent* meshComp = cube->GetComponent<MeshComponent>();
                    if (meshComp) meshComp->LoadMesh(modelPath, false);
                    TransformComponent* tComp = cube->GetComponent<TransformComponent>();
                    if (tComp) tComp->SetPosition(0.0f, 10.0f, 0.0f);
                    Syngine::PhysicsComponent* physComp = cube->GetComponent<Syngine::PhysicsComponent>();
                    std::vector<float> shapeParams = {0.5f, 0.5f, 0.5f}; // half extents for box shape
                    if (physComp)
                        physComp->Init(tComp,
                                       this->app->physicsManager,
                                       Syngine::PhysicsShapes::BOX,
                                       1.0f,
                                       0.5f,
                                       JPH::EMotionType::Dynamic,
                                       Syngine::Layers::MOVING,
                                       shapeParams);
                    this->app->gameObjects.push_back(cube);
                    count++;
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    rmb = false;
                    SDL_WarpMouseInWindow(this->app->graphics->win, mouseX, mouseY);
                }
            }else if (event.type == SDL_EVENT_MOUSE_MOTION && rmb) {
                float dx = event.motion.xrel * sensitivity;
                float dy = event.motion.yrel * sensitivity;

                camera.yaw += dx;
                camera.pitch -= dy;
                camera.pitch = bx::clamp(camera.pitch, -maxPitch, maxPitch); // Clamp pitch to avoid gimbal lock
            }
        }

        //mouse move
        if (this->app->graphics->win) {
            bool desiredMouseState = rmb;
            if (desiredMouseState != mouseState) {
                SDL_SetWindowRelativeMouseMode(this->app->graphics->win, desiredMouseState);
                mouseState = desiredMouseState;
            }

        }

        //simulation stuff
        if (simulate) {
            if(this->app->physicsManager) {
                this->app->physicsManager->Update(1);
            }
  
            startDir.x += 0.02f;
        }

        //camera movement
        if (keystate[SDL_SCANCODE_W]) {
            moveVector = {
                cosf(camera.pitch) * sinf(camera.yaw),
                sinf(camera.pitch),
                cosf(camera.pitch) * cosf(camera.yaw)
            };
            moveVector = bx::mul(moveVector, moveSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = moveVector.x;
            camera.eye[1] = moveVector.y;
            camera.eye[2] = moveVector.z;
        }
        if (keystate[SDL_SCANCODE_S]) {
            moveVector = {
                -cosf(camera.pitch) * sinf(camera.yaw),
                -sinf(camera.pitch),
                -cosf(camera.pitch) * cosf(camera.yaw)
            };
            moveVector = bx::mul(moveVector, moveSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = moveVector.x;
            camera.eye[1] = moveVector.y;
            camera.eye[2] = moveVector.z;
        }
        if (keystate[SDL_SCANCODE_A]) {
            moveVector = {
                sinf(camera.yaw - bx::kPiHalf),
                0.0f,
                cosf(camera.yaw - bx::kPiHalf)
            };
            moveVector = bx::mul(moveVector, moveSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = moveVector.x;
            camera.eye[1] = moveVector.y;
            camera.eye[2] = moveVector.z;
        }
        if (keystate[SDL_SCANCODE_D]) {
            moveVector = {
                sinf(camera.yaw + bx::kPiHalf),
                0.0f,
                cosf(camera.yaw + bx::kPiHalf)
            };
            moveVector = bx::mul(moveVector, moveSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = moveVector.x;
            camera.eye[1] = moveVector.y;
            camera.eye[2] = moveVector.z;
        }
        if (keystate[SDL_SCANCODE_Q]) {
            moveVector = {
                0.0f,
                -1.0f,
                0.0f
            };
            moveVector = bx::mul(moveVector, moveSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = moveVector.x;
            camera.eye[1] = moveVector.y;
            camera.eye[2] = moveVector.z;
        }
        if (keystate[SDL_SCANCODE_E]) {
            moveVector = {
                0.0f,
                1.0f,
                0.0f
            };
            moveVector = bx::mul(moveVector, moveSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(camera.eye[0], camera.eye[1], camera.eye[2]), moveVector);
            camera.eye[0] = moveVector.x;
            camera.eye[1] = moveVector.y;
            camera.eye[2] = moveVector.z;
        }

        //sun
        if (keystate[SDL_SCANCODE_LEFT]) {
            startDir.z += 2.0f;
        }
        if (keystate[SDL_SCANCODE_RIGHT]) {
            startDir.z -= 2.0f;
        }
        if (keystate[SDL_SCANCODE_UP]) {
            startDir.x += 2.0f;
        }
        if (keystate[SDL_SCANCODE_DOWN]) {
            startDir.x -= 2.0f;
        }

        if (startDir.z > 360.0f) startDir.z -= 360.0f;
        if (startDir.z < 0.0f)   startDir.z += 360.0f;
        if (startDir.x > 360.0f) startDir.x -= 360.0f;
        if (startDir.x < 0.0f)   startDir.x += 360.0f;

        // Convert to pitch/yaw
        float pitch = bx::toRad(90.0f - startDir.x); // vertical angle, 0 = up, 90 = horizon
        float yaw   = bx::toRad(startDir.z); // horizontal rotation (azimuth)

        float cy = cosf(pitch);
        bx::Vec3 lightDir = {
            cy * sinf(yaw),     // x
            sinf(pitch),       // y (Y-up world, light pointing down is negative)
            cy * cosf(yaw)      // z
        };
        lightDir = bx::normalize(lightDir);

        for (auto* gameObject : this->app->gameObjects) {
            if (gameObject) {
                Syngine::PhysicsComponent* physComp = gameObject->GetComponent<Syngine::PhysicsComponent>();
                if (physComp) {
                    physComp->Update(simulate); // Update physics component
                }
            }
        }
        
        if (this->app && this->app->graphics) {
            this->app->graphics->RenderFrame(this->app->gameObjects, lightDir); // render frame
        }
        
        if (frame % 60 == 0) {
            SDL_Log("Sun Direction: (%f, %f, %f)",
                    startDir.x,
                    startDir.y,
                    startDir.z);
            SDL_Log("Num cubes: %d", count);
        }
        ++frame;
    }
    
    this->app->synModels->UnloadAll();
    SDL_Log("Exiting event loop");
    return 0;
}

int SyngineCore::DeleteGameobject(GameObject* gameobject) {
    if(!gameobject) return 0;
    MeshComponent* mesh = gameobject->GetComponent<MeshComponent>();
    if(mesh) {
        mesh->UnloadMesh();
    }
    delete gameobject; // delete the gameobject
    gameobject = nullptr; // set to null to avoid dangling pointer
    return 0;
}