#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <intrin.h>

#elif __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>

#else
#include "sys/utsname.h"

#endif

#include "SyngineCore.h"
#include "SyngineGameobject.h"
#include "SyngineLogger.h"
#include "SynginePhys.h"
#include "Registry.h"
#include "Components.h"
#include "SynComponents.h"
#include "FsUtils.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"

#include "bx/bx.h"
#include "bx/constants.h"
#include "bx/math.h"
#include "bgfx/defines.h"


#include <filesystem>

using namespace Syngine;

Syngine::Core* Syngine::Core::s_instance = nullptr;

Core::Core(const std::string& appName) {
    if (s_instance) {
        Syngine::Logger::Fatal("Only one instance of Core is allowed.");
    }

    s_instance = this;

    //initialize app
    this->app = new App();
    this->app->appName = appName;
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
        s_instance = nullptr; // Reset the singleton instance
    }
}

Syngine::Core* Syngine::Core::_Get() { return s_instance; }
Syngine::App* Syngine::Core::_GetApp() {
    return s_instance ? s_instance->app : nullptr;
}

int Core::AttachGraphics(Graphics* graphics) {
    //attach graphics to app
    if (!graphics) {
        Syngine::Logger::Error("No graphics object provided to attach");
        return 1;
    }
    
    this->app->graphics = graphics;
    return 0;
}

int Core::DetachGraphics() {
    //detach graphics from app
    if (!this->app->graphics) {
        Syngine::Logger::Error("No graphics object to detach");
        return 1;
    }
    
    this->app->graphics = nullptr;
    return 0;
}

