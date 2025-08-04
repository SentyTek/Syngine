#pragma once
#include "Components.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"

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

    ~MeshComponent();
      
    /// @brief Get the type of this component
    /// @return The component type as an enum value
    /// @threadsafety read-only
    /// @since v0.0.1
    Components GetComponentType() override;

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
    int LoadMesh(const std::string& path, bool loadTextures = true);

    /// @brief Reload the mesh from the file
    /// @return 0 on success, non-zero on failure
    /// @note Mostly used for reloading the mesh after changes in the model file
    /// @threadsafety not-safe
    /// @since v0.0.1
    int ReloadMesh();

    /// @brief Unload the mesh
    /// @return 0 on success, non-zero on failure
    /// @note This is called when the component is removed or the GameObject is
    /// destroyed
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    int UnloadMesh(); //TODO: Make this RAII in the destructor

    MeshData meshData; //* Mesh data for the GameObject
};

} // namespace Syngine