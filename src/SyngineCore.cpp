#include "SyngineCore.h"
#include "Components/CameraComponent.h"
#include "SDL3/SDL_scancode.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"
#include "Components.h"
#include "MeshComponent.h"
#include "RigidbodyComponent.h"
#include "TransformComponent.h"
#include "PlayerComponent.h"
#include "helpers.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_timer.h"

#include "bx/bx.h"
#include "bx/constants.h"
#include "bx/math.h"
#include "bgfx/defines.h"

using namespace Syngine;

Core::Core() {
    //initialize app
    this->app = new App();
    this->app->graphics = nullptr; // No graphics attached initially
    this->app->synModels = new AssimpLoader(); // Initialize the model loader

    this->app->physicsManager = new Phys(); // Initialize the physics manager
    this->app->physicsManager->Init(this->app->debug); // Initialize the physics system
}

Core::~Core() {
    //cleanup
    if (this->app) {
        if (this->app->graphics) {
            delete this->app->graphics;
            this->app->graphics = nullptr;
        }
        if (this->app->synModels) {
            this->app->synModels->UnloadAll();
            //delete this->app->synModels; //It was getting mad.
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

int Core::AttachGraphics(Graphics* graphics) {
    //attach graphics to app
    if (!graphics) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No graphics object provided to attach");
        return 1;
    }
    
    this->app->graphics = graphics;
    return 0;
}

int Core::DetachGraphics() {
    //detach graphics from app
    if (!this->app->graphics) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No graphics object to detach");
        return 1;
    }
    
    this->app->graphics = nullptr;
    return 0;
}

int Core::SyngineEventLoop() {
    //main loop. this will obviously be replaced with a more complex and modular/multi-threaded loop in the Future
    SDL_Log("Entering event loop");

    if (!this->app->graphics || !this->app->graphics->win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Graphics not initialized or window not created");
        return 1;
    }

    GameObject* player = new GameObject("player", "default");

    player->AddComponent(Syngine::SYN_COMPONENT_TRANSFORM);
    player->AddComponent(Syngine::SYN_COMPONENT_PLAYER);
    player->AddComponent(Syngine::SYN_COMPONENT_RIGIDBODY);
    player->AddComponent(Syngine::SYN_COMPONENT_CAMERA);

    TransformComponent* pTransform = player->GetComponent<TransformComponent>();
    pTransform->SetPosition(0.0f, 20.0f, 0.0f);

    RigidbodyComponent* rbComp =
        player->GetComponent<RigidbodyComponent>();
    if (!rbComp) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get RigidbodyComponent for player");
        delete player; // Clean up the player object
        return 1;
    }
    rbComp->Init(pTransform,
                 this->app->physicsManager,
                 PhysicsShapes::CAPSULE,
                 { 60.0f, 0.3f, 0.1f }, // mass, friction, restitution
                 JPH::EMotionType::Dynamic,
                 Layers::MOVING,
                 { 0.7f, 0.3f });

    player->GetComponent<PlayerComponent>()->Init(
        player->GetComponent<CameraComponent>(),
        this->app->graphics->win,
        rbComp);
    
    this->app->gameObjects.push_back(player);
    
    bool running = true;
    bool playerMode = false; // False is editor mode, True is player mode
    bool simulate = false; // Toggles the physics simulation (always true in player mode)
    bool rmb = false;
    bool mouseState = false;
    float mouseX, mouseY;

    CameraComponent* cam = new CameraComponent(nullptr);
    // FinalCam specifically is so we can keep track of both the player camera
    // and the editor camera, switching between them as needed.
    CameraComponent* finalCam = new CameraComponent(nullptr);
    bx::Vec3 startDir = {180.0f, 0.0f, 90.0f};
    
    const float sensitivity = 0.002f; // Adjust sensitivity as needed
    const float maxPitch =
    bx::kPiHalf - 0.01f; // Limit pitch to avoid gimbal lock
    const float moveSpeed = 2.0f; // Speed of camera movement
    const float sprintMult = 2.0f;
    const float crouchSpeed = 0.5f;
    
    const float physicsTimestep = 1.0f / 60.0f;
    const int   physicsSteps    = 1;
    float accumlator = 0.0f;
    
    float oneSec       = 0.0f;
    int   frame        = 0;
    int   frameDisplay = 0;
    int   physCounter  = 0;
    int   lastFPS      = 0;
    int   lastTPS      = 0;    

    SDL_Event event;

    uint64 NOW = SDL_GetPerformanceCounter();
    uint64 LAST = 0;

    while (running) {
        LAST = NOW;
        NOW  = SDL_GetPerformanceCounter();
        float deltaTime =
            (float)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());
        oneSec += deltaTime;
        frame++;
        frameDisplay++;

        const bool* keystate    = SDL_GetKeyboardState(NULL);
        bx::Vec3    moveVector  = { 0, 0, 0 };

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
                    model->AddComponent(Syngine::SYN_COMPONENT_TRANSFORM);
                    model->AddComponent(Syngine::SYN_COMPONENT_MESH);
                    model->AddComponent(Syngine::SYN_COMPONENT_RIGIDBODY);
                    MeshComponent* meshComp = model->GetComponent<MeshComponent>();
                    if (meshComp) meshComp->LoadMesh(modelPath);
                    RigidbodyComponent* physComp = model->GetComponent<RigidbodyComponent>();
                    if (physComp) {
                            physComp->Init(
                                model->GetComponent<TransformComponent>(),
                                this->app->physicsManager,
                                PhysicsShapes::MESH,
                                {0.0f, 0.8f, 0.1f},
                                JPH::EMotionType::Static,
                                Layers::NON_MOVING,
                                {}
                            );
                    }
                    this->app->gameObjects.push_back(model);
                } else if (event.key.key == SDLK_ESCAPE) {
                    playerMode = !playerMode;
                    simulate = playerMode; // Always simulate in player mode
                } else if (event.key.key == SDLK_P) {
                    if (!playerMode) {
                        simulate = !simulate;
                    }
                } else if (event.key.key == SDLK_1) {
                    string modelPath = resolveOSPath("meshes/cube.glb");
                    GameObject* cube = new GameObject("cube", "default");
                    cube->AddComponent(Syngine::SYN_COMPONENT_TRANSFORM);
                    cube->AddComponent(Syngine::SYN_COMPONENT_MESH);
                    cube->AddComponent(Syngine::SYN_COMPONENT_RIGIDBODY);
                    MeshComponent* meshComp = cube->GetComponent<MeshComponent>();
                    if (meshComp) meshComp->LoadMesh(modelPath, false);
                    TransformComponent* tComp = cube->GetComponent<TransformComponent>();
                    if (tComp) tComp->SetPosition(0.0f, 10.0f, 0.0f);
                    RigidbodyComponent* physComp = cube->GetComponent<RigidbodyComponent>();
                    std::vector<float> shapeParams = {1.0f, 1.0f, 1.0f}; // full extents
                    if (physComp)
                        physComp->Init(tComp,
                                       this->app->physicsManager,
                                       PhysicsShapes::BOX,
                                       {1000.0f, 0.7f, 0.02f}, // mass, friction, restitution
                                       JPH::EMotionType::Dynamic,
                                       Layers::MOVING,
                                       shapeParams);
                    this->app->gameObjects.push_back(cube);
                } else if (event.key.key == SDLK_2) {
                                        string modelPath = resolveOSPath("meshes/sphere.glb");
                    GameObject* sphere = new GameObject("sphere", "default");
                    sphere->AddComponent(Syngine::SYN_COMPONENT_TRANSFORM);
                    sphere->AddComponent(Syngine::SYN_COMPONENT_MESH);
                    sphere->AddComponent(Syngine::SYN_COMPONENT_RIGIDBODY);
                    MeshComponent* meshComp = sphere->GetComponent<MeshComponent>();
                    if (meshComp) meshComp->LoadMesh(modelPath, false);
                    TransformComponent* tComp = sphere->GetComponent<TransformComponent>();
                    if (tComp) tComp->SetPosition(0.0f, 10.0f, 0.0f);
                    RigidbodyComponent* physComp = sphere->GetComponent<RigidbodyComponent>();
                    std::vector<float> shapeParams = { 0.95f }; // half extents for sphere shape (diameter)
                    if (physComp)
                        physComp->Init(tComp,
                                       this->app->physicsManager,
                                       PhysicsShapes::SPHERE,
                                       {520.0f, 0.7f, 0.02f}, // mass, friction, restitution
                                       JPH::EMotionType::Dynamic,
                                       Layers::MOVING,
                                       shapeParams);
                    this->app->gameObjects.push_back(sphere);
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    rmb = true;
                    mouseX = event.button.x;
                    mouseY = event.button.y;
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && !playerMode) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    rmb = false;
                    SDL_WarpMouseInWindow(this->app->graphics->win, mouseX, mouseY);
                }
            } else if (event.type == SDL_EVENT_MOUSE_MOTION && rmb && !playerMode) {
                //mouse motion
                float dx = event.motion.xrel * sensitivity;
                float dy = event.motion.yrel * sensitivity;

                float cx, cy = 0.0f;
                cam->GetAngles(cx, cy);
                cx += dx;
                cy -= dy;
                cy = bx::clamp(cy, -maxPitch, maxPitch);
                // Update camera angles
                cam->SetAngles(cx, cy);
            }

            if (playerMode) {
                if (player) {
                    PlayerComponent* playerComp =
                        player->GetComponent<PlayerComponent>();
                    if (playerComp)
                        playerComp->HandleInput(event, playerMode, sensitivity, maxPitch);
                }
            }
        }

        //mouse move
        if (this->app->graphics->win) {
            bool desiredMouseState = rmb || playerMode;
            if (desiredMouseState != mouseState) {
                SDL_SetWindowRelativeMouseMode(this->app->graphics->win, desiredMouseState);
                mouseState = desiredMouseState;
            }
        }

        // camera mode stuff
        if (!playerMode) {
            // camera movement
            float realSpeed = moveSpeed; // Default speed
            if (keystate[SDL_SCANCODE_LSHIFT]) realSpeed = moveSpeed * sprintMult;
            if (keystate[SDL_SCANCODE_LCTRL]) realSpeed = moveSpeed * crouchSpeed;

            if (keystate[SDL_SCANCODE_W]) {
                float yaw, pitch;
                cam->GetAngles(yaw, pitch);
                const float* posPtr = cam->GetPosition();
                float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

                moveVector = {
                    cosf(pitch) * sinf(yaw),
                    sinf(pitch),
                    cosf(pitch) * cosf(yaw)
                };
                moveVector = bx::mul(moveVector, realSpeed * deltaTime);
                moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
                cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
            }
            if (keystate[SDL_SCANCODE_S]) {
                float yaw, pitch;
                cam->GetAngles(yaw, pitch);
                const float* posPtr = cam->GetPosition();
                float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

                moveVector = {
                    -cosf(pitch) * sinf(yaw),
                    -sinf(pitch),
                    -cosf(pitch) * cosf(yaw)
                };
                moveVector = bx::mul(moveVector, realSpeed * deltaTime);
                moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
                cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
            }
            if (keystate[SDL_SCANCODE_A]) {
                float yaw, pitch;
                cam->GetAngles(yaw, pitch);
                const float* posPtr = cam->GetPosition();
                float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

                moveVector = {
                    sinf(yaw - bx::kPiHalf),
                    0.0f,
                    cosf(yaw - bx::kPiHalf)
                };
                moveVector = bx::mul(moveVector, realSpeed * deltaTime);
                moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
                cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
            }
            if (keystate[SDL_SCANCODE_D]) {
                float yaw, pitch;
                cam->GetAngles(yaw, pitch);
                const float* posPtr = cam->GetPosition();
                float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

                moveVector = {
                    sinf(yaw + bx::kPiHalf),
                    0.0f,
                    cosf(yaw + bx::kPiHalf)
                };
                moveVector = bx::mul(moveVector, realSpeed * deltaTime);
                moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
                cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
            }
            if (keystate[SDL_SCANCODE_Q]) {
                float yaw, pitch;
                cam->GetAngles(yaw, pitch);
                const float* posPtr = cam->GetPosition();
                float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

                moveVector = {
                    0.0f,
                    -1.0f,
                    0.0f
                };
                moveVector = bx::mul(moveVector, realSpeed * deltaTime);
                moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
                cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
            }
            if (keystate[SDL_SCANCODE_E]) {
                float yaw, pitch;
                cam->GetAngles(yaw, pitch);
                const float* posPtr = cam->GetPosition();
                float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

                moveVector = {
                    0.0f,
                    1.0f,
                    0.0f
                };
                moveVector = bx::mul(moveVector, realSpeed * deltaTime);
                moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
                cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
            }
            finalCam = cam;
        } else {
            // player mode, use player camera
            if (player && player->GetComponent<CameraComponent>()) {
                finalCam = player->GetComponent<CameraComponent>();
            } else {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Player or CameraComponent not found");
                return 1;
            }
        }

        // simulation stuff
        if (simulate) {
            if (this->app->physicsManager) {
                accumlator += deltaTime;

                while(accumlator >= physicsTimestep) {
                    this->app->physicsManager->Update(physicsTimestep, physicsSteps);
                    physCounter++;
                    accumlator -= physicsTimestep;
                }
            }
        }

        //sun
        if (keystate[SDL_SCANCODE_LEFT]) {
            startDir.x += 2.0f;
        }
        if (keystate[SDL_SCANCODE_RIGHT]) {
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

        if (playerMode) {
            if (player->GetComponent<PlayerComponent>()) {
                player->GetComponent<PlayerComponent>()->Update(
                    keystate, moveSpeed, sprintMult, crouchSpeed);
            }
        }

        for (auto* gameObject : this->app->gameObjects) {
            if (gameObject) {
                RigidbodyComponent* physComp = gameObject->GetComponent<RigidbodyComponent>();
                if (physComp && physComp->isEnabled) {
                    physComp->Update(simulate);
                }
            }
        }

        if (this->app && this->app->graphics) {
            if (this->app->debug) {
                this->app->physicsManager->DrawDebug(
                    finalCam->GetCamera().view,
                    finalCam->GetCamera().proj,
                    this->app->graphics->width,
                    this->app->graphics->height,
                    this->app->graphics->GetProgram("debugger").program);
            }
            this->app->graphics->RenderFrame(this->app->gameObjects, lightDir, finalCam); // render frame
        }

        if (oneSec >= 1.0f) {
            lastFPS = frameDisplay;
            lastTPS = physCounter;
            physCounter = 0;
            frameDisplay = 0;
            oneSec -= 1.0f;
        }
        
        if (frame % 60 == 0) {
            SDL_Log("Frame: %d, Gameobjects: %d, Sim: %s, Mode: %s, FPS/TPS: "
                    "%d/%d, Camera coords: (%.1f, %.1f, %.1f)",
                    frame,
                    (int)this->app->gameObjects.size(),
                    simulate ? "ON" : "OFF",
                    playerMode ? "Player" : "Editor",
                    lastFPS,
                    lastTPS,
                    finalCam->GetPosition()[0],
                    finalCam->GetPosition()[1],
                    finalCam->GetPosition()[2]);
        }
    }
    
    this->app->synModels->UnloadAll();
    SDL_Log("Exiting event loop");
    return 0;
}

int Core::DeleteGameobject(GameObject* gameobject) {
    if(!gameobject) return 0;
    MeshComponent* mesh = gameobject->GetComponent<MeshComponent>();
    if(mesh) {
        mesh->UnloadMesh();
    }
    delete gameobject; // delete the gameobject
    gameobject = nullptr; // set to null to avoid dangling pointer
    return 0;
}