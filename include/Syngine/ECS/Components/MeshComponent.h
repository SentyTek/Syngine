// ╒════════════════════ MeshComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/Utils/Serializer.h"

namespace Syngine {

/// @brief Structure representing an axis-aligned bounding box (AABB)
/// @since v0.0.1
struct MeshAABB {
    float min[3] = {0.0f, 0.0f, 0.0f};        //* Minimum corner of the AABB
    float max[3] = {0.0f, 0.0f, 0.0f};        //* Maximum corner of the AABB
    float center[3] = {0.0f, 0.0f, 0.0f};     //* Center of the AABB
    float halfExtents[3] = {0.0f, 0.0f, 0.0f}; //* Half extents of the AABB
};

/// @brief Component for rendering 3D mesh models. Supports loading 3D asset
/// files and uploading custom mesh data.
/// @since v0.0.1
class MeshComponent : public Syngine::Component {
  public:
    bool receiveShadows  = true; //* Whether the mesh receives shadows
    bool castShadows     = true; //* Whether the mesh casts shadows
    bool receiveSunLight = true; //* Whether the mesh receives sunlight

    static constexpr Syngine::Components componentType =
        SYN_COMPONENT_MESH; //* Mesh component type

    /// @brief Constructor for the MeshComponent class
    /// @param owner Pointer to the GameObject that owns this component
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @note This should only be called by GameObject::AddComponent<T>()
    /// @since v0.0.1
    MeshComponent(GameObject*        owner,
                  const std::string& path         = "",
                  bool               loadTextures = true);

    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& other);

    ~MeshComponent();

    /// @brief Get the type of this component
    /// @return The component type as an enum value
    /// @threadsafety read-only
    /// @since v0.0.1
    Components GetComponentType() override;

    /// @brief Clone the MeshComponent
    /// @return A unique pointer to the cloned MeshComponent
    std::unique_ptr<Component> Clone() const override {
        return std::make_unique<MeshComponent>(*this);
    }

    /// @brief Serializes the MeshComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// MeshComponent's state
    Serializer::DataNode Serialize() const override;

    /// @brief Initialize the mesh component
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @note This should only be called when the component is added to a
    /// GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Init(const std::string& path = "", bool loadTextures = true);

    /// @brief Update the mesh component
    /// @note There is no specific update logic for the mesh component
    /// @since v0.0.1
    void Update() {};

    /// @brief Load a mesh from a file
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @return 0 on success, non-zero code on failure
    /// @threadsafety not-safe
    /// @since v0.0.1
    bool LoadMesh(const std::string& path, bool loadTextures = true);

    /// @brief Reload the mesh from the file
    /// @return 0 on success, non-zero on failure
    /// @note Mostly used for reloading the mesh after changes in the model file
    /// @threadsafety not-safe
    /// @since v0.0.1
    bool ReloadMesh();

    /// @brief Unload the mesh
    /// @return 0 on success, non-zero on failure
    /// @note This is called when the component is removed or the GameObject is
    /// destroyed
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    bool UnloadMesh();

    /// @brief Check if the mesh is loaded
    /// @return true if the mesh is loaded, false otherwise
    /// @threadsafety read-only
    /// @since v0.0.1
    bool IsMeshLoaded() const {
        return bgfx::isValid(this->meshData.vbh) &&
               bgfx::isValid(this->meshData.ibh);
    }

    /// @brief Upload mesh data directly
    /// @param vertices Vector of vertex data. Expected format per vertex:
    /// [pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uv0.u, uv0.v,
    /// color.r, color.g, color.b, color.a]
    /// However, if you provide a baseColor, do NOT include color data in the
    /// vertices.
    /// @param indices Vector of index data.
    /// @param baseColor Optional base color to apply to the entire mesh if
    /// vertex colors are not provided. Format: [r, g, b, [a]] (0-255, a is
    /// optional)
    /// @return true on success, false on failure
    /// @threadsafety not-safe
    /// @since v0.0.1
    bool UploadMesh(std::vector<float>    vertices,
                    std::vector<uint32_t> indices,
                    std::vector<uint8_t>  baseColor = {});

    MeshData meshData; //* Mesh data for the GameObject

    /// @brief Get the axis-aligned bounding box (AABB) of the mesh
    /// @return Reference to the MeshAABB structure
    /// @threadsafety read-only
    /// @since v0.0.1
    MeshAABB& GetAABB();

  private:
    mutable MeshAABB m_aabb; //* Axis-aligned bounding box of the mesh
    mutable bool     m_aabbDirty =
        true; //* Whether the AABB needs to be recalculated
    mutable uint64_t m_cachedTransformVersion =
        0; //* Cached version of the transform when AABB was last calculated
};

} // namespace Syngine
