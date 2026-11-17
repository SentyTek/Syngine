// ╒═════════════════ SerializeScene.cpp ═╕
// │ Syngine                              │
// │ Created 2026-11-07                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Logger.h"
#include "Syngine/Scene/GameObjectRegistry.h"
#include <Syngine/Utils/Serializer.h>
#include <Syngine/Utils/UUID.h>
#include <Syngine/Core/Core.h>
#include <Syngine/Utils/FsUtils.h>

#include <string_view>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace Syngine {
namespace {
using Node = Serializer::DataNode;

void _WriteScene(scl::xml::XmlDocument& doc, const Node& node) {
    auto uuidElem = doc.new_elem("uuid");
    uuidElem->set_data(node["uuid"].As<Syngine::UUID>().ToString());
    auto persistentElem = doc.new_elem("persistent");
    persistentElem->set_data(node["persistent"].As<bool>() ? "true" : "false");
    doc.add_child(uuidElem);
    doc.add_child(persistentElem);

    // recursively write child nodes
    auto gameObjectsElem = doc.new_elem("gameObjects");
    doc.add_child(gameObjectsElem);
    for (const auto& child : node["gameObjects"].As<std::vector<Node>>()) {
        scl::xml::XmlElem* goElem = doc.new_elem("gameObject");
        Serializer::Prefab::WriteGameObject(child, doc, goElem);
        gameObjectsElem->add_child(goElem);
    }

    // TODO: implement this fr
    /*auto sceneSettingsElem = doc.new_elem("sceneSettings");
    doc.add_child(sceneSettingsElem);
    for (const auto& child : node["sceneSettings"].As<std::vector<Node>>()) {
        Serializer::Prefab::WriteGameObject(child, doc, sceneSettingsElem);
    }*/
}

std::string _Attribute(const scl::xml::XmlElem& elem, const char* name) {
    const auto* attribute = elem.find_attribute(name);
    if (!attribute) {
        Logger::Error(std::string("Missing attribute: ") + name);
        return "";
    }

    const scl::string value = attribute->data();
    return value.cstr() ? value.cstr() : "";
}

Node _ReadScene(const scl::xml::XmlDocument& elem) {
    if (!elem.find_children("uuid").empty() &&
        !elem.find_children("persistent").empty()) {
        Node node;
        node["uuid"] = elem.find_children("uuid").front()->data().cstr();
        node["persistent"] =
            elem.find_children("persistent").front()->data().cstr();

        auto gameObjectsChildren = elem.find_children("gameObjects");
        if (!gameObjectsChildren.empty()) {
            std::vector<Node> gameObjects;
            for (const auto& go : gameObjectsChildren.front()->children()) {
                Node gameObject;
                Serializer::Prefab::Deserialize(go, gameObject);
                gameObjects.push_back(gameObject);
            }
            node["gameObjects"] = gameObjects;
        }

        // TODO: Implement reading sceneSettings

        return node;
    }
    Logger::Error("Invalid scene XML structure");
    return Node();
}
} // namespace

Serializer::DataNode Serializer::Scene::Serialize() const {
    DataNode node;
    node["uuid"]        = uuid;
    node["persistent"]  = persistent;
    node["gameObjects"] = gameObjects;
    // node["sceneSettings"] = sceneSettings; //TODO: Implement sceneSettings

    return node;
}

void Serializer::Scene::Deserialize(const DataNode& node) {
    // what a terrible conditional
    if (!node.IsObject() || !node.Has("uuid") || !node.Has("persistent") ||
        !node.Has("gameObjects") ||
        (node["uuid"].GetType() != DataNode::Type::String &&
         !(node["uuid"].IsArray() && node["uuid"].Size() == 1 &&
           node["uuid"].At(0).GetType() == DataNode::Type::String)) ||
        (node["uuid"].GetType() != DataNode::Type::String &&
         !(node["uuid"].IsArray() && node["uuid"].Size() == 1 &&
           node["uuid"].At(0).GetType() == DataNode::Type::String)) ||
        (node["persistent"].GetType() != DataNode::Type::String &&
         node["persistent"].GetType() != DataNode::Type::Boolean) ||
        !node["gameObjects"].IsArray()) {
        Logger::Error("Invalid scene data");
        return;
    }

    Scene parsed;
    parsed.uuid = node["uuid"].As<Syngine::UUID>();
    if (node["persistent"].GetType() == DataNode::Type::String) //ugh
        parsed.persistent = node["persistent"].As<std::string>() == "true";
    else
        parsed.persistent  = node["persistent"].As<bool>();
    parsed.gameObjects = node["gameObjects"];
    // parsed.sceneSettings = node["sceneSettings"]; // TODO: Implement
    // sceneSettings
    parsed.isValid = true;
    *this          = parsed;
}

