// ╒═════════════════════════ Prefab.cpp ═╕
// │ Syngine                              │
// │ Created 2026-03-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/Scene/GameObjectRegistry.h>
#include <Syngine/Utils/Serializer.h>
#include <Syngine/Utils/FsUtils.h>
#include <Syngine/Core/Core.h>
#include <Syngine/GameObjects/ComponentRegistry.h>

#include <locale>
#include <sstream>
#include <string>

namespace Syngine {
namespace {

std::string SerializeCompoundParts(const Serializer::DataNode& parts) {
    using Node = Serializer::DataNode;
    if (!parts.IsArray() || parts.Size() == 0)
        throw std::runtime_error("Invalid compound shape parts");

    std::ostringstream result;
    result.imbue(std::locale::classic());
    for (size_t i = 0; i < parts.Size(); ++i) {
        const auto& part = parts.At(i);
        if (part["shape"].GetType() != Node::Type::Integer)
            throw std::runtime_error("Invalid compound shape part type");
        if (i != 0) result << '|';
        result << part["shape"].As<int>();
        for (const auto& field :
             { "shapeParameters", "position", "rotation" }) {
            const auto&  values = part[field];
            const size_t count  = std::string(field) == "rotation" ? 4 : 3;
            if (!values.IsArray() || values.Size() != count)
                throw std::runtime_error("Invalid compound shape part vector");
            result << ':';
            for (size_t j = 0; j < count; ++j) {
                if (values.At(j).GetType() != Node::Type::Float)
                    throw std::runtime_error(
                        "Invalid compound shape part value");
                if (j != 0) result << ',';
                result << Internal::FormatSerializedFloat(
                    values.At(j).As<float>());
            }
        }
    }
    return result.str();
}

bool PrefabDataSupported(const Serializer::DataNode& object) {
    if (object.Has("components")) {
        for (const auto& [type, component] : object["components"]) {
            for (const auto& [key, value] : component) {
                if (type == std::to_string(SYN_COMPONENT_RIGIDBODY) &&
                    key == "compoundParts") {
                    SerializeCompoundParts(value);
                    continue;
                }
                if (value.IsObject()) return false;
                if (value.IsArray()) {
                    for (size_t i = 0; i < value.Size(); ++i) {
                        if (value.At(i).IsArray() || value.At(i).IsObject())
                            return false;
                    }
                }
            }
        }
    }
    if (object.Has("children") && object["children"].IsArray()) {
        for (size_t i = 0; i < object["children"].Size(); ++i) {
            if (!PrefabDataSupported(object["children"].At(i))) return false;
        }
    }
    return true;
}

} // namespace

Serializer::Prefab::Prefab(GameObject* root)
    : rootGameObjectData(root->Serialize()), isValid(true),
      rootGameObject(root) {}
Serializer::Prefab::Prefab()
    : rootGameObjectData(DataNode{}), isValid(false), rootGameObject(nullptr) {}

Serializer::Prefab::Prefab(const std::string& path)
    : rootGameObjectData(DataNode{}), isValid(false), rootGameObject(nullptr) {
    *this = LoadFromFile(path);
    if (!isValid) {
        Logger::Error("Failed to load prefab from path: " + path);
        return;
    }
}

void Serializer::Prefab::WriteGameObject(const DataNode&        node,
                                         scl::xml::XmlDocument& doc,
                                         scl::xml::XmlElem*     parent) {
    // Convert DataNode data to XML attributes
    // Defines a lambda function named `ConvertData` that takes a `DataNode`
    // object as input and returns a `std::string`. Within this lambda function,
    // there is a nested lambda function named `convert` that recursively
    // converts the data within the `DataNode` object to a string representation
    // based on its type.
    auto ConvertData = [](const DataNode& node) -> std::string {
        std::function<std::string(const DataNode&)> convert =
            [&convert](const DataNode& n) -> std::string {
            switch (n.GetType()) {
            case DataNode::Type::Null: return std::string("null");
            case DataNode::Type::Boolean:
                return n.As<bool>() ? std::string("true")
                                    : std::string("false");
            case DataNode::Type::Integer: return std::to_string(n.As<int>());
            case DataNode::Type::UnsignedInteger:
                return std::to_string(n.As<uint64_t>());
            case DataNode::Type::Float:
                return Internal::FormatSerializedFloat(n.As<float>());
            case DataNode::Type::String: return n.As<std::string>();
            case DataNode::Type::Object: {
                for (const auto& [k, v] :
                     std::get<DataNode::NodeMap>(n.m_data)) {
                    if (!v.IsNull()) {
                        return convert(v);
                    }
                }
                return std::string("null");
            }
            case DataNode::Type::Array: {
                std::string valueStr; // Default value required so XML doesn't
                                      // have null attribs
                const auto& arr = std::get<DataNode::NodeArray>(n.m_data);
                if (arr.empty()) return std::string("null");
                for (size_t i = 0; i < arr.size(); ++i) {
                    // Special case for empty string arrays to avoid "null"
                    // values
                    if (i == 0 && arr[i].GetType() == DataNode::Type::String &&
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

    // Iterate through the DataNode's data and add it as attributes to the XML
    // element
    if (!node.IsNull()) {
        for (const auto& [key, value] :
             std::get<DataNode::NodeMap>(node.m_data)) {
            if (key == "components" || key == "children") continue;
            std::string valueStr = ConvertData(value);
            if (valueStr == "null")
                continue; // Skip null values to avoid cluttering XML with empty
                          // attributes
            scl::xml::XmlAttr* attr = doc.new_attr(key, valueStr);

            if (!attr) {
                Logger::Error("Failed to create XML attribute for key: " + key);
                continue;
            }
            parent->add_attr(attr);
        }
    }

    // Handle components separately as child elements
    if (node.Has("components")) {
        const auto& components = node["components"].As<DataNode::NodeMap>();
        for (const auto& comp : components) {
            scl::xml::XmlElem* compElem = doc.new_elem("Component");
            for (const auto& [key, value] :
                 std::get<DataNode::NodeMap>(comp.second.m_data)) {
                std::string valueStr =
                    key == "compoundParts" &&
                            comp.first ==
                                std::to_string(SYN_COMPONENT_RIGIDBODY)
                        ? SerializeCompoundParts(value)
                        : ConvertData(value);
                scl::xml::XmlAttr* attr = doc.new_attr(key, valueStr);
                if (!attr) {
                    Logger::Error(
                        "Failed to create XML attribute for component key: " +
                        key);
                    continue;
                }
                compElem->add_attr(attr);
            }
            parent->add_child(compElem);
        }
    }

    // Handle child GameObjects as nested elements
    if (node.Has("children")) {
        scl::xml::XmlElem* childElem = doc.new_elem("Children");
        parent->add_child(childElem);
        const auto& children = node["children"].As<DataNode::NodeArray>();
        for (const auto& child : children) {
            // Recursively serialize child GameObjects
            Serializer::DataNode childGO   = child;
            scl::xml::XmlElem*   childNode = doc.new_elem("GameObject");
            WriteGameObject(childGO, doc, childNode);
            childElem->add_child(childNode);
        }
    }
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
    try {
        if (!PrefabDataSupported(rootGameObjectData)) {
            Logger::Error(
                "Prefab format does not support nested component data");
            return false;
        }
    } catch (const std::runtime_error& e) {
        Logger::Error("Invalid prefab component data: " +
                      std::string(e.what()));
        return false;
    }

    Serializer::DataNode node = rootGameObjectData;
    // Resolve path, delete existing file if it exists, and create new XML
    // document
    std::string programName =
        Core::Get() ? Core::Get()->m_context->config.gameName : "UnknownGame";
    std::string           adp = (_GetAppDataPath(programName) / path).string();
    scl::path             outputPath = adp.c_str();
    scl::xml::XmlDocument doc;

    // Base of the file is Prefab root with GameObject child.
    scl::xml::XmlAttr* v = doc.new_attr("Version", SYNINT_PREFAB_VERSION);
    scl::xml::XmlAttr* n = doc.new_attr(
        "Name",
        node.Has("name") ? node["name"].As<std::string>() : "UnnamedPrefab");
    doc.set_tag("Prefab");
    doc.add_attr(v);
    doc.add_attr(n);

    scl::xml::XmlElem* root = doc.new_elem("GameObject");
    doc.add_child(root);

    // Convert DataNode data to XML attributes and child elements
    Prefab::WriteGameObject(node, doc, root);

    // Write XML document to file
    scl::stream file;
    file.open(outputPath, scl::OpenMode::RWTRUNC, true);
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

Serializer::Prefab Serializer::Prefab::LoadFromFile(const std::string& path) {
    Serializer::Prefab prefab;
    std::string        gameName =
        Core::Get() ? Core::Get()->m_context->config.gameName : "UnknownGame";
    std::string adp       = (_GetAppDataPath(gameName) / path).string();
    scl::path   inputPath = adp.c_str();
    if (!inputPath.exists()) {
        Logger::Error("Prefab file not found: " + path);
        return prefab;
    }

    scl::xml::XmlDocument doc;
    if (doc.load_file(inputPath).code != scl::xml::OK) {
        Logger::Error("Failed to load prefab XML file: " + path);
        return prefab;
    }

    // Check version compatibility
    auto versionAttr = doc.find_attr("Version");
    if (!versionAttr) {
        Logger::Error("Prefab XML missing Version attribute: " + path);
        return prefab;
    }
    scl::string version = versionAttr->data();
    if (doc.tag() != "Prefab" || version != SYNINT_PREFAB_VERSION) {
        Logger::Error(&"Prefab version mismatch (expected "[scl::string(
                          SYNINT_PREFAB_VERSION)] +
                      scl::string(", got ") + version + scl::string("): ") +
                      scl::string(path));
        return prefab;
    }

    // Deserialize the XML document into a DataNode structure
    auto rootElem = doc.find_children("GameObject").front();
    if (!rootElem) {
        Logger::Error("Prefab XML missing root GameObject element: " + path);
        return prefab;
    }
    prefab.rootGameObjectData =
        prefab.Deserialize(rootElem, prefab.rootGameObjectData);

    // Now that the prefab is deserialized into a DataNode, we can instantiate
    // it into actual GameObjects and Components
    prefab.rootGameObject =
        &GameObjectRegistry::Instantiate(prefab.rootGameObjectData);
    prefab.name =
        prefab.rootGameObject
            ->name; // Set prefab name to root GameObject's name for convenience

    prefab.isValid = true; // Mark as valid after successful deserialization
    return prefab;
}

Serializer::DataNode&
Serializer::Prefab::Deserialize(const scl::xml::XmlElem* elem,
                                Serializer::DataNode&    outNode) {
    // Convert XML attributes to DataNode data
    const auto& attributes = elem->attributes();
    for (const auto& attr : elem->attributes()) {
        scl::string key   = attr->tag();
        scl::string value = attr->data();

        if (key == "name" || key == "type" || key == "gizmo") {
            outNode[key.cstr()] = value.cstr();
        } else if (key == "isActive") {
            outNode["isActive"] = (value == "true");
        } else if (key == "tags") {
            // tags are CSV in XML, convert back to array
            std::vector<std::string> tags = Serializer::SplitString(value, ',');
            outNode["tags"]               = tags;
        }
    }

    // Handle components
    std::vector<scl::xml::XmlElem*> componentsElem =
        elem->find_children("Component");
    if (!componentsElem.empty()) {
        DataNode componentsNode;
        for (const auto& compElem : componentsElem) {
            auto typeAttr = compElem->find_attr("type");
            if (!typeAttr) continue;
            Syngine::ComponentTypeID typeId = std::stoull(
                typeAttr->data()
                    .cstr()); // Pray it doesn't get negative somehow
            DataNode compNode = ComponentRegistry::ParseXml(typeId, compElem);
            if (auto* enabled = compElem->find_attr("enabled")) {
                compNode["enabled"] = (enabled->data() == "true");
            }
            componentsNode[std::to_string(typeId)] =
                compNode; // Use component type as key
        }
        outNode["components"] = componentsNode;
    }

    // Handle child GameObjects
    if (elem->find_children("Children").empty()) {
        return outNode;
    }

    auto childrenElem = elem->find_children("Children").front();
    if (childrenElem) {
        DataNode childrenNode;
        for (const auto& childElem :
             childrenElem->find_children("GameObject")) {
            DataNode childNode;
            Deserialize(
                childElem,
                childNode); // Recursive call to deserialize child GameObject
            childrenNode.Append(childNode);
        }
        outNode["children"] = childrenNode;
    }

    return outNode;
}

} // namespace Syngine
