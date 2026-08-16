// ╒══════════════════ MeshComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Core.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/GameObjects/ComponentRegistry.h"
#include "Syngine/GameObjects/Components/TransformComponent.h"
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Graphics/Resources/ModelLoader.h"
#include "Syngine/Math/Vector4.hpp"
#include "Syngine/GameObjects/Components/MeshComponent.h"
#include "Syngine/GameObjects/GameObject.h"
#include "Syngine/Utils/Profiler.h"
#include "Syngine/Utils/Serializer.h"

#include "bgfx/bgfx.h"
#include <SDL3/SDL.h>

#include <sol/sol.hpp>
#include "miniscl.hpp"

#include <cfloat>
#include <string>

namespace Syngine {

// This constructor does nothing. It is used for creating an empty MeshComponent
// that can be initialized later.
MeshComponent::MeshComponent(GameObject* owner) : IComponent(owner) {
    this->modelData     = Syngine::ModelData();
    this->m_bundlePath  = "";
    this->m_texturePath = "";
}

MeshComponent::MeshComponent(GameObject*        owner,
                             const std::string& path,
                             bool               loadTextures)
    : IComponent(owner) {
    this->modelData      = Syngine::ModelData();
    this->m_bundlePath   = "meshes/meshes.spk";
    this->m_texturePath  = path;
    this->m_loadTextures = loadTextures;
    this->Init(this->m_bundlePath, this->m_texturePath, loadTextures);
}

MeshComponent::MeshComponent(GameObject*        owner,
                             const std::string& bundlePath,
                             const std::string& texturePath,
                             bool               loadTextures)
    : IComponent(owner) {
    this->modelData      = Syngine::ModelData();
    this->m_bundlePath   = bundlePath;
    this->m_texturePath  = texturePath;
    this->m_loadTextures = loadTextures;
    this->Init(bundlePath, texturePath, loadTextures);
}

MeshComponent::MeshComponent(const MeshComponent& other)
    : IComponent(other.m_owner) {
    this->modelData = other.modelData; // Shallow copy, deep copy may be needed
    this->m_bundlePath  = other.m_bundlePath;
    this->m_texturePath = other.m_texturePath;
}

MeshComponent& MeshComponent::operator=(const MeshComponent& other) {
    if (this != &other) {
        IComponent::m_owner = other.m_owner;
        this->modelData     = other.modelData;
        this->m_bundlePath  = other.m_bundlePath;
        this->m_texturePath = other.m_texturePath;
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
    node / "type"   = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_MESH);
    node / "bundle" = m_bundlePath;
    node / "path"   = m_texturePath;
    // TODO: mats will need to be serialized at some point
    return node;
}

void MeshComponent::Init(const std::string& bundlePath,
                         const std::string& texturePath,
                         bool               loadTextures) {
    if (!bundlePath.empty() && !texturePath.empty())
        this->LoadMesh(bundlePath, texturePath, loadTextures);
}

bool MeshComponent::LoadMesh(const std::string& bundlePath,
                             const std::string& texturePath,
                             bool               loadTextures) {
    // Get the data stream from the bundle
    std::string resolvedBundlePath =
        Syngine::Internal::ResolvePath(bundlePath.c_str());
    auto meshStream = std::make_shared<scl::stream>(
        Serializer::_ReadFromBundle(resolvedBundlePath, texturePath));
    if (meshStream->size() == 0) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "Failed to load mesh from bundle %s with texture %s",
            resolvedBundlePath.c_str(),
            texturePath.c_str());
        return false; // Error loading mesh stream
    }

    this->modelData       = ModelData(); // Reset model data before loading
    this->modelData.valid = false; // Mark as invalid until loading is complete
    this->m_loadTextures  = loadTextures;
    this->m_isReloadingMesh      = false;
    this->m_isWaitingForMeshLoad = true;
    this->m_aabbDirty            = true;
    // Load the mesh data from the bundle
    this->m_meshLoadJob = Jobs().DispatchWithResult(
        [meshStream, texturePath, loadTextures]() mutable {
            ModelData    out;
            AssimpLoader loader;
            loader._LoadModel(out, meshStream.get(), texturePath, loadTextures);
            return out;
        });

    // For hot reloading the bundle will update, not textures specifically.
    try {
        this->modelData.lastWriteTime =
            std::filesystem::last_write_time(resolvedBundlePath);
    } catch (const std::filesystem::filesystem_error& e) {
        Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                              true,
                              "Failed to get last write time for %s: %s",
                              resolvedBundlePath.c_str(),
                              e.what()); // e.what() lol what a name
    }

    return true; // Success
}

