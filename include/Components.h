#pragma once

namespace Syngine {
// Forward declaration
class GameObject;

/*
    An enum of all the components that can be attached to a gameobject.
*/
enum Components {
    SYN_COMPONENT_NONE          = 0,
    SYN_COMPONENT_MESH          = 1,
    SYN_COMPONENT_TRANSFORM     = 2,
    SYN_COMPONENT_AI            = 4,
    SYN_COMPONENT_PLAYER        = 8,
    SYN_COMPONENT_CAMERA        = 16,
    SYN_COMPONENT_LIGHT         = 32,
    SYN_COMPONENT_AUDIO         = 64,
    SYN_COMPONENT_PARTICLE      = 128,
    SYN_COMPONENT_UI            = 256,
    SYN_COMPONENT_SCRIPT        = 512,
    SYN_COMPONENT_ANIMATION     = 1024,
    SYN_COMPONENT_RIGIDBODY   = 2048,
    SYN_COMPONENT_PHYSICS_MOTOR = 4096,
    SYN_COMPONENT_PHYSICS_CLOTH = 8192,
    SYN_COMPONENT_PHYSICS =
        SYN_COMPONENT_RIGIDBODY | SYN_COMPONENT_PHYSICS_MOTOR |
        SYN_COMPONENT_PHYSICS_CLOTH, // Combined physics component
    SYN_COMPONENT_TERRAIN = 16384 | SYN_COMPONENT_MESH |
                            SYN_COMPONENT_RIGIDBODY
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