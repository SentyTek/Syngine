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
#include "Syngine/Core/Input.h"

#include "Syngine/Graphics/Windowing.h"
#include "Syngine/Graphics/Renderer.h"

#include "Syngine/Physics/Physics.h"

#include "Syngine/ECS/GameObject.h"
#include "Syngine/ECS/AllComponents.h"
#include "Syngine/ECS/Component.h"

#include "Syngine/Utils/FsUtils.h"
#include "Syngine/Utils/Version.h"

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
#include <memory>

using namespace Syngine;

Syngine::Core*      Syngine::Core::m_instance = nullptr;
Syngine::App*       Syngine::Core::m_app      = nullptr;
Core::_internal     Syngine::Core::m_internal;
Core::_FrameCounter Syngine::Core::m_frameCounter;

float Syngine::Core::deltaTime     = 0.0f;
bool  Syngine::Core::m_shouldClose = false;

Core::Core(const EngineConfig config) {
    if (m_instance) {
        Syngine::Logger::Fatal("Only one instance of Core is allowed.");
    }
    m_instance = this;

    // Check if required folders exist (shaders, meshes)
    // CheckRequiredFolders will abort if any folder is missing
    if (Syngine::_CheckRequiredFolders()) {
        Syngine::Logger::LogF(
            LogLevel::INFO, "Using Syngine v%s", SYN_VERSION_STRING);
        m_app         = new App();
        m_app->config = config;
    }
}

Core::~Core() {
    // Cleanup
    if (m_app) {
        if (m_app->window) {
            m_app->window.reset();
        }
        if (m_app->renderer) {
            m_app->renderer.reset();
        }
        if (m_app->synModels) {
            m_app->synModels->_UnloadAllMeshes();
            m_app->synModels.reset();
        }
        if (m_app->physicsManager) {
            m_app->physicsManager->_Shutdown();
            m_app->physicsManager.reset();
        }
        m_instance = nullptr; // Reset the singleton instance
    }
}

bool Core::Initialize() {
    if (!m_app) {
        Syngine::Logger::Fatal("Core not initialized properly. App is null.");
        return false;
    }

    try {
        m_app->window = std::make_unique<Window>(m_app->config);
        if (!m_app->window) {
            Logger::Error("Failed to create window. Check the log for more details.");
        }

        m_app->renderer = std::make_unique<Renderer>(
            m_app->config.windowWidth, m_app->config.windowHeight);
        if (!m_app->renderer) {
            Logger::Error("Failed to create renderer. Check the log for more details.");
        }

        m_app->synModels = std::make_unique<AssimpLoader>();
        if (!m_app->synModels) {
            Logger::Error("Failed to create AssimpLoader. Check the log for more details.");
        }

        m_app->physicsManager = std::make_unique<Phys>();
        if (!m_app->physicsManager) {
            Logger::Error("Failed to create PhysicsManager. Check the log for more details.");
        }

        m_app->physicsManager->_Init();
    } catch(const std::exception& e) {
        Syngine::Logger::LogF(
            LogLevel::FATAL, "Failed to initialize Core: %s", e.what());
        return false;
    }

    // if we ever need to disable debug keybinds we can put this block into an
    // if statement
    {
        Logger::Log("Initializing debug keybinds");

        /*InputAction::RegisterAction("syngine.debugMode",
                                    "Toggle debug mode",
                                    "Debug",
                                    KeyBinding(Keycode::F1),
                                    { .onPressed = Core::_ToggleDebugEnabled });*/
        InputAction::RegisterAction(
            "syngine.debugWireframes",
            "Toggle debug wireframes",
            "Debug",
            KeyBinding(Keycode::F1),
            { .onPressed = Core::_ToggleDebugWireframes });
        InputAction::RegisterAction("syngine.debugGizmos",
                                    "Toggle debug gizmos",
                                    "Debug",
                                    KeyBinding(Keycode::F2),
                                    { .onPressed = Core::_ToggleDebugGizmos });
        InputAction::RegisterAction("syngine.debugShadows",
                                    "Toggle debug shadows",
                                    "Debug",
                                    KeyBinding(Keycode::F3),
                                    { .onPressed = Core::_ToggleDebugShadows });

        InputAction::RegisterAction(
            "syngine.reloadAssets",
            "Reload Assets",
            "",
            KeyBinding(Keycode::F5),
            { .onPressed = Core::_ReloadChangedAssets });

        InputAction::RegisterAction("syngine.reloadShaders",
                                    "Reload Shaders",
                                    "",
                                    KeyBinding(Keycode::F6),
                                    { .onPressed = Core::_ReloadShaders });
    }

    return true;
}