void MeshComponent::Update(float deltaTime) {
    if (this->m_isWaitingForMeshLoad) {
        if (m_meshLoadJob.IsComplete()) {
            m_isWaitingForMeshLoad = false;
            ModelData data         = m_meshLoadJob.Get();
            AssimpLoader::CreateBGFXResources(data);
            if (!data.valid) {
                m_isReloadingMesh = false;
                return;
            }
            if (m_isReloadingMesh) {
                UnloadMesh();
            }
            this->modelData   = std::move(data);
            m_isReloadingMesh = false;
        }
    }
}

bool MeshComponent::UnloadMesh() {
    // Unload the mesh data
    if (bgfx::isValid(this->modelData.vbh)) {
        bgfx::destroy(this->modelData.vbh);
        this->modelData.vbh = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(this->modelData.ibh)) {
        bgfx::destroy(this->modelData.ibh);
        this->modelData.ibh = BGFX_INVALID_HANDLE;
    }
    for (auto& mat : this->modelData.materials) {
        mat.Destroy();
    }
    this->modelData.materials.clear();
    return true; // Success
}

bool MeshComponent::ReloadMesh() {
    if (m_isWaitingForMeshLoad) return false;

    // Get the data stream from the bundle
    std::string resolvedBundlePath =
        Syngine::Internal::ResolvePath(this->m_bundlePath.c_str());
    auto meshStream = std::make_shared<scl::stream>(
        Serializer::_ReadFromBundle(resolvedBundlePath, this->m_texturePath));
    if (meshStream->size() == 0) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "Failed to load mesh from bundle %s with texture %s",
            resolvedBundlePath.c_str(),
            this->m_texturePath.c_str());
        return false; // Error loading mesh stream
    }

    const int         modelId      = this->modelData.id;
    const std::string texturePath  = this->m_texturePath;
    const bool        loadTextures = this->m_loadTextures;
    this->m_isWaitingForMeshLoad   = true;
    this->m_isReloadingMesh        = true;
    this->m_meshLoadJob            = Jobs().DispatchWithResult(
        [meshStream, texturePath, modelId, loadTextures]() mutable {
            ModelData    out;
            AssimpLoader loader;
            loader._ReloadModel(
                out, meshStream.get(), texturePath, modelId, loadTextures);
            return out;
        });

    try {
        this->modelData.lastWriteTime =
            std::filesystem::last_write_time(resolvedBundlePath);
    } catch (const std::filesystem::filesystem_error& e) {
        Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                              true,
                              "Failed to get last write time for %s: %s",
                              this->m_texturePath.c_str(),
                              e.what());
    }
    return true; // Success
}

uint8_t MeshComponent::GetSubmeshCount() const {
    const size_t count = this->modelData.subMeshes.size();
    return static_cast<uint8_t>(count > UINT8_MAX ? UINT8_MAX : count);
}

uint8_t MeshComponent::GetSubmeshMaterialIndex(uint8_t submeshIndex) const {
    if (submeshIndex >= this->modelData.subMeshes.size()) {
        const int maxIndex =
            this->modelData.subMeshes.empty()
                ? -1
                : static_cast<int>(this->modelData.subMeshes.size() - 1);
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "Submesh index %d out of bounds (max %d)",
                              submeshIndex,
                              maxIndex);
        return 0; // Return default material index on error
    }
    return this->modelData.subMeshes[submeshIndex].materialIndex;
}

bool MeshComponent::SetSubmeshMaterialIndex(uint8_t submeshIndex,
                                            uint8_t materialIndex) {
    if (submeshIndex >= this->modelData.subMeshes.size()) {
        const int maxIndex =
            this->modelData.subMeshes.empty()
                ? -1
                : static_cast<int>(this->modelData.subMeshes.size() - 1);
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "Submesh index %d out of bounds (max %d)",
                              submeshIndex,
                              maxIndex);
        return false; // Error: submesh index out of bounds
    }
    if (materialIndex >= this->modelData.materials.size()) {
        const int maxIndex =
            this->modelData.materials.empty()
                ? -1
                : static_cast<int>(this->modelData.materials.size() - 1);
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "Material index %d out of bounds (max %d)",
                              materialIndex,
                              maxIndex);
        return false; // Error: material index out of bounds
    }
    this->modelData.subMeshes[submeshIndex].materialIndex = materialIndex;
    return true; // Success
}

MaterialInstance* MeshComponent::GetMaterialInstance(uint8_t submeshIndex) {
    if (submeshIndex >= modelData.subMeshes.size()) return nullptr;
    const uint8_t materialIndex =
        modelData.subMeshes[submeshIndex].materialIndex;
    if (materialIndex >= modelData.materials.size()) return nullptr;
    return &modelData.materials[materialIndex];
}

const MaterialInstance*
MeshComponent::GetMaterialInstance(uint8_t submeshIndex) const {
    if (submeshIndex >= modelData.subMeshes.size()) return nullptr;
    const uint8_t materialIndex =
        modelData.subMeshes[submeshIndex].materialIndex;
    if (materialIndex >= modelData.materials.size()) return nullptr;
    return &modelData.materials[materialIndex];
}

