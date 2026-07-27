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
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Graphics/Resources/ModelLoader.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/ECS/Components/MeshComponent.h"
#include "Syngine/ECS/GameObject.h"
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
MeshComponent::MeshComponent(GameObject* owner) {
    this->modelData     = Syngine::ModelData();
    this->m_owner       = owner;
    this->m_bundlePath  = "";
    this->m_texturePath = "";
}

MeshComponent::MeshComponent(GameObject*        owner,
                             const std::string& path,
                             bool               loadTextures) {
    this->modelData     = Syngine::ModelData();
    this->m_owner       = owner;
    this->m_bundlePath  = "meshes/meshes.spk";
    this->m_texturePath = path;
    this->Init(this->m_bundlePath, this->m_texturePath, loadTextures);
}

MeshComponent::MeshComponent(GameObject*        owner,
                             const std::string& bundlePath,
                             const std::string& texturePath,
                             bool               loadTextures) {
    this->modelData     = Syngine::ModelData();
    this->m_owner       = owner;
    this->m_bundlePath  = bundlePath;
    this->m_texturePath = texturePath;
    this->Init(bundlePath, texturePath, loadTextures);
}

MeshComponent::MeshComponent(const MeshComponent& other) {
    this->modelData = other.modelData; // Shallow copy, deep copy may be needed
    this->m_owner   = other.m_owner;
    this->m_bundlePath  = other.m_bundlePath;
    this->m_texturePath = other.m_texturePath;
}

