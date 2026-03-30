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
#include <string>
#include <variant>
#include <stdexcept>

namespace Syngine {
namespace Internal {
std::string ResolvePath(const char* path) { return _ResolveOSPath(path); }
} // namespace Internal

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

} // namespace Syngine
