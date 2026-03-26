// ╒════════════════════════ Component.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include <memory>
#include "Syngine/Utils/Serializer.h"

namespace Syngine {
// Forward declaration
class GameObject;


using ComponentTypeID = uint64_t;

/// @brief An enum of built-in component types.
/// @section Component
enum DefaultComponents : ComponentTypeID {
    SYN_COMPONENT_MESH          = 1,  //* 3D mesh or model
    SYN_COMPONENT_TRANSFORM     = 2,  //* Position, rotation, scale
    SYN_COMPONENT_AI            = 3,  //* AI logic/behavior
    SYN_COMPONENT_PLAYER        = 4,  //* Player controller
    SYN_COMPONENT_CAMERA        = 5,  //* Camera logic
    SYN_COMPONENT_LIGHT         = 6,  //* Lighting
    SYN_COMPONENT_AUDIO_EMIT    = 7,  //* Audio source
    SYN_COMPONENT_AUDIO_LISTEN  = 8,  //* Audio listener
    SYN_COMPONENT_PARTICLE      = 9,  //* Particle system
    SYN_COMPONENT_UI            = 10, //* UI elements
    SYN_COMPONENT_SCRIPT        = 11, //* Custom scripting
    SYN_COMPONENT_ANIMATION     = 12, //* Animation controller
    SYN_COMPONENT_RIGIDBODY     = 13, //* Physics rigidbody
    SYN_COMPONENT_PHYSICS_MOTOR = 14, //* Physics motor/actuator
    SYN_COMPONENT_PHYSICS_CLOTH = 15, //* Cloth simulation
    SYN_COMPONENT_PHYSICS_JOINT = 16, //* Physics joint/constraint
    SYN_COMPONENT_TERRAIN       = 17, //* Terrain
    SYN_COMPONENT_ZONE          = 18, //* Trigger zone or area
    SYN_COMPONENT_BILLBOARD     = 19, //* Billboard component
    SYN_COMPONENT_COUNT         = 20  //* Total number of component types
};

/// @brief Syngine::Component is the base class for all components that can be
/// attached to GameObject to inherit from. They are used to add functionality
/// to GameObjects, such as mesh, transform, physics, AI, PlayerController, etc.
/// @section Component
class Component {
  public:
    /// @brief Get the type of the component, used for identifying the component
    /// type
    /// @return The type of the component, as defined in the Components enum
    virtual ComponentTypeID GetComponentType() = 0;

    /// @brief Clone the component, used for copying components when duplicating
    /// GameObjects or when copying components from one GameObject to another
    /// @return A unique pointer to the cloned component
    virtual std::unique_ptr<Component> Clone() const = 0;

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

    virtual ~Component() = default;

    bool isEnabled = true; //* Whether the component is enabled or not
    GameObject* m_owner = nullptr; //* The owner of the component, the GameObject it is attached to
};

}; // namespace Syngine