Syngine::Core* Syngine::Core::Get() { return m_instance; }
Syngine::App*  Syngine::Core::_GetApp() {
    return m_instance ? m_instance->m_app : nullptr;
}

Syngine::Phys* Core::GetPhysicsManager() { return m_app->physicsManager.get(); }

bool Core::IsRunning() {
    return !m_shouldClose && !m_app->window->ShouldClose();
}

void Core::SetSimulationState(bool simulate) { m_internal.simulate = simulate; }

bool Core::GetSimulationState() { return m_internal.simulate; }

bool Core::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            Logger::Info("Quit event received, will exit on next frame");
            m_shouldClose = true;
            break;
        case SDL_EVENT_WINDOW_RESIZED: {
            int         w, h;
            SDL_Window* resizedWindow =
                SDL_GetWindowFromID(event.window.windowID);
            SDL_GetWindowSize(resizedWindow, &w, &h);

            m_app->renderer->width  = w;
            m_app->renderer->height = h;
            bgfx::reset(
                w, h, BGFX_RESET_VSYNC); // reset bgfx with new window size
            bgfx::setViewRect(
                0, 0, 0, uint16_t(w), uint16_t(h)); // reset view rect
            break;
        }
        }

        // Handle input events
        InputAction::_HandleEvent(event);

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
    deltaTime       = (m_internal.now - m_internal.last) /
                (float)SDL_GetPerformanceFrequency();

    // Simulate physics
    if (m_app->physicsManager && m_internal.simulate) {
        m_internal.accumulator += deltaTime;

        while (m_internal.accumulator >= m_internal.DEFAULT_PHYSICS_TIMESTEP) {
            m_app->physicsManager->_Update(m_internal.DEFAULT_PHYSICS_TIMESTEP,
                                           m_internal.DEFAULT_PHYSICS_STEPS);
            m_internal.accumulator -= m_internal.DEFAULT_PHYSICS_TIMESTEP;
            m_frameCounter.physCounter++;
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

        auto rigidbodies =
            Registry::GetGameObjectsWithComponent(SYN_COMPONENT_RIGIDBODY);
        for (auto go : rigidbodies) {
            if (!go) continue;
            RigidbodyComponent* rb = go->GetComponent<RigidbodyComponent>();
            if (!rb) continue;
            rb->Update(m_internal.DEFAULT_PHYSICS_TIMESTEP);
        }

        // Player mode: update player camera
        for (auto go : players) {
            if (!go) continue;
            PlayerComponent* pc = go->GetComponent<PlayerComponent>();
            if (!pc) continue;
            pc->_PostPhysicsUpdate();
        }
    }

    m_frameCounter.Update(
        deltaTime, m_internal.simulate, Registry::GetGameObjectCount());

    return true;
}

bool Core::Render(CameraComponent* camera) {
    // Render the application
    if (Renderer::IsReady()) {
        m_frameCounter.frameCount++;

        m_app->renderer->_RenderFrame(camera, m_app->debug);
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
    specs.osName         = std::string(platform);

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
        memcpy(
            CPUBrandString + (i - 0x80000002) * 16, CPUInfo, sizeof(CPUInfo));
    }
    CPUBrandString[sizeof(CPUBrandString) - 1] =
        '\0'; // Null-terminate the string
    cpu            = std::string(CPUBrandString);
    specs.cpuModel = cpu;

    // Get arch
    std::string cpuArch;
    switch (sysInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64: cpuArch = "x64"; break;
    case PROCESSOR_ARCHITECTURE_INTEL: cpuArch = "x86"; break;
    case PROCESSOR_ARCHITECTURE_ARM: cpuArch = "ARM"; break;
    case PROCESSOR_ARCHITECTURE_ARM64: cpuArch = "ARM64"; break;
    default: cpuArch = "Unknown"; break;
    }
    specs.cpuArch  = cpuArch;
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
        char   cpuBrand[256];
        size_t size = sizeof(cpuBrand);
        sysctlbyname("machdep.cpu.brand_string", cpuBrand, &size, NULL, 0);
        specs.cpuModel = std::string(cpuBrand);

        // Get logical CPU count
        int cpuProc = 0;
        size        = sizeof(cpuProc);
        sysctlbyname("machdep.cpu.core_count", &cpuProc, &size, NULL, 0);
        specs.cpuCores = cpuProc;

        // Get RAM info
        uint64_t ramSize = 0;
        size             = sizeof(ramSize);
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
    specs.osName =
        std::string(sysInfo.sysname) + " " + std::string(sysInfo.release);

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
    specs.cpuCores     = numProcessors;

    // Get total physical memory
    long totalMemory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE);
    specs.ramMB      = totalMemory / (1024 * 1024);
