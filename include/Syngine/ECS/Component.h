// ╒════════════════════════ Component.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include <memory>

namespace Syngine {
// Forward declaration
class GameObject;

/**
 * @brief An enum of all the components that can be attached to a GameObject.
 * @section Component
 */
enum Components {
    SYN_COMPONENT_MESH          = 1,      //* 3D mesh or model
    SYN_COMPONENT_TRANSFORM     = 2,      //* Position, rotation, scale
    SYN_COMPONENT_AI            = 4,      //* AI logic/behavior
    SYN_COMPONENT_PLAYER        = 8,      //* Player controller
    SYN_COMPONENT_CAMERA        = 16,     //* Camera logic
    SYN_COMPONENT_LIGHT         = 32,     //* Lighting
    SYN_COMPONENT_AUDIO_EMIT    = 64,     //* Audio source
    SYN_COMPONENT_AUDIO_LISTEN  = 128,    //* Audio listener
    SYN_COMPONENT_PARTICLE      = 256,    //* Particle system
    SYN_COMPONENT_UI            = 512,    //* UI elements
    SYN_COMPONENT_SCRIPT        = 1024,   //* Custom scripting
    SYN_COMPONENT_ANIMATION     = 2048,   //* Animation controller
    SYN_COMPONENT_RIGIDBODY     = 4096,   //* Physics rigidbody
    SYN_COMPONENT_PHYSICS_MOTOR = 8192,   //* Physics motor/actuator
    SYN_COMPONENT_PHYSICS_CLOTH = 16384,  //* Cloth simulation
    SYN_COMPONENT_PHYSICS_JOINT = 32768,  //* Physics joint/constraint
    SYN_COMPONENT_TERRAIN       = 65536,  //* Terrain
    SYN_COMPONENT_ZONE          = 131072, //* Trigger zone or area
    SYN_COMPONENT_CUSTOM        = 262144, //* Custom component (user-defined)
    SYN_COMPONENT_BILLBOARD     = 524288  //* Billboard component
};

/**
 * @brief Syngine::Component is the base class for all components that can be
 * attached to GameObject to inherit from. They are used to add functionality to
 * GameObjects, such as mesh, transform, physics, AI, PlayerController, etc.
 * @section Component
 */
class Component {
  public:
    /**
     * @brief Get the type of this component.
     * @return The component type as an enum value.
     */
    virtual Components GetComponentType() = 0;

    virtual std::unique_ptr<Component> Clone() const = 0;

    virtual ~Component() = default;

    bool isEnabled = true; //* Whether the component is enabled or not
    GameObject* m_owner = nullptr; //* The owner of the component, the GameObject it is attached to
};

}; // namespace Syngine