bool Serializer::Scene::SaveToFile(const std::string&   path,
                                   const SceneInstance& sceneInstance) {
    try {
        const Scene scene = _FromSceneInstance(sceneInstance);

        if (!scene.isValid) {
            Logger::Error("Scene is not valid");
            return false;
        }

        if (!scene.gameObjects || !scene.gameObjects.IsArray() ||
            scene.gameObjects.Size() == 0) {
            Logger::Error("Scene gameObjects array invalid");
            return false;
        }

        // Resolve path, delete existing file if it exists, and create new XML
        // document
        std::string programName = Core::Get()
                                      ? Core::Get()->m_context->config.gameName
                                      : "UnknownGame";
        std::string adp        = (_GetAppDataPath(programName) / path).string();
        scl::path   outputPath = adp.c_str();
        scl::xml::XmlDocument doc;

        // Base of the file is Scene root with attributes for version and name.
        scl::xml::XmlAttr* v = doc.new_attr("Version", SYNINT_SCENE_VERSION);
        scl::xml::XmlAttr* n = doc.new_attr(
            "Name", !scene.name.empty() ? scene.name : "UnnamedScene");
        doc.set_tag("Scene");
        doc.add_attr(v);
        doc.add_attr(n);

        _WriteScene(doc, scene.Serialize());

        scl::stream file;
        file.open(outputPath, scl::OpenMode::RWTRUNC);
        doc.print(file);
        file.close();
        Logger::LogF(LogLevel::INFO,
                     true,
                     "Saved scene \"%s\" to %s",
                     scene.name.c_str(),
                     outputPath.cstr());

    } catch (const std::exception& e) {
        Logger::Error("Failed to save scene '" + path + "': " + e.what());
        return false;
    }
    return true;
}

Serializer::Scene Serializer::Scene::LoadFromFile(const std::string& path,
                                                  const std::string& bundle) {
    try {
        scl::stream stream;
        if (bundle.empty()) {
            scl::path appdataPath(
                (_GetAppDataPath(Core::Get()->m_context->config.gameName) /
                 path)
                    .string());
            stream.open(appdataPath, scl::OpenMode::READ);
        } else {
            stream = Serializer::_ReadFromBundle(bundle, path);
        }
        scl::string sceneData;
        stream >> sceneData;

        scl::xml::XmlDocument document;
        if (!document.load_string(sceneData)) {
            Logger::Error("Failed to load scene file: " + path);
            return {};
        }

        if (document.tag() != "Scene" ||
            _Attribute(document, "Version") != SYNINT_SCENE_VERSION) {
            Logger::Error("Unsupported scene format or version "
                          "(Expected " SYNINT_SCENE_VERSION ", got " +
                          _Attribute(document, "Version") + ")");
            return {};
        }

        const auto children = document.children();
        if (children.size() != 3) {
            Logger::Error("Scene XML does not have expected values");
            return {};
        }

        Scene result;
        result.Deserialize(_ReadScene(document));
        result.name = _Attribute(document, "Name");

        return result;
    } catch (const std::exception& e) {
        Logger::Error("Failed to load scene '" + path + "': " + e.what());
        return {};
    }
}

const Serializer::Scene
Serializer::Scene::_FromSceneInstance(const SceneInstance& sceneInstance) {
    Scene scene;

    scene.name = !sceneInstance.GetName().empty() ? sceneInstance.GetName()
                                                  : "UnnamedScene";
    scene.uuid = sceneInstance.GetUUID();
    scene.persistent = sceneInstance.IsPersistent();
    scene.isValid    = true;
    // TODO: Implement scene settings retrieval from SceneInstance

    for (const auto& id : sceneInstance.GetGameObjects()) {
        auto go = GameObjectRegistry::GetGameObjectByID(id);
        if (go) {
            auto serializedGameObject = go->Serialize();
            scene.gameObjects.Append(serializedGameObject);
        }
    }

    return scene;
}

} // namespace Syngine
