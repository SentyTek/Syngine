// ╒═══════════════════════ Renderer.cpp ═╕
// │ Syngine                              │
// │ Created 2025-04-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Logger.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/ECS/Components/BillboardComponent.h"
#include "Syngine/Graphics/Shaders.h"
#include "Syngine/Graphics/TextureHelpers.h"
#include "Syngine/Graphics/RenderCore.h"
#include "Syngine/ECS/AllComponents.h"
#include "Syngine/Core/Core.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_properties.h>

#include <bgfx/platform.h>
#include "bgfx/bgfx.h"
#include "bx/math.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <unordered_map>
#include <vector>

#if BX_PLATFORM_OSX
#include "MetalBridge.h"
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace Syngine {

// I present to you an absurd amount of static member definitions.
std::string Renderer::m_title;
bool        Renderer::m_isReady = false;

float Renderer::m_gizmoSize = 1.0f;
std::unordered_map<std::string, Syngine::BillboardComponent*>
    Renderer::m_gizmoRegistry;
static std::unordered_map<bgfx::ViewId, std::vector<Program>> viewPrograms;

int                          Renderer::width        = 0;
int                          Renderer::height       = 0;

CameraComponent*             Renderer::m_pseudoCamera = nullptr;

Renderer::Renderer(int width, int height, const RendererConfig& config) {
    Renderer::width = width;
    Renderer::height = height;

    if (!_CreateRenderer(config)) {
        Syngine::Logger::Fatal("Failed to create renderer");
    }
};

Renderer::~Renderer() {
    // Destroy all registered uniforms
    for (auto& [id, uniform] : m_uniformRegistry) {
        if (bgfx::isValid(uniform.handle)) {
            bgfx::destroy(uniform.handle);
        }
        if (uniform.data) {
            free(uniform.data);
            uniform.data = nullptr;
        }
    }
    m_uniformRegistry.clear();

    // Destroy all programs. Uniforms are already destroyed above via m_uniformRegistry
    // (RegisterUniform copies the same handle/data pointer into both stores, so only
    // destroy from one of them).
    for (auto& view : viewPrograms) {
        for (auto& prog : view.second) {
            bgfx::destroy(prog.program);
        }
    }
    viewPrograms.clear();

    // Clear gizmos
    for (auto& [tag, gizmo] : m_gizmoRegistry) {
        delete gizmo;
    }
    m_gizmoRegistry.clear();

    RenderCore::_Shutdown(); // Destroys RenderCore buffers/textures/VBs and calls bgfx::shutdown()
}

bool Renderer::_CreateRenderer(const RendererConfig& config) {
    RenderCore::_Initialize(config);

    // Initial sun direction in degrees (yaw, pitch, roll)
    // Stored as (yaw, pitch, roll) with pitch = degrees above horizon (positive = up).
    const float initialSunDir[3] = { 0.0f, 45.0f, 0.0f };
    float pitch = bx::toRad(initialSunDir[1]);
    float yaw   = bx::toRad(initialSunDir[0]);
    float cp    = cosf(pitch);
    float sp    = sinf(pitch);
    float cy    = cosf(yaw);
    float sy    = sinf(yaw);

    // y = +sin(pitch) when pitch is above horizon. (Ensure convention matches UI)
    bx::Vec3 dirVec = { cp * sy, sp, cp * cy };
    
    dirVec = bx::normalize(dirVec);

    float final[3] = { dirVec.x, dirVec.y, dirVec.z };
    SetSunDirection(final);

    m_isReady = true;
    Syngine::Logger::Info("Renderer created successfully");
    return true;
}

size_t Renderer::AddProgram(const std::string& path,
                            const std::string& name,
                            Syngine::ViewID    viewId) {
    if (!Renderer::IsReady())
        Syngine::Logger::Fatal("Cannot add program before renderer is ready");
    if (name.empty()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Program name cannot be empty");
        return -1;
    }

    if (GetProgram(name).id != 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Program with name \"%s\" already exists",
                              name.c_str());
        return -1;
    }

    std::string vsPath = path + ".vert.bin";
    std::string fsPath = path + ".frag.bin";

    bgfx::ShaderHandle vs = _LoadShader(vsPath.c_str());
    bgfx::ShaderHandle fs = _LoadShader(fsPath.c_str());
    bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (bgfx::isValid(vs) && bgfx::isValid(fs)) {
        programHandle = bgfx::createProgram(vs, fs, true);
    }

    if (!bgfx::isValid(programHandle)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to create program %s", name.c_str());
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
    prog.id = programHandle.idx;

    viewPrograms[viewId].push_back(prog); // Store program by viewId
    Syngine::Logger::LogF(Syngine::LogLevel::INFO, "Program %s created successfully", name.c_str());
    return prog.id;
}

size_t Renderer::AddProgram(const std::string& bundlePath, const std::string& path, const std::string& name, Syngine::ViewID viewId) {
    if (!Renderer::IsReady()) {
        Syngine::Logger::Fatal("Cannot add program before renderer is ready");
        return -1;
    }
    if (name.empty() || path.empty() || bundlePath.empty()) {
         Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Invalid parameters for AddProgram from bundle");
        return -1;
    }
    if (GetProgram(name).id != 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Program with name \"%s\" already exists",
                              name.c_str());
        return -1;
    }

    // Fallback if bundle doesn't exist
    if (!_FileExists(bundlePath.c_str())) {
        Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                              "Bundle %s not found, falling back to regular AddProgram",
                              bundlePath.c_str());
        return AddProgram(path, name, viewId);
    }

    bgfx::ShaderHandle vs = _LoadShaderFromBundle(bundlePath.c_str(), (path + ".vert.bin").c_str());
    bgfx::ShaderHandle fs = _LoadShaderFromBundle(bundlePath.c_str(), (path + ".frag.bin").c_str());
    bgfx::ProgramHandle programHandle = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid(vs) || !bgfx::isValid(fs)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Failed to load shaders for program %s from bundle",
                              name.c_str());
        if (bgfx::isValid(vs)) bgfx::destroy(vs);
        if (bgfx::isValid(fs)) bgfx::destroy(fs);
        return -1;
    }
    programHandle = bgfx::createProgram(vs, fs, true);

    if (!bgfx::isValid(programHandle)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to create program %s from bundle", name.c_str());
        bgfx::destroy(vs);
        bgfx::destroy(fs);
        return -1;
    }

    Program prog;
    prog.program = programHandle;
    prog.name    = name;
    prog.viewId  = viewId;
    prog.vsPath  = bundlePath + ":" + path + ".vert.bin";
    prog.fsPath  = bundlePath + ":" + path + ".frag.bin";
    prog.id      = programHandle.idx;

    viewPrograms[viewId].push_back(prog); // Store program by viewId
    Syngine::Logger::LogF(Syngine::LogLevel::INFO, "Program %s created successfully from bundle", name.c_str());
    return prog.id;
}

