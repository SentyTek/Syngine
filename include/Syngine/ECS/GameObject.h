// ╒═══════════════════════ GameObject.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include <memory>
#include <string>
#include <map>
#include <vector>

#include "Syngine/Core/Registry.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/AllComponents.h"

using namespace std;

namespace Syngine {

/// @brief GameObject class representing an entity in the game world, such as a
/// player, enemy, or item. They can have various components attached to them,
/// such as a mesh, physics, AI, etc.
/// @section GameObject
/// @since v0.0.1
class GameObject {
  public:
    string name; //* Name of the GameObject, used for identification and
                 //* debugging.

    string type; //* Type of the GameObject, used for shaders.

    string gizmo; //* Gizmo type for rendering in the editor, e.g.,
                  //* "camera_render", "mesh_render"

    std::vector<std::string> tags; //* Tags for grouping and identifying GameObjects

    /// @brief Constructor for the GameObject class
    /// @param name Name of the GameObject
    /// @param type Type of the GameObject, defaults to "default"
    /// @since v0.0.1
    GameObject(string name, string type = "default", string initialTag = "");

    GameObject(const GameObject& other);
    GameObject& operator=(const GameObject& other);

    ~GameObject();

    // Operator overload yippee
    bool operator==(const GameObject& other) const {
        return this->id == other.id;
    }

    /// @brief Get the ID of the GameObject
    /// @return ID of the GameObject
    /// @threadsafety safe
    /// @since v0.0.1
    inline long GetID() noexcept { return this->id; };

    /// @brief Set the ID of the GameObject
    /// @param id ID of the GameObject
    /// @threadsafety safe
    /// @since v0.0.1
    /// @internal
    inline void _SetID(long id) noexcept { this->id = id; }

    /// @brief Check if the GameObject is active
    /// @return true if the GameObject is active, false otherwise
    /// @threadsafety safe
    /// @since v0.0.1
    inline bool IsActive() const noexcept { return this->isActive; }

    /// @brief Set the active state of the GameObject
    /// @param active true to set the GameObject as active, false to set it as
    /// inactive
    /// @threadsafety safe
    /// @since v0.0.1
    void SetActive(bool active) noexcept;

    /// @brief Get the tags of the GameObject
    /// @return Vector of tags of the GameObject
    /// @threadsafety safe
    /// @since v0.0.1
    inline std::vector<std::string> GetTags() const noexcept { return tags; }

    /// @brief Add a tag to the GameObject
    /// @param tag Tag to add
    /// @threadsafety not-safe
    /// @since v0.0.1
    void AddTag(const std::string& tag);

    /// @brief Remove a tag from the GameObject
    /// @param tag Tag to remove
    /// @threadsafety not-safe
    /// @since v0.0.1
    void RemoveTag(const std::string& tag);

    /// @brief Check if the GameObject has a specific tag
    /// @param tag Tag to check
    /// @return true if the tag exists, false otherwise
    /// @threadsafety safe
    /// @since v0.0.1
    bool HasTag(const std::string& tag) const;

    /// @brief Clear all tags from the GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    void ClearTags();

    /// @brief Get the number of components attached to the GameObject
    /// @return Number of components attached to the GameObject
    /// @threadsafety safe
    /// @since v0.0.1
    size_t GetComponentCount() const noexcept;

    /// @brief Remove a component from the GameObject
    /// @param type Type of the component to remove
    /// @return 0 on success, -1 if the component was not found
    /// @threadsafety not-safe
    /// @since v0.0.1
    int RemoveComponent(Syngine::Components type);

    /// @brief Check if the GameObject has a component of the specified type
    /// @param type Type of the component to check
    /// @return true if the component exists, false otherwise
    /// @threadsafety safe
    /// @since v0.0.1
    bool HasComponent(Syngine::Components type);

    /// @brief Get a component of the specified type
    /// @param T Type of the component to get
    /// @return Pointer to the component if it exists, nullptr otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    template <typename T> T* GetComponent() const;

    /// @brief Add a component to the GameObject
    /// @param T Type of the component to add
    /// @param args Arguments to pass to the component constructor
    /// @return Pointer to the added component if successful, nullptr otherwise
    /// @threadsafety not-safe
    /// @since v0.0.1
    template <typename T, typename... Args> T* AddComponent(Args&&... args);

  private:
    long id; // Unique ID for the GameObject

    std::map<Syngine::Components, unique_ptr<Syngine::Component>>
        components; // Map of components attached to the GameObject

    bool isActive = true; // Whether the GameObject is active or not
};

template<typename T>
T* GameObject::GetComponent() const {
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