#endif

    // Get display size
    SDL_DisplayID dId = SDL_GetDisplayForWindow(m_app->window->_GetSDLWindow());
    SDL_DisplayMode disMode = *SDL_GetCurrentDisplayMode(dId);
    specs.screenWidth       = disMode.w;
    specs.screenHeight      = disMode.h;

    // Get window resolution
    int w, h;
    SDL_GetWindowSize(m_app->window->_GetSDLWindow(), &w, &h);
    specs.winWidth  = w;
    specs.winHeight = h;

    // Get various GPU info from bgfx
    const bgfx::Caps* caps = bgfx::getCaps();
    if (caps) {
        specs.gpuVendorID     = caps->vendorId;
        specs.gpuDeviceID     = caps->deviceId;
        specs.maxTextureSize  = caps->limits.maxTextureSize;
        specs.supportsCompute = (caps->supported & BGFX_CAPS_COMPUTE) != 0;
        specs.supports3DTextures =
            (caps->supported & BGFX_CAPS_TEXTURE_3D) != 0;
    } else {
        specs.gpuVendorID        = 0;
        specs.gpuDeviceID        = 0;
        specs.maxTextureSize     = 0;
        specs.supportsCompute    = false;
        specs.supports3DTextures = false;
    }
    return specs;
}

// copied the internals of _HandleKeyEvents into these two functions to make
// it a bit nicer to call with the new keybind system
void Core::_ToggleDebugEnabled() {
    m_app->debug.Enabled = !m_app->debug.Enabled;
}

void Core::_ToggleDebugWireframes() {
    m_app->debug.PhysWireframes = !m_app->debug.PhysWireframes;
}

void Core::_ToggleDebugGizmos() { m_app->debug.Gizmos = !m_app->debug.Gizmos; }

void Core::_ToggleDebugShadows() {
    m_app->debug.CSMBounds = !m_app->debug.CSMBounds;
}

void Core::_ReloadChangedAssets() {
    for (auto& go : Registry::GetGameObjectsWithComponent(SYN_COMPONENT_MESH)) {
        MeshComponent* mc = go->GetComponent<MeshComponent>();
        if (!mc) continue;
        MeshData& mesh = mc->meshData;
        if (!mesh.valid) continue;
        if (mesh.lastWriteTime != std::filesystem::last_write_time(mesh.path)) {
            mc->ReloadMesh();
        }
    }
}

void Core::_ReloadShaders() { m_app->renderer->ReloadAllPrograms(); }

// DEPRECATED - in favor of the new input system
void Core::_HandleKeyEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        switch (event.key.key) {
            // Toggle debug modes
            case SDLK_F1: {
                // Toggle physics wireframes
                this->m_app->debug.PhysWireframes = !this->m_app->debug.PhysWireframes;
                if (this->m_app->debug.PhysWireframes) {
                    Syngine::Logger::Info("Physics wireframes enabled");
                } else {
                    Syngine::Logger::Info("Physics wireframes disabled");
                }
                break;
            }
            case SDLK_F2: {
                // Toggle gizmos
                this->m_app->debug.Gizmos = !this->m_app->debug.Gizmos;
                if (this->m_app->debug.Gizmos) {
                    Syngine::Logger::Info("Gizmos enabled");
                } else {
                    Syngine::Logger::Info("Gizmos disabled");
                }
                break;
            }
            case SDLK_F3: {
                // Toggle CSM bounds
                this->m_app->debug.CSMBounds = !this->m_app->debug.CSMBounds;
                if (this->m_app->debug.CSMBounds) {
                    Syngine::Logger::Info("CSM Bounds enabled");
                } else {
                    Syngine::Logger::Info("CSM Bounds disabled");
                }
                break;
            }
            case SDLK_F5: {
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
                break;
            }
            case SDLK_F6: {
                // Reload all shaders
                m_app->renderer->ReloadAllPrograms();
            }
        }
    }
}