int Core::SyngineEventLoop() {
    //main loop. this will obviously be replaced with a more complex and modular/multi-threaded loop in the Future
    Syngine::Logger::Info("Entering event loop");

    if (!this->app->graphics || !this->app->graphics->win) {
        Syngine::Logger::Error("Graphics not initialized or window not created");
        return 1;
    }

    GameObject* player = new GameObject("player");

    player->AddComponent<Syngine::TransformComponent>()->SetPosition(
        0.0f, 20.0f, 0.0f);
    auto* playerCamera = player->AddComponent<Syngine::CameraComponent>();
    player->AddComponent<Syngine::PlayerComponent>(playerCamera, this->app->graphics->win);
    Registry::AddGameObject(player);

    bool running = true;
    bool playerMode = false; // False is editor mode, True is player mode
    bool simulate = false; // Toggles the physics simulation (always true in player mode)
    bool rmb = false;
    bool mouseState = false;
    float mouseX, mouseY;

    // Lol this is foul creating a component directly like this, but it works for now
    CameraComponent* cam = new CameraComponent(nullptr);
    cam->SetFarPlane(2000);
    // FinalCam specifically is so we can keep track of both the player camera
    // and the editor camera, switching between them as needed.
    CameraComponent* finalCam = new CameraComponent(nullptr);
    bx::Vec3 startDir = {180.0f, 0.0f, 90.0f};

    const float sensitivity = 0.002f; // Adjust sensitivity as needed
    const float maxPitch =
    bx::kPiHalf - 0.01f; // Limit pitch to avoid gimbal lock
    float editorMoveSpeed = 3.0f; // Speed of camera movement
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
                    std::string modelPath = Syngine::ResolveOSPath("meshes/ground.glb");
                    GameObject* model = new GameObject("ground", "terrain");
                    Syngine::RigidbodyParameters params = {
                        .shape           = PhysicsShapes::MESH,
                        .mass            = 0.0f,
                        .friction        = 0.8f,
                        .restitution     = 0.1f,
                        .shapeParameters = {},
                        .motionType      = JPH::EMotionType::Static,
                        .layer           = Layers::NON_MOVING
                    };
                    
                    model->AddComponent<Syngine::TransformComponent>();
                    model->AddComponent<Syngine::MeshComponent>(modelPath);
                    model->AddComponent<Syngine::RigidbodyComponent>(params);

                    Registry::AddGameObject(model);
                } else if (event.key.key == SDLK_ESCAPE) {
                    playerMode = !playerMode;
                    simulate = playerMode; // Always simulate in player mode
                } else if (event.key.key == SDLK_P) {
                    if (!playerMode) {
                        simulate = !simulate;
                    }
                } else if (event.key.key == SDLK_1) {
                    std::string modelPath = Syngine::ResolveOSPath("meshes/cube.glb");
                    GameObject* cube = new GameObject("cube", "default");

                    std::vector<float> boxExtents = { 2.0f,
                                                       2.0f,
                                                       2.0f }; // full extents
                    Syngine::RigidbodyParameters params     = {
                            .shape           = PhysicsShapes::BOX,
                            .mass            = 0.0f, // Static body
                            .friction        = 0.7f,
                            .restitution     = 0.02f,
                            .shapeParameters = boxExtents,
                            .motionType      = JPH::EMotionType::Dynamic,
                            .layer           = Layers::MOVING
                    };
                    
                    auto* tComp = cube->AddComponent<Syngine::TransformComponent>();
                    if (tComp) {
                        tComp->SetPosition(0.0f, 10.0f, 0.0f);
                        tComp->SetScale(2.0f, 2.0f, 2.0f);
                    }

                    cube->AddComponent<Syngine::MeshComponent>(modelPath, false);
                    cube->AddComponent<Syngine::RigidbodyComponent>(params);
                    
                    Registry::AddGameObject(cube);
                } else if (event.key.key == SDLK_2) {
                    std::string modelPath = Syngine::ResolveOSPath("meshes/sphere.glb");
                    GameObject* sphere = new GameObject("sphere", "default");

                    std::vector<float> sphereExtents = { // Full extents for sphere shape (diameter)
                        1.9f
                    };
                    Syngine::RigidbodyParameters sphereParams = {
                        .shape       = PhysicsShapes::SPHERE,
                        .mass        = 0.0f, // Static body
                        .friction    = 0.7f,
                        .restitution = 0.02f,
                        .shapeParameters = sphereExtents,
                        .motionType = JPH::EMotionType::Dynamic,
                        .layer = Layers::MOVING
                    };

                    auto* tComp = sphere->AddComponent<Syngine::TransformComponent>();
                    if (tComp) tComp->SetPosition(0.0f, 10.0f, 0.0f), tComp->SetScale(2.0f, 2.0f, 2.0f);
                    sphere->AddComponent<Syngine::MeshComponent>(modelPath, false);
                    sphere->AddComponent<Syngine::RigidbodyComponent>(sphereParams);

                    Registry::AddGameObject(sphere);
                } else if (event.key.key == SDLK_F1) {
                    // Toggle debug mode
                    this->app->debug = !this->app->debug;
                    if (this->app->debug) {
                        Syngine::Logger::Info("Debug mode enabled");
                    } else {
                        Syngine::Logger::Info("Debug mode disabled");
                    }
                } else if (event.key.key == SDLK_F5) {
                    // Reload changed assets
                    for (auto& go : Registry::GetAllGameObjects()) {
                        if (!go.second) continue;
                        MeshComponent* mc = go.second->GetComponent<MeshComponent>();
                        if (!mc) continue;
                        MeshData& mesh = mc->meshData;
                        if (!mesh.valid) continue;
                        if (mesh.lastWriteTime !=
                            std::filesystem::last_write_time(mesh.path)) {
                            mc->ReloadMesh();
                        }
                    }
                } else if (event.key.key == SDLK_F6) {
                    // Reload all shaders
                    this->app->graphics->ReloadAllPrograms();
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT && !playerMode) {
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
            } else if (event.type == SDL_EVENT_MOUSE_WHEEL && !playerMode) {
                // Mouse wheel scroll
                if (event.wheel.y > 0) {
                    // Scroll up
                    editorMoveSpeed += 0.5f; // Increase speed
                    if (editorMoveSpeed > 100.0f) {
                        editorMoveSpeed = 100.0f; // Cap speed
                    }
                } else if (event.wheel.y < 0) {
                    // Scroll down
                    editorMoveSpeed -= 0.5f; // Decrease speed
                    if (editorMoveSpeed < 0.0f) {
                        editorMoveSpeed = 0.0f; // Cap speed
                    }
                }
            }

            if (playerMode) {
                if (player) {
                    PlayerComponent* playerComp =
                        player->GetComponent<PlayerComponent>();
                    if (playerComp)
                        playerComp->HandleInput(event);
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
            float realSpeed = editorMoveSpeed; // Default speed
            if (keystate[SDL_SCANCODE_LSHIFT]) realSpeed = editorMoveSpeed * sprintMult;
            if (keystate[SDL_SCANCODE_LCTRL]) realSpeed = editorMoveSpeed * crouchSpeed;

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
                Syngine::Logger::Error("Player or CameraComponent not found");
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
                player->GetComponent<PlayerComponent>()->Update(keystate,
                                                                playerMode);
            }
        }

        for (auto& gameObject : Registry::GetPhysicsObjects()) {
            gameObject->GetComponent<RigidbodyComponent>()->Update(simulate);
        }

        // Post-physics update for player component
        // (I do not like the two if playermode here but eh)
        if (playerMode) {
            if (player->GetComponent<PlayerComponent>()) {
                player->GetComponent<PlayerComponent>()->PostPhysicsUpdate();
            }
        }

        if (this->app && this->app->graphics) {
            this->app->graphics->_RenderFrame(lightDir,
                                             finalCam,
                                             this->app->debug);

            if (this->app->debug) {
                // christ on a stick this call is ridiculous
                this->app->physicsManager->_DrawDebug(
                    this->app->graphics->width,
                    this->app->graphics->height,
                    this->app->graphics->GetProgram("debugger").program,
                    Registry::GetGameObjectByName("player") // Player object has the camera
                        ->GetComponent<Syngine::CameraComponent>()
                        ->GetCamera(), finalCam->GetCamera());
            }
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
                    (int)Registry::GetGameObjectCount(),
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
    Syngine::Logger::Info("Exiting event loop");
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

Syngine::HardwareSpecs Core::GetSystemSpecifications() {
    Syngine::HardwareSpecs specs;
#ifdef _WIN32
    // On Windows, gather system information using Windows API
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    // Get OS
    const char* platform = SDL_GetPlatform();
    specs.osName = std::string(platform);

    // Get CPU info (why on EARTH is this so complicated?)
    std::string cpu;
    int         CPUInfo[4] = { -1 };
    char        CPUBrandString[0x40]; // Buffer for CPU brand string

    // Get the highest extended function supported by CPUID
    __cpuid(CPUInfo, 0x80000000);
    unsigned int nExIds = CPUInfo[0];

    // Get the CPU brand string if supported
    for (unsigned int i = 0x80000002; i <= nExIds && i <= 0x80000004; ++i) {
        __cpuid(CPUInfo, i);
        // Copy returned values into the CPU brand string buffer
        memcpy(CPUBrandString + (i - 0x80000002) * 16, CPUInfo, sizeof(CPUInfo));
    }
    CPUBrandString[sizeof(CPUBrandString) - 1] = '\0'; // Null-terminate the string
    cpu = std::string(CPUBrandString);
    specs.cpuModel = cpu;

    // Get arch
    std::string cpuArch;
    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: cpuArch = "x64"; break;
        case PROCESSOR_ARCHITECTURE_INTEL: cpuArch = "x86"; break;
        case PROCESSOR_ARCHITECTURE_ARM:   cpuArch = "ARM"; break;
        case PROCESSOR_ARCHITECTURE_ARM64: cpuArch = "ARM64"; break;
        default:                            cpuArch = "Unknown"; break;
    }
    specs.cpuArch = cpuArch;
    specs.cpuCores = sysInfo.dwNumberOfProcessors;

    // Get total physical memory
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    specs.ramMB = statex.ullTotalPhys / (1024 * 1024);

#elif __APPLE__
    // On macOS, gather system information using sysctl
    specs.osName = "macOS";
    try {
        char cpuBrand[256];
        size_t size = sizeof(cpuBrand);
        sysctlbyname("machdep.cpu.brand_string", cpuBrand, &size, NULL, 0);
        specs.cpuModel = std::string(cpuBrand);

        // Get logical CPU count
        int cpuProc = 0;
        size = sizeof(cpuProc);
        sysctlbyname("machdep.cpu.core_count", &cpuProc, &size, NULL, 0);
        specs.cpuCores = cpuProc;

        // Get RAM info
        uint64_t ramSize = 0;
        size = sizeof(ramSize);
        sysctlbyname("hw.memsize", &ramSize, &size, NULL, 0);
        specs.ramMB = ramSize / (1024 * 1024);
    } catch (const std::exception& e) {
        specs.ramMB = 0;
    }

#else
    // Get OS
    struct utsname sysInfo;
    if (uname(&sysInfo) < 0) {
        specs.osName = "Unknown";
        return specs;
    }
    specs.osName = std::string(sysInfo.sysname) + " " + std::string(sysInfo.release);

    // Get CPU info (name, architecture, etc.)
    std::ifstream cpuInfoFile("/proc/cpuinfo");
    std::string   line;
    std::string   cpuBrand = "Unknown";

    if (cpuInfoFile.is_open()) {
        while (std::getline(cpuInfoFile, line)) {
            if (line.find("model name") != std::string::npos) {
                cpuBrand = line.substr(line.find(":") + 2);
                break;
            }
        }
        cpuInfoFile.close();
    }
    specs.cpuModel = cpuBrand;

    specs.cpuArch = std::string(sysInfo.machine);

    // Get number of processors
    long numProcessors = sysconf(_SC_NPROCESSORS_ONLN);
    specs.cpuCores = numProcessors;

    // Get total physical memory
    long totalMemory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
    specs.ramMB = totalMemory / (1024 * 1024);
#endif

    // Get display size
    SDL_DisplayID dId = SDL_GetDisplayForWindow(app->graphics->win);
    SDL_DisplayMode disMode = *SDL_GetCurrentDisplayMode(dId);
    specs.screenWidth = disMode.w;
    specs.screenHeight = disMode.h;

    // Get window resolution
    int w, h;
    SDL_GetWindowSize(app->graphics->win, &w, &h);
    specs.winWidth = w;
    specs.winHeight = h;

    // Get various GPU info from bgfx
    const bgfx::Caps* caps = bgfx::getCaps();
    if (caps) {
        specs.gpuVendorID = caps->vendorId;
        specs.gpuDeviceID = caps->deviceId;
        specs.maxTextureSize = caps->limits.maxTextureSize;
        specs.supportsCompute = (caps->supported & BGFX_CAPS_COMPUTE) != 0;
        specs.supports3DTextures = (caps->supported & BGFX_CAPS_TEXTURE_3D) != 0;
    } else {
        specs.gpuVendorID = 0;
        specs.gpuDeviceID = 0;
        specs.maxTextureSize = 0;
        specs.supportsCompute = false;
        specs.supports3DTextures = false;
    }
    return specs;
}
