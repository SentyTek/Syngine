#pragma once
#include <vector>
#include <string>

#include <bgfx/bgfx.h>

struct Vertex {
    float pos[3] = {0.0f, 0.0f, 0.0f};
    float normal[3] = {0.0f, 0.0f, 0.0f};
    float uv[2] = {0.0f, 0.0f};
    float color[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // RGBA
};

struct Material {
    std::string name = "empty";
    bgfx::TextureHandle baseColor = BGFX_INVALID_HANDLE;
};

struct SynMeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    uint32_t numVertices;
    uint32_t numIndices;
    uint8_t numMaterials;
};

class SynModelLoader {
    protected:
    std::vector<SynMeshData> meshes;
    public:
    virtual bool LoadModel(SynMeshData& out, const std::string& path, bool loadTextures) = 0;
    virtual void UnloadAll() = 0;
    std::vector<SynMeshData>& getMeshes();
};

class AssimpLoader : public SynModelLoader {
    public:
    bool LoadModel( SynMeshData& out, const std::string& path, bool loadTextures) override;
    void UnloadAll() override;
};