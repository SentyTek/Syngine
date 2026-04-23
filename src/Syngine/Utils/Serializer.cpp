// ╒═════════════════════ Serializer.cpp ═╕
// │ Syngine                              │
// │ Created 2026-01-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Utils/Serializer.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/ECS/GameObject.h"

#include "../../lib/miniscl.hpp"
#include <filesystem>
#include <string>
#include <variant>
#include <stdexcept>

namespace Syngine {
namespace Internal {
std::string ResolvePath(const char* path) { return _ResolveOSPath(path); }
} // namespace Internal

Serializer::CoreSettings Serializer::m_coreSettings = Serializer::CoreSettings{};

bool Serializer::DataNode::Has(const std::string& key) const {
    if (!std::holds_alternative<NodeMap>(m_data)) return false;
    const auto& map = std::get<NodeMap>(m_data);
    return map.find(key) != map.end();
}

Serializer::DataNode::Type Serializer::DataNode::GetType() const {
    if (std::holds_alternative<std::monostate>(m_data)) return Type::Null;
    if (std::holds_alternative<int>(m_data)) return Type::Integer;
    if (std::holds_alternative<uint64_t>(m_data)) return Type::UnsignedInteger;
    if (std::holds_alternative<float>(m_data)) return Type::Float;
    if (std::holds_alternative<bool>(m_data)) return Type::Boolean;
    if (std::holds_alternative<std::string>(m_data)) return Type::String;
    if (std::holds_alternative<NodeMap>(m_data)) return Type::Object;
    if (std::holds_alternative<NodeArray>(m_data)) return Type::Array;
    throw std::runtime_error("Unknown DataNode type");
}

void Serializer::DataNode::Append(DataNode& node) {
    if (!std::holds_alternative<NodeArray>(m_data)) {
        m_data = NodeArray{};
    }
    auto& arr = std::get<NodeArray>(m_data);
    arr.push_back(node);
}

bool Serializer::DataNode::IsObject() const {
    return std::holds_alternative<NodeMap>(m_data);
}

bool Serializer::DataNode::IsArray() const {
    return std::holds_alternative<NodeArray>(m_data);
}

bool Serializer::DataNode::IsNull() const {
    return std::holds_alternative<std::monostate>(m_data);
}

size_t Serializer::DataNode::Size() const {
    if (!IsArray()) return 0;
    return std::get<NodeArray>(m_data).size();
}

Serializer::DataNode& Serializer::DataNode::At(size_t index) {
    if (!IsArray()) {
        throw std::runtime_error("DataNode is not an array");
    }
    auto& arr = std::get<NodeArray>(m_data);
    if (index >= arr.size()) {
        throw std::out_of_range("Index out of range in DataNode array");
    }
    return arr[index];
}

Serializer::DataNode::NodeMap::const_iterator Serializer::DataNode::begin() const {
    static const NodeMap emptyMap{};
    if (!std::holds_alternative<NodeMap>(m_data)) {
        return emptyMap.begin();
    }
    return std::get<NodeMap>(m_data).begin();
}

Serializer::DataNode::NodeMap::const_iterator Serializer::DataNode::end() const {
    static const NodeMap emptyMap{};
    if (!std::holds_alternative<NodeMap>(m_data)) {
        return emptyMap.end();
    }
    return std::get<NodeMap>(m_data).end();
}

std::vector<std::string> Serializer::DataNode::GetKeys() const {
    std::vector<std::string> keys;
    if (!std::holds_alternative<NodeMap>(m_data)) return keys;
    const auto& map = std::get<NodeMap>(m_data);
    for (const auto& [key, _] : map) {
        keys.push_back(key);
    }
    return keys;
}

void Serializer::_LoadCoreSettings(const std::string& gameName) {
    std::filesystem::path settingsPath = Syngine::_GetAppDataPath(gameName) / "settings.xml";
    if (std::filesystem::exists(settingsPath)) {
        m_coreSettings = CoreSettings::LoadFromFile(settingsPath.string());
    } else {
        Logger::Info("No existing settings found, using defaults");
    }
}

void Serializer::_SaveCoreSettings(const std::string& gameName) {
    std::filesystem::path settingsPath = Syngine::_GetAppDataPath(gameName) / "settings.xml";
    if (!CoreSettings::SaveToFile(settingsPath.string(), m_coreSettings)) {
        Logger::Error("Failed to save settings to file: " + settingsPath.string());
    } else {
        Logger::Info("Settings saved successfully to " + settingsPath.string());
    }
}

Serializer::CoreSettings
Serializer::CoreSettings::LoadFromFile(const std::string& path) {
    scl::xml::XmlDocument doc;
    scl::path             inputPath = path.c_str();

    if (!doc.load_file(inputPath)) {
        Logger::Error("Failed to load settings file", false);
        return CoreSettings{};
    }

    auto* versionAttr = doc.find_attr("Version");
    if (!versionAttr) {
        Logger::Error("Settings missing Version attribute, cannot load", false);
        return CoreSettings{};
    }
    if (versionAttr->data() != SYNINT_CORESETTINGS_VERSION) {
        Logger::Error(&"Settings version mismatch (expected "[scl::string(
                          SYNINT_CORESETTINGS_VERSION)] +
                          scl::string(", got ") + versionAttr->data() +
                          scl::string("), cannot load"),
                      false);
        return CoreSettings{};
    }

    CoreSettings settings;

    // Get root node
    auto* root = doc.find_children("CoreSettings").front();
    if (!root) {
        Logger::Error("Settings file missing root CoreSettings element", false);
        return CoreSettings{};
    }

    // Deserialize main video settings
    auto videoNode = root->find_children("Video").front();
    for (const auto& attr : videoNode->attributes()) {
        if (attr->tag() == "width") {
            settings.video.width = static_cast<int>(attr->data_int());
        } else if (attr->tag() == "height") {
            settings.video.height = static_cast<int>(attr->data_int());
        } else if (attr->tag() == "fullscreen") {
            settings.video.fullscreen = attr->data() == "true";
        } else if (attr->tag() == "brightness") {
            settings.video.brightness = static_cast<float>(attr->data_float());
        } else if (attr->tag() == "vSync") {
            settings.video.vSync = attr->data() == "true";
        } else if (attr->tag() == "useShadows") {
            settings.video.useShadows = attr->data() == "true";
        } else if (attr->tag() == "shadowDist") {
            settings.video.shadowDist = static_cast<int>(attr->data_int());
        } else if (attr->tag() == "useSSAO") {
            settings.video.useSSAO = attr->data() == "true";
        }
    }

    // And then the datanode tree
    // Recursive lambda to deserialize XML into DataNode tree
    auto DeserializeNode = [](const scl::xml::XmlElem* elem,
                              DataNode&                dataNode,
                              const auto&              self) -> void {
        for (const auto& attr : elem->attributes()) {
            dataNode[attr->tag().cstr()] = attr->data();
        }
        for (const auto& child : elem->children()) {
            DataNode childNode;
            self(child, childNode, self);
            dataNode[child->tag().cstr()] = childNode;
        }
    };

    auto cvs = videoNode->find_children("game");
    if (!cvs.empty()) {
        DeserializeNode(cvs.front(), settings.customSettings, DeserializeNode);
    }

    Logger::Info("Loaded from settings");
    return settings;
}

bool Serializer::CoreSettings::SaveToFile(const std::string& path, const CoreSettings& settings) {
    scl::xml::XmlDocument doc;
    scl::path             inputPath = path.c_str();
    bool                  fileExists = inputPath.exists();

    doc.set_tag("CoreSettings");

    scl::xml::XmlAttr* versionAttr = doc.new_attr("Version", SYNINT_CORESETTINGS_VERSION);
    doc.add_attr(versionAttr);

    // Create root node
    scl::xml::XmlElem* root = doc.new_elem("CoreSettings");
    doc.add_child(root);

    // Serialize main video settings as XML attributes
    scl::xml::XmlElem* videoElem = doc.new_elem("Video");
    videoElem->add_attr(doc.new_attr("width", std::to_string(settings.video.width)));
    videoElem->add_attr(doc.new_attr("height", std::to_string(settings.video.height)));
    videoElem->add_attr(doc.new_attr("fullscreen", settings.video.fullscreen ? "true" : "false"));
    videoElem->add_attr(doc.new_attr("brightness", std::to_string(settings.video.brightness)));
    videoElem->add_attr(doc.new_attr("vSync", settings.video.vSync ? "true" : "false"));
    videoElem->add_attr(doc.new_attr("useShadows", settings.video.useShadows ? "true" : "false"));
    videoElem->add_attr(doc.new_attr("shadowDist", std::to_string(settings.video.shadowDist)));
    videoElem->add_attr(doc.new_attr("useSSAO", settings.video.useSSAO ? "true" : "false"));
    root->add_child(videoElem);

    // Write XML document to file
    scl::stream file;
    file.open(inputPath, scl::OpenMode::RWTRUNC, true);
    if (!file.is_open()) {
        Logger::Error("Failed to open file for writing: " + path);
        return false;
    }
    auto res = doc.print(file);
    if (res.code != scl::xml::OK) {
        Logger::Error("Failed to write XML document to file: " + path);
        return false;
    }
    file.flush();
    file.close();

    return true;
}

} // namespace Syngine
