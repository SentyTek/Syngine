// ╒═══════════════════ SunComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2026-08-08                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/GameObjects/Components/SunComponent.h>
#include <Syngine/GameObjects/ComponentRegistry.h>
#include <Syngine/Utils/Serializer.h>
#include <Syngine/Core/Logger.h>

namespace Syngine {

DirectionalLightComponent::DirectionalLightComponent(
    GameObject*             owner,
    const Math::Quaternion& rotation,
    const Math::Vec3&       color,
    float                   intensity)
    : IComponent(owner), m_rotation(rotation), m_color(color),
      m_intensity(intensity) {
    // Initialization code for the directional light component
}

static Syngine::ComponentRegistrar s_directionalLightRegistrar(
    Syngine::SYN_COMPONENT_LIGHT_DIRECTIONAL,
    // ParseXML: XML element -> DataNode
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" = static_cast<Syngine::ComponentTypeID>(
            Syngine::SYN_COMPONENT_LIGHT_DIRECTIONAL);
        for (const auto& attr : elem->attributes()) {
            scl::string key   = attr->tag();
            scl::string value = attr->data();
            if (key == "rotation") {
                std::vector<float> rot = Serializer::_ParseFloatArray(value);
                if (rot.size() != 4) {
                    Syngine::Logger::Warn(
                        "Incorrect number of elements for quaternion "
                        "initializer in Dirlight parseFn");
                }
                node / "rotation" =
                    Math::Quaternion(rot[0], rot[1], rot[2], rot[3]);
            } else if (key == "color") {
                std::vector<float> col = Serializer::_ParseFloatArray(value);
                if (col.size() != 3) {
                    Syngine::Logger::Warn(
                        "Incorrect number of elements for color initializer in "
                        "Dirlight parseFn");
                }
                node / "color" = Math::Vec3(col[0], col[1], col[2]);
            } else if (key == "intensity") {
                node / "intensity" = std::stof(value.cstr());
            }
        }
        return node;
    },

    // Instantiate: DataNode -> Component instance
    [](GameObject*                 owner,
       const Serializer::DataNode& data) -> std::unique_ptr<IComponent> {
        Math::Quaternion rotation =
            data.Has("rotation")
                ? Math::Quaternion(data["rotation"].As<Math::Quaternion>())
                : Math::Quaternion();
        Math::Vec3 color = data.Has("color")
                               ? Math::Vec3(data["color"].As<Math::Vec3>())
                               : Math::Vec3(1.0f, 1.0f, 1.0f);
        float      intensity =
            data.Has("intensity") ? data["intensity"].As<float>() : 1.0f;
        return std::make_unique<DirectionalLightComponent>(
            owner, rotation, color, intensity);
    });
} // namespace Syngine