float MeshComponent::GetObjectUVScaleOverride() const {
    return this->m_objectUVScaleOverride;
}

void MeshComponent::SetObjectUVScaleOverride(float uvScaleOverride) {
    this->m_objectUVScaleOverride = uvScaleOverride;
}

bool MeshComponent::UploadMesh(std::vector<float>    vertices,
                               std::vector<uint32_t> indices,
                               Math::Vector4         baseColor) {
    if (this->modelData.vertices.size() > 0 ||
        this->modelData.subMeshes.size() > 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                              false,
                              "MeshComponent already has mesh data");
        return false;
    }

    Syngine::ModelData modelData;
    bool useVertexColors = (baseColor == Math::Vector4(1.0f, 1.0f, 1.0f, 0.0f));
    int  vertexSize      = useVertexColors
                               ? 12
                               : 8; // if no baseColor provided, expect vertex colors

    Syngine::SubMesh subMesh;
    subMesh.indexStart    = 0;
    subMesh.indexCount    = static_cast<uint32_t>(indices.size());
    subMesh.materialIndex = 0;
    modelData.subMeshes.push_back(subMesh);
    modelData.numSubMeshes = 1;

    // set vertex data
    modelData.vertices.resize(
        static_cast<uint32_t>(vertices.size() / vertexSize));

    // build vertices, apply baseColor if provided
    if (!useVertexColors) {
        // apply base color to all vertices
        std::vector<Vertex>::iterator vertexIt = modelData.vertices.begin();
        for (size_t i = 0; i < vertices.size(); i += 8) {
            Vertex vertex;
            vertex.pos = Vector3(vertices[i], vertices[i + 1], vertices[i + 2]);
            vertex.normal =
                Vector3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
            vertex.uv0   = Vector2(vertices[i + 6], vertices[i + 7]);
            vertex.color = baseColor;
            *vertexIt++  = vertex;
        }
    } else {
        std::vector<Vertex>::iterator vertexIt = modelData.vertices.begin();
        for (size_t i = 0; i < vertices.size(); i += 12) {
            Vertex vertex;
            vertex.pos = Vector3(vertices[i], vertices[i + 1], vertices[i + 2]);
            vertex.normal =
                Vector3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
            vertex.uv0   = Vector2(vertices[i + 6], vertices[i + 7]);
            vertex.color = Vector4(vertices[i + 8],
                                   vertices[i + 9],
                                   vertices[i + 10],
                                   vertices[i + 11]);
            *vertexIt++  = vertex;
        }
    }
    modelData.indices = indices;

    // create bgfx vertex layout
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
    const bgfx::Memory* mem = bgfx::alloc(
        static_cast<uint32_t>(modelData.vertices.size() * sizeof(Vertex)));
    memcpy(mem->data,
           modelData.vertices.data(),
           static_cast<uint32_t>(modelData.vertices.size() * sizeof(Vertex)));
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(mem, layout);

    mem = bgfx::alloc(static_cast<uint32_t>(modelData.indices.size()) *
                      sizeof(uint32_t));
    memcpy(mem->data,
           modelData.indices.data(),
           static_cast<uint32_t>(modelData.indices.size()) * sizeof(uint32_t));
    bgfx::IndexBufferHandle ibh =
        bgfx::createIndexBuffer(mem, BGFX_BUFFER_INDEX32);

    // Add dummy material
    MaterialInstance mat =
        MaterialManager::GetDefaultMaterialPBR().CreateInstance();
    mat.Set("u_baseColor", baseColor.data(), sizeof(Math::Vector4));

    if (useVertexColors) {
        Math::Vector4 mp = mat.Get<Math::Vector4>("u_materialParams1");
        mp.setW(1.0f); // Enable VERTEX color usage
        mat.Set("u_materialParams1", mp.data(), sizeof(Math::Vector4));
    }

    Math::Vector4 mp = mat.Get<Math::Vector4>("u_materialParams2");
    mp.setY(1.0f); // Enable normal map usage
    mat.Set("u_materialParams2", mp.data(), sizeof(Math::Vector4));

    modelData.materials.push_back(mat);

    // checks
    if (!bgfx::isValid(vbh) || !bgfx::isValid(ibh)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "Failed to create vertex/index buffer");
        return false;
    }

    // create aabb
    Math::Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    Math::Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (const auto& vertex : modelData.vertices) {
        min.setX(std::min(min.x(), vertex.pos.x()));
        min.setY(std::min(min.y(), vertex.pos.y()));
        min.setZ(std::min(min.z(), vertex.pos.z()));

        max.setX(std::max(max.x(), vertex.pos.x()));
        max.setY(std::max(max.y(), vertex.pos.y()));
        max.setZ(std::max(max.z(), vertex.pos.z()));
    }
    modelData.localMin = min;
    modelData.localMax = max;
    m_aabb.min         = min;
    m_aabb.max         = max;

    // assign to meshData
    modelData.valid     = true;
    this->modelData     = modelData;
    this->modelData.vbh = vbh;
    this->modelData.ibh = ibh;
    return true;
}

