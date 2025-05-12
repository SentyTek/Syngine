#include "SynModelLoader.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "bgfx/bgfx.h"
#include "bx/math.h"
#include "defines.h"
#include "helpers.h"
#include <cstdint>
#include <time.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

//Returns true if the model was loaded successfully, false otherwise
bool AssimpLoader::LoadModel(SynMeshData& out, const std::string& path, bool loadTextures) {
    Assimp::Importer importer;

    const uint16_t flags    = aiProcess_JoinIdenticalVertices
                            | aiProcess_CalcTangentSpace
                            | aiProcess_GenSmoothNormals;

    //read file. ideally use some kind of post processing (tangents, join indices, etc), but this is a simple example
    const aiScene* scene = importer.ReadFile(path, flags);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->HasMeshes()) {
        SDL_Log("Error loading model: %s", importer.GetErrorString());
        return false;
    }

    if(scene->mNumMeshes == 0) {
        SDL_Log("No meshes found in model: %s", path.c_str());
        return false;
    }

    SynMeshData meshData;
    meshData.numVertices = scene->mNumMeshes > 0 ? scene->mMeshes[0]->mNumVertices : 0;

    aiMesh* mesh = scene->mMeshes[0]; //only loads first mesh for now

    meshData.vertices.reserve(mesh->mNumVertices);
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.pos[0] = mesh->mVertices[i].x;
        vertex.pos[1] = mesh->mVertices[i].y;
        vertex.pos[2] = mesh->mVertices[i].z;

        if(mesh->HasNormals()) {
            vertex.normal[0] = mesh->mNormals[i].x;
            vertex.normal[1] = mesh->mNormals[i].y;
            vertex.normal[2] = mesh->mNormals[i].z;
        }

        if(mesh->HasTextureCoords(0)) {
            vertex.uv[0] = mesh->mTextureCoords[0][i].x;
            vertex.uv[1] = mesh->mTextureCoords[0][i].y;
        }

        if(mesh->HasTangentsAndBitangents()) {
            vertex.tangent[0] = mesh->mTangents[i].x;
            vertex.tangent[1] = mesh->mTangents[i].y;
            vertex.tangent[2] = mesh->mTangents[i].z;
            vertex.tangent[3] = mesh->mBitangents[i].x * 
                                (bx::dot(bx::Vec3{mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z}, 
                                         bx::Vec3{mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z})
                                > 0.0f ? 1.0f : -1.0f);
        }

        meshData.vertices.push_back(vertex);
    }

    //prepare index buffer
    meshData.indices.reserve(mesh->mNumFaces * 3);
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        if(face.mNumIndices == 3) {
            meshData.indices.push_back(face.mIndices[0]);
            meshData.indices.push_back(face.mIndices[1]);
            meshData.indices.push_back(face.mIndices[2]);
        } else {
            SDL_Log("Face %d has %d indices, not 3", i, face.mNumIndices);
        }
    }

    //create bgfx vertex layout
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, false, false)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Float)
        .end(); //stride = 48 bytes

    //create buffers
    const bgfx::Memory* mem = bgfx::alloc(meshData.vertices.size() * sizeof(Vertex));
    memcpy(mem->data, meshData.vertices.data(), meshData.vertices.size() * sizeof(Vertex));
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(mem, layout);

    mem = bgfx::alloc(meshData.indices.size() * sizeof(uint32_t));
    memcpy(mem->data, meshData.indices.data(), meshData.indices.size() * sizeof(uint32_t));
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(mem, BGFX_BUFFER_INDEX32);

    meshData.vertices.clear(); //clear vertices and indices from meshData bc its on the GPU now
    meshData.indices.clear(); //clear indices from meshData bc its on the GPU now

    //checks
    if(!bgfx::isValid(vbh) || !bgfx::isValid(ibh)) {
        SDL_Log("Failed to create vertex/index buffer");
        return false;
    }

    if(loadTextures) {
        //load materials
        meshData.materials.reserve(scene->mNumMaterials);
        for(unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* material = scene->mMaterials[i];
            Material mat;

            aiString texPath;
            if(material->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == AI_SUCCESS) {
                const aiTexture* tex = scene->GetEmbeddedTexture(texPath.C_Str());
                if(tex->mHeight == 0) {
                    //compressed texture
                    bgfx::TextureHandle texHandle = SynLoadTextureFromMemory(
                        (const uint8_t*)tex->pcData,
                        tex->mWidth,
                        texPath.C_Str() //for debugging or caching
                    );
                    if(!bgfx::isValid(texHandle)) {
                        SDL_Log("Failed to load texture %s", texPath.C_Str());
                        return false;
                    }
                    mat.baseColor = texHandle;
                } else {
                    //uncompressed
                }
            } else {
                mat.baseColor = BGFX_INVALID_HANDLE;
            }

            aiString normPath;
            if(material->GetTexture(aiTextureType_NORMALS, 0, &normPath) == AI_SUCCESS) {
                const aiTexture* normTex = scene->GetEmbeddedTexture(normPath.C_Str());
                if(normTex->mHeight == 0) {
                    //compressed texture
                    bgfx::TextureHandle texHandle = SynLoadTextureFromMemory(
                        (const uint8_t*)normTex->pcData,
                        normTex->mWidth,
                        normPath.C_Str() //for debugging or caching
                    );
                    if(!bgfx::isValid(texHandle)) {
                        SDL_Log("Failed to load normal map %s", normPath.C_Str());
                        return false;
                    }
                    mat.normalMap = texHandle;
                } else {
                    //uncompressed
                }
            } else {
                mat.normalMap = BGFX_INVALID_HANDLE;
            }
            
            aiString heightPath;
            if(material->GetTexture(aiTextureType_HEIGHT, 0, &heightPath) == AI_SUCCESS) {
                const aiTexture* heightTex = scene->GetEmbeddedTexture(heightPath.C_Str());
                if(heightTex->mHeight == 0) {
                    //compressed texture
                    bgfx::TextureHandle texHandle = SynLoadTextureFromMemory(
                        (const uint8_t*)heightTex->pcData,
                        heightTex->mWidth,
                        heightPath.C_Str() //for debugging or caching
                    );
                    if(!bgfx::isValid(texHandle)) {
                        SDL_Log("Failed to load height map %s", heightPath.C_Str());
                        return false;
                    }
                    mat.heightMap = texHandle;
                } else {
                    //uncompressed
                }
            } else {
                mat.heightMap = BGFX_INVALID_HANDLE;
            }
            mat.name = "material_" + std::to_string(i);
            meshData.materials.push_back(mat);
        }
    }

    //and output
    meshData.vbh = vbh;
    meshData.ibh = ibh;
    out = meshData;
    meshes.push_back(meshData); //store mesh data for later use
    return true;
}

void AssimpLoader::UnloadAll() {
    for (auto& mesh : meshes) {
        bgfx::destroy(mesh.vbh);
        bgfx::destroy(mesh.ibh);
        mesh.vertices.clear();
        mesh.indices.clear();
    }
    meshes.clear();
}

std::vector<SynMeshData>& SynModelLoader::getMeshes() {
    return meshes;
}
