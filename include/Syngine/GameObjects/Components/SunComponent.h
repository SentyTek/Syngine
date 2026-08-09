// ╒═════════════════════ SunComponent.h ═╕
// │ Syngine                              │
// │ Created 2026-08-08                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯
//
// Note: This component was previously named SunComponent.

#pragma once

#include <Syngine/GameObjects/Component.h>
#include <Syngine/Math/Math.hpp>

#include <memory>

namespace Syngine {

/// @brief Syngine Directional Light Component. The DirectionalLightComponent is
/// used to represent a directional light source in the 3D game world, such as
/// the sun. It provides lighting information for rendering and shading.
/// @section DirectionalLightComponent
/// @nameoverride DirectionalLightComponent
/// @note There can only be two directional light components in the scene at any
/// given time. If a third directional light component is added, it will be
/// ignored and a warning will be logged.
/// @since v0.0.2
class DirectionalLightComponent : public IComponent {
    Math::Quaternion m_rotation; //* Light rotation (orientation) in world space
    Math::Vec3 m_color     = Math::Vec3(1.0f, 1.0f, 1.0f); //* Light color (RGB)
    float      m_intensity = 1.0f; //* Light intensity (brightness)
    bool       m_enabled   = true; //* Whether the light is enabled or disabled

  public:
    static constexpr Syngine::ComponentTypeID componentType =
        SYN_COMPONENT_LIGHT_DIRECTIONAL; //* Light component type

    /// @brief Get the type of this component
    /// @return The component type as an enum value
    /// @since v0.0.2
    virtual ComponentTypeID GetComponentType() override {
        return SYN_COMPONENT_LIGHT_DIRECTIONAL;
    }

    /// @brief Clone the component, used for copying components when duplicating
    /// GameObjects
    /// @return A unique pointer to a new instance of the component with the
    /// same state
    virtual std::unique_ptr<IComponent> Clone() const override {
        return std::make_unique<DirectionalLightComponent>(*this);
    }

    /// @brief Serialize the component to a data node for saving or networking
    /// @return A data node representing the component's state
    /// @since v0.0.2
    virtual Serializer::DataNode Serialize() const override;

    /// @brief Construct a new Directional Light Component object
    /// @param owner Pointer to the owner GameObject
    /// @param rotation The initial rotation (orientation) of the light in world
    /// space
    /// @param color The initial color of the light (RGB)
    /// @param intensity The initial intensity (brightness) of the light
    /// @since v0.0.2
    DirectionalLightComponent(GameObject*             owner,
                              const Math::Quaternion& rotation,
                              const Math::Vec3&       color,
                              float                   intensity);

    /// @brief Set the rotation (orientation) of the light in world space
    /// @param rotation The new rotation (orientation) of the light in world
    /// space
    /// @since v0.0.2
    void SetRotation(const Math::Quaternion& rotation) {
        m_rotation = rotation;
    }

    /// @brief Get the rotation (orientation) of the light in world space
    /// @return The rotation (orientation) of the light in world space
    /// @since v0.0.2
    const Math::Quaternion& GetRotation() const { return m_rotation; }

    /// @brief Set the color of the light (RGB)
    /// @param color The new color of the light (RGB)
    /// @since v0.0.2
    void SetColor(const Math::Vec3& color) { m_color = color; }

    /// @brief Get the color of the light (RGB)
    /// @return The color of the light (RGB)
    /// @since v0.0.2
    const Math::Vec3& GetColor() const { return m_color; }

    /// @brief Set the intensity (brightness) of the light
    /// @param intensity The new intensity (brightness) of the light
    /// @since v0.0.2
    void SetIntensity(float intensity) { m_intensity = intensity; }

    /// @brief Get the intensity (brightness) of the light
    /// @return The intensity (brightness) of the light
    /// @since v0.0.2
    float GetIntensity() const { return m_intensity; }
};

} // namespace Syngine