void MeshComponent::_RecalculateAABB() {
    uint64_t currentTransformVersion = 0;

    if (m_transform == nullptr) {
        m_transform = this->m_owner->GetComponent<TransformComponent>();
        if (!m_transform) {
            return; // No transform available
        }
    }

    if (m_transform) {
        currentTransformVersion = m_transform->GetVersion();
    }

    // Fast Cache Return
    if (!this->m_aabbDirty &&
        this->m_cachedTransformVersion == currentTransformVersion) {
        return;
    }

    if (this->modelData.subMeshes.empty()) {
        return; // Return default empty AABB
    }

    // Get local AABB
    Math::Vector3 localMin(this->modelData.localMin);
    Math::Vector3 localMax(this->modelData.localMax);

    Math::Vector3 localCenter  = (localMin + localMax) * 0.5f;
    Math::Vector3 localExtents = (localMax - localMin) * 0.5f;

    // If no transform component, return local AABB straight up
    if (!m_transform) {
        this->m_aabb.min         = localMin;
        this->m_aabb.max         = localMax;
        this->m_aabb.center      = localCenter;
        this->m_aabb.halfExtents = localExtents;

        this->m_aabbDirty              = false;
        this->m_cachedTransformVersion = currentTransformVersion;
        return;
    }

    // Fast World AABB Transformation via Basis Vectors (Jim Arvo's Method)
    // Avoids transforming 8 individual corners or touching vertex memory.
    const Mat4& M = m_transform->GetModelMatrix();

    // World position from matrix translation column/row
    Math::Vector3 worldCenter = Math::Vector3(M.m(3, 0), M.m(3, 1), M.m(3, 2));
    Math::Vector3 worldExtents(0.0f);

    for (int i = 0; i < 3; ++i) {
        // Transform center: center_world.i += localCenter.x * M[0][i] +
        // localCenter.y * M[1][i] + localCenter.z * M[2][i]
        worldCenter.set(i,
                        worldCenter[i] + (localCenter[0] * M.m(0, i) +
                                          localCenter[1] * M.m(1, i) +
                                          localCenter[2] * M.m(2, i)));
        // Transform extents using absolute values of the transform matrix basis
        float extentI = 0.0f;
        for (int j = 0; j < 3; ++j) {
            extentI += std::abs(M.m(j, i)) * localExtents[j];
        }
        worldExtents.set(i, extentI);
    }

    // Store final world AABB
    this->m_aabb.center      = worldCenter;
    this->m_aabb.halfExtents = worldExtents;
    this->m_aabb.min         = worldCenter - worldExtents;
    this->m_aabb.max         = worldCenter + worldExtents;

    this->m_aabbDirty              = false;
    this->m_cachedTransformVersion = currentTransformVersion;
}

const MeshAABB& MeshComponent::GetAABB() const { return this->m_aabb; }

void MeshComponent::PostPhysicsUpdate() { _RecalculateAABB(); }

static Syngine::ComponentRegistrar s_meshRegistrar(
    Syngine::SYN_COMPONENT_MESH,
    // ParseXML: XML element -> DataNode
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" =
            static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_MESH);
        for (const auto& attr : elem->attributes()) {
            scl::string key   = attr->tag();
            scl::string value = attr->data();
            if (key == "path") {
                std::string svalue = std::string(value.cstr());
                node / "path"      = svalue;
            } else if (key == "hasTextures") {
                node / "hasTextures" = (value == "true");
            } else if (key == "bundle") {
                std::string svalue = std::string(value.cstr());
                node / "bundle"    = svalue;
            }
        }
        return node;
    },

    // Instantiate: DataNode -> Component instance
    [](Syngine::GameObject* owner, const Serializer::DataNode& data)
        -> std::unique_ptr<Syngine::IComponent> {
        std::string path =
            data.Has("path") ? data["path"].As<std::string>() : "";
        std::string bundlePath = data.Has("bundle")
                                     ? data["bundle"].As<std::string>()
                                     : "meshes/meshes.spk";
        bool        hasTextures =
            data.Has("hasTextures") ? data["hasTextures"].As<bool>() : false;
        auto meshComp = std::make_unique<MeshComponent>(
            owner, bundlePath, path, hasTextures);
        return meshComp;
    },

    [](sol::state& lua) {
        lua.new_usertype<MeshComponent>("MeshComponent"
                                        // Methods
                                        // (none worth exposing to Lua yet)
        );
    });

} // namespace Syngine
