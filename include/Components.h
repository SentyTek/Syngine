#pragma once

/*
    An enum of all the components that can be attached to a gameobject.
*/
enum SynComponents {
    SYN_COMPONENT_NONE = 0,
    SYN_COMPONENT_MESH = 1,
    SYN_COMPONENT_TRANSFORM = 2,
    SYN_COMPONENT_PHYSICS = 4,
    SYN_COMPONENT_AI = 8,
    SYN_COMPONENT_PLAYER_CONTROLLER = 16,
    SYN_COMPONENT_CAMERA = 32,
    SYN_COMPONENT_LIGHT = 64
};

#include "SyngineGameobject.h"
/*
Syngine Components are the base class for all components attached to gameobjects.
They are used to add functionality to gameobjects, such as mesh, transform, physics, AI, PlayerController, etc.
*/
class SynComponent {
    public:
    virtual SynComponents getComponentType() = 0; //Get the type of component
    virtual ~SynComponent() = default; //Virtual destructor for cleanup
    bool isEnabled = true;
    GameObject* m_owner = nullptr; //The owner of the component, the gameobject it is attached to
};