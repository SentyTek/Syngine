// ╒══════════════════ MeshComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Logger.h"
#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/ECS/Components/MeshComponent.h"
#include "Syngine/ECS/GameObject.h"

#include "bgfx/bgfx.h"
#include <SDL3/SDL.h>

namespace Syngine {
MeshComponent::MeshComponent(GameObject*        owner,
                             const std::string& path,
                             bool               loadTextures) {
    this->meshData = Syngine::MeshData();
    this->m_owner  = owner;
    this->Init(path, loadTextures);
}

MeshComponent::MeshComponent(const MeshComponent& other) {
    this->meshData = other.meshData; // Shallow copy, deep copy may be needed
    this->m_owner  = other.m_owner;
}

MeshComponent& MeshComponent::operator=(const MeshComponent& other) {
    if (this != &other) {
        this->meshData = other.meshData; // Shallow copy, deep copy may be needed
        this->m_owner  = other.m_owner;
    }
    return *this;
}

MeshComponent::~MeshComponent() {
    // Unload the mesh data when the component is destroyed
    this->UnloadMesh();
}

Syngine::Components MeshComponent::GetComponentType() {
    return SYN_COMPONENT_MESH;
}

void MeshComponent::Init(const std::string& path, bool loadTextures) {
    if (!path.empty())
        this->LoadMesh(path, loadTextures);
}

bool MeshComponent::LoadMesh(const std::string& path, bool loadTextures) {
    // Load the mesh data from the file
    AssimpLoader loader;
    if (!loader._LoadModel(
            this->meshData, path.c_str(), loadTextures)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Failed to load mesh from %s",
                              _ResolveOSPath(path.c_str()).c_str());
        return false; // Error loading mesh
    }
    return true; // Success
}

bool MeshComponent::UnloadMesh() {
    // Unload the mesh data
    if (bgfx::isValid(this->meshData.vbh)) {
        // bgfx::destroy(this->meshData.vbh);
        this->meshData.vbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(this->meshData.ibh)) {
        // bgfx::destroy(this->meshData.ibh);
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
    return true; // Success
}

bool MeshComponent::ReloadMesh() {
    // Reload the mesh data from the file
    if (this->meshData.path.empty()) {
        Syngine::Logger::Error("No mesh path set for reloading");
        return false; // Error: no path set
    }
    AssimpLoader loader;
    if (!loader._ReloadModel(this->meshData, this->meshData.id)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Failed to reload mesh from %s",
                              this->meshData.path.c_str());
        return 1; // Error reloading mesh
    }
    return 0; // Success
}

bool MeshComponent::UploadMesh(std::vector<float> vertices,
                                std::vector<uint32_t> indices) {
    Syngine::MeshData meshData;
    meshData.hasTextures = false;
    meshData.numVertices = static_cast<uint32_t>(vertices.size() / 12); // 12 floats per vertex (3 pos, 3 normal, 2 uv0, 4 color)
    meshData.numIndices  = static_cast<uint32_t>(indices.size());
    
    // set vertex data
    meshData.vertices.resize(static_cast<uint32_t>(vertices.size() / 12));
    
    std::vector<Vertex>::iterator vertexIt = meshData.vertices.begin();
    for (size_t i = 0; i < vertices.size(); i += 12) {
        Vertex vertex;
        vertex.pos[0]   = vertices[i];
        vertex.pos[1]   = vertices[i + 1];
        vertex.pos[2]   = vertices[i + 2];
        vertex.normal[0]= vertices[i + 3];
        vertex.normal[1]= vertices[i + 4];
        vertex.normal[2]= vertices[i + 5];
        vertex.uv0[0]   = vertices[i + 6];
        vertex.uv0[1]   = vertices[i + 7];
        vertex.color[0] = vertices[i + 8];
        vertex.color[1] = vertices[i + 9];
        vertex.color[2] = vertices[i + 10];
        vertex.color[3] = vertices[i + 11];
        *vertexIt++    = vertex;
    }
    meshData.indices = indices;
    
    //create bgfx vertex layout
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, false, false)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float) // macro UV
        .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float) // detail UV
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Float)
        .end(); // stride = 72 bytes
    
    // create buffers
    const bgfx::Memory* mem = bgfx::alloc(uint32_t(meshData.numVertices * sizeof(Vertex)));
    memcpy(mem->data, meshData.vertices.data(), meshData.numVertices * sizeof(Vertex));
    bgfx::VertexBufferHandle vbh =
    bgfx::createVertexBuffer(mem, layout);
    
    mem = bgfx::alloc(meshData.numIndices * sizeof(uint32_t));
    memcpy(mem->data, meshData.indices.data(), meshData.numIndices * sizeof(uint32_t));
    bgfx::IndexBufferHandle ibh =
        bgfx::createIndexBuffer(mem, BGFX_BUFFER_INDEX32);

    // Add dummy material
    Material mat;
    mat.baseColor[0] = 1.0f;
    mat.baseColor[1] = 1.0f;
    mat.baseColor[2] = 1.0f;
    mat.baseColor[3] = 1.0f;
    meshData.materials.push_back(mat);
    
    // checks
    if (!bgfx::isValid(vbh) || !bgfx::isValid(ibh)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR, "Failed to create vertex/index buffer");
        return false;
    }
    
    // assign to meshData
    meshData.valid     = true;
    this->meshData = meshData;
    this->meshData.vbh = vbh;
    this->meshData.ibh = ibh;
    return true;
}

} // namespace Syngine