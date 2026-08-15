// ╒══════════════════════ ModelLoader.h ═╕
// │ Syngine                              │
// │ Created 2025-05-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <Syngine/Graphics/Resources/MaterialManager.h>
#include <Syngine/Math/Math.hpp>

#include <bgfx/bgfx.h>

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

// forward declarations
namespace scl {
class stream;
};

struct aiScene;
struct aiMaterial;
namespace Syngine {

/// @brief Vertex structure for mesh data
/// @section ModelLoader
/// @since v0.0.1
struct Vertex {
    Math::Vector3 pos;    //* Position of the vertex in 3D space
    Math::Vector3 normal; //* Normal vector at the vertex
    Math::Vector2 uv0;    //* Primary texture coordinates (macro UV)
    Math::Vector2 uv1;    //* Secondary texture coordinates (for detail maps)
    Math::Vector4 color = Math::Vector4(1.0f); //* Vertex color (RGBA)
    Math::Vector4 tangent; //* Tangent vector at the vertex (for normal mapping)
};

/// @brief SubMesh structure for storing submesh information
/// @section ModelLoader
/// @since v0.0.1
struct SubMesh {
    // Index range within the combined index buffer
    uint32_t indexStart; //* Starting index in the index buffer for this submesh
    uint32_t indexCount; //* Number of indices in this submesh

    // Material index for this submesh
    uint8_t materialIndex; //* Index of the material used by this submesh

    // Base properties
    std::string
        name; //* Name of the submesh (for debugging and editor purposes)
    Math::Vector3 boundMin; //* Minimum corner of the axis-aligned bounding box
    Math::Vector3 boundMax; //* Maximum corner of the axis-aligned bounding box
};

/// @brief ModelData structure for storing mesh information
/// @section ModelLoader
/// @since v0.0.1
struct ModelData {
    // Base properties
    std::vector<Vertex>   vertices;  //* List of vertices in the mesh
    std::vector<uint32_t> indices;   //* List of indices for indexed drawing
    std::vector<SubMesh>  subMeshes; //* List of submeshes in the mesh
    std::vector<MaterialInstance>
            materials;    //* Per-mesh material instances used by the mesh
    uint8_t numSubMeshes; //* Number of submeshes in the mesh
    uint8_t numMaterials; //* Number of materials used by the mesh

    // GPU resources
    bgfx::VertexBufferHandle vbh; //* Handle to the vertex buffer on the GPU
    bgfx::IndexBufferHandle  ibh; //* Handle to the index buffer on the GPU

    // Metadata
    int  id; //* Unique ID for the mesh (for hot reloading and editor purposes)
    bool valid; //* Whether the mesh data is valid and can be rendered
    std::filesystem::file_time_type
        lastWriteTime; //* Last write time of the mesh file (for hot reloading)

    Math::Vector3
        localMin; //* Minimum corner of the local axis-aligned bounding box
    Math::Vector3
        localMax; //* Maximum corner of the local axis-aligned bounding box
};

/// @brief Model class for loading 3D models
/// @section ModelLoader
/// @since v0.0.1
class ModelLoader {
  public:
    virtual bool _LoadModel(ModelData&         out,
                            scl::stream*       meshStream,
                            const std::string& assetPath,
                            bool               loadTextures) = 0;
    virtual bool _ReloadModel(ModelData&         out,
                              scl::stream*       stream,
                              const std::string& assetPath,
                              int                id,
                              bool               loadTextures) = 0;

    /// @brief Creates BGFX resources for the given ModelData
    /// @param data ModelData for which to create BGFX resources
    /// @threadsafety not-safe
    /// @since v0.0.2
    static void CreateBGFXResources(ModelData& out);

    virtual ~ModelLoader() = default; // this keeps getting deleted???
};

/// @brief AssimpLoader class for loading 3D models using Assimp
/// @section ModelLoader
/// @since v0.0.1
class AssimpLoader : public ModelLoader {
  public:
    /// @brief Loads a model from the specified path, returns true if
    /// successful
    /// @param out ModelData to fill with the loaded model
    /// @param meshStream Stream containing the model data
    /// @param loadTextures Whether to load textures for the model
    /// @return true if the model was loaded successfully, false otherwise
    /// @threadsafety safe
    /// @since v0.0.2
    /// @internal
    bool _LoadModel(ModelData&         out,
                    scl::stream*       meshStream,
                    const std::string& assetPath,
                    bool               loadTextures) override;

    /// @brief Reloads a model by its ID, returns true if successful
    /// @param out ModelData to fill with the reloaded model
    /// @param id ID of the model to reload
    /// @return true if the model was reloaded successfully, false otherwise
    /// @note This is used to reload models only when they change on disk for
    /// hot reloading
    /// @internal
    bool _ReloadModel(ModelData&         out,
                      scl::stream*       stream,
                      const std::string& assetPath,
                      int                id,
                      bool               loadTextures) override;

  private:
    /// @brief Processes the Assimp scene and fills the MeshData structure
    /// @param out ModelData to fill with the processed data
    /// @param scene Assimp scene to process
    /// @param meshStream Stream containing the model data (for resolving
    /// relative texture paths)
    /// @return true if the scene was processed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static bool processScene(ModelData&     out,
                             const aiScene* scene,
                             scl::stream*   meshStream,
                             bool           loadTextures = true);

    /// @brief Processes an Assimp material and fills the Material structure
    /// @param aiMat Assimp material to process
    /// @param meshStream Stream containing the model data (for resolving
    /// relative texture paths)
    /// @param loadTextures Whether to load textures for the material
    /// @return Material structure filled with the processed material data
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static MaterialInstance _ProcessMaterial(aiMaterial*    aiMat,
                                             const aiScene* scene,
                                             scl::stream*   meshStream,
                                             bool loadTextures = true);

    struct TempProcessedMesh {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
        SubMesh               subMesh;
    };
};

} // namespace Syngine
