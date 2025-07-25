#pragma once

namespace Syngine {
// Forward declaration
class GameObject;

/*
    An enum of all the components that can be attached to a gameobject.
*/
enum Components {
    SYN_COMPONENT_MESH          = 1,     // 3D mesh or model
    SYN_COMPONENT_TRANSFORM     = 2,     // Position, rotation, scale
    SYN_COMPONENT_AI            = 4,     // AI logic/behavior
    SYN_COMPONENT_PLAYER        = 8,     // Player controller
    SYN_COMPONENT_CAMERA        = 16,    // Camera logic
    SYN_COMPONENT_LIGHT         = 32,    // Lighting
    SYN_COMPONENT_AUDIO         = 64,    // Audio source/listener
    SYN_COMPONENT_PARTICLE      = 128,   // Particle system
    SYN_COMPONENT_UI            = 256,   // UI elements
    SYN_COMPONENT_SCRIPT        = 512,   // Custom scripting
    SYN_COMPONENT_ANIMATION     = 1024,  // Animation controller
    SYN_COMPONENT_RIGIDBODY     = 2048,  // Physics rigidbody
    SYN_COMPONENT_PHYSICS_MOTOR = 4096,  // Physics motor/actuator
    SYN_COMPONENT_PHYSICS_CLOTH = 8192,  // Cloth simulation
    SYN_COMPONENT_PHYSICS_JOINT = 16384, // Physics joint/constraint
    SYN_COMPONENT_TERRAIN       = 32768, // Terrain
    SYN_COMPONENT_ZONE          = 65536, // Trigger zone or area
    SYN_COMPONENT_CUSTOM        = 131072, // Custom component (user-defined)

    // Composite types
    SYN_COMPONENT_RENDERABLE = SYN_COMPONENT_MESH | SYN_COMPONENT_TRANSFORM,
    SYN_COMPONENT_PHYSICS = SYN_COMPONENT_RENDERABLE | SYN_COMPONENT_RIGIDBODY,
};

/*
Syngine Components are the base class for all components attached to
gameobjects. They are used to add functionality to gameobjects, such as mesh,
transform, physics, AI, PlayerController, etc.
*/
class Component {
    public:
    virtual Components getComponentType() = 0; //Get the type of component
    virtual ~Component() = default; //Virtual destructor for cleanup
    bool isEnabled = true;
    GameObject* m_owner = nullptr; //The owner of the component, the gameobject it is attached to
};

}; // namespace Syngine