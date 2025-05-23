#pragma once
#include "Components.h"
#include "SynModelLoader.h"

/*
    Syngine Mesh Component
    The MeshComponent is used to represent a 3D model in the game world.
    It is used to render the model using the graphics engine.
    It contains a reference to a 3D model and it's associated materials.
*/
class MeshComponent : public SynComponent {
    public:
    static constexpr SynComponents componentType = SYN_COMPONENT_MESH;

    MeshComponent();
    SynMeshData meshData; //Mesh data for the gameobject
    SynComponents getComponentType() override;
    
    int LoadMesh(const std::string& path, bool loadTextures = true); //Load a mesh from a file
};