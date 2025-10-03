// ╒═════════════════════ GameObject.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Registry.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"

#include <type_traits>

using namespace Syngine;

GameObject::GameObject(string name, string type) {
    this->name = name;
    this->type = type;
    this->id   = -1;
    this->gizmo = "none";

    Registry::AddGameObject(this);
}

GameObject::GameObject(const GameObject& other) {
    this->name = other.name;
    this->type = other.type;
    this->gizmo = other.gizmo;
    this->id   = other.id;
    this->isActive = other.isActive;

    // Deep copy components
    for (const auto& [type, comp] : other.components) {
        // Use the AddComponent method to ensure proper registration
        this->AddComponent<typename std::remove_reference_t<decltype(*comp)>>(*comp);
    }

    Registry::AddGameObject(this);
}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this == &other) return *this; // Self-assignment check

    this->name = other.name;
    this->type = other.type;
    this->gizmo = other.gizmo;
    this->id   = other.id;
    this->isActive = other.isActive;

    // Clear existing components
    this->components.clear();

    // Deep copy components
    for (const auto& [type, comp] : other.components) {
        // Use the AddComponent method to ensure proper registration
        this->AddComponent<typename std::remove_reference_t<decltype(*comp)>>(*comp);
    }

    return *this;
}

GameObject::~GameObject() {
    Registry::RemoveGameObject(this);
}

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
