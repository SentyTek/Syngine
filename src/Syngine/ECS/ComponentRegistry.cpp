// ╒══════════════ ComponentRegistry.cpp ═╕
// │ Syngine                              │
// │ Created 2026-03-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/Core/Logger.h"

namespace Syngine {

std::unordered_map<uint64_t, ComponentRegistry::Entry>& ComponentRegistry::m_registry() {
    static std::unordered_map<uint64_t, Entry> s_map; // initialized once, avoids static-init-order issues
    return s_map;
}

void ComponentRegistry::Register(uint64_t type, ParseXmlFn parseXml, InstantiateFn instantiate) {
    m_registry()[type] = { std::move(parseXml), std::move(instantiate) };
}

Serializer::DataNode ComponentRegistry::ParseXml(uint64_t type, const scl::xml::XmlElem* elem) {
    auto it = m_registry().find(type);
    if (it == m_registry().end()) {
        Logger::Error("ComponentRegistry: No parser for component type " + std::to_string(type));
        return {};
    }
    return it->second.parseXml(elem);
}

std::unique_ptr<Component> ComponentRegistry::Instantiate(uint64_t type, GameObject* owner,
                                                           const Serializer::DataNode& data) {
    auto it = m_registry().find(type);
    if (it == m_registry().end()) {
        Logger::Error("ComponentRegistry: No factory for component type " + std::to_string(type));
        return nullptr;
    }
    return it->second.instantiate(owner, data);
}

} // namespace Syngine