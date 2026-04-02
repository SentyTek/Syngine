// ╒══════════════════ MeshComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Logger.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/ECS/Components/MeshComponent.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Utils/Profiler.h"

#include "Syngine/Utils/Serializer.h"
#include "bgfx/bgfx.h"
#include <SDL3/SDL.h>
#include "miniscl.hpp"

#include <cfloat>
#include <string>

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

Syngine::ComponentTypeID MeshComponent::GetComponentType() {
    return SYN_COMPONENT_MESH;
}

Serializer::DataNode MeshComponent::Serialize() const {
    Serializer::DataNode node;
    node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_MESH);
    node / "path" = _MakeRelativeToRoot(this->meshData.path);
    //TODO: mats will need to be serialized at some point
    return node;
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
                              _MakeRelativeToRoot(path).c_str());
        return false; // Error loading mesh
    }
    return true; // Success
}

bool MeshComponent::UnloadMesh() {
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
                              _MakeRelativeToRoot(this->meshData.path).c_str());
        return false; // Error reloading mesh
    }
    return true; // Success
}

uint8_t MeshComponent::GetSubmeshCount() const {
    const size_t count = this->meshData.subMeshes.size();
    return static_cast<uint8_t>(count > UINT8_MAX ? UINT8_MAX : count);
}

uint8_t MeshComponent::GetSubmeshMaterialIndex(uint8_t submeshIndex) const {
    if (submeshIndex >= this->meshData.subMeshes.size()) {
        const int maxIndex =
            this->meshData.subMeshes.empty()
                ? -1
                : static_cast<int>(this->meshData.subMeshes.size() - 1);
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Submesh index %d out of bounds (max %d)",
                              submeshIndex,
                              maxIndex);
        return 0; // Return default material index on error
    }
    return this->meshData.subMeshes[submeshIndex].materialIndex;
}

bool MeshComponent::SetSubmeshMaterialIndex(uint8_t submeshIndex, uint8_t materialIndex) {
    if (submeshIndex >= this->meshData.subMeshes.size()) {
        const int maxIndex =
            this->meshData.subMeshes.empty()
                ? -1
                : static_cast<int>(this->meshData.subMeshes.size() - 1);
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Submesh index %d out of bounds (max %d)",
                              submeshIndex,
                              maxIndex);
        return false; // Error: submesh index out of bounds
    }
    if (materialIndex >= this->meshData.materials.size()) {
        const int maxIndex =
            this->meshData.materials.empty()
                ? -1
                : static_cast<int>(this->meshData.materials.size() - 1);
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Material index %d out of bounds (max %d)",
                              materialIndex,
                              maxIndex);
        return false; // Error: material index out of bounds
    }
    this->meshData.subMeshes[submeshIndex].materialIndex = materialIndex;
    return true; // Success
}

float* MeshComponent::GetMaterialUVScale(uint8_t materialIndex, uint8_t textureType) const {
    if (materialIndex >= this->meshData.numMaterials) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Material index %d out of bounds (max %d)",
                              materialIndex, this->meshData.numMaterials - 1);
        return nullptr; // Error: material index out of bounds
    }
    if (textureType > 2) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Texture type %d out of bounds (max 2)",
                              textureType);
        return nullptr; // Error: invalid texture type
    }
    return const_cast<float*>(this->meshData.materials[materialIndex].uvScale + textureType);
}

bool MeshComponent::SetMaterialUVScale(uint8_t materialIndex, uint8_t textureType, float uvScale[2]) {
    if (materialIndex >= this->meshData.numMaterials) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Material index %d out of bounds (max %d)",
                              materialIndex, this->meshData.numMaterials - 1);
        return false; // Error: material index out of bounds
    }
    if (textureType > 2) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Texture type %d out of bounds (max 2)",
                              textureType);
        return false; // Error: invalid texture type
    }
    this->meshData.materials[materialIndex].uvScale[textureType * 2] = uvScale[0];
    this->meshData.materials[materialIndex].uvScale[textureType * 2 + 1] = uvScale[1];
    return true; // Success
}

float MeshComponent::GetObjectUVScaleOverride() const {
    return this->m_objectUVScaleOverride;
}

void MeshComponent::SetObjectUVScaleOverride(float uvScaleOverride) {
    this->m_objectUVScaleOverride = uvScaleOverride;
}

bool MeshComponent::UploadMesh(std::vector<float>    vertices,
                               std::vector<uint32_t> indices,
                               std::vector<uint8_t>  baseColor) {
    Syngine::MeshData meshData;
    int vertexSize = baseColor.empty() ? 12 : 8; // if no baseColor provided, expect vertex colors

    Syngine::SubMesh subMesh;
    subMesh.indexStart = 0;
    subMesh.indexCount = static_cast<uint32_t>(indices.size());
    subMesh.materialIndex = 0;
    meshData.subMeshes.push_back(subMesh);
    meshData.numSubMeshes = 1;

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
    const bgfx::Memory* mem = bgfx::alloc(uint32_t(meshData.vertices.size() * sizeof(Vertex)));
    memcpy(mem->data, meshData.vertices.data(), meshData.vertices.size() * sizeof(Vertex));
    bgfx::VertexBufferHandle vbh =
    bgfx::createVertexBuffer(mem, layout);

    mem = bgfx::alloc(meshData.indices.size() * sizeof(uint32_t));
    memcpy(mem->data, meshData.indices.data(), meshData.indices.size() * sizeof(uint32_t));
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

MeshAABB& MeshComponent::GetAABB() {
    SYN_PROFILE_FUNCTION();
    uint64_t currentTransformVersion = 0;
    if (this->m_owner && this->m_owner->HasComponent(SYN_COMPONENT_TRANSFORM)) {
        TransformComponent* transform = this->m_owner->GetComponent<TransformComponent>();
        currentTransformVersion = transform->GetVersion();
    }

    if (!m_aabbDirty && m_cachedTransformVersion == currentTransformVersion) {
        return m_aabb;
    }

    MeshAABB& boundingBox = m_aabb;

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

    m_aabbDirty = false;
    m_cachedTransformVersion = currentTransformVersion;
    m_aabb = result;
    return m_aabb;
}

static Syngine::ComponentRegistrar s_meshRegistrar(Syngine::SYN_COMPONENT_MESH,
    // ParseXML: XML element -> DataNode
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_MESH);
        for (const auto& attr : elem->attributes()) {
            scl::string key = attr->tag();
            scl::string value = attr->data();
            if (key == "path") {
                std::string svalue = std::string(value.cstr());
                node / "path" = svalue;
            } else if (key == "hasTextures") {
                node / "hasTextures" = (value == "true");
            }
        }
        return node;
    },

    // Instantiate: DataNode -> Component instance
    [](Syngine::GameObject* owner, const Serializer::DataNode& data) -> std::unique_ptr<Syngine::Component> {
        std::string path = data.Has("path") ? data["path"].As<std::string>() : "";
        bool hasTextures = data.Has("hasTextures") ? data["hasTextures"].As<bool>() : false;
        auto meshComp = std::make_unique<MeshComponent>(owner, path, hasTextures);
        return meshComp;
    }
);

} // namespace Syngine
