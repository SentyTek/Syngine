// ╒════════════════════ MeshComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/Core/JobSystem.h"
#include "Syngine/GameObjects/Components/TransformComponent.h"
#include <Syngine/GameObjects/Component.h>
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/Graphics/Resources/ModelLoader.h>
#include <Syngine/Utils/Serializer.h>
#include <Syngine/Math/Math.hpp>

#include <string>

namespace Syngine {

/// @brief Structure representing an axis-aligned bounding box (AABB)
/// @since v0.0.1
struct MeshAABB {
    Math::Vector3 min         = Math::Vector3(); //* Minimum corner of the AABB
    Math::Vector3 max         = Math::Vector3(); //* Maximum corner of the AABB
    Math::Vector3 center      = Math::Vector3(); //* Center of the AABB
    Math::Vector3 halfExtents = Math::Vector3(); //* Half extents of
};

/// @brief Component for rendering 3D mesh models. Supports loading 3D asset
/// files and uploading custom mesh data.
/// @section MeshComponent
/// @nameoverride MeshComponent
/// @since v0.0.1
class MeshComponent : public Syngine::IComponent {
  public:
    bool receiveShadows  = true; //* Whether the mesh receives shadows
    bool castShadows     = true; //* Whether the mesh casts shadows
    bool receiveSunLight = true; //* Whether the mesh receives sunlight

    static constexpr Syngine::ComponentTypeID componentType =
        SYN_COMPONENT_MESH; //* Mesh component type

    // Basic constructor for doing nothing.
    MeshComponent(GameObject* owner);

    /// @brief Constructor for the MeshComponent class
    /// @param owner Pointer to the GameObject that owns this component
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @note This should only be called by GameObject::AddComponent<T>()
    /// @since v0.0.1
    MeshComponent(GameObject*        owner,
                  const std::string& path,
                  bool               loadTextures = true);

    /// @brief Constructor for the MeshComponent class
    /// @param owner Pointer to the GameObject that owns this component
    /// @param bundlePath Path to the shader bundle containing the mesh
    /// @param texturePath Path to the mesh within the bundle
    /// @param loadTextures Whether to load textures for the model
    /// @note This should only be called by GameObject::AddComponent<T>()
    /// @since v0.0.1
    MeshComponent(GameObject*        owner,
                  const std::string& bundlePath,
                  const std::string& texturePath,
                  bool               loadTextures = true);

    MeshComponent(const MeshComponent& other);
    MeshComponent& operator=(const MeshComponent& other);

    ~MeshComponent();

    /// @brief Get the type of this component
    /// @return The component type as an enum value
    /// @threadsafety read-only
    /// @since v0.0.1
    Syngine::ComponentTypeID GetComponentType() override;

    /// @brief Clone the MeshComponent
    /// @return A unique pointer to the cloned MeshComponent
    std::unique_ptr<IComponent> Clone() const override {
        return std::make_unique<MeshComponent>(*this);
    }

    /// @brief Serializes the MeshComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// MeshComponent's state
    Serializer::DataNode Serialize() const override;

    /// @brief Initialize the mesh component
    /// @param bundlePath Path to the shader bundle containing the mesh
    /// @param texturePath Path to the mesh within the bundle
    /// @param loadTextures Whether to load textures for the model
    /// @note This should only be called when the component is added to a
    /// GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Init(const std::string& bundlePath,
              const std::string& texturePath,
              bool               loadTextures = true);

    /// @brief Update the mesh component. Unused.
    /// @param deltaTime Time elapsed since the last update in seconds
    /// @since v0.0.1
    void Update(float deltaTime) override;

    void PostPhysicsUpdate() override;

    /// @brief Load a mesh from a file
    /// @param bundlePath Path to the shader bundle containing the mesh
    /// @param texturePath Path to the mesh within the bundle
    /// @param loadTextures Whether to load textures for the model
    /// @return true on success, false on failure
    /// @threadsafety not-safe
    /// @since v0.0.1
    bool LoadMesh(const std::string& bundlePath,
                  const std::string& texturePath,
                  bool               loadTextures = true);

    /// @brief Reload the mesh from the file
    /// @return true on success, false on failure
    /// @note Mostly used for reloading the mesh after changes in the model file
    /// @threadsafety not-safe
    /// @since v0.0.1
    bool ReloadMesh();

    /// @brief Unload the mesh
    /// @return true on success, false on failure
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
        return bgfx::isValid(this->modelData.vbh) &&
               bgfx::isValid(this->modelData.ibh);
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
    bool
    UploadMesh(std::vector<float>    vertices,
               std::vector<uint32_t> indices,
               Math::Vector4 baseColor = Math::Vector4(1.0f, 1.0f, 1.0f, 0.0f));

    ModelData modelData; //* Mesh data for the GameObject

    /// @brief Get the axis-aligned bounding box (AABB) of the mesh
    /// @return Reference to the MeshAABB structure
    /// @threadsafety read-only
    /// @since v0.0.1
    const MeshAABB& GetAABB() const;

    /// @brief Get the submesh count
    /// @return Number of submeshes in the mesh
    /// @threadsafety read-only
    /// @since v0.0.1
    uint8_t GetSubmeshCount() const;

    /// @brief Get material index for a specific submesh
    /// @param submeshIndex Index of the submesh to query
    /// @return Material index for the specified submesh, or 255 if invalid
    /// index
    /// @threadsafety read-only
    /// @since v0.0.1
    uint8_t GetSubmeshMaterialIndex(uint8_t submeshIndex) const;

    /// @brief Assign a material to a specific submesh
    /// @param submeshIndex Index of the submesh to assign the material to
    /// @param materialIndex Index of the material to assign to the submesh
    /// @return true if the assignment was successful, false if the submesh
    /// index is invalid
    /// @threadsafety not-safe
    /// @since v0.0.1
    bool SetSubmeshMaterialIndex(uint8_t submeshIndex, uint8_t materialIndex);

    /// @brief Get the mutable material instance assigned to a submesh.
    /// @return The instance, or nullptr when the submesh/material index is
    /// invalid.
    MaterialInstance*       GetMaterialInstance(uint8_t submeshIndex);
    const MaterialInstance* GetMaterialInstance(uint8_t submeshIndex) const;

    /// @brief Gets the UV scale override for the whole object
    /// @return UV scale override for the whole object. 1.0f by default.
    /// @threadsafety read-only
    /// @since v0.0.1
    float GetObjectUVScaleOverride() const;

    /// @brief Sets the UV scale override for the whole object
    /// @param uvScaleOverride UV scale override to set for the whole object
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetObjectUVScaleOverride(float uvScaleOverride);

  private:
    mutable MeshAABB m_aabb; //* Axis-aligned bounding box of the mesh
    mutable bool     m_aabbDirty =
        true; //* Whether the AABB needs to be recalculated
    mutable uint64_t m_cachedTransformVersion =
        0; //* Cached version of the transform when AABB was last calculated
    float m_objectUVScaleOverride =
        1.0f;                  //* UV scale override for the whole object
    std::string m_bundlePath;  //* Path to the shader bundle containing the mesh
    std::string m_texturePath; //* Path to the mesh within the bundle
    bool        m_loadTextures = true;

    bool m_isWaitingForMeshLoad =
        false; //* Whether the component is waiting for the mesh to load
    bool                 m_isReloadingMesh = false;
    JobResult<ModelData> m_meshLoadJob; //* Job result for the mesh load job

    void                _RecalculateAABB();
    TransformComponent* m_transform;

    friend class Core;
};

} // namespace Syngine
