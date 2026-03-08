// ╒═════════════════════ Serializer.cpp ═╕
// │ Syngine                              │
// │ Created 2026-01-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Utils/Serializer.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/Core/Core.h"

#include "../../lib/miniscl.hpp"

namespace Syngine {
namespace Internal {
std::string ResolvePath(const char* path) { return _ResolveOSPath(path); }
} // namespace Internal

Serializer::Prefab::Prefab(GameObject* root) : rootGameObject(root->Serialize()), isValid(true) {}

bool Serializer::DataNode::Has(const std::string& key) const {
    if (!std::holds_alternative<NodeMap>(m_data)) return false;
    const auto& map = std::get<NodeMap>(m_data);
    return map.find(key) != map.end();
}

Serializer::DataNode::Type Serializer::DataNode::GetType() const {
    if (std::holds_alternative<std::monostate>(m_data)) return Type::Null;
    if (std::holds_alternative<int>(m_data)) return Type::Integer;
    if (std::holds_alternative<float>(m_data)) return Type::Float;
    if (std::holds_alternative<bool>(m_data)) return Type::Boolean;
    if (std::holds_alternative<std::string>(m_data)) return Type::String;
    if (std::holds_alternative<NodeMap>(m_data)) return Type::Object;
    if (std::holds_alternative<NodeArray>(m_data)) return Type::Array;  
    throw std::runtime_error("Unknown DataNode type");
}

bool Serializer::Prefab::SaveToFile(const std::string& path) {
    // Serialize the prefab to a DataNode
    if (!isValid) {
        Logger::Error("Cannot serialize invalid prefab");
        return false;
    }
    if (!rootGameObject) {
        Logger::Error("Prefab has no root GameObject to serialize");
        return false;
    }

    Serializer::DataNode node = rootGameObject; // Assuming rootGameObject is already a DataNode
    // Resolve path, delete existing file if it exists, and create new XML document
    auto adp = _GetAppdataPath(Core::Get()->m_app->config.windowTitle);
    scl::path outputPath = (adp / path).c_str();
    std::filesystem::remove(outputPath.cstr()); // Remove existing file if it exists
    scl::xml::XmlDocument doc;
    if (doc.load_file(outputPath) != scl::xml::OK) {
        Logger::Error("Couldn't open XML file at " + path);
        return false;
    }

    // Base of the file is Prefab root with GameObject child.
    scl::xml::XmlAttr* v    = doc.new_attr("Version", SYNINT_PREFAB_VERSION);
    scl::xml::XmlAttr* n    = doc.new_attr(
        "Name",
        node.Has("name") ? node["name"].As<std::string>() : "UnnamedPrefab");
    doc.set_tag("Prefab");
    doc.add_attr(v);
    doc.add_attr(n);

    scl::xml::XmlElem* root = doc.new_elem("GameObject");
    doc.add_child(root);

    // Convert DataNode data to XML attributes
    // Defines a lambda function named `ConvertData` that takes a `DataNode`
    // object as input and returns a `std::string`. Within this lambda function,
    // there is a nested lambda function named `convert` that recursively
    // converts the data within the `DataNode` object to a string representation
    // based on its type.
    auto ConvertData = [](const DataNode& node) -> std::string {
        std::function<std::string(const DataNode&)> convert = [&convert](const DataNode& n) -> std::string {
            switch (n.GetType()) {
                case DataNode::Type::Null: return std::string("null");
                case DataNode::Type::Boolean: return n.As<bool>() ? std::string("true") : std::string("false");
                case DataNode::Type::Integer: return std::to_string(n.As<int>());
                case DataNode::Type::Float: {
                    std::string s = std::to_string(n.As<float>());
                    auto dot = s.find('.');
                    if (dot != std::string::npos) {
                        auto last = s.find_last_not_of('0');
                        s = (last == dot) ? s.substr(0, dot) : s.substr(0, last + 1);
                    }
                    return s;
                }
                case DataNode::Type::String: return n.As<std::string>();
                case DataNode::Type::Object: return std::string("<object>");
                case DataNode::Type::Array: {
                    std::string valueStr; // Default value required so XML doesn't have null attribs
                    const auto& arr = std::get<DataNode::NodeArray>(n.m_data);
                    for (size_t i = 0; i < arr.size(); ++i) {
                        // Special case for empty string arrays to avoid "null" values
                        if (i == 0 &&
                            arr[i].GetType() == DataNode::Type::String &&
                            arr[i].As<std::string>() == "") {
                            valueStr = "null"; 
                            break;
                        };
                        if (i > 0) valueStr += ",";
                        valueStr += convert(arr[i]);
                    }
                    return valueStr;
                }
                default: return std::string("unknown");
            }
        };
        return convert(node);
    };

    // Iterate through the DataNode's data and add it as attributes to the XML element
    for (const auto& [key, value] : std::get<DataNode::NodeMap>(node.m_data)) {
        if (key == "components") continue;
        std::string valueStr = ConvertData(value);
        if (valueStr == "null") continue; // Skip null values to avoid cluttering XML with empty attributes
        scl::xml::XmlAttr* attr = doc.new_attr(key, valueStr);
        
        if (!attr) {
            Logger::Error("Failed to create XML attribute for key: " + key);
            continue;
        }
        root->add_attr(attr);
    }

    // Handle components separately as child elements
    if (node.Has("components")) {
        const auto& components = node["components"].As<DataNode::NodeMap>();
        for (const auto& comp : components) {
            scl::xml::XmlElem* compElem = doc.new_elem("Component");
            for (const auto& [key, value] : std::get<DataNode::NodeMap>(comp.second.m_data)) {
                std::string valueStr = ConvertData(value);
                scl::xml::XmlAttr* attr = doc.new_attr(key, valueStr);
                if (!attr) {
                    Logger::Error("Failed to create XML attribute for component key: " + key);
                    continue;
                }
                compElem->add_attr(attr);
            }
            root->add_child(compElem);
        }
    }

    // Write XML document to file
    scl::stream file;
    file.open(outputPath, scl::OpenMode::WRITE, true);
    if (!file.is_open()) {
        Logger::Error("Failed to open file for writing: " + path);
        return false;
    }
    auto res = doc.print(file);
    if (res != scl::xml::OK) {
        Logger::Error("Failed to write XML document to file: " + path);
        return false;
    }
    file.flush();
    file.close();

    return true;
}

} // namespace Syngine
