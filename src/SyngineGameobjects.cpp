#include "Components.h"
#include "SyngineGameobject.h"
#include "Registry.h"

#include "Components/MeshComponent.h"
#include "Components/TransformComponent.h"
#include "Components/RigidbodyComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/CameraComponent.h"

using namespace Syngine;

GameObject::GameObject(string name, string type) {
    this->name = name;
    this->type = type;
    this->id   = -1;
    this->gizmo = "none";
}

GameObject::~GameObject() {}

void GameObject::SetID(long id) noexcept { this->id = id; }
void GameObject::SetActive(bool active) noexcept {
    this->isActive = active;
}

int GameObject::AddComponent(Syngine::Components type) {
    auto it = this->components.find(type);
    if (it != this->components.end()) {
        return 1; // Component already exists
    }

    // Create and add the component based on type
    Syngine::Component* component = nullptr;
    switch (type) {
        case Syngine::SYN_COMPONENT_MESH:
            component = new MeshComponent(this);
            this->components[type] = unique_ptr<Syngine::Component>(component);
            break;
        case Syngine::SYN_COMPONENT_TRANSFORM:
            component = new TransformComponent(this);
            this->components[type] = unique_ptr<Syngine::Component>(component);
            break;
        case Syngine::SYN_COMPONENT_RIGIDBODY:
            component = new RigidbodyComponent(this);
            this->components[type] = unique_ptr<Syngine::Component>(component);
            break;
        case Syngine::SYN_COMPONENT_PLAYER:
            component = new PlayerComponent(this);
            this->components[type] = unique_ptr<Syngine::Component>(component);
            break;
        case Syngine::SYN_COMPONENT_CAMERA:
            component = new CameraComponent(this);
            gizmo = "camera_render";
            this->components[type] = unique_ptr<Syngine::Component>(component);
            break;
        case Syngine::SYN_COMPONENT_RENDERABLE: // Alias for Mesh + Transform
            this->components[SYN_COMPONENT_MESH] =
                unique_ptr<Syngine::Component>(new MeshComponent(this));
            this->components[SYN_COMPONENT_TRANSFORM] =
                unique_ptr<Syngine::Component>(new TransformComponent(this));
        case Syngine::SYN_COMPONENT_PHYSICS: // Alias for Mesh + Transform + Rigidbody
            this->components[SYN_COMPONENT_RIGIDBODY] =
                unique_ptr<Syngine::Component>(new RigidbodyComponent(this));
            this->components[SYN_COMPONENT_MESH] =
                unique_ptr<Syngine::Component>(new MeshComponent(this));
            this->components[SYN_COMPONENT_TRANSFORM] =
                unique_ptr<Syngine::Component>(new TransformComponent(this));
        default:
            return 2; // Unknown component type
    }
    

    return 0;
}

int GameObject::RemoveComponent(Syngine::Components type) {
    auto it = this->components.find(type);
    if (it == this->components.end()) {
        return 1; // Component not found
    }
    
    this->components.erase(it);
    return 0;
}

bool GameObject::HasComponent(Syngine::Components type) {
    // Check if the component exists in the map
    auto it = this->components.find(type);
    if (it != this->components.end()) return true;

    // Component not found, might be a composite type
    switch (type) {
        case Syngine::SYN_COMPONENT_RENDERABLE:
            return HasComponent(Syngine::SYN_COMPONENT_MESH) &&
                   HasComponent(Syngine::SYN_COMPONENT_TRANSFORM);
        case Syngine::SYN_COMPONENT_PHYSICS:
            return HasComponent(Syngine::SYN_COMPONENT_MESH) &&
                   HasComponent(Syngine::SYN_COMPONENT_TRANSFORM) &&
                   HasComponent(Syngine::SYN_COMPONENT_RIGIDBODY);
        default:
            return false; // No composite type match
    }
    return false; // Default case, should not be reached
}