Program Renderer::GetProgram(const std::string_view& name) {
    for (const auto& programs : viewPrograms) {
        for (const auto& program : programs.second) {
            if (program.name == name && bgfx::isValid(program.program)) {
                return program;
            }
        }
    }
    return Program();
}
Program Renderer::GetProgram(size_t id) {
    Program* prog = _GetProgram(id);
    if (prog) {
        return *prog;
    }
    return Program();
}
Program* Renderer::_GetProgram(size_t id) {
    for (auto& pair : viewPrograms) {
        for (auto& prog : pair.second) {
            if (prog.id == id) {
                return &prog;
            }
        }
    }
    return nullptr;
}

bool Renderer::RemoveProgram(Syngine::ViewID viewId, const std::string_view& name) {
    for (int i = 0; i < viewPrograms[viewId].size(); ++i) {
        if (viewPrograms[viewId][i].name == name) {
            for (auto& uniform : viewPrograms[viewId][i].uniforms) {
                m_uniformRegistry.erase(uniform.handle.idx);
                bgfx::destroy(uniform.handle);
                if (uniform.data) {
                    free(uniform.data);
                }
            }
            bgfx::destroy(viewPrograms[viewId][i].program);
            viewPrograms[viewId].erase(viewPrograms[viewId].begin() + i);
            return true;
        }
    }
    return false;
}
bool Renderer::RemoveProgram(Syngine::ViewID viewId, size_t id) {
    if (id < viewPrograms[viewId].size()) {
        for (auto& uniform : viewPrograms[viewId][id].uniforms) {
            m_uniformRegistry.erase(uniform.handle.idx);
            bgfx::destroy(uniform.handle);
            if (uniform.data) {
                free(uniform.data);
            }
        }
        bgfx::destroy(viewPrograms[viewId][id].program);
        viewPrograms[viewId].erase(viewPrograms[viewId].begin() + id);
        return true;
    }
    return false;
}

