#include "SynModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>

//Returns true if the model was loaded successfully, false otherwise
bool AssimpLoader::LoadModel(const std::string& path, SynMeshData& out) {
    Assimp::Importer importer;

    //read file. ideally use some kind of post processing (tangents, join indices, etc), but this is a simple example
    const aiScene* scene = importer.ReadFile(path, 0);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->HasMeshes()) {
        SDL_Log("Error loading model: %s", importer.GetErrorString());
        return false;
    }

    if(scene->mNumMeshes == 0) {
        SDL_Log("No meshes found in model: %s", path.c_str());
        return false;
    }

    SynMeshData meshData;
    meshData.numVertices = scene->mNumMeshes;

    aiMesh* mesh = scene->mMeshes[0]; //only loads first mesh for now
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

        meshData.vertices.push_back(vertex);
    }

    //prepare index buffer
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        if(face.mNumIndices == 3) {
            meshData.indices.push_back(face.mIndices[0]);
            meshData.indices.push_back(face.mIndices[1]);
            meshData.indices.push_back(face.mIndices[2]);
        }
    }

    //create bgfx vertex layout
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    //create buffers
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(meshData.vertices.data(), sizeof(Vertex) * meshData.vertices.size()),
        layout
    );
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(meshData.indices.data(), sizeof(uint16_t) * meshData.indices.size())
    );

    //and output
    meshData.vbh = vbh;
    meshData.ibh = ibh;
    meshData.material.name = "default"; //set default material name
    meshData.material.texturePath = ""; //set default texture path
    out = meshData;
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
