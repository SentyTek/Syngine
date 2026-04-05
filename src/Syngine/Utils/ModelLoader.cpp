// ╒════════════════════ ModelLoader.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Logger.h"
#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/Graphics/TextureHelpers.h"

#include "Syngine/Utils/FsUtils.h"
#include "assimp/color4.h"
#include "assimp/types.h"
#include "assimp/vector3.h"
#include "bgfx/bgfx.h"

#include "bx/math.h"
#include <cstdint>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "assimp/material.h"
#include "assimp/postprocess.h"

namespace Syngine {

namespace {

bgfx::TextureHandle _LoadAssimpTexture(const aiScene*                  scene,
                                       const aiString&                 texPath,
                                       const std::filesystem::path&    modelDir) {
    if (texPath.length == 0) {
        return BGFX_INVALID_HANDLE;
    }

    if (scene) {
        if (const aiTexture* embedded = scene->GetEmbeddedTexture(texPath.C_Str())) {
            if (embedded->mHeight == 0) {
                return Syngine::LoadTextureFromMemory(
                    reinterpret_cast<const uint8_t*>(embedded->pcData),
                    embedded->mWidth,
                    texPath.C_Str());
            }

            // Assimp stores raw textures as aiTexel (BGRA8); convert to RGBA8.
            const uint32_t pixelCount = embedded->mWidth * embedded->mHeight;
            std::vector<uint8_t> rgba(pixelCount * 4);
            for (uint32_t i = 0; i < pixelCount; ++i) {
                const aiTexel& src = embedded->pcData[i];
                rgba[i * 4 + 0] = src.r;
                rgba[i * 4 + 1] = src.g;
                rgba[i * 4 + 2] = src.b;
                rgba[i * 4 + 3] = src.a;
            }

            return bgfx::createTexture2D(
                static_cast<uint16_t>(embedded->mWidth),
                static_cast<uint16_t>(embedded->mHeight),
                false,
                1,
                bgfx::TextureFormat::RGBA8,
                BGFX_TEXTURE_NONE,
                bgfx::copy(rgba.data(), static_cast<uint32_t>(rgba.size())));
        }
    }

    std::filesystem::path resolvedPath(texPath.C_Str());
    if (resolvedPath.is_relative()) {
        resolvedPath = modelDir / resolvedPath;
    }
    resolvedPath = resolvedPath.lexically_normal();

    return Syngine::LoadTextureFromFile(resolvedPath.string().c_str());
}

} // namespace

/* Base class */
std::vector<MeshData> ModelLoader::loadedMeshes;

// Returns a vector of all loaded meshes
std::vector<MeshData>& ModelLoader::_GetMeshes() { return loadedMeshes; }

// Returns a pointer to a mesh by its ID, or nullptr if not found.
// ID param is the index in the meshes vector, returned from the LoadModel
// function.
MeshData* ModelLoader::_GetMeshById(int id) {
    for (auto& mesh : loadedMeshes) {
        if (mesh.id == id) {
            return &mesh;
        }
    }
    return nullptr; // not found
}

void ModelLoader::_UnloadAllMeshes() {
    for (auto& mesh : loadedMeshes) {
        for (auto& mat : mesh.materials) {
            if (bgfx::isValid(mat.albedo)) {
                bgfx::destroy(mat.albedo);
            }
            if (bgfx::isValid(mat.normalMap)) {
                bgfx::destroy(mat.normalMap);
            }
            if (bgfx::isValid(mat.heightMap)) {
                bgfx::destroy(mat.heightMap);
            }
        }
        mesh.materials.clear();
        bgfx::destroy(mesh.vbh);
        bgfx::destroy(mesh.ibh);
    }
    loadedMeshes.clear();
}

/* Assimp importer */

// Returns true if the model was loaded successfully, false otherwise
bool AssimpLoader::_LoadModel(MeshData&          out,
                              const std::string& path,
                              bool               loadTextures) {
    Assimp::Importer importer;

    const int flags = aiProcess_JoinIdenticalVertices |
                      aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
                      aiProcess_DropNormals;

    // read file. ideally use some kind of post processing (tangents, join
    // indices, etc), but this is a simple example
    const std::string resolvedPath = _ResolveOSPath(path.c_str());
    const aiScene*    scene        = importer.ReadFile(resolvedPath, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->HasMeshes()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Error loading model: %s",
                              importer.GetErrorString());
        return false;
    }

