// ╒════════════════════ ModelLoader.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-05                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Core.h"
#include <Syngine/Graphics/Resources/ModelLoader.h>
#include <Syngine/Graphics/Resources/ShaderManager.h>
#include <Syngine/Graphics/Resources/TextureHelpers.h>
#include <Syngine/Graphics/Resources/MaterialManager.h>
#include <Syngine/Core/Logger.h>
#include <Syngine/Core/JobSystem.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <miniscl.hpp>

#include <algorithm>
#include <cstdint>
#include <optional>

namespace Syngine {

namespace {

static inline aiVector3D _MirrorX(const aiVector3D& v) {
    return aiVector3D(-v.x, v.y, v.z);
}

static inline aiVector3D _Normalized(const aiVector3D& v) {
    aiVector3D out = v;
    out.Normalize();
    return out;
}

static inline Math::Vector3 _ToVector3(const aiVector3D& v) {
    return Math::Vector3(v.x, v.y, v.z);
}

static inline Math::Vector4 _ToVector4(const aiColor4D& c) {
    return Math::Vector4(c.r, c.g, c.b, c.a);
}

std::string _GetAssimpFormatHint(const std::string& assetPath) {
    const size_t dotPos = assetPath.find_last_of('.');
    if (dotPos == std::string::npos || dotPos + 1 >= assetPath.size()) {
        return std::string();
    }

    // Get the file extension and convert to lowercase
    std::string ext = assetPath.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    // Assimp registers legacy glTF1 before glTF2 for .gltf/.glb. When loading
    // from memory with a hint, this can hit a first-chance throw in glTF1 on
    // valid GLB2 files before glTF2 gets a chance. Route these through the
    // glTF2 importer path explicitly.
    if (ext == "glb" || ext == "gltf") {
        return "vrm";
    }

    return ext;
}

struct DecodedAssimpTexture {
    ModelData::DeferredTextureData::PayloadType payloadType =
        ModelData::DeferredTextureData::PayloadType::Encoded;
    std::vector<uint8_t> payload;
    uint16_t             width  = 0;
    uint16_t             height = 0;
    std::string          debugName;
};

std::optional<DecodedAssimpTexture>
_DecodeAssimpTexturePayload(const aiScene* scene, const aiString& texPath) {
    if (texPath.length == 0) {
        return std::nullopt;
    }

    if (scene) {
        if (const aiTexture* embedded =
                scene->GetEmbeddedTexture(texPath.C_Str())) {
            if (embedded->mHeight == 0) {
                DecodedAssimpTexture decoded;
                decoded.payloadType =
                    ModelData::DeferredTextureData::PayloadType::Encoded;
                decoded.payload.resize(embedded->mWidth);
                std::memcpy(
                    decoded.payload.data(), embedded->pcData, embedded->mWidth);
                decoded.debugName = texPath.C_Str();
                return decoded;
            }

            // Assimp stores raw textures as aiTexel (BGRA8); convert to RGBA8.
            const uint32_t pixelCount = embedded->mWidth * embedded->mHeight;
            DecodedAssimpTexture decoded;
            decoded.payloadType =
                ModelData::DeferredTextureData::PayloadType::RGBA8;
            decoded.payload.resize(pixelCount * 4);
            decoded.width     = static_cast<uint16_t>(embedded->mWidth);
            decoded.height    = static_cast<uint16_t>(embedded->mHeight);
            decoded.debugName = texPath.C_Str();
            for (uint32_t i = 0; i < pixelCount; ++i) {
                const aiTexel& src         = embedded->pcData[i];
                decoded.payload[i * 4 + 0] = src.r;
                decoded.payload[i * 4 + 1] = src.g;
                decoded.payload[i * 4 + 2] = src.b;
                decoded.payload[i * 4 + 3] = src.a;
            }

            return decoded;
        }
    }

    // Textures should always be in the glb files.
    return std::nullopt;
}

bgfx::TextureHandle
_CreateDeferredTexture(const ModelData::DeferredTextureData& deferred) {
    if (deferred.payload.empty()) {
        return BGFX_INVALID_HANDLE;
    }

    if (deferred.payloadType ==
        ModelData::DeferredTextureData::PayloadType::RGBA8) {
        if (deferred.width == 0 || deferred.height == 0) {
            return BGFX_INVALID_HANDLE;
        }
        return bgfx::createTexture2D(
            deferred.width,
            deferred.height,
            false,
            1,
            bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE,
            bgfx::copy(deferred.payload.data(),
                       static_cast<uint32_t>(deferred.payload.size())));
    }

    return Syngine::LoadTextureFromMemory(deferred.payload.data(),
                                          deferred.payload.size(),
                                          deferred.debugName.c_str());
}

} // namespace

/* Base class */

void ModelLoader::CreateBGFXResources(ModelData& out) {
    if (out.vertices.empty() || out.indices.empty()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              false,
                              "Cannot create BGFX resources for empty mesh");
        return;
    }

