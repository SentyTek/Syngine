// ╒════════════════════ ZoneComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-10-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"

#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Math/Quaternion.hpp"

#include <string>
#include <vector>
#include <functional>

namespace Syngine {

/// @brief Enum representing the shape of the zone.
/// @section ZoneComponent
enum class ZoneShape : uint8_t {
    BOX = 0, //* Axis-aligned box shape (size.x, size.y, size.z are width,
             // height, depth)
    SPHERE =
        1, //* Sphere shape (size.x is radius, size.y and size.z are ignored)
};

/// @brief ZoneComponent is used to define an area within the game world that
/// can be used to trigger events
/// @section ZoneComponent
/// @nameoverride ZoneComponent
/// @since v0.0.1
class ZoneComponent : public Syngine::Component {
    Math::Vector3 m_size =
        Math::Vector3(1.0f); //* Size of the zone (whd for box, r for sphere)
    Math::Vector3 m_pos =
        Math::Vector3(); //* Position of the zone center (world space)
    Math::Quaternion m_rot =
        Math::Quaternion(); //* Rotation of the zone (for box shape). This is a
                            //quaternion. Not used for sphere shape

    ZoneShape m_shape = ZoneShape::BOX; //* Shape of the zone (box or sphere)

    bool m_active = true; //* Whether the zone is active or not

    bool m_oneShot = false; //* Whether the zone is a one-shot zone
    std::vector<GameObject*>
        m_objectsIn; //* Objects that are currently in the zone
    std::vector<GameObject*>
        m_triggeredObjects; //* Objects that have triggered the zone (for one-shot zones)

    std::vector<std::string> m_tags; //* Tags of the zone

    GameObject* m_owner; // Reference to the owner game object
  public:
    static constexpr Syngine::ComponentTypeID componentType =
        SYN_COMPONENT_ZONE; //* Zone component type

    /// @brief Constructor for ZoneComponent
    /// @param owner The GameObject that owns this component.
    /// @param shape The shape of the zone (box or sphere).
    /// @param pos The position of the zone center.
    /// @param size The size of the zone (whd for box, r for sphere).
    /// @param oneShot Whether the zone is a one-shot zone (triggers only once per object).
    /// @note This should only be called by GameObject::AddComponent<T>()
    ZoneComponent(GameObject*          owner,
                  ZoneShape            shape,
                  const Math::Vector3& pos,
                  const Math::Vector3& size,
                  bool                 oneShot = false);

    ZoneComponent(const ZoneComponent& other);
    ZoneComponent& operator=(const ZoneComponent& other);

    /// @brief Clone the ZoneComponent
    /// @return A unique pointer to the cloned ZoneComponent
    std::unique_ptr<Component> Clone() const override {
        return std::make_unique<ZoneComponent>(*this);
    }

    /// @brief Serializes the ZoneComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// ZoneComponent's state
    Serializer::DataNode Serialize() const override;

    ~ZoneComponent();

    /// @brief Gets the component type of this component.
    /// @return The component type of this component.
    /// @threadsafety read-only
    /// @since v0.0.1
    Syngine::ComponentTypeID GetComponentType() override;

    /// @brief Initializes the zone component with shape, position, and size.
    /// @param shape The shape of the zone (box or sphere).
    /// @param pos The position of the zone center.
    /// @param size The size of the zone (whd for box, r for sphere).
    /// @param oneShot Whether the zone is a one-shot zone (triggers only once per object).
    /// @note This should only be called when the component is added to a GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void Init(ZoneShape            shape,
              const Math::Vector3& pos,
              const Math::Vector3& size,
              bool                 oneShot);

    /// @brief Update the zone component.
    /// @note There is no specific update logic for the zone component
    /// @since v0.0.1
    void Update() {};

    // --- Getters and setters

    /// @brief Get the shape of the zone.
    /// @return The shape of the zone (ZoneShape).
    /// @threadsafety read-only
    /// @since v0.0.1
    ZoneShape GetShape() const;

    /// @brief Get the position of the zone center.
    /// @return The position of the zone center as a Math::Vector3.
    /// @note For box shape, this is the center of the box. For sphere shape, this is the center of the sphere.
    /// @threadsafety read-only
    /// @since v0.0.1
    Math::Vector3 GetPosition() const;

    /// @brief Set the position of the zone center.
    /// @param pos The new position of the zone center (size 3).
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetPosition(const Math::Vector3& pos);

    /// @brief Get the size of the zone.
    /// @return The size of the zone as a Math::Vector3 (whd for box, r for sphere).
    /// @threadsafety read-only
    /// @since v0.0.1
    Math::Vector3 GetSize() const;