    if (scene->mNumMeshes == 0) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "No meshes found in model: %s",
                              resolvedPath.c_str());
        return false;
    }

    MeshData meshData;
    if (!processScene(meshData, scene, resolvedPath, loadTextures)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Failed to process scene for model: %s",
                              resolvedPath.c_str());
        return false;
    }
    meshData.path        = resolvedPath;
    meshData.id          = static_cast<int>(loadedMeshes.size());

    try {
        meshData.lastWriteTime = std::filesystem::last_write_time(resolvedPath);
    } catch (const std::filesystem::filesystem_error& e) {
        Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                              "Failed to get last write time for %s: %s",
                              resolvedPath.c_str(),
                              e.what()); // e.what() lol what a name
    }
    meshData.valid = true; // Mark as valid after processing

    Syngine::Logger::LogF(
        Syngine::LogLevel::INFO, "Loaded mesh %s", path.c_str());
    loadedMeshes.push_back(meshData);
    out = meshData;
    return true;
}

bool AssimpLoader::processScene(MeshData&          out,
                                const aiScene*     scene,
                                const std::string& path,
                                bool               loadTextures) {
    out.vertices.clear();
    out.indices.clear();
    out.materials.clear();
    out.subMeshes.clear();

    std::vector<Material> allMaterials;
    {
        // Process all aiScene materials
        for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* aiMat = scene->mMaterials[i];
            Material    mat   = _ProcessMaterial(aiMat, scene, path, loadTextures);
            allMaterials.push_back(mat);
        }

        // Ensure at least one fallback material
        if (allMaterials.empty()) {
            allMaterials.push_back(_CreateDefaultMaterial());
        }

        out.materials = allMaterials;
        out.numMaterials = static_cast<uint8_t>(allMaterials.size());
    }

    // Process all meshes in scene
    uint32_t vertexOffset = 0;
    uint32_t indexOffset  = 0;

    for (uint32_t meshIdx = 0; meshIdx < scene->mNumMeshes; ++meshIdx) {
        aiMesh* aiMeshPtr = scene->mMeshes[meshIdx];

        // Create submesh record for this Assimp mesh
        SubMesh subMesh;
        subMesh.indexStart = indexOffset;
        subMesh.indexCount = aiMeshPtr->mNumFaces * 3; // assuming triangles

        // Map Assimp material index to our materials array
        // Invalid indicies are 255
        subMesh.materialIndex =
            (aiMeshPtr->mMaterialIndex < allMaterials.size())
                ? static_cast<uint8_t>(aiMeshPtr->mMaterialIndex)
                : 255;

        subMesh.name = aiMeshPtr->mName.C_Str();

        // Compute AABB for this submesh
        if (aiMeshPtr->mNumVertices > 0) {
            aiVector3D min = aiMeshPtr->mVertices[0];
            aiVector3D max = aiMeshPtr->mVertices[0];
            for (uint32_t v = 1; v < aiMeshPtr->mNumVertices; ++v) {
                const aiVector3D& vert = aiMeshPtr->mVertices[v];
                min.x = std::min(min.x, vert.x);
                min.y = std::min(min.y, vert.y);
                min.z = std::min(min.z, vert.z);
                max.x = std::max(max.x, vert.x);
                max.y = std::max(max.y, vert.y);
                max.z = std::max(max.z, vert.z);
            }
            subMesh.boundMin[0] = min.x;
            subMesh.boundMin[1] = min.y;
            subMesh.boundMin[2] = min.z;
            subMesh.boundMax[0] = max.x;
            subMesh.boundMax[1] = max.y;
            subMesh.boundMax[2] = max.z;
        } else {
            // No vertices, set empty bounds
            subMesh.boundMin[0] = subMesh.boundMin[1] =
                subMesh.boundMin[2] = 0.0f;
            subMesh.boundMax[0] = subMesh.boundMax[1] =
                subMesh.boundMax[2] = 0.0f;
        }
        out.subMeshes.push_back(subMesh);

        // Process vertices for this mesh
        for (uint32_t v = 0; v < aiMeshPtr->mNumVertices; ++v) {
            Vertex vertex = {};

            // Position
            vertex.pos[0] = aiMeshPtr->mVertices[v].x;
            vertex.pos[1] = aiMeshPtr->mVertices[v].y;
            vertex.pos[2] = aiMeshPtr->mVertices[v].z;

            // Normal
            if (aiMeshPtr->HasNormals()) {
                vertex.normal[0] = aiMeshPtr->mNormals[v].x;
                vertex.normal[1] = aiMeshPtr->mNormals[v].y;
                vertex.normal[2] = aiMeshPtr->mNormals[v].z;
            }

            // UV0
            if (aiMeshPtr->HasTextureCoords(0)) {
                vertex.uv0[0] = aiMeshPtr->mTextureCoords[0][v].x;
                vertex.uv0[1] = aiMeshPtr->mTextureCoords[0][v].y;
            }

            // UV1
            if (aiMeshPtr->HasTextureCoords(1)) {
                vertex.uv1[0] = aiMeshPtr->mTextureCoords[1][v].x;
                vertex.uv1[1] = aiMeshPtr->mTextureCoords[1][v].y;
            } else {
                // Fallback replicate UV0
                vertex.uv1[0] = vertex.uv0[0];
                vertex.uv1[1] = vertex.uv0[1];
            }

            // Color
            if (aiMeshPtr->HasVertexColors(0)) {
                const aiColor4D& col = aiMeshPtr->mColors[0][v];
                vertex.color[0] = col.r;
                vertex.color[1] = col.g;
                vertex.color[2] = col.b;
                vertex.color[3] = col.a;
            } else {
                // Default to white
                vertex.color[0] = vertex.color[1] = vertex.color[2] = 1.0f;
                vertex.color[3] = 1.0f;
            }

            // Tangent and bitangent
            if (aiMeshPtr->HasTangentsAndBitangents()) {
                vertex.tangent[0] = aiMeshPtr->mTangents[v].x;
                vertex.tangent[1] = aiMeshPtr->mTangents[v].y;
                vertex.tangent[2] = aiMeshPtr->mTangents[v].z;
                // Store handedness in w component of tangent
                // sign(dot(cross(normal, tangent), bitangent))
                aiVector3D cross = aiMeshPtr->mNormals[v] ^
                                    aiMeshPtr->mTangents[v];
                vertex.tangent[3] =
                    (cross * aiMeshPtr->mBitangents[v]) < 0.0f ? -1.0f : 1.0f;
            }

            out.vertices.push_back(vertex);
        }

        // Process indices for this mesh
        for (uint32_t f = 0; f < aiMeshPtr->mNumFaces; ++f) {
            const aiFace& face = aiMeshPtr->mFaces[f];
            if (face.mNumIndices == 3) { // Only support triangles
                out.indices.push_back(vertexOffset + face.mIndices[0]);
                out.indices.push_back(vertexOffset + face.mIndices[1]);
                out.indices.push_back(vertexOffset + face.mIndices[2]);
            }
        }

        vertexOffset += aiMeshPtr->mNumVertices;
        indexOffset += aiMeshPtr->mNumFaces * 3;
    }

    out.numSubMeshes = static_cast<uint8_t>(out.subMeshes.size());
    
    // create bgfx vertex layout
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, false, false)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float) // macro UV
        .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float) // detail UV
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Float)
        .end(); // stride = 72 bytes

    // create buffers
    const bgfx::Memory* mem =
        bgfx::alloc(uint32_t(out.vertices.size() * sizeof(Vertex)));
    memcpy(mem->data,
           out.vertices.data(),
           out.vertices.size() * sizeof(Vertex));
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(mem, layout);

    mem = bgfx::alloc(
        static_cast<uint32_t>(out.indices.size() * sizeof(uint32_t)));
    memcpy(mem->data,
           out.indices.data(),
           out.indices.size() * sizeof(uint32_t));
    bgfx::IndexBufferHandle ibh =
        bgfx::createIndexBuffer(mem, BGFX_BUFFER_INDEX32);

    // checks
    if (!bgfx::isValid(vbh) || !bgfx::isValid(ibh)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Failed to create vertex/index buffer");
        return false;
    }

    // and output
    out.vbh = vbh;
    out.ibh = ibh;

    return true;
}

