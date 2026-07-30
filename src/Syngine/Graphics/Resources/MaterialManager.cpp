// ╒════════════════════════════ MaterialManager.cpp ═╕
// │ Syngine                              │
// │ Created 2026-07-25                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Graphics/Resources/MaterialManager.h>
#include <Syngine/Utils/Serializer.h>
#include "SDL3/SDL_iostream.h"
#include "Syngine/Utils/FsUtils.h"

#include <miniscl.hpp>

#define SYNINT_MATERIAL_VERSION "1.0"

namespace Syngine {

Material MaterialManager::_DeserializeMaterial(scl::stream& xmlStream) {
    scl::xml::XmlDocument doc;
    scl::string           xmlStr;
    xmlStream.seek(scl::StreamPos::start, 0);
    xmlStr.clear();
    xmlStream >> xmlStr;
    if (!doc.load_string(xmlStr)) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR, true, "Failed to parse material XML");
        return Material("");
    }

    auto versionAttr = doc.find_attr(SYNINT_MATERIAL_VERSION);
    if (!versionAttr || versionAttr->data() != SYNINT_MATERIAL_VERSION) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR, true, "Material version mismatch");
        return Material("");
    }

    auto root = doc.find_children("Material").front();
    if (!root) {
        Syngine::Logger::LogF(
            Syngine::LogLevel::ERR, true, "Material XML missing root element");
        return Material("");
    }

    std::string matName = root->find_attr("name")
                              ? root->find_attr("name")->data().cstr()
                              : "unknown material";
    Material    mat(matName);

    // Deserialize the default material
    auto texElem = root->find_children("Textures").front();
    if (texElem) {
        // TODO: Implement this
    }

    auto paramElem = root->find_children("Parameters").front();
    if (paramElem) {
        for (auto* param : paramElem->find_children("Param")) {
            auto nameAttr = param->find_attr("name");
            auto typeAttr = param->find_attr("type");
            auto numAttr  = param->find_attr("num");
            auto value    = param->find_attr("value");

            if (!nameAttr || !typeAttr || !numAttr) {
                Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                      true,
                                      "Default material parameter missing "
                                      "required attributes in %s",
                                      matName.c_str());
                continue;
            }

            std::string uniformName = nameAttr->data().cstr();
            std::string paramType   = typeAttr->data().cstr();
            uint16_t    paramNum =
                static_cast<uint16_t>(std::stoi(numAttr->data().cstr()));

            if (value) {
                if (paramType == "vec4") {
                    scl::string        valueStr = value->data();
                    std::vector<float> components =
                        Serializer::_ParseFloatArray(valueStr);

                    if (components.size() == 4) {
                        mat.Set(uniformName,
                                components.data(),
                                sizeof(float) * components.size());
                    } else {
                        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                              true,
                                              "Default material parameter '%s' "
                                              "has invalid vec4 value in %s",
                                              uniformName.c_str(),
                                              matName.c_str());
                    }
                } else {
                    float fvalue = std::stof(value->data().cstr());
                    mat.Set(uniformName, &fvalue, sizeof(fvalue));
                }
            } else {
                // For vec4s, parse its child components as individual
                // components
                if (paramType == "vec4") {
                    Math::Vector4 vec4Value(0.0f, 0.0f, 0.0f, 0.0f);
                    for (auto* compElem : param->find_children("Component")) {
                        auto compNameAttr  = compElem->find_attr("name");
                        auto compValueAttr = compElem->find_attr("value");
                        if (compNameAttr && compValueAttr) {
                            scl::string compName = compNameAttr->data();
                            float       compValue =
                                std::stof(compValueAttr->data().cstr());
                            if (compName == "x")
                                vec4Value.setX(compValue);
                            else if (compName == "y")
                                vec4Value.setY(compValue);
                            else if (compName == "z")
                                vec4Value.setZ(compValue);
                            else if (compName == "w")
                                vec4Value.setW(compValue);
                        }
                    }

                    mat.Set(
                        uniformName, vec4Value.data(), sizeof(Math::Vector4));
                } else {
                    Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                                          true,
                                          "Default material parameter '%s' has "
                                          "unsupported type '%s' in %s",
                                          uniformName.c_str(),
                                          paramType.c_str(),
                                          matName.c_str());
                }
            }
        }
    }

    return mat;
}

Material
MaterialManager::GetMaterialFromBundle(const std::string& bundlePath,
                                       const std::string& materialName) {
    scl::stream xmlStream =
        Serializer::_ReadFromBundle(bundlePath, materialName);

    return _DeserializeMaterial(xmlStream);
}

Material MaterialManager::GetMaterialFromFile(const std::string& filePath) {
    std::string filePathStr(_ResolveOSPath(filePath));

    SDL_IOStream* rw = SDL_IOFromFile(filePathStr.c_str(), "rb");
    if (!rw) {
        Syngine::Logger::LogF(Syngine::LogLevel::ERR,
                              true,
                              "Failed to open material file %s",
                              filePathStr.c_str());
        return Material("");
    }

    // Not the biggest fan of loading into a vector and then copying into a
    // stream, but I'm not sure if scl::stream has a way to read directly from
    // an SDL_IOStream. If it does, we can change this later.
    Sint64               size = SDL_GetIOSize(rw);
    std::vector<uint8_t> data(size);
    SDL_ReadIO(rw, data.data(), size);
    SDL_CloseIO(rw);

    scl::stream xmlStream;
    xmlStream.write(data.data(), static_cast<size_t>(size));
    xmlStream.seek(scl::StreamPos::start, 0);

    return _DeserializeMaterial(xmlStream);
}

Material MaterialManager::GetDefaultMaterialPBR(bool textured) {
    std::string name = textured ? "default_texture" : "default";
    Material    mat(name, ShaderManager::Get(name));
    mat.Set("u_materialParams1",
            Math::Vector4(0.0f, 0.2f, 0.0f, 0.0f).data(),
            sizeof(Math::Vector4));
    mat.Set("u_uvScale",
            Math::Vector4(1.0f, 1.0f, 1.0f, 0.0f).data(),
            sizeof(Math::Vector4));
    mat.Set("u_baseColor",
            Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f).data(),
            sizeof(Math::Vector4));
    return mat;
}

} // namespace Syngine
