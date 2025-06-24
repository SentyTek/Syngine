#include "SyngineGameobject.h"

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

long GameObject::GetID() {
    return this->id;
}

int GameObject::AddComponent(Syngine::Components type) {
    auto it = this->components.find(type);
    if (it != this->components.end()) {
        return 1; // Component already exists
    }
    
    Syngine::Component* component = nullptr;
    switch (type) {
        case Syngine::SYN_COMPONENT_MESH:
            component = new MeshComponent(this);
            break;
        case Syngine::SYN_COMPONENT_TRANSFORM:
            component = new TransformComponent(this);
            break;
        case Syngine::SYN_COMPONENT_RIGIDBODY:
            component = new RigidbodyComponent(this);
            break;
        case Syngine::SYN_COMPONENT_PLAYER:
            component = new PlayerComponent(this);
            break;
        case Syngine::SYN_COMPONENT_CAMERA:
            component = new CameraComponent(this);
            gizmo = "camera_render";
            break;
        default:
            return 2; // Unknown component type
    }
    
    this->components[type] = unique_ptr<Syngine::Component>(component);
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
    return this->components.find(type) != this->components.end();
}