#pragma once
#include "Components.h"
#include "SynModelLoader.h"
#include "SyngineGameobject.h"

namespace Syngine {
/*
    Syngine Mesh Component
    The MeshComponent is used to represent a 3D model in the game world.
    It is used to render the model using the graphics engine.
    It contains a reference to a 3D model and it's associated materials.
*/
class MeshComponent : public Syngine::Component {
    public:
      static constexpr Syngine::Components componentType = SYN_COMPONENT_MESH;
      
      MeshComponent(GameObject* owner, const std::string& path = "", bool loadTextures = true);
      MeshData meshData; // Mesh data for the gameobject
      Components getComponentType() override;

      void Init(const std::string& path = "", bool loadTextures = true); // Initialize the mesh component with a path to the model file
      void Update() {}; // No update logic needed for mesh component
      
      int LoadMesh(const std::string& path,
                   bool loadTextures = true); // Load a mesh from a file
      int UnloadMesh();                       // Unload the mesh data
      int ReloadMesh(); // Reload the mesh data from the file, useful for
                        // hot-reloading
};

} // namespace Syngine