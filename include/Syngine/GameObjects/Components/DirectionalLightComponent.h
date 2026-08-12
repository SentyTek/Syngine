// ╒════════ DirectionalLightComponent.h ═╕
// │ Syngine                              │
// │ Created 2026-08-08                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯
//
// Note: This component was previously named DirectionalLightComponent.

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
    Math::Vec3 m_direction; //* Light rotation (orientation) in world space
    Math::Vec3 m_color =
        Math::Vec3(1.0f, 1.0f, 1.0f); //* Light color (RGB Normalized)
    float m_intensity = 1.0f;         //* Light intensity (brightness)
    bool  m_enabled   = true; //* Whether the light is enabled or disabled

    // Internal helper to set the direction of the light based on a rotation
    // vector (pitch, yaw, roll)
    // Literally the code from the old renderer sun
    inline void _SetDirection(const Math::Vec3& direction) {
        float pitch = static_cast<float>(direction.x());
        float yaw   = static_cast<float>(direction.y());
        float cp    = cosf(pitch);
        float sp    = sinf(pitch);
        float cy    = cosf(yaw);
        float sy    = sinf(yaw);

        // y = +sin(pitch) when pitch is above horizon. (Ensure convention
        // matches UI)
        Math::Vector3 dirVec(cy * cp, sp, sy * cp);
        dirVec      = dirVec.normalized();
        m_direction = dirVec;
    }

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
    /// @param direction The initial direction of the light in world space
    /// @param color The initial color of the light (RGB)
    /// @param intensity The initial intensity (brightness) of the light
    /// @since v0.0.2
    DirectionalLightComponent(GameObject*       owner,
                              const Math::Vec3& direction,
                              const Math::Vec3& color,
                              float             intensity);

    /// @brief Set the light direction using Euler angles.
    /// @param direction Euler angles in radians: (pitch, yaw, roll).
    /// @note Roll is ignored because a direction vector cannot represent roll.
    /// @since v0.0.2
    inline void SetDirection(const Math::Vec3& direction) {
        _SetDirection(direction);
    }

    /// @brief Get the light direction as Euler angles.
    /// @return Euler angles in radians: (pitch, yaw, 0).
    /// @note Roll is always 0 because the light stores only a direction vector.
    /// @since v0.0.2
    inline Math::Vec3 GetDirection() const {
        float pitch = asinf(m_direction.y());
        float yaw   = atan2f(m_direction.z(), m_direction.x());
        return Math::Vec3(pitch, yaw, 0.0f);
    }

    /// @brief Get the light's world-space direction vector.
    /// @return A normalized direction vector representing the light's
    /// direction.
    /// @since v0.0.2
    inline const Math::Vec3& GetDirectionVector() const { return m_direction; }

    /// @brief Set the light's world-space direction vector directly.
    /// @param direction A normalized or approximately normalized direction
    /// vector.
    /// @since v0.0.2
    inline void SetDirectionVector(const Math::Vec3& direction) {
        m_direction = direction.normalized();
    }

    /// @brief Set the color of the light (RGB)
    /// @param color The new color of the light (RGB)
    /// @since v0.0.2
    inline void SetColor(const Math::Vec3& color) { m_color = color; }

    /// @brief Get the color of the light (RGB)
    /// @return The color of the light (RGB)
    /// @since v0.0.2
    inline const Math::Vec3& GetColor() const { return m_color; }

    /// @brief Set the intensity (brightness) of the light
    /// @param intensity The new intensity (brightness) of the light
    /// @since v0.0.2
    inline void SetIntensity(float intensity) { m_intensity = intensity; }

    /// @brief Get the intensity (brightness) of the light
    /// @return The intensity (brightness) of the light
    /// @since v0.0.2
    inline float GetIntensity() const { return m_intensity; }
};

} // namespace Syngine