    // Create vertex layout
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, false, false)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float) // macro UV
        .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float) // detail UV
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Float)
        .end(); // stride = 72 bytes

    // Create vertex buffer
    const bgfx::Memory* mem = bgfx::alloc(
        static_cast<uint32_t>(out.vertices.size() * sizeof(Vertex)));
    memcpy(mem->data,
           out.vertices.data(),
           static_cast<uint32_t>(out.vertices.size() * sizeof(Vertex)));
    out.vbh = bgfx::createVertexBuffer(mem, layout);

    // Create index buffer
    mem = bgfx::alloc(static_cast<uint32_t>(out.indices.size()) *
                      sizeof(uint32_t));
    memcpy(mem->data,
           out.indices.data(),
           static_cast<uint32_t>(out.indices.size()) * sizeof(uint32_t));
    out.ibh = bgfx::createIndexBuffer(mem, BGFX_BUFFER_INDEX32);

    if (!bgfx::isValid(out.vbh) || !bgfx::isValid(out.ibh)) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "Failed to create vertex/index buffer");
        return;
    }

    for (const auto& deferred : out.deferredTextures) {
        const bgfx::TextureHandle handle = _CreateDeferredTexture(deferred);
        if (!bgfx::isValid(handle)) {
            Syngine::Logger::LogF(Syngine::LogLevel::WARN,
                                  true,
                                  "Failed to create deferred texture for %s",
                                  deferred.paramName.c_str());
            continue;
        }

        if (deferred.materialIndex >= out.materials.size()) {
            Syngine::Logger::LogF(
                Syngine::LogLevel::WARN,
                true,
                "Deferred texture material index out of range: %u",
                deferred.materialIndex);
            bgfx::destroy(handle);
            continue;
        }

        out.materials[deferred.materialIndex].SetTexture(
            deferred.paramName, handle, deferred.samplerFlags, deferred.stage);
    }

    out.deferredTextures.clear();

    out.valid = true; // Mark as valid after creating BGFX resources
}

/* Assimp importer */

// Returns true if the model was loaded successfully, false otherwise
bool AssimpLoader::_LoadModel(ModelData&         out,
                              scl::stream*       stream,
                              const std::string& assetPath,
                              bool               loadTextures) {
    Assimp::Importer importer;

    const int flags = aiProcess_JoinIdenticalVertices |
                      aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
                      aiProcess_DropNormals;

    const std::string formatHint = _GetAssimpFormatHint(assetPath);

    // read file. ideally use some kind of post processing (tangents, join
    // indices, etc), but this is a simple example
    const aiScene* scene = importer.ReadFileFromMemory(
        stream->data(), stream->size(), flags, formatHint.c_str());
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->HasMeshes()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "Error loading model: %s",
                              importer.GetErrorString());
        return false;
    }

    if (scene->mNumMeshes == 0) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR, true, "No meshes found in model.");
        return false;
    }

    ModelData modelData;
    if (!processScene(modelData, scene, stream, loadTextures)) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR, true, "Failed to process model scene.");
        return false;
    }

    Syngine::Logger::Info("Loaded mesh", true);
    out = modelData;
    return true;
}

