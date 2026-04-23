// ╒══════════════ ComponentRegistry.cpp ═╕
// │ Syngine                              │
// │ Created 2026-03-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/ECS/Component.h"

namespace Syngine {

std::unordered_map<Syngine::ComponentTypeID, ComponentRegistry::Entry>& ComponentRegistry::m_registry() {
    static std::unordered_map<Syngine::ComponentTypeID, Entry> s_map; // initialized once, avoids static-init-order issues
    return s_map;
}

void ComponentRegistry::Register(Syngine::ComponentTypeID type,
                                 ParseXmlFn               parseXml,
                                 InstantiateFn            instantiate,
                                 RegisterLuaFn            registerLua) {
    // Prevent duplicates
    if (m_registry().find(type) != m_registry().end()) {
        Logger::Error("ComponentRegistry: Component type " + std::to_string(type) + " is already registered");
        return;
    }
    m_registry()[type] = { type, std::move(parseXml), std::move(instantiate), std::move(registerLua) };
}

Serializer::DataNode ComponentRegistry::ParseXml(Syngine::ComponentTypeID type, const scl::xml::XmlElem* elem) {
    auto it = m_registry().find(type);
    if (it == m_registry().end()) {
        Logger::Error("ComponentRegistry: No parser for component type " + std::to_string(type));
        return {};
    }
    return it->second.parseXml(elem);
}

std::unique_ptr<Component> ComponentRegistry::Instantiate(Syngine::ComponentTypeID type, GameObject* owner,
                                                           const Serializer::DataNode& data) {
    auto it = m_registry().find(type);
    if (it == m_registry().end()) {
        Logger::Error("ComponentRegistry: No factory for component type " + std::to_string(type));
        return nullptr;
    }
    return it->second.instantiate(owner, data);
}

void ComponentRegistry::_RegisterAllLuaBindings(sol::state& lua) {
    for (const auto& [type, entry] : m_registry()) {
        if (entry.registerLua) {
            entry.registerLua(lua);
        }
    }
}

} // namespace Syngine
