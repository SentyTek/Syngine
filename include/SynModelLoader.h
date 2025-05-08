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
    std::string name = "default";
    std::string texturePath = "";
};

struct SynMeshData {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    uint32_t numVertices;
    Material material;
    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
};

class SynModelLoader {
    protected:
    std::vector<SynMeshData> meshes;
    public:
    virtual bool LoadModel(const std::string& path, SynMeshData& out) = 0;
    virtual void UnloadAll() = 0;
    std::vector<SynMeshData>& getMeshes();
};

class AssimpLoader : public SynModelLoader {
    public:
    bool LoadModel(const std::string& path, SynMeshData& out) override;
    void UnloadAll() override;
};