bool AssimpLoader::processScene(ModelData&     out,
                                const aiScene* scene,
                                scl::stream*   meshStream,
                                bool           loadTextures) {
    out.vertices.clear();
    out.indices.clear();
    out.subMeshes.clear();
    out.deferredTextures.clear();
    out.valid = false;

    std::vector<MaterialInstance> allMaterials;
    {
        // Process all aiScene materials
        for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial*      aiMat = scene->mMaterials[i];
            MaterialInstance mat   = _ProcessMaterial(
                aiMat, scene, meshStream, out, i, loadTextures);
            allMaterials.push_back(mat);
        }

        // Ensure at least one fallback material
        if (allMaterials.empty()) {
            allMaterials.push_back(
                MaterialManager::GetDefaultMaterialPBR().CreateInstance());
        }

        out.materials    = allMaterials;
        out.numMaterials = static_cast<uint8_t>(allMaterials.size());
    }

    // Process all meshes in scene
    std::vector<TempProcessedMesh> tempMeshes(scene->mNumMeshes);
    Jobs().ParallelFor(scene->mNumMeshes, [&](size_t begin, size_t end) {
        for (uint32_t meshIdx = static_cast<uint32_t>(begin);
             meshIdx < static_cast<uint32_t>(end);
             ++meshIdx) {
            aiMesh*            aiMeshPtr = scene->mMeshes[meshIdx];
            TempProcessedMesh& tempMesh  = tempMeshes[meshIdx];

            // Create submesh record for this Assimp mesh
            SubMesh subMesh;
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
                    min.x                  = std::min(min.x, vert.x);
                    min.y                  = std::min(min.y, vert.y);
                    min.z                  = std::min(min.z, vert.z);
                    max.x                  = std::max(max.x, vert.x);
                    max.y                  = std::max(max.y, vert.y);
                    max.z                  = std::max(max.z, vert.z);
                }
                subMesh.boundMin.setX(min.x);
                subMesh.boundMin.setY(min.y);
                subMesh.boundMin.setZ(min.z);
                subMesh.boundMax.setX(max.x);
                subMesh.boundMax.setY(max.y);
                subMesh.boundMax.setZ(max.z);
            } else {
                // No vertices, set empty bounds
                subMesh.boundMin = Math::Vector3(); // No arg zeroes it out
                subMesh.boundMax = Math::Vector3();
            }
            tempMesh.subMesh = subMesh;

            // Process vertices for this mesh
            tempMesh.vertices.resize(aiMeshPtr->mNumVertices);
            for (size_t v = 0; v < aiMeshPtr->mNumVertices; v++) {
                Vertex vertex;

                const aiVector3D pos = _MirrorX(aiMeshPtr->mVertices[v]);

                // Position
                vertex.pos = _ToVector3(pos);

                // Normal
                if (aiMeshPtr->HasNormals()) {
                    const aiVector3D n = _Normalized(aiMeshPtr->mNormals[v]);
                    const aiVector3D nMirrored = _MirrorX(n);
                    vertex.normal              = _ToVector3(nMirrored);
                }

                // UV0
                if (aiMeshPtr->HasTextureCoords(0)) {
                    vertex.uv0.setX(aiMeshPtr->mTextureCoords[0][v].x);
                    vertex.uv0.setY(aiMeshPtr->mTextureCoords[0][v].y);
                }

                // UV1
                if (aiMeshPtr->HasTextureCoords(1)) {
                    vertex.uv1.setX(aiMeshPtr->mTextureCoords[1][v].x);
                    vertex.uv1.setY(aiMeshPtr->mTextureCoords[1][v].y);
                } else {
                    // Fallback replicate UV0
                    vertex.uv1 = vertex.uv0;
                }

                // Color
                if (aiMeshPtr->HasVertexColors(0)) {
                    const aiColor4D& col = aiMeshPtr->mColors[0][v];
                    vertex.color         = _ToVector4(col);
                } else {
                    // Default to white
                    vertex.color = Math::Vector4(1.0f);
                }

                // Tangent and bitangent
                if (aiMeshPtr->HasTangentsAndBitangents()) {
                    const aiVector3D t = _Normalized(aiMeshPtr->mTangents[v]);
                    const aiVector3D b = _Normalized(aiMeshPtr->mBitangents[v]);

                    const aiVector3D tMirrored = _MirrorX(t);
                    const aiVector3D bMirrored = _MirrorX(b);

                    // Store handedness in w component of tangent
                    // sign(dot(cross(normal, tangent), bitangent))
                    aiVector3D cross = aiVector3D(vertex.normal[0],
                                                  vertex.normal[1],
                                                  vertex.normal[2]) ^
                                       tMirrored;

                    vertex.tangent = Math::Vector4(
                        tMirrored.x,
                        tMirrored.y,
                        tMirrored.z,
                        (cross * bMirrored) < 0.0f ? -1.0f : 1.0f);
                }

                tempMesh.vertices[v] = vertex;
            }

            // Process indices for this mesh
            tempMesh.indices.resize(aiMeshPtr->mNumFaces *
                                    3); // assuming triangles
            for (size_t f = 0; f < aiMeshPtr->mNumFaces; ++f) {
                const aiFace& face = aiMeshPtr->mFaces[f];

                if (face.mNumIndices == 3) {
                    const size_t i = f * 3;

                    tempMesh.indices[i + 0] = face.mIndices[0];
                    tempMesh.indices[i + 1] = face.mIndices[2];
                    tempMesh.indices[i + 2] = face.mIndices[1];
                }
            }
        }
    });

    // Merge temp meshes into the output model data
    uint32_t vertexOffset = 0;
    uint32_t indexOffset  = 0;
    for (const auto& tempMesh : tempMeshes) {
        SubMesh subMesh    = tempMesh.subMesh;
        subMesh.indexStart = indexOffset;

        out.vertices.insert(out.vertices.end(),
                            tempMesh.vertices.begin(),
                            tempMesh.vertices.end());
        out.indices.reserve(out.indices.size() + tempMesh.indices.size());
        for (const auto& idx : tempMesh.indices) {
            out.indices.push_back(idx + vertexOffset);
        }
        out.subMeshes.push_back(subMesh);

        vertexOffset += static_cast<uint32_t>(tempMesh.vertices.size());
        indexOffset += static_cast<uint32_t>(tempMesh.indices.size());
    }

    Math::Vector3 minBounds(std::numeric_limits<float>::max());
    Math::Vector3 maxBounds(std::numeric_limits<float>::lowest());

    for (const auto& subMesh : out.subMeshes) {
        for (int i = 0; i < 3; ++i) {
            minBounds.set(i, std::min(minBounds[i], subMesh.boundMin[i]));
            maxBounds.set(i, std::max(maxBounds[i], subMesh.boundMax[i]));
        }
    }

    out.localMin = minBounds;
    out.localMax = maxBounds;

    out.numSubMeshes = static_cast<uint8_t>(out.subMeshes.size());

    return true;
}

