#include "Components/MeshComponent.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"
#include "bgfx/bgfx.h"
#include <SDL3/SDL.h>

namespace Syngine {
MeshComponent::MeshComponent(GameObject* owner) {
    this->meshData = Syngine::MeshData();
    this->m_owner = owner;
}

Syngine::Components MeshComponent::getComponentType() {
    return SYN_COMPONENT_MESH;
}

int MeshComponent::LoadMesh(const std::string& path, bool loadTextures) {
    // Load the mesh data from the file
    AssimpLoader loader;
    if (!loader.LoadModel(this->meshData, path, loadTextures)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load mesh from %s", path.c_str());
        return 1; // Error loading mesh
    }
    return 0; // Success
}

int MeshComponent::UnloadMesh() {
    // Unload the mesh data
    if (bgfx::isValid(this->meshData.vbh)) {
        bgfx::destroy(this->meshData.vbh);
        this->meshData.vbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(this->meshData.ibh)) {
        bgfx::destroy(this->meshData.ibh);
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

} // namespace Syngine