bool Renderer::RemoveAllPrograms() {
    for (auto& programs : viewPrograms) {
        for (auto& program : programs.second) {
            for (auto& uniform : program.uniforms) {
                m_uniformRegistry.erase(uniform.handle.idx);
                bgfx::destroy(uniform.handle);
                if (uniform.data) {
                    free(uniform.data);
                }
            }
            bgfx::destroy(program.program);
        }
        programs.second.clear();
    }
    return true;
}

bool Renderer::ReloadProgram(Syngine::ViewID viewId, const std::string_view& name) {
    for (auto& prog : viewPrograms[viewId]) {
        if (prog.name == name) {
            bgfx::ShaderHandle vs = _LoadShader(prog.vsPath.c_str());
            bgfx::ShaderHandle fs = _LoadShader(prog.fsPath.c_str());
            bgfx::ProgramHandle newProgram = bgfx::createProgram(vs, fs, true);

            if (!bgfx::isValid(newProgram)) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                      "Failed to reload program %s",
                                      name.data());
                bgfx::destroy(vs);
                bgfx::destroy(fs);
                return false;
            }

            bgfx::destroy(prog.program); // Destroy old program
            prog.program = newProgram;   // Update to new program
            return true;
        }
    }
    return false;
}

bool Renderer::ReloadAllPrograms() {
    for (auto& programs : viewPrograms) {
        for (auto& prog : programs.second) {
            bgfx::ShaderHandle vs = _LoadShader(prog.vsPath.c_str());
            bgfx::ShaderHandle fs = _LoadShader(prog.fsPath.c_str());
            bgfx::ProgramHandle newProgram = bgfx::createProgram(vs, fs, true);

            if (!bgfx::isValid(newProgram)) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                      "Failed to reload program %s",
                                      prog.name.c_str());
                bgfx::destroy(vs);
                bgfx::destroy(fs);
                return false;
            }

            bgfx::destroy(prog.program); // Destroy old program
            prog.program = newProgram;   // Update to new program
        }
    }
    return true;
}

bool Renderer::IsReady() {
    return m_isReady;
}