    /// @brief Set the size of the zone.
    /// @param size The new size of the zone (size 3).
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetSize(const Math::Vector3& size);

    /// @brief Get the rotation of the zone (for box shape).
    /// @return The rotation of the zone as a Math::Quaternion.
    /// @note For sphere shape, this will return the identity quaternion. For
    /// box shape, this represents the rotation of the box in world space.
    /// @threadsafety read-only
    /// @since v0.0.1
    Math::Quaternion GetRotation() const;

    /// @brief Set the rotation of the zone (for box shape).
    /// @param rot The new rotation of the zone as a Math::Quaternion.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetRotation(const Math::Quaternion& rot);

    /// @brief Check if the zone is active.
    /// @return True if the zone is active, false otherwise.
    /// @threadsafety read-only
    /// @since v0.0.1
    bool IsActive() const;

    /// @brief Set the active state of the zone.
    /// @param active True to activate the zone, false to deactivate.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetActive(bool active);

    /// @brief Check if the zone is a one-shot zone.
    /// @return True if the zone is a one-shot zone, false otherwise.
    /// @threadsafety read-only
    /// @since v0.0.1
    bool IsOneShot() const;

    /// @brief Add an object to the triggered list (for one-shot zones).
    /// @param object The GameObject to add.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _AddObject(GameObject* object);

    /// @brief Check if an object is in the triggered list (for one-shot zones).
    /// @param object The GameObject to check.
    /// @return True if the object is in the triggered list, false otherwise.
    /// Returns false if not a one-shot zone
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    bool _HasOneTimeObject(GameObject* object);

    /// @brief Check if an object is currently inside the zone.
    /// @param object The GameObject to check.
    /// @return True if the object is inside the zone, false otherwise.
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    bool _IsTrackingObject(GameObject* object) const;

    /// @brief Remove an object from the triggered list (for one-shot zones).
    /// @param object The GameObject to remove.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _RemoveObject(GameObject* object);

    /// @brief Add a tag to the zone.
    /// @param tag The tag to add.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void AddTag(const std::string& tag);

    /// @brief Remove a tag from the zone.
    /// @param tag The tag to remove.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void RemoveTag(const std::string& tag);

    /// @brief Check if the zone has a specific tag.
    /// @param tag The tag to check.
    /// @return True if the zone has the specified tag, false otherwise.
    /// @threadsafety read-only
    /// @since v0.0.1
    bool HasTag(const std::string& tag) const;

    /// @brief Get all tags of the zone.
    /// @return A vector of all tags of the zone.
    /// @threadsafety read-only
    /// @since v0.0.1
    std::vector<std::string> GetTags() const;

    /// @brief Set several tags to the zone at once, replacing existing tags.
    /// @param tags A vector of tags to set.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetTags(const std::vector<std::string>& tags);

    // --- Actual zone functions

    /// @brief Check if a point is inside the zone.
    /// @param point The point to check (size 3).
    /// @return True if the point is inside the zone, false otherwise.
    /// @threadsafety read-only
    /// @since v0.0.1
    bool IsInZone(const Math::Vector3& point) const;

    /// @brief Check if a GameObject is inside the zone.
    /// @param object The GameObject to check.
    /// @return True if the GameObject is inside the zone, false otherwise.
    /// @pre The GameObject must have a TransformComponent.
    /// @threadsafety read-only
    /// @since v0.0.1
    bool IsInZone(const GameObject* object) const;

    /// @brief Get all GameObjects inside the zone.
    /// @return A vector of pointers to GameObjects inside the zone.
    /// @pre The GameObjects must have a TransformComponent.
    /// @threadsafety read-only
    /// @since v0.0.1
    std::vector<GameObject*> GetObjectsInZone() const;

    /// @brief Get all GameObjects inside the zone with a specific tag.
    /// @param tag The tag to filter GameObjects.
    /// @return A vector of pointers to GameObjects inside the zone with the specified tag.
    /// @pre The GameObjects must have a TransformComponent.
    /// @threadsafety read-only
    /// @since v0.0.1
    std::vector<GameObject*>
    GetObjectsInZoneByTag(const std::string& tag) const;

    /// @brief Get the owner GameObject of this zone component.
    /// @return Pointer to the owner GameObject.
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    GameObject* _GetOwner() const;

    std::function<void(GameObject*)> OnEnter; //* Function called when an object enters the zone
    std::function<void(GameObject*)> OnExit;  //* Function called when an object exits the zone
};

} // namespace Syngine
