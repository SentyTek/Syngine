// ╒═════════════════════ GameObject.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Registry.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/Components/RigidbodyComponent.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/Utils/Serializer.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include <unordered_map>


using namespace Syngine;

GameObject::GameObject(std::string name, std::string type, std::string initialTag) {
    this->name = name;
    this->type = type;
    this->tags.push_back(initialTag);
    this->id   = -1;
    this->gizmo = "none";

    Registry::AddGameObject(this);
}

GameObject::GameObject(const Serializer::DataNode& data) {
    this->name = data["name"].As<std::string>();
    this->type = data["type"].As<std::string>();
    this->gizmo = data["gizmo"].As<std::string>();
    this->isActive = data["isActive"].As<bool>();
    if (data.Has("tags")) {
        this->tags = data["tags"].As<std::vector<std::string>>();
    }

    // Deserialize components
    if (data.Has("components")) {
        const auto& componentsNode = data["components"];
        for (const auto& [typeStr, data] : componentsNode.As<Serializer::DataNode::NodeMap>()) {
            Syngine::ComponentTypeID typeId = std::stoull(typeStr);
            std::unique_ptr<Component> comp = ComponentRegistry::Instantiate(typeId, this, data);
            if (comp) {
                this->components[typeId] = std::move(comp);
                Registry::_NotifyComponentAdded(this, typeId);
            }
        }
    }

    Registry::AddGameObject(this);

    // Children.
    if (data.Has("children")) {
        const auto& childrenNode = data["children"];
        for (const auto& childData : childrenNode.As<Serializer::DataNode::NodeArray>()) {
            GameObject* child = new GameObject(childData);
            this->AddChild(child);
        }
    }
}

GameObject::GameObject(const GameObject& other) {
    this->name = other.name;
    this->type = other.type;
    this->gizmo = other.gizmo;
    this->id   = other.id;
    this->isActive = other.isActive;

    // Deep copy components
    for (const auto& [type, comp] : other.components) {
        this->components[type] = comp->Clone();
        this->components[type]->m_owner = this;
    }

    // Register the new GameObject
    // This should ensure it can be categorized properly
    Registry::AddGameObject(this);
}

GameObject& GameObject::operator=(const GameObject& other) {
    if (this == &other) return *this; // Self-assignment check

    // Unregister the current GameObject
    Registry::RemoveGameObject(this);

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
        this->components[type] = comp->Clone();
        this->components[type]->m_owner = this;
        Registry::_NotifyComponentAdded(this, type);
    }

    // Register the new GameObject
    // This should ensure it can be categorized properly
    Registry::AddGameObject(this);

    return *this;
}

GameObject::~GameObject() {
    Registry::RemoveGameObject(this);
}

void GameObject::SetActive(bool active) noexcept { this->isActive = active; }

void GameObject::AddTag(const std::string& tag) {
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }
}

void GameObject::RemoveTag(const std::string& tag) {
    auto it = std::find(tags.begin(), tags.end(), tag);
    if (it != tags.end()) {
        tags.erase(it);
    }
}

bool GameObject::HasTag(const std::string& tag) const {
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

void GameObject::ClearTags() { tags.clear(); }

size_t GameObject::GetComponentCount() const noexcept {
    return this->components.size();
}

int GameObject::RemoveComponent(Syngine::ComponentTypeID type) {
    auto it = this->components.find(type);
    if (it == this->components.end()) {
        return -1; // Component not found
    }

    this->components.erase(it);
    Registry::_NotifyComponentRemoved(this, type);
    return 0;
}

bool GameObject::HasComponent(Syngine::ComponentTypeID type) {
    // Check if the component exists in the map
    auto it = this->components.find(type);
    if (it != this->components.end()) return true; // Component found
    return false;
}

Component* GameObject::GetComponent(Syngine::ComponentTypeID type) const {
    auto it = this->components.find(type);
    if (it == this->components.end()) {
        return nullptr; // Component not found
    }

    return it->second.get();
}

Serializer::DataNode GameObject::Serialize() const {
    Serializer::DataNode node;
    node["name"] = this->name;
    node["type"] = this->type;
    node["gizmo"] = this->gizmo;
    node["isActive"] = this->isActive;

    // Serialize tags
    std::vector<std::string> tagList = this->tags;
    node["tags"] = tagList;

    // Serialize components
    Serializer::DataNode componentsNode;
    for (const auto& [type, comp] : this->components) {
        componentsNode[std::to_string(type)] = comp->Serialize();
    }
    node["components"] = componentsNode;

    // Serialize parent-child relationships
    // This assumes that the TransformComponent is responsible for parent-child relationships
    TransformComponent* tComp = this->GetComponent<TransformComponent>();
    if (tComp) {
        Serializer::DataNode childrenNodes;
        for (TransformComponent* child : tComp->GetChildren()) {
            if (child && child->m_owner) {
                Serializer::DataNode childNode = child->m_owner->Serialize();
                childrenNodes.Append(childNode);
            }
        }
        node["children"] = childrenNodes;
    }

    return node;
}

// Parent system relies on TransformComponent

void GameObject::SetParent(GameObject* parent) {
    if (parent == this) {
        Syngine::Logger::Warn("GameObject cannot be its own parent");
        return;
    }
    if (parent == nullptr) {
        // Detach from current parent
        TransformComponent* tComp = this->GetComponent<TransformComponent>();
        if (tComp) {
            tComp->SetParent(nullptr);
        }
        return;
    }
    this->GetComponent<TransformComponent>()->SetParent(parent->GetComponent<TransformComponent>());
    parent->AddChild(this);
}

GameObject* GameObject::GetParent() const {
    TransformComponent* tComp = this->GetComponent<TransformComponent>();
    if (tComp) {
        TransformComponent* parentT = tComp->GetParent();
        if (parentT) {
            return parentT->m_owner;
        }
    }
    return nullptr;
}

void GameObject::RemoveChild(GameObject* child) {
    TransformComponent* tComp = child->GetComponent<TransformComponent>();
    if (tComp) {
        tComp->SetParent(nullptr);
    }
}

void GameObject::AddChild(GameObject* child) {
    if (child == this) {
        Syngine::Logger::Warn("GameObject cannot be a child of itself");
        return;
    }
    if (child == nullptr) {
        Syngine::Logger::Warn("Cannot add null child to GameObject");
        return;
    }
    TransformComponent* tComp = child->GetComponent<TransformComponent>();
    if (tComp) {
        tComp->SetParent(this->GetComponent<TransformComponent>());

        // Prefab children can create rigidbodies before parent linkage.
        // After parenting, push the updated world pose into the physics body.
        if (auto* rb = child->GetComponent<RigidbodyComponent>()) {
            rb->SyncBodyToTransform();
        }
    } else {
        Syngine::Logger::Error("Child GameObject must have a TransformComponent to be added as a child");
    }
}