// Reloads a model by its ID, returns true if successful
bool AssimpLoader::_ReloadModel(MeshData& out, int id) {
    MeshData* mesh = _GetMeshById(id);
    if (!mesh) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR, "Mesh with ID %d not found", id);
        return false;
    }
    mesh->valid = false; // Mark as invalid before reloading

    Assimp::Importer importer;
    const uint16_t   flags = aiProcess_JoinIdenticalVertices |
                           aiProcess_CalcTangentSpace |
                           aiProcess_GenSmoothNormals;
    const aiScene* scene = importer.ReadFile(mesh->path, flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->HasMeshes()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Error reloading model: %s",
                              importer.GetErrorString());
        return false;
    }

    MeshData temp;
    bool     hasTextures = false;
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        if (!mesh->materials[i].useVertexColor) {
            hasTextures = true;
            break;
        }
    }
    if (!processScene(temp, scene, mesh->path, hasTextures)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              "Failed to process scene for model: %s",
                              mesh->path.c_str());
        return false;
    }

    // Unload existing resources
    bgfx::destroy(mesh->vbh);
    bgfx::destroy(mesh->ibh);
    for (auto& mat : mesh->materials) {
        if (bgfx::isValid(mat.albedo)) {
            bgfx::destroy(mat.albedo);
        }
        if (bgfx::isValid(mat.normalMap)) {
            bgfx::destroy(mat.normalMap);
        }
        if (bgfx::isValid(mat.heightMap)) {
            bgfx::destroy(mat.heightMap);
        }
    }

    mesh->vertices     = std::move(temp.vertices);
    mesh->indices      = std::move(temp.indices);
    mesh->materials    = std::move(temp.materials);
    mesh->numMaterials = temp.numMaterials;
    mesh->vbh          = temp.vbh;
    mesh->ibh          = temp.ibh;
    mesh->id =
        static_cast<int>(loadedMeshes.size()); // Update ID to the new index

    try {
        mesh->lastWriteTime = std::filesystem::last_write_time(mesh->path);
    } catch (const std::filesystem::filesystem_error& e) {
        Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                              "Failed to get last write time for %s: %s",
                              mesh->path.c_str(),
                              e.what());
    }
    mesh->valid = true;

    Syngine::Logger::LogF(
        Syngine::LogLevel::INFO, "Reloaded mesh %s", mesh->path.c_str());
    out = *mesh; // Update output with reloaded data
    return true;
}

