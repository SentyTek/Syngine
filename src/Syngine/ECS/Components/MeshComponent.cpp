// ╒══════════════════ MeshComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Logger.h"
#include "Syngine/ECS/Components/TransformComponent.h"
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

bool MeshComponent::UploadMesh(std::vector<float>    vertices,
                               std::vector<uint32_t> indices,
                               std::vector<uint8_t>  baseColor) {
    Syngine::MeshData meshData;
    meshData.hasTextures = false;

    int vertexSize = baseColor.empty() ? 12 : 8; // if no baseColor provided, expect vertex colors

    meshData.numVertices = static_cast<uint32_t>(vertices.size() / vertexSize); // 12 floats per vertex (3 pos, 3 normal, 2 uv0, 4 color)
    meshData.numIndices  = static_cast<uint32_t>(indices.size());

    // set vertex data
    meshData.vertices.resize(static_cast<uint32_t>(vertices.size() / vertexSize));

    // build vertices, apply baseColor if provided
    if (!baseColor.empty()) {
        if (baseColor.size() == 3) {
            baseColor.push_back(255); // add alpha if missing
        }
        // apply base color to all vertices
        std::vector<Vertex>::iterator vertexIt = meshData.vertices.begin();
        for (size_t i = 0; i < vertices.size(); i += 8) {
            Vertex vertex;
            vertex.pos[0]   = vertices[i];
            vertex.pos[1]   = vertices[i + 1];
            vertex.pos[2]   = vertices[i + 2];
            vertex.normal[0]= vertices[i + 3];
            vertex.normal[1]= vertices[i + 4];
            vertex.normal[2]= vertices[i + 5];
            vertex.uv0[0]   = vertices[i + 6];
            vertex.uv0[1]   = vertices[i + 7];
            vertex.color[0] = static_cast<float>(baseColor[0]) / 255.0f;
            vertex.color[1] = static_cast<float>(baseColor[1]) / 255.0f;
            vertex.color[2] = static_cast<float>(baseColor[2]) / 255.0f;
            vertex.color[3] = static_cast<float>(baseColor[3]) / 255.0f;
            *vertexIt++     = vertex;
        }
    } else {
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
            *vertexIt++     = vertex;
        }
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
    mat.useVertexColor = true;
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

const MeshAABB MeshComponent::GetAABB() const {
    static MeshAABB boundingBox = MeshAABB();

    if (this->meshData.vertices.empty()) {
        return boundingBox; // Return default AABB if no vertices
    }

    // Compute local min/max
    float min[3], max[3];
    for (int i = 0; i < 3; ++i) {
        min[i] = max[i] = this->meshData.vertices[0].pos[i];
    }
    for (const auto& vertex : this->meshData.vertices) {
        for (int i = 0; i < 3; ++i) {
            min[i] = std::min(min[i], vertex.pos[i]);
            max[i] = std::max(max[i], vertex.pos[i]);
        }
    }

    MeshAABB result;
    // If transform exists, transform all 8 corners
    if (this->m_owner && this->m_owner->HasComponent(SYN_COMPONENT_TRANSFORM)) {
        TransformComponent* transform = this->m_owner->GetComponent<TransformComponent>();
        float modelMatrix[16];
        transform->GetModelMatrix(modelMatrix);

        float corners[8][3] = {
            {min[0], min[1], min[2]},
            {min[0], min[1], max[2]},
            {min[0], max[1], min[2]},
            {min[0], max[1], max[2]},
            {max[0], min[1], min[2]},
            {max[0], min[1], max[2]},
            {max[0], max[1], min[2]},
            {max[0], max[1], max[2]}
        };

        float tmin[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
        float tmax[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
        for (int c = 0; c < 8; ++c) {
            float x = corners[c][0], y = corners[c][1], z = corners[c][2];
            float tx = modelMatrix[0]*x + modelMatrix[4]*y + modelMatrix[8]*z + modelMatrix[12];
            float ty = modelMatrix[1]*x + modelMatrix[5]*y + modelMatrix[9]*z + modelMatrix[13];
            float tz = modelMatrix[2]*x + modelMatrix[6]*y + modelMatrix[10]*z + modelMatrix[14];
            tmin[0] = std::min(tmin[0], tx); tmax[0] = std::max(tmax[0], tx);
            tmin[1] = std::min(tmin[1], ty); tmax[1] = std::max(tmax[1], ty);
            tmin[2] = std::min(tmin[2], tz); tmax[2] = std::max(tmax[2], tz);
        }
        for (int i = 0; i < 3; ++i) {
            result.min[i] = tmin[i];
            result.max[i] = tmax[i];
            result.center[i] = (tmin[i] + tmax[i]) / 2.0f;
            result.halfExtents[i] = (tmax[i] - tmin[i]) / 2.0f;
        }
    } else { // No transform, use local min/max
        for (int i = 0; i < 3; ++i) {
            result.min[i] = min[i];
            result.max[i] = max[i];
            result.center[i] = (min[i] + max[i]) / 2.0f;
            result.halfExtents[i] = (max[i] - min[i]) / 2.0f;
        }
    }
    return result;
}

} // namespace Syngine