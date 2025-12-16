// ╒════════════════════ MeshComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Utils/ModelLoader.h"

namespace Syngine {
/*
 * @brief Syngine Mesh Component. The MeshComponent is used to represent a 3D
 * model in the game world.
 * @section MeshComponent
*/
class MeshComponent : public Syngine::Component {
  public:
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

    /// @brief Initialize the mesh component
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @note This should only be called when the component is added to a GameObject
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
    /// @param vertices Vector of vertex data
    /// @param indices Vector of index data
    /// @return true on success, false on failure
    /// @threadsafety not-safe
    /// @since v0.0.1
    bool UploadMesh(std::vector<float> vertices, std::vector<uint32_t> indices);

    MeshData meshData; //* Mesh data for the GameObject
};

} // namespace Syngine