Material AssimpLoader::_ProcessMaterial(aiMaterial*        aiMat,
                                        const aiScene*     scene,
                                        const std::string& path,
                                        bool               loadTextures) {
    Material mat = {};
    mat.useVertexColor = false;

    // base color
    aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        mat.baseColor[0] = color.r;
        mat.baseColor[1] = color.g;
        mat.baseColor[2] = color.b;
        mat.baseColor[3] = color.a;
    }

    // Import or derive UV scales
    float scaleProperty = 1.0f;
    if (aiMat->Get("$uvScale", 0, 0 , scaleProperty) != AI_SUCCESS) {
        scaleProperty = 1.0f; // default if not specified
    }

    mat.uvScale[0] = scaleProperty;
    mat.uvScale[1] = scaleProperty;
    mat.uvScale[2] = scaleProperty;

    // Load textures if requested
    if (loadTextures) {
        const std::filesystem::path modelDir =
            std::filesystem::path(path).parent_path();

        aiString texPath;
        if (aiMat->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
            aiMat->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath);
            mat.albedo = _LoadAssimpTexture(scene, texPath, modelDir);
        } else if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
            mat.albedo = _LoadAssimpTexture(scene, texPath, modelDir);
        } else {
            mat.albedo = BGFX_INVALID_HANDLE;
        }

        // Normal map
        if (aiMat->GetTextureCount(aiTextureType_NORMALS) > 0) {
            aiString normalPath;
            aiMat->GetTexture(aiTextureType_NORMALS, 0, &normalPath);
            mat.normalMap = _LoadAssimpTexture(scene, normalPath, modelDir);
        } else {
            mat.normalMap = BGFX_INVALID_HANDLE;
        }

        // Height map - try to find a texture with _height suffix
        std::string heightPath =
            path.substr(0, path.find_last_of('.')) + "_height.png";
        if (std::filesystem::exists(heightPath)) {
            mat.heightMap = Syngine::LoadTextureFromFile(heightPath.c_str());
        } else {
            Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                                  "Height map not found for material %s",
                                  path.c_str());
            mat.heightMap = Syngine::CreateFlatTexture();
        }
    } else {
        mat.albedo = BGFX_INVALID_HANDLE;
        mat.normalMap = BGFX_INVALID_HANDLE;
        mat.heightMap = Syngine::CreateFlatTexture();
    }
    return mat;
}

Material AssimpLoader::_CreateDefaultMaterial() {
    Material mat;
    mat.baseColor[0] = mat.baseColor[1] = mat.baseColor[2] = 1.0f;
    mat.baseColor[3] = 1.0f;
    mat.albedo       = BGFX_INVALID_HANDLE;
    mat.normalMap    = BGFX_INVALID_HANDLE;
    mat.heightMap    = Syngine::CreateFlatTexture();
    mat.name         = "default_material";
    return mat;
}

} // namespace Syngine
