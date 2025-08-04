// ╒══════════════════ MeshComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Components/MeshComponent.h"
#include "SyngineLogger.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"
#include "bgfx/bgfx.h"
#include <SDL3/SDL.h>

namespace Syngine {
MeshComponent::MeshComponent(GameObject* owner, const std::string& path, bool loadTextures) {
    this->meshData = Syngine::MeshData();
    this->m_owner = owner;
    this->Init(path, loadTextures);
}

MeshComponent::~MeshComponent() {
    // Unload the mesh data when the component is destroyed
    this->UnloadMesh();
}

Syngine::Components MeshComponent::GetComponentType() {
    return SYN_COMPONENT_MESH;
}

void MeshComponent::Init(const std::string& path, bool loadTextures) {
    this->LoadMesh(path, loadTextures);
}

int MeshComponent::LoadMesh(const std::string& path, bool loadTextures) {
    // Load the mesh data from the file
    AssimpLoader loader;
    if (!loader._LoadModel(this->meshData, path, loadTextures)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to load mesh from %s", path.c_str());
        return 1; // Error loading mesh
    }
    return 0; // Success
}

int MeshComponent::UnloadMesh() {
    // Unload the mesh data
    if (bgfx::isValid(this->meshData.vbh)) {
        //bgfx::destroy(this->meshData.vbh);
        this->meshData.vbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(this->meshData.ibh)) {
        //bgfx::destroy(this->meshData.ibh);
        this->meshData.ibh = BGFX_INVALID_HANDLE;
    }
    for (auto& mat : this->meshData.materials) {
        if (bgfx::isValid(mat.albedo)) {
            bgfx::destroy(mat.albedo);
            mat.albedo = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(mat.normalMap)) {
            bgfx::destroy(mat.normalMap);
            mat.normalMap = BGFX_INVALID_HANDLE;
        }
        if (bgfx::isValid(mat.heightMap)) {
            bgfx::destroy(mat.heightMap);
            mat.heightMap = BGFX_INVALID_HANDLE;
        }
    }
    this->meshData.materials.clear();
    return 0; // Success
}

int MeshComponent::ReloadMesh() {
    // Reload the mesh data from the file
    if (this->meshData.path.empty()) {
        Syngine::Logger::Error("No mesh path set for reloading");
        return 1; // Error: no path set
    }
    AssimpLoader loader;
    if (!loader._ReloadModel(this->meshData, this->meshData.id)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to reload mesh from %s", this->meshData.path.c_str());
        return 1; // Error reloading mesh
    }
    return 0; // Success
}

} // namespace Syngine