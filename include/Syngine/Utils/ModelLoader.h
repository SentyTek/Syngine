// ╒══════════════════════ ModelLoader.h ═╕
// │ Syngine                              │
// │ Created 2025-05-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include <vector>
#include <string>
#include <filesystem>

#include <bgfx/bgfx.h>

// Forward declaration of Assimp scene because I cant actually include it for some stupid reason
struct aiScene;
namespace Syngine {

/// @brief Vertex structure for mesh data
/// @section SynModelLoader
/// @since v0.0.1
struct Vertex {
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float normal[3] = {0.0f, 0.0f, 0.0f};
    float uv0[2] = {0.0f, 0.0f}; // Macro UV
    float uv1[2] = {0.0f, 0.0f}; // For detail maps
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA
    float tangent[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // XYZW
};

/// @brief Material structure for mesh data
/// @section SynModelLoader
/// @since v0.0.1
struct Material {
    std::string name = "empty"; //* Name of the material
    bgfx::TextureHandle albedo = BGFX_INVALID_HANDLE; //* Albedo texture handle
    bgfx::TextureHandle normalMap = BGFX_INVALID_HANDLE; //* Normal map texture handle
    bgfx::TextureHandle heightMap = BGFX_INVALID_HANDLE; //* Height map texture handle

    float tileDetail = 30.0f; //* How many repeats of detail maps
    float heightScale = 0.01f; //* Matches blender displacement
    float mixFactor = 0.7f; //* Mix between detail and macro maps
    float ambient = 0.2f; //* Ambient floor
    float baseColor[4] = {1.0f, 1.0f, 1.0f, 1.0f}; //* RGBA base color
};

/// @brief MeshData structure for storing mesh information
/// @section SynModelLoader
/// @since v0.0.1
struct MeshData {
    std::vector<Vertex> vertices; //* Vertices of the mesh
    std::vector<uint32_t> indices; //* Indices of the mesh
    std::vector<Material> materials; //* Materials of the mesh
    bgfx::VertexBufferHandle vbh; //* Vertex buffer handle
    bgfx::IndexBufferHandle  ibh; //* Index buffer handle
    uint32_t                 numVertices; //* Number of vertices in the mesh
    uint32_t                 numIndices; //* Number of indices in the mesh
    uint8_t                  numMaterials; //* Number of materials in the mesh
    bool                     hasTextures = true; //* Whether the mesh has textures
    std::string              path        = ""; //* Path to the mesh file (relative to the game root)
    int                      id          = 0; //* Unique ID of the mesh
    std::filesystem::file_time_type lastWriteTime; //* For tracking changes
    bool                            valid = false; //* Meshes are hardly ever actually invalid, but this is useful for reloading
};

/// @brief SynModelLoader class for loading 3D models
/// @section SynModelLoader
/// @since v0.0.1
class SynModelLoader {
    protected:
      static std::vector<MeshData> loadedMeshes;
    public:
      virtual bool
      _LoadModel(MeshData& out, const std::string& path, bool loadTextures) = 0;
      virtual bool _ReloadModel(MeshData& out, int id) = 0;

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
};

/// @brief AssimpLoader class for loading 3D models using Assimp
/// @section SynModelLoader
/// @since v0.0.1
class AssimpLoader : public SynModelLoader {
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
    /// @param meshData MeshData to fill with the processed data
    /// @param scene Assimp scene to process
    /// @param path Path to the model file
    /// @param loadTextures Whether to load textures for the model
    /// @return true if the scene was processed successfully, false otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    static bool processScene(MeshData& meshData, const aiScene* scene, const std::string& path, bool loadTextures);
};

} // namespace Syngine