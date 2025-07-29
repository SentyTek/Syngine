#include "Components.h"
#include "SyngineGameobject.h"

using namespace Syngine;

GameObject::GameObject(string name, string type) {
    this->name = name;
    this->type = type;
    this->id   = -1;
    this->gizmo = "none";
}

GameObject::~GameObject() {}

void GameObject::SetActive(bool active) noexcept {
    this->isActive = active;
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
    if (it != this->components.end()) return true; // Component found
    return false;
}
