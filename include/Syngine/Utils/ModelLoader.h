// ╒══════════════════════ ModelLoader.h ═╕
// │ Syngine                              │
// │ Created 2025-05-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once
#include <vector>
#include <string>
#include <filesystem>

#include <bgfx/bgfx.h>

struct aiScene;
struct aiMaterial;
namespace Syngine {

/// @brief Vertex structure for mesh data
/// @section ModelLoader
/// @since v0.0.1
struct Vertex {
    float pos[3]     = { 0.0f, 0.0f, 0.0f };
    float normal[3]  = { 0.0f, 0.0f, 0.0f };
    float uv0[2]     = { 0.0f, 0.0f };             // Macro UV
    float uv1[2]     = { 0.0f, 0.0f };             // For detail maps
    float color[4]   = { 1.0f, 1.0f, 1.0f, 1.0f }; // RGBA
    float tangent[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // XYZW
};

/// @brief Material structure for mesh data
/// @section ModelLoader
/// @since v0.0.1
struct Material {
    // Base properties
    std::string         name   = "empty";             //* Name of the material
    bgfx::TextureHandle albedo = BGFX_INVALID_HANDLE; //* Albedo texture handle
    bgfx::TextureHandle normalMap =
        BGFX_INVALID_HANDLE; //* Normal map texture handle
    bgfx::TextureHandle heightMap =
        BGFX_INVALID_HANDLE; //* Height map texture handle

    // Per material
    float uvScale[3] = { 1.0f, 1.0f, 1.0f }; //* UV scale for each texture type (albedo, normal, height)

    // Per material properties
    float heightScale = 0.01f; //* Matches blender displacement
    float mixFactor   = 0.7f;  //* Mix between detail and macro maps
    float ambient     = 0.2f;  //* Ambient floor

    // baseColor is used when there is no albedo texture, or when useVertexColor
    // is true. It can also be used to tint the albedo texture if useVertexColor
    // is false.
    bool useVertexColor = false; //* Whether to use vertex color or base color
    float baseColor[4]   = { 1.0f, 1.0f, 1.0f, 1.0f }; //* RGBA base color
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
    float boundMin[3]; //* Minimum bounding box coordinates for the submesh
    float boundMax[3]; //* Maximum bounding box coordinates for the submesh
};

/// @brief MeshData structure for storing mesh information
/// @section ModelLoader
/// @since v0.0.1
struct MeshData {
    // Base properties
    std::vector<Vertex>   vertices;     //* List of vertices in the mesh
    std::vector<uint32_t> indices;      //* List of indices for indexed drawing
    std::vector<Material> materials;    //* List of materials used by the mesh
    uint8_t               numMaterials; //* Number of materials used by the mesh
    std::vector<SubMesh>  subMeshes;    //* List of submeshes in the mesh
    uint8_t               numSubMeshes; //* Number of submeshes in the mesh

    // GPU resources
    bgfx::VertexBufferHandle vbh; //* Handle to the vertex buffer on the GPU
    bgfx::IndexBufferHandle  ibh; //* Handle to the index buffer on the GPU

    // Metadata
    std::string
         path; //* Path to the mesh file (for debugging and editor purposes)
    int  id; //* Unique ID for the mesh (for hot reloading and editor purposes)
    bool valid; //* Whether the mesh data is valid and can be rendered
    std::filesystem::file_time_type
         lastWriteTime; //* Last write time of the mesh file (for hot reloading)
};

/// @brief Model class for loading 3D models
/// @section ModelLoader
/// @since v0.0.1
class ModelLoader {
  protected:
    static std::vector<MeshData> loadedMeshes;

  public:
    virtual bool
    _LoadModel(MeshData& out, const std::string& path, bool loadTextures) = 0;
    virtual bool _ReloadModel(MeshData& out, int id)                      = 0;

    /// @brief Unloads all loaded models
    /// @note This is used to clear all loaded models, for example when the
    /// game is shutting down
    /// @threadsafety not-safe
    /// @post All loaded models are unloaded and their resources are freed
    /// @since v0.0.1
    /// @internal
    static void _UnloadAllMeshes();

    /// @brief Get all loaded meshes
    /// @return std::vector<MeshData>& A reference to the vector of all loaded
    /// meshes
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    static std::vector<MeshData>& _GetMeshes();

    /// @brief Get a mesh by its ID
    /// @param id ID of the mesh to get
    /// @return MeshData* Pointer to the mesh with the given ID, nullptr if
    /// not found
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    static MeshData* _GetMeshById(int id);

    virtual ~ModelLoader() = default; // this keeps getting deleted???
};

/// @brief AssimpLoader class for loading 3D models using Assimp
/// @section ModelLoader
/// @since v0.0.1
class AssimpLoader : public ModelLoader {
  public:
    /// @brief Loads a model from the specified path, returns true if
    /// successful
    /// @param out MeshData to fill with the loaded model
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @return true if the model was loaded successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    bool _LoadModel(MeshData&          out,
                    const std::string& path,
                    bool               loadTextures) override;

    /// @brief Reloads a model by its ID, returns true if successful
    /// @param out MeshData to fill with the reloaded model
    /// @param id ID of the model to reload
    /// @return true if the model was reloaded successfully, false otherwise
    /// @note This is used to reload models only when they change on disk for
    /// hot reloading
    /// @internal
    bool _ReloadModel(MeshData& out, int id) override;

  private:
    /// @brief Processes the Assimp scene and fills the MeshData structure
    /// @param out MeshData to fill with the processed data
    /// @param scene Assimp scene to process
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @return true if the scene was processed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static bool processScene(MeshData&          out,
                             const aiScene*     scene,
                             const std::string& path,
                             bool               loadTextures);


    /// @brief Processes an Assimp material and fills the Material structure
    /// @param aiMat Assimp material to process
    /// @param path Path to the model file (for resolving relative texture
    /// paths)
    /// @param loadTextures Whether to load textures for the material
    /// @return Material structure filled with the processed material data
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static Material _ProcessMaterial(aiMaterial*        aiMat,
                                     const aiScene*     scene,
                                     const std::string& path,
                                     bool               loadTextures);

    /// @brief Creates a default material with no textures
    /// @return Material structure filled with default material data
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static Material _CreateDefaultMaterial();
};

} // namespace Syngine
