// ╒════════════════ ComponentRegistry.h ═╕
// │ Syngine                              │
// │ Created 2026-03-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

#include <functional>
#include <unordered_map>

#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Utils/Serializer.h"
#include "miniscl.hpp"

namespace Syngine {

class ComponentRegistry {
    public:
      using ParseXmlFn =
          std::function<Serializer::DataNode(const scl::xml::XmlElem*)>;
      using InstantiateFn = std::function<std::unique_ptr<Component>(GameObject*, const Serializer::DataNode&)>;

      static void
      Register(uint64_t type, ParseXmlFn parseFn, InstantiateFn instantiateFn);

      static Serializer::DataNode ParseXml(uint64_t                 type,
                                           const scl::xml::XmlElem* node);

      std::unique_ptr<Component> Instantiate(uint64_t                    type,
                                             GameObject*                 owner,
                                             const Serializer::DataNode& data);

    private:
        struct Entry {
            ParseXmlFn parseXml;
            InstantiateFn instantiate;
        };

        static std::unordered_map<uint64_t, Entry>& m_registry();
};

// Helper struct to register components in the ComponentRegistry at static initialization time
struct ComponentRegistrar {
    ComponentRegistrar(uint64_t type,
                       ComponentRegistry::ParseXmlFn parseFn,
                       ComponentRegistry::InstantiateFn instantiateFn) {
        ComponentRegistry::Register(type, std::move(parseFn), std::move(instantiateFn));
    }
};

}; // namespace Syngine
