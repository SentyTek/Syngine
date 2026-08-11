// ╒════════════════════════ Component.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once
#include <Syngine/Utils/Serializer.h>
#include <memory>

namespace Syngine {
// Forward declaration
class GameObject;

using ComponentTypeID = uint64_t;

/// @brief An enum of built-in component types.
/// @section Component
enum DefaultComponents : ComponentTypeID {
    SYN_COMPONENT_MESH              = 1,  //* 3D mesh or model
    SYN_COMPONENT_TRANSFORM         = 2,  //* Position, rotation, scale
    SYN_COMPONENT_AI                = 3,  //* AI logic/behavior
    SYN_COMPONENT_PLAYER            = 4,  //* Player controller
    SYN_COMPONENT_CAMERA            = 5,  //* Camera logic
    SYN_COMPONENT_LIGHT_POINT       = 6,  //* Point light source
    SYN_COMPONENT_LIGHT_DIRECTIONAL = 7,  //* Sun light source
    SYN_COMPONENT_LIGHT_SPOT        = 8,  //* Spot light source
    SYN_COMPONENT_AUDIO_EMIT        = 9,  //* Audio source
    SYN_COMPONENT_AUDIO_LISTEN      = 10, //* Audio listener
    SYN_COMPONENT_PARTICLE          = 11, //* Particle system
    SYN_COMPONENT_UI                = 12, //* UI elements
    SYN_COMPONENT_SCRIPT            = 13, //* Custom scripting
    SYN_COMPONENT_ANIMATION         = 14, //* Animation controller
    SYN_COMPONENT_RIGIDBODY         = 15, //* Physics rigidbody
    SYN_COMPONENT_PHYSICS_MOTOR     = 16, //* Physics motor/actuator
    SYN_COMPONENT_PHYSICS_CLOTH     = 17, //* Cloth simulation
    SYN_COMPONENT_PHYSICS_JOINT     = 18, //* Physics joint/constraint
    SYN_COMPONENT_TERRAIN           = 19, //* Terrain
    SYN_COMPONENT_ZONE              = 20, //* Trigger zone or area
    SYN_COMPONENT_BILLBOARD         = 21, //* Billboard component
    SYN_COMPONENT_COUNT             = 22  //* Total number of component types
};

/// @brief Syngine::IComponent is the base class for all components that can be
/// attached to GameObject to inherit from. They are used to add functionality
/// to GameObjects, such as mesh, transform, physics, AI, PlayerController, etc.
/// @section Component
class IComponent {
  public:
    /// @brief Get the type of the component, used for identifying the component
    /// type
    /// @return The type of the component, as defined in the Components enum
    virtual ComponentTypeID GetComponentType() = 0;

    /// @brief Clone the component, used for copying components when duplicating
    /// GameObjects or when copying components from one GameObject to another
    /// @return A unique pointer to the cloned component
    virtual std::unique_ptr<IComponent> Clone() const = 0;

    /// @brief Serialize the component to a data node
    /// @return A pointer to the serialized data node representing the
    /// component's state
    virtual Serializer::DataNode Serialize() const = 0;

    virtual void Init() {
    } // Optional initialization function for components that need it

    virtual void Update(float deltaTime) {
    } // Optional update function for components that need it

    virtual void OnEnable() {
    } // Optional function called when the component is enabled

    virtual void OnDisable() {
    } // Optional function called when the component is disabled

    virtual void PostPhysicsUpdate() {
    } // Optional function called after physics update, for components that need
      // it

    inline void SetEnabled(bool enabled) {
        m_isEnabled = enabled;
        if (enabled) {
            OnEnable();
        } else {
            OnDisable();
        }
    }

    inline bool IsEnabled() const { return m_isEnabled; }

    IComponent(GameObject* owner) : m_owner(owner) {}
    IComponent(GameObject* owner, bool isEnabled)
        : m_owner(owner), m_isEnabled(isEnabled) {}
    IComponent(const IComponent& other)            = default;
    IComponent& operator=(const IComponent& other) = default;

    virtual ~IComponent() = default;
#if defined(SYN_TEST_MODE)
    // For testing purposes, allow access to the owner GameObject
    inline GameObject* _GetOwner() const { return m_owner; }
#endif

  protected:
    bool        m_isEnabled = true; //* Whether the component is enabled or not
    GameObject* m_owner     = nullptr; //* The owner of the component, the
                                       // GameObject it is attached to
    friend class GameObject;
    friend class Renderer;
    friend class RenderDirector;
    friend class GameObjectRegistry;
};

}; // namespace Syngine
