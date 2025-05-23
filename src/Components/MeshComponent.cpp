#include "Components/MeshComponent.h"
#include "SynModelLoader.h"
#include <SDL3/SDL.h>

MeshComponent::MeshComponent() {
    this->meshData = SynMeshData();
}

SynComponents MeshComponent::getComponentType() {
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