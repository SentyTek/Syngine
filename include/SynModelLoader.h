#pragma once
#include <vector>
#include <string>
#include <filesystem>

#include <bgfx/bgfx.h>

// forward declaration of assimp scene because i cant actually include it for
// some stupid reason
struct aiScene;

namespace Syngine {

struct Vertex {
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float normal[3] = {0.0f, 0.0f, 0.0f};
    float uv0[2] = {0.0f, 0.0f}; //macro UV
    float uv1[2] = {0.0f, 0.0f}; //for detail maps
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA
    float tangent[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // XYZW
};

struct Material {
    std::string name = "empty";
    bgfx::TextureHandle albedo = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle normalMap = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle heightMap = BGFX_INVALID_HANDLE;

    float tileDetail = 30.0f; //how many repeats of detail maps
    float heightScale = 0.01f; //matches blender displacement
    float mixFactor = 0.7f; //mix between detail and macro maps
    float ambient = 0.2f; //ambient floor
    float baseColor[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA base color
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle  ibh;
    uint32_t                 numVertices;
    uint32_t                 numIndices;
    uint8_t                  numMaterials;
    bool                     hasTextures = true;
    std::string              path        = "";
    int                      id          = 0;
    std::filesystem::file_time_type lastWriteTime; // for tracking changes
    bool                            valid = false; // Meshes are hardly ever actually invalid, but this is useful for reloading
};

class SynModelLoader {
    protected:
      static std::vector<MeshData> loadedMeshes;
    public:
    // Loads a model from the specified path, returns true if successful
    virtual bool
    LoadModel(MeshData& out, const std::string& path, bool loadTextures) = 0;
    // Reloads a model by its ID, returns true if successful
    virtual bool ReloadModel(MeshData& out, int id) = 0;
    static void UnloadAll();
    static std::vector<MeshData>& getMeshes();
    static MeshData* getMeshById(int id);
};

class AssimpLoader : public SynModelLoader {
  public:
    bool LoadModel(MeshData&          out,
                   const std::string& path,
                   bool               loadTextures) override;
    bool ReloadModel(MeshData& out, int id) override;

  private:
    static bool processScene(MeshData& meshData, const aiScene* scene, const std::string& path, bool loadTextures);
};

} // namespace Syngine