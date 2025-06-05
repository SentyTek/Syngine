#include "SyngineGameobject.h"

#include "Components/MeshComponent.h"
#include "Components/TransformComponent.h"
#include "Components/PhysComponent.h"

GameObject::GameObject(string name, string type) {
    this->name = name;
    this->type = type;
    this->id = -1;
}

GameObject::~GameObject() {}

long GameObject::GetID() {
    return this->id;
}

int GameObject::AddComponent(SynComponents type) {
    auto it = this->components.find(type);
    if (it != this->components.end()) {
        return 1; // Component already exists
    }
    
    SynComponent* component = nullptr;
    switch (type) {
        case SYN_COMPONENT_MESH:
            component = new MeshComponent(this);
            break;
        case SYN_COMPONENT_TRANSFORM:
            component = new TransformComponent(this);
            break;
        // Add cases for other components here
        case SYN_COMPONENT_PHYSICS:
            component = new Syngine::PhysicsComponent(this);
            break;
        default:
            return 2; // Unknown component type
    }
    
    this->components[type] = unique_ptr<SynComponent>(component);
    return 0;
}

int GameObject::RemoveComponent(SynComponents type) {
    auto it = this->components.find(type);
    if (it == this->components.end()) {
        return 1; // Component not found
    }
    
    this->components.erase(it);
    return 0;
}

bool GameObject::HasComponent(SynComponents type) {
    return this->components.find(type) != this->components.end();
}