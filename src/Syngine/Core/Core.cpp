// ╒═══════════════════════════ Core.cpp ═╕
// │ Syngine                              │
// │ Created 2025-04-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

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

#include "Syngine/Core/Core.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/Core/Registry.h"
#include "Syngine/Core/TempInput.h"
#include "Syngine/Graphics/Windowing.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/Physics/Physics.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/ECS/AllComponents.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/Utils/FsUtils.h"

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

Syngine::Core* Syngine::Core::m_instance = nullptr;
Syngine::App*  Syngine::Core::m_app      = nullptr;
bool           Syngine::Core::m_shouldClose = false;
Core::_internal Syngine::Core::m_internal;

Core::Core(const EngineConfig config) {
    if (m_instance) {
        Syngine::Logger::Fatal("Only one instance of Core is allowed.");
    }
    m_instance = this;

    // Check if required folders exist (shaders, meshes)
    // CheckRequiredFolders will abort if any folder is missing
    if (Syngine::_CheckRequiredFolders()) {
        m_app = new App();
        m_app->config = config;
    }
}

Core::~Core() {
    // Cleanup
    if (m_app) {
        if (m_app->renderer) {
            delete m_app->renderer;
            m_app->renderer = nullptr;
        }
        if (m_app->synModels) {
            m_app->synModels->_UnloadAllMeshes();
            //delete m_app->synModels; //It was getting mad.
            m_app->synModels = nullptr;
        }
        if (m_app->physicsManager) {
            m_app->physicsManager->_Shutdown();
            delete m_app->physicsManager;
            m_app->physicsManager = nullptr;
        }
        delete m_app;
        m_app = nullptr;
        m_instance = nullptr; // Reset the singleton instance
    }
}

bool Core::Initialize() {
    m_app->window = new Window(m_app->config);
    m_app->renderer = new Renderer(m_app->config.windowWidth, m_app->config.windowHeight);
    m_app->synModels = new AssimpLoader();
    m_app->physicsManager = new Phys();

    m_app->physicsManager->_Init(m_app->debug);

    // Create base components
    _MakePlayer();
    _MakeEditorCamera();

    return true;
}

Syngine::Core* Syngine::Core::_Get() { return m_instance; }
Syngine::App* Syngine::Core::_GetApp() {
    return m_instance ? m_instance->m_app : nullptr;
}

bool Core::IsRunning() {
    return !m_shouldClose && !m_app->window->ShouldClose();
}

bool Core::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            m_shouldClose = true;
            break;
        case SDL_EVENT_WINDOW_RESIZED: {
            int w, h;
            SDL_Window* resizedWindow = SDL_GetWindowFromID(event.window.windowID);
            SDL_GetWindowSize(resizedWindow, &w, &h);

            m_app->renderer->width = w;
            m_app->renderer->height = h;
            bgfx::reset(w, h, BGFX_RESET_VSYNC); // reset bgfx with new window size
            bgfx::setViewRect(
                0, 0, 0, uint16_t(w), uint16_t(h)); // reset view rect
            break;
            }
        case SDL_EVENT_KEY_DOWN: {
            if (event.key.key == SDLK_ESCAPE) {
                m_internal.simulate = !m_internal.simulate;
                this->m_app->debug = !m_internal.simulate; // Disable debug mode when simulating
            } else if (event.key.key == SDLK_F1) {
                // Toggle debug mode
                this->m_app->debug = !this->m_app->debug;
                if (this->m_app->debug) {
                    Syngine::Logger::Info("Debug mode enabled");
                } else {
                    Syngine::Logger::Info("Debug mode disabled");
                }
            } else if (event.key.key == SDLK_F5) {
                // Reload changed assets
                for (auto& go : Registry::GetGameObjectsWithComponent(SYN_COMPONENT_MESH)) {
                    MeshComponent* mc = go->GetComponent<MeshComponent>();
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
                m_app->renderer->ReloadAllPrograms();
            }
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if (event.button.button == SDL_BUTTON_RIGHT && !m_internal.simulate) {
                m_internal.rmb = true;
                m_internal.mouseX = event.button.x;
                m_internal.mouseY = event.button.y;
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            if (event.button.button == SDL_BUTTON_RIGHT && !m_internal.simulate) {
                m_internal.rmb = false;
                SDL_WarpMouseInWindow(m_app->window->_GetSDLWindow(), m_internal.mouseX, m_internal.mouseY);
            }
            break;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            if (m_internal.rmb && !m_internal.simulate) {
                // Mouse motion
                float dx = event.motion.xrel * m_internal.sensitivity;
                float dy = event.motion.yrel * m_internal.sensitivity;

                float cx, cy = 0.0f;
                m_internal.cam->GetAngles(cx, cy);
                cx += dx;
                cy -= dy;
                cy = bx::clamp(cy, -m_internal.maxPitch, m_internal.maxPitch);
                // Update camera angles
                m_internal.cam->SetAngles(cx, cy);
            }
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            // Mouse wheel scroll
            if (!m_internal.simulate) {
                if (event.wheel.y > 0) {
                    // Scroll up
                    m_internal.editorMoveSpeed += 0.5f; // Increase speed
                    if (m_internal.editorMoveSpeed > 100.0f) {
                        m_internal.editorMoveSpeed = 100.0f; // Cap speed
                    }
                } else if (event.wheel.y < 0) {
                    // Scroll down
                    m_internal.editorMoveSpeed -= 0.5f; // Decrease speed
                    if (m_internal.editorMoveSpeed < 0.0f) {
                        m_internal.editorMoveSpeed = 0.0f; // Cap speed
                    }
                }
            }
        }
        }

        // Handle other events as needed
        InputManager::HandleEvent(event);

        if (m_internal.simulate) {
            auto players =
                Registry::GetGameObjectsWithComponent(SYN_COMPONENT_PLAYER);
            for (auto go : players) {
                if (!go) continue;
                PlayerComponent* pc = go->GetComponent<PlayerComponent>();
                if (!pc) continue;
                pc->_HandleInput(event);
            }
        }
    }
    return true;
}

bool Core::Update() {
    m_internal.last = m_internal.now;
    m_internal.now  = SDL_GetPerformanceCounter();
    float deltaTime = (m_internal.now - m_internal.last) / (float)SDL_GetPerformanceFrequency();
    m_internal.oneSecond += deltaTime;
    
    // Simulate physics
    if (m_app->physicsManager && m_internal.simulate) {
        m_internal.accumulator += deltaTime;

        while(m_internal.accumulator >= m_internal.physicsTimestep) {
            m_app->physicsManager->_Update(m_internal.physicsTimestep, m_internal.physicsSteps);
            m_internal.accumulator -= m_internal.physicsTimestep;
            m_internal.physCounter++;
        }
    }

    const bool* keystate = SDL_GetKeyboardState(NULL);
    if (m_internal.simulate) {
        auto players =
            Registry::GetGameObjectsWithComponent(SYN_COMPONENT_PLAYER);
        for (auto go : players) {
            if (!go) continue;
            PlayerComponent* pc = go->GetComponent<PlayerComponent>();
            if (!pc) continue;
            pc->Update(keystate, deltaTime);
        }

        auto rigidbodies = Registry::GetGameObjectsWithComponent(SYN_COMPONENT_RIGIDBODY);
        for (auto go : rigidbodies) {
            if (!go) continue;
            RigidbodyComponent* rb = go->GetComponent<RigidbodyComponent>();
            if (!rb) continue;
            rb->Update(m_internal.physicsTimestep);
        }

        // Player mode: update player camera
        for (auto go : players) {
            if (!go) continue;
            PlayerComponent* pc = go->GetComponent<PlayerComponent>();
            if (!pc) continue;
            pc->_PostPhysicsUpdate();
        }
    } else {
        // Editor mode: handle editor camera
        _HandleEditorCamera(keystate, deltaTime);
    }

    // Hide camera when moving mouse or simulating
    if (Renderer::IsReady()) {
        bool desiredMouseState = m_internal.rmb || m_internal.simulate;
        if (desiredMouseState != m_internal.mouseState) {
            SDL_Log("Mouse mode changed: %s", desiredMouseState ? "ON" : "OFF");
            SDL_SetWindowRelativeMouseMode(m_app->window->_GetSDLWindow(), desiredMouseState);
            m_internal.mouseState = desiredMouseState;
        }
    }

    if (m_internal.oneSecond >= 1.0f) {
        m_internal.lastFPS = m_internal.frameDisplay;
        m_internal.lastTPS = m_internal.physCounter;
        m_internal.frameDisplay = 0;
        m_internal.physCounter = 0;
        m_internal.oneSecond    = 0.0f;

        SDL_Log("Frame: %d, Gameobjects: %d, Sim: %s, FPS/TPS: %d/%d",
                m_internal.frameCount,
                (int)Registry::GetGameObjectCount(),
                m_internal.simulate ? "ON" : "OFF",
                m_internal.lastFPS,
                m_internal.lastTPS);
    }

    return true;
}

bool Core::Render() {
    // Select camera based on mode
    CameraComponent* activeCam = m_internal.cam;
    if (m_internal.simulate) {
        activeCam = Registry::GetGameObjectByName("player")
                        ->GetComponent<CameraComponent>();
    }

    // Render the application
    // TODO: fucking fix this this is not okay
    bx::Vec3 lightDir = { 0.0f, -1.0f, 0.0f };
    if (Renderer::IsReady()) {
        m_internal.frameCount++;

        m_app->renderer->_RenderFrame(lightDir,
                                             activeCam,
                                             this->m_app->debug);

        if (this->m_app->debug) {
            // christ on a stick this call is ridiculous
            this->m_app->physicsManager->_DrawDebug(
                this->m_app->renderer->width,
                this->m_app->renderer->height,
                this->m_app->renderer->GetProgram("debugger").program,
                Registry::GetGameObjectByName("player") // Player object has the camera
                    ->GetComponent<Syngine::CameraComponent>()
                    ->GetCamera(), activeCam->GetCamera());
        }
    }
    return true;
}