MeshComponent& MeshComponent::operator=(const MeshComponent& other) {
    if (this != &other) {
        this->modelData =
            other.modelData; // Shallow copy, deep copy may be needed
        this->m_owner       = other.m_owner;
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
    scl::stream meshStream =
        Serializer::_ReadFromBundle(resolvedBundlePath, texturePath);
    if (meshStream.size() == 0) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "Failed to load mesh from bundle %s with texture %s",
            resolvedBundlePath.c_str(),
            texturePath.c_str());
        return false; // Error loading mesh stream
    }

    // Load the mesh data from the bundle
    AssimpLoader loader;
    if (!loader._LoadModel(
            this->modelData, &meshStream, texturePath, loadTextures)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "Failed to load mesh from %s",
                              texturePath.c_str());
        return false; // Error loading mesh
    }

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
    // Get the data stream from the bundle
    std::string resolvedBundlePath =
        Syngine::Internal::ResolvePath(this->m_bundlePath.c_str());
    scl::stream meshStream =
        Serializer::_ReadFromBundle(resolvedBundlePath, this->m_texturePath);
    if (meshStream.size() == 0) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR,
            false,
            "Failed to load mesh from bundle %s with texture %s",
            resolvedBundlePath.c_str(),
            this->m_texturePath.c_str());
        return false; // Error loading mesh stream
    }

    // Reload the mesh data from the bundle
    AssimpLoader loader;
    if (!loader._ReloadModel(this->modelData,
                             &meshStream,
                             this->m_texturePath,
                             this->modelData.id)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "Failed to reload mesh from %s",
                              _MakeRelativeToRoot(this->m_texturePath).c_str());
        return false; // Error reloading mesh
    }

    try {
        this->modelData.lastWriteTime =
            std::filesystem::last_write_time(this->m_texturePath);
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

float MeshComponent::GetObjectUVScaleOverride() const {
    return this->m_objectUVScaleOverride;
}

void MeshComponent::SetObjectUVScaleOverride(float uvScaleOverride) {
    this->m_objectUVScaleOverride = uvScaleOverride;
}

bool MeshComponent::UploadMesh(std::vector<float>    vertices,
                               std::vector<uint32_t> indices,
                               std::vector<uint8_t>  baseColor) {
    Syngine::ModelData modelData;
    int vertexSize = baseColor.empty()
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
    if (!baseColor.empty()) {
        if (baseColor.size() == 3) {
            baseColor.push_back(255); // add alpha if missing
        }
        // apply base color to all vertices
        std::vector<Vertex>::iterator vertexIt = modelData.vertices.begin();
        for (size_t i = 0; i < vertices.size(); i += 8) {
            Vertex vertex;
            vertex.pos = Vector3(vertices[i], vertices[i + 1], vertices[i + 2]);
            vertex.normal =
                Vector3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
            vertex.uv0   = Vector2(vertices[i + 6], vertices[i + 7]);
            vertex.color = Vector4(static_cast<float>(baseColor[0]) / 255.0f,
                                   static_cast<float>(baseColor[1]) / 255.0f,
                                   static_cast<float>(baseColor[2]) / 255.0f,
                                   static_cast<float>(baseColor[3]) / 255.0f);
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
    Material mat("default_material");
    mat.Set("u_baseColor",
            Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f).data(),
            sizeof(Math::Vector4));
    modelData.materials.push_back(mat);

    // checks
    if (!bgfx::isValid(vbh) || !bgfx::isValid(ibh)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "Failed to create vertex/index buffer");
        return false;
    }

    // assign to meshData
    modelData.valid     = true;
    this->modelData     = modelData;
    this->modelData.vbh = vbh;
    this->modelData.ibh = ibh;
    return true;
}

MeshAABB& MeshComponent::GetAABB() {
    uint64_t currentTransformVersion = 0;
    if (this->m_owner && this->m_owner->HasComponent(SYN_COMPONENT_TRANSFORM)) {
        TransformComponent* transform =
            this->m_owner->GetComponent<TransformComponent>();
        currentTransformVersion = transform->GetVersion();
    }

    if (!m_aabbDirty && m_cachedTransformVersion == currentTransformVersion) {
        return m_aabb;
    }

    MeshAABB& boundingBox = m_aabb;

    if (this->modelData.vertices.empty()) {
        return boundingBox; // Return default AABB if no vertices
    }

    // Compute local min/max
    Math::Vector3 min(FLT_MAX);
    Math::Vector3 max(-FLT_MAX);
    min = this->modelData.vertices[0].pos;
    max = this->modelData.vertices[0].pos;
    for (const auto& vertex : this->modelData.vertices) {
        for (int i = 0; i < 3; ++i) {
            min = min.min(vertex.pos);
            max = max.max(vertex.pos);
        }
    }

    MeshAABB result;
    // If transform exists, transform all 8 corners
    if (this->m_owner && this->m_owner->HasComponent(SYN_COMPONENT_TRANSFORM)) {
        TransformComponent* transform =
            this->m_owner->GetComponent<TransformComponent>();
        Mat4 modelMatrix = transform->GetModelMatrix();

        Vec3 corners[8] = {
            Vec3(min.x(), min.y(), min.z()), Vec3(max.x(), min.y(), min.z()),
            Vec3(min.x(), max.y(), min.z()), Vec3(max.x(), max.y(), min.z()),
            Vec3(min.x(), min.y(), max.z()), Vec3(max.x(), min.y(), max.z()),
            Vec3(min.x(), max.y(), max.z()), Vec3(max.x(), max.y(), max.z())
        };

        Vector3 tmin(FLT_MAX);
        Vector3 tmax(-FLT_MAX);
        for (int c = 0; c < 8; ++c) {
            Math::Vector4 t =
                Math::Vector4(
                    corners[c][0], corners[c][1], corners[c][2], 1.0f) *
                modelMatrix;
            tmin = tmin.min(t.xyz());
            tmax = tmax.max(t.xyz());
        }
        for (int i = 0; i < 3; ++i) {
            result.min.set(i, tmin[i]);
            result.max.set(i, tmax[i]);
            result.center.set(i, (tmin[i] + tmax[i]) / 2.0f);
            result.halfExtents.set(i, (tmax[i] - tmin[i]) / 2.0f);
        }
    } else { // No transform, use local min/max
        for (int i = 0; i < 3; ++i) {
            result.min.set(i, min[i]);
            result.max.set(i, max[i]);
            result.center.set(i, (min[i] + max[i]) / 2.0f);
            result.halfExtents.set(i, (max[i] - min[i]) / 2.0f);
        }
    }

    m_aabbDirty              = false;
    m_cachedTransformVersion = currentTransformVersion;
    m_aabb                   = result;
    return m_aabb;
}

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
        -> std::unique_ptr<Syngine::Component> {
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
