// ╒═════════════════════ GameObject.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Scene/GameObjectRegistry.h>
#include <Syngine/GameObjects/Component.h>
#include <Syngine/GameObjects/Components/RigidbodyComponent.h>
#include <Syngine/GameObjects/Components/TransformComponent.h>
#include <Syngine/Utils/Serializer.h>
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/GameObjects/ComponentRegistry.h>

using namespace Syngine;

GameObject::GameObject(std::string              name,
                       std::string              type,
                       std::vector<std::string> initialTags)
    : name(std::move(name)), type(std::move(type)),
      tags(std::move(initialTags)), id(-1), gizmo("none") {}

GameObject::GameObject(const Serializer::DataNode& data) {
    this->name     = data["name"].As<std::string>();
    this->type     = data["type"].As<std::string>();
    this->gizmo    = data["gizmo"].As<std::string>();
    this->isActive = data["isActive"].As<bool>();
    if (data.Has("tags")) {
        this->tags = data["tags"].As<std::vector<std::string>>();
    }

    // Deserialize components
    if (data.Has("components")) {
        const auto& componentsNode = data["components"];
        for (const auto& [typeStr, data] :
             componentsNode.As<Serializer::DataNode::NodeMap>()) {
            Syngine::ComponentTypeID    typeId = std::stoull(typeStr);
            std::unique_ptr<IComponent> comp =
                ComponentRegistry::Instantiate(typeId, this, data);
            if (comp) {
                this->components[typeId] = std::move(comp);
                GameObjectRegistry::_NotifyComponentAdded(this, typeId);
            }
        }
    }

    // Children.
    if (data.Has("children")) {
        const auto& childrenNode = data["children"];
        for (const auto& childData :
             childrenNode.As<Serializer::DataNode::NodeArray>()) {
            GameObject* child = new GameObject(childData);
            this->AddChild(child);
        }
    }
}

GameObject::~GameObject() {
    // Notify parent that this game object is being removed
    if (this->GetParent()) {
        this->GetParent()->RemoveChild(this);
    }

    // Delete components
    this->components.clear();
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

void GameObject::SetTags(const std::vector<std::string>& newTags) {
    tags = newTags;
}

size_t GameObject::GetComponentCount() const noexcept {
    return this->components.size();
}

bool GameObject::RemoveComponent(Syngine::ComponentTypeID type) {
    auto it = this->components.find(type);
    if (it == this->components.end()) {
        return false; // Component not found
    }

    GameObjectRegistry::_NotifyComponentRemoved(this, type);
    return true;
}

bool GameObject::HasComponent(Syngine::ComponentTypeID type) {
    // Check if the component exists in the map
    auto it = this->components.find(type);
    if (it != this->components.end()) return true; // Component found
    return false;
}

IComponent* GameObject::GetComponent(Syngine::ComponentTypeID type) const {
    auto it = this->components.find(type);
    if (it == this->components.end()) {
        return nullptr; // Component not found
    }

    return it->second.get();
}

Serializer::DataNode GameObject::Serialize() const {
    Serializer::DataNode node;
    node["name"]     = this->name;
    node["type"]     = this->type;
    node["gizmo"]    = this->gizmo;
    node["isActive"] = this->isActive;

    // Serialize tags
    std::vector<std::string> tagList = this->tags;
    node["tags"]                     = tagList;

    // Serialize components
    Serializer::DataNode componentsNode;
    for (const auto& [type, comp] : this->components) {
        componentsNode[std::to_string(type)] = comp->Serialize();
    }
    node["components"] = componentsNode;

    // Serialize parent-child relationships
    // This assumes that the TransformComponent is responsible for parent-child
    // relationships
    TransformComponent* tComp = this->GetComponent<TransformComponent>();
    if (tComp) {
        Serializer::DataNode childrenNodes;
        for (GameObject* child : this->GetChildren()) {
            if (child) {
                Serializer::DataNode childNode = child->Serialize();
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
    TransformComponent* tComp = this->GetComponent<TransformComponent>();
    TransformComponent* parentTComp =
        parent->GetComponent<TransformComponent>();
    if (tComp && parentTComp) {
        tComp->SetParent(parentTComp);
        parent->AddChild(this);
    } else {
        Syngine::Logger::Warn(
            "GameObject has no TransformComponent, cannot set parent");
    }
}

GameObject* GameObject::GetParent() const {
    TransformComponent* tComp = this->GetComponent<TransformComponent>();
    if (tComp) {
        TransformComponent* parentT = tComp->GetParent();
        if (parentT && parentT->m_owner) {
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
        Syngine::Logger::Error("Child GameObject must have a "
                               "TransformComponent to be added as a child");
    }
}

const std::vector<GameObject*>& GameObject::GetChildren() const {
    TransformComponent* tComp = this->GetComponent<TransformComponent>();
    static std::vector<GameObject*> children;
    // can't just return the transform's children because that would just return
    // a bunch of transforms, so we have to construct the vector ourselves from
    // the transforms owners
    if (tComp) {
        auto childTComps = tComp->GetChildren();
        children.clear();
        for (TransformComponent* childTComp : childTComps) {
            if (childTComp && childTComp->m_owner) {
                children.push_back(childTComp->m_owner);
            }
        }
    }
    return children; // Return empty vector if no TransformComponent
}

bool GameObject::CanBeParentedTo(const GameObject* potentialParent) const {
    if (potentialParent == nullptr) return true;
    if (potentialParent == this) return false;
    const GameObject* current = potentialParent;
    while (current) {
        if (current == this) return false;
        current = current->GetParent();
    }
    return true;
}