Syngine::HardwareSpecs Core::GetSystemSpecifications() {
    if (!Renderer::IsReady()) {
        Syngine::Logger::Warn("Attempt to get hardware specs before renderer "
                              "is ready. This is not allowed.");
        return Syngine::HardwareSpecs();
    }
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
    SDL_DisplayID dId = SDL_GetDisplayForWindow(m_app->window->_GetSDLWindow());
    SDL_DisplayMode disMode = *SDL_GetCurrentDisplayMode(dId);
    specs.screenWidth = disMode.w;
    specs.screenHeight = disMode.h;

    // Get window resolution
    int w, h;
    SDL_GetWindowSize(m_app->window->_GetSDLWindow(), &w, &h);
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

void Core::_MakePlayer() {
    // Create player GameObject
    GameObject* player = new GameObject("player");
    player->AddComponent<TransformComponent>()->SetPosition(0.0f, 20.0f, 0.0f);
    player->AddComponent<CameraComponent>();
    player->AddComponent<PlayerComponent>(player->GetComponent<CameraComponent>(), m_app->window->_GetSDLWindow());
}

void Core::_MakeEditorCamera() {
    m_internal.cam = new CameraComponent(nullptr);
    m_internal.cam->SetFarPlane(2000);
}

void Core::_HandleEditorCamera(const bool* keyState, float deltaTime) {
    if (!m_internal.simulate) {
        // Handle editor camera movement
        bx::Vec3 moveVector = { 0, 0, 0 };
        float realSpeed = m_internal.editorMoveSpeed; // Default speed
        if (keyState[SDL_SCANCODE_LSHIFT]) realSpeed = m_internal.editorMoveSpeed * m_internal.sprintMultiplier;
        if (keyState[SDL_SCANCODE_LCTRL]) realSpeed = m_internal.editorMoveSpeed * m_internal.crouchSpeed;

        if (keyState[SDL_SCANCODE_W]) {
            float yaw, pitch;
            m_internal.cam->GetAngles(yaw, pitch);
            const float* posPtr = m_internal.cam->GetPosition();
            float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

            moveVector = {
                cosf(pitch) * sinf(yaw),
                sinf(pitch),
                cosf(pitch) * cosf(yaw)
            };
            moveVector = bx::mul(moveVector, realSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
            m_internal.cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
        }
        if (keyState[SDL_SCANCODE_S]) {
            float yaw, pitch;
            m_internal.cam->GetAngles(yaw, pitch);
            const float* posPtr = m_internal.cam->GetPosition();
            float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

            moveVector = {
                -cosf(pitch) * sinf(yaw),
                -sinf(pitch),
                -cosf(pitch) * cosf(yaw)
            };
            moveVector = bx::mul(moveVector, realSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
            m_internal.cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
        }
        if (keyState[SDL_SCANCODE_A]) {
            float yaw, pitch;
            m_internal.cam->GetAngles(yaw, pitch);
            const float* posPtr = m_internal.cam->GetPosition();
            float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

            moveVector = {
                sinf(yaw - bx::kPiHalf),
                0.0f,
                cosf(yaw - bx::kPiHalf)
            };
            moveVector = bx::mul(moveVector, realSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
            m_internal.cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
        }
        if (keyState[SDL_SCANCODE_D]) {
            float yaw, pitch;
            m_internal.cam->GetAngles(yaw, pitch);
            const float* posPtr = m_internal.cam->GetPosition();
            float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

            moveVector = {
                sinf(yaw + bx::kPiHalf),
                0.0f,
                cosf(yaw + bx::kPiHalf)
            };
            moveVector = bx::mul(moveVector, realSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
            m_internal.cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
        }
        if (keyState[SDL_SCANCODE_Q]) {
            float yaw, pitch;
            m_internal.cam->GetAngles(yaw, pitch);
            const float* posPtr = m_internal.cam->GetPosition();
            float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

            moveVector = {
                0.0f,
                -1.0f,
                0.0f
            };
            moveVector = bx::mul(moveVector, realSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
            m_internal.cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
        }
        if (keyState[SDL_SCANCODE_E]) {
            float yaw, pitch;
            m_internal.cam->GetAngles(yaw, pitch);
            const float* posPtr = m_internal.cam->GetPosition();
            float pos[3] = { posPtr[0], posPtr[1], posPtr[2] };

            moveVector = {
                0.0f,
                1.0f,
                0.0f
            };
            moveVector = bx::mul(moveVector, realSpeed * deltaTime);
            moveVector = bx::add(bx::Vec3(pos[0], pos[1], pos[2]), moveVector);
            m_internal.cam->SetPosition(moveVector.x, moveVector.y, moveVector.z);
        }
    }
}
