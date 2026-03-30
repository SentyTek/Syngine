// ╒════════════════ ComponentRegistry.h ═╕
// │ Syngine                              │
// │ Created 2026-03-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once

#include <functional>
#include <unordered_map>

#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Utils/Serializer.h"
#include "miniscl.hpp"

namespace Syngine {

/// @brief ComponentRegistry is a global registry that maps component type IDs
/// to their corresponding XML parsing and instantiation functions. This allows
/// for dynamic creation of components from serialized data without hardcoding
/// each type in the deserialization logic.
/// @section ComponentRegistry
class ComponentRegistry {
    public:
      using ParseXmlFn =
          std::function<Serializer::DataNode(const scl::xml::XmlElem*)>;
      using InstantiateFn = std::function<std::unique_ptr<Component>(GameObject*, const Serializer::DataNode&)>;

      /// @brief Register a component type with its XML parsing and
      /// instantiation functions
      /// @param type The unique ID of the component type (should match the enum
      /// value in Components)
      /// @param parseFn A function that takes an XML element and returns a
      /// DataNode representing the component's data
      /// @param instantiateFn A function that takes a GameObject and a
      /// DataNode, and returns a unique pointer to a new instance of the
      /// component initialized with the data
      /// @example
      /// static Syngine::ComponentRegistrar s_compRegistrar(MY_COMPONENT_TYPE_ID, 
      /// // ParseXML: XML element -> DataNode
      /// [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {},
      /// // Instantiate: DataNode -> Component instance
      /// [](GameObject* owner, const Serializer::DataNode& data) -> std::unique_ptr<Syngine::Component> {}
      /// );
      /// @since v0.0.1
      static void
      Register(Syngine::ComponentTypeID type, ParseXmlFn parseFn, InstantiateFn instantiateFn);

      /// @brief Parse an XML element into a DataNode for a given component type
      /// @param type The unique ID of the component type
      /// @param node The XML element containing the component data
      /// @return A DataNode representing the component's data, or an empty
      /// DataNode if the type is not registered
      /// @since v0.0.1
      static Serializer::DataNode ParseXml(Syngine::ComponentTypeID type,
                                           const scl::xml::XmlElem* node);

      /// @brief Instantiate a component of a given type from a DataNode
      /// @param type The unique ID of the component type
      /// @param owner The GameObject that will own the new component instance
      /// @param data The DataNode containing the component's data
      /// @return A unique pointer to the new component instance, or nullptr if
      /// the type is not registered
      static std::unique_ptr<Component>
      Instantiate(Syngine::ComponentTypeID    type,
                  GameObject*                 owner,
                  const Serializer::DataNode& data);

    private:
      struct Entry {
          ComponentTypeID type; // Optional: for debugging/logging
          ParseXmlFn      parseXml;
          InstantiateFn   instantiate;
      };

      static std::unordered_map<Syngine::ComponentTypeID, Entry>& m_registry();
};

// Helper struct to register components in the ComponentRegistry at static initialization time
struct ComponentRegistrar {
    ComponentRegistrar(Syngine::ComponentTypeID type,
                       ComponentRegistry::ParseXmlFn parseFn,
                       ComponentRegistry::InstantiateFn instantiateFn) {
        ComponentRegistry::Register(type, std::move(parseFn), std::move(instantiateFn));
    }
};

}; // namespace Syngine