// Reloads a model by its ID, returns true if successful
bool AssimpLoader::_ReloadModel(ModelData&         out,
                                scl::stream*       stream,
                                const std::string& assetPath,
                                int                id,
                                bool               loadTextures) {
    Assimp::Importer importer;
    const int        flags = aiProcess_JoinIdenticalVertices |
                      aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
                      aiProcess_DropNormals;
    const std::string formatHint = _GetAssimpFormatHint(assetPath);
    const aiScene*    scene      = importer.ReadFileFromMemory(
        stream->data(), stream->size(), flags, formatHint.c_str());
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->HasMeshes()) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "Error reloading model: %s",
                              importer.GetErrorString());
        return false;
    }

    ModelData temp;
    if (!processScene(temp, scene, stream, loadTextures)) {
        Syngine::Logger::Error("Failed to process model scene during reload.");
        return false;
    }

    temp.id = id;
    out     = std::move(temp);
    return true;
}

MaterialInstance AssimpLoader::_ProcessMaterial(aiMaterial*    aiMat,
                                                const aiScene* scene,
                                                scl::stream*   meshStream,
                                                ModelData&     out,
                                                uint32_t       materialIndex,
                                                bool           loadTextures) {
    (void)meshStream;

    // Start with default material
    // This loads in the base parameters used by most shaders.
    bool hasTex = (aiMat->GetTextureCount(aiTextureType_BASE_COLOR) > 0 ||
                   aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0);
    MaterialInstance mat =
        MaterialManager::GetDefaultMaterialPBR().CreateInstance();

    // base color
    aiColor4D baseColor(1.0f, 1.0f, 1.0f, 1.0f);
    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
        mat.Set(
            "u_baseColor", _ToVector4(baseColor).data(), sizeof(Math::Vector4));
    }

    const float   useTexture = (loadTextures && hasTex) ? 1.0f : 0.0f;
    Math::Vector4 materialFlags(useTexture, 0.0f, 0.0f, 0.0f);
    mat.Set("u_materialParams2", materialFlags.data(), sizeof(Math::Vector4));

    // Import or derive UV scales
    float scaleProperty = 1.0f;
    if (aiMat->Get("$uvScale", 0, 0, scaleProperty) != AI_SUCCESS) {
        scaleProperty = 1.0f; // default if not specified
    }

    mat.Set("u_uvScale", &scaleProperty, sizeof(float));

    // Load textures if requested
    if (loadTextures) {
        auto queueTexture = [&](aiTextureType      texType,
                                const std::string& paramName,
                                uint32_t           samplerFlags,
                                uint8_t            stage) {
            if (aiMat->GetTextureCount(texType) == 0) {
                return false;
            }

            aiString texPath;
            aiMat->GetTexture(texType, 0, &texPath);
            const auto decoded = _DecodeAssimpTexturePayload(scene, texPath);
            if (!decoded.has_value()) {
                return false;
            }

            ModelData::DeferredTextureData deferred;
            deferred.materialIndex = materialIndex;
            deferred.paramName     = paramName;
            deferred.samplerFlags  = samplerFlags;
            deferred.stage         = stage;
            deferred.payloadType   = decoded->payloadType;
            deferred.payload       = decoded->payload;
            deferred.width         = decoded->width;
            deferred.height        = decoded->height;
            deferred.debugName     = decoded->debugName;
            out.deferredTextures.push_back(std::move(deferred));
            return true;
        };

        if (aiMat->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
            queueTexture(aiTextureType_BASE_COLOR, "s_albedo", 0, 0);
        } else if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            queueTexture(aiTextureType_DIFFUSE, "s_albedo", 0, 0);
        }

        // Normal map
        if (aiMat->GetTextureCount(aiTextureType_NORMALS) > 0) {
            queueTexture(aiTextureType_NORMALS, "s_normalMap", 0, 2);
        }

        // Height map - try to find a texture with _height suffix
        /*std::string heightPath =
            path.substr(0, path.find_last_of('.')) + "_height.png";
        if (std::filesystem::exists(heightPath)) {
            mat.heightMap = Syngine::LoadTextureFromFile(heightPath.c_str());
        } else {
            Syngine::Logger::LogF(Syngine::LogLevel::WARN, true,
                                  "Height map not found for material %s",
                                  path.c_str());
            mat.heightMap = Syngine::CreateFlatTexture();
        }*/

        // HEIGHT MAPS ARE NO LONGER USED BY SHADERS
    }
    return mat;
}

} // namespace Syngine