size_t Renderer::RegisterUniform(size_t             program,
                                 const std::string& name,
                                 UniformType        type,
                                 uint16_t           num) {
    Program* prog = _GetProgram(program);
    if (!prog) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            "Cannot register uniform to invalid program id %zu",
            program);
        return 0;
    }

    if (name.empty()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Uniform name cannot be empty");
        return 0;
    }

    for (const auto& u : prog->uniforms) {
        if (u.name == name) {
            Syngine::Logger::LogF(
                Syngine::LogLevel::ERR,
                "Uniform with name \"%s\" already exists in program \"%s\"",
                name.c_str(),
                prog->name.c_str());
            return u.handle.idx;
        }
    }

    Uniform u = {
        .handle = bgfx::createUniform(
            name.c_str(), static_cast<bgfx::UniformType::Enum>(type), num),
                    .type = type,
                    .name = name,
                    .num  = num };

    // Allocate memory based on uniform type
    size_t size = 0;
    switch (type) {
    case UNIFORM_SAMPLER:
        u.data = nullptr; // Samplers don't need data storage
        break;
    case UNIFORM_VEC4:
        size = sizeof(float) * 4;
        break;
    case UNIFORM_MAT3:
        size = sizeof(float) * 9;
        break;
    case UNIFORM_MAT4:
        size = sizeof(float) * 16;
        break;
    default:
        u.data = nullptr;
        break;
    }

    if (size > 0) {
        u.data = malloc(size * num);
        memset(u.data, 0, size * num);
    }

    prog->uniforms.push_back(u);
    // Store in uniform registry for fast lookup
    m_uniformRegistry[u.handle.idx] = u;
    return u.handle.idx;
}

Uniform* Renderer::GetUniform(size_t id) {
    auto it = m_uniformRegistry.find(static_cast<uint16_t>(id));
    if (it != m_uniformRegistry.end()) {
        return &it->second;
    }
    return nullptr;
}

void Renderer::SetUniform(size_t id, const void* data, uint16_t num) {
    Uniform* u = GetUniform(id);
    if (u && bgfx::isValid(u->handle)) {
        size_t size = 0;
        switch (u->type) {
        case UNIFORM_SAMPLER:
            SDL_Log("Cannot set texture uniform. Use bgfx::setTexture instead.");
            break;
        case UNIFORM_VEC4:
            size = sizeof(float) * 4;
            bgfx::setUniform(u->handle, static_cast<const float*>(data), num);
            break;
        case UNIFORM_MAT3:
            size = sizeof(float) * 9;
            bgfx::setUniform(u->handle, static_cast<const float*>(data), num);
            break;
        case UNIFORM_MAT4:
            size = sizeof(float) * 16;
            bgfx::setUniform(u->handle, static_cast<const float*>(data), num);
            break;
        default:
            Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Unknown uniform type");
            break;
        }

        if (size > 0) {
            memcpy(u->data, data, size * num);
        }
    }
}

void Renderer::_RegisterGizmo(const std::string& tag) {
    for (auto& [existingTag, gizmo] : m_gizmoRegistry) {
        if (existingTag == tag) {
            // Gizmo already registered
            return;
        }
    }

    Syngine::BillboardComponent* gizmo = new Syngine::BillboardComponent(nullptr, "default/gizmos/" + tag + ".png", BillboardMode::CAMERA_ALIGNED, m_gizmoSize);

    m_gizmoRegistry[tag] = gizmo;
}

void Renderer::GetSunDirection(float* outDir) {
    Uniform* u = RenderCore::_GetDefaultUniform("u_default_lightDir");
    if (u && u->data) {
        float* dir = static_cast<float*>(u->data);
        outDir[0] = dir[0];
        outDir[1] = dir[1];
        outDir[2] = dir[2];
    } else {
        // If the uniform is not found, default to a downward light direction
        outDir[0] = 0.0f;
        outDir[1] = 1.0f;
        outDir[2] = 0.0f;
    }
}

void Renderer::SetSunDirection(const float* lightDir) {
    float dir[4] = { lightDir[0], lightDir[1], lightDir[2], 0.0f };
    // Loop over every uniform with "u_lightDir" as the name and set its value
    for (auto& pair : m_uniformRegistry) {
        Uniform& uniform = pair.second;
        const std::string& name = uniform.name;
        if (name.find("u_lightDir") != std::string::npos && bgfx::isValid(uniform.handle)) {
            SetUniform(pair.first, dir);
        }
    }
}

void Renderer::_RenderFrame(CameraComponent* camera, DebugModes debug) {
    RenderCore::_RenderFrame(camera, debug);
}

void Renderer::SetPseudoCamera(CameraComponent* camera) {
    m_pseudoCamera = camera;
}

} // namespace Syngine
