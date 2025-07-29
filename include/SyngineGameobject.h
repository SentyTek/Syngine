#pragma once
#include <memory>
#include <string>
#include <map>
#include "Components.h"
#include "Registry.h"
#include "SyngineLogger.h"
#include "SynComponents.h"

using namespace std;

/*
    Syngine Gameobjects are the base class for all game objects used within a
   game. They are used to represent objects in the game world, such as players,
   enemies, and items. They are also used to represent objects that are not
   directly related to the game world, such as UI elements and menus.

    Gameobjects can then have various components attached to them, such as a
   mesh, transform, physics, AI, PlayerController, etc.
*/
namespace Syngine {

class GameObject {
  private:
    // Unique ID for the GameObject
    long id;
    // Map of components attached to the GameObject
    map<Syngine::Components, unique_ptr<Syngine::Component>> components;
    // Whether the GameObject is active or not
    bool isActive = true;
  public:
     // Name of the GameObject, used for identification and debugging
    string name;
    // Type of the GameObject, used for categorization and filtering.
    // Type can be used to categorize gameobjects, e.g., "player", "enemy", "item"
    string type;
    // Gizmo type for rendering in the editor, e.g., "camera_render", "mesh_render"
    string gizmo;

    GameObject(string name, string type = "default");
    ~GameObject();

    // Get the ID of the GameObject
    inline long GetID() noexcept { return this->id; };
    // Set the ID of the GameObject
    inline void _SetID(long id) noexcept { this->id = id; }
    // Check if the GameObject is active
    inline bool IsActive() const noexcept { return this->isActive; }
    // Set the active state of the GameObject
    void SetActive(bool active) noexcept;
    // Remove a component from the GameObject
    int RemoveComponent(Syngine::Components type);
    // Check if the GameObject has a component of the specified type
    bool HasComponent(Syngine::Components type);

    // Get a component of the specified type
    template <typename T> T* GetComponent();
    
    // Add a component to the GameObject
    template <typename T, typename... Args> T* AddComponent(Args&&... args);
};

template<typename T>
T* GameObject::GetComponent() {
    auto it = this->components.find(T::componentType);
    if (it == this->components.end()) {
        return nullptr; // Component not found
    }
    
    return dynamic_cast<T*>(it->second.get());
}

template <typename T, typename... Args>
T* GameObject::AddComponent(Args&&... args) {
    auto type = T::componentType;

    if (components.contains(type)) return nullptr;

    // Forward the arguments to the component constructor
    std::unique_ptr<T> component = std::make_unique<T>(this, std::forward<Args>(args)...);
    T* raw = component.get();

    components[type] = std::move(component);
    Syngine::Registry::_NotifyComponentAdded(this, type);
    return raw;
}

} // namespace Syngine
