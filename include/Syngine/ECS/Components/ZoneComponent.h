// ╒════════════════════ ZoneComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-10-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace Syngine {

/// @brief Enum representing the shape of the zone.
/// @section ZoneComponent
enum class ZoneShape {
    BOX = 0,
    SPHERE = 1,
};

/// @brief ZoneComponent is used to define an area within the game world that
/// can be used to trigger events
/// @section ZoneComponent
class ZoneComponent : public Syngine::Component {
    float m_size[3] = { 1.0f,
                        1.0f,
                        1.0f }; //* Size of the zone (whd for box, r for sphere)

    ZoneShape m_shape = ZoneShape::BOX; //* Shape of the zone (box or sphere)

    float m_pos[3] = { 0.0f, 0.0f, 0.0f }; //* Position of the zone center

    float m_rot[3] = {
        0.0f, 0.0f, 0.0f
    }; //* Rotation of the zone (for box shape). This is a
       // Euler angle in degrees. Not used for sphere shape

    bool m_active = true; //* Whether the zone is active or not

    bool m_oneShot = false; //* Whether the zone is a one-shot zone
    std::vector<GameObject*> m_triggeredObjects; //* Objects that have been triggered

    std::vector<std::string> m_tags; //* Tags of the zone

    GameObject* m_owner; // Reference to the owner game object
  public:
    static constexpr Syngine::Components componentType =
        SYN_COMPONENT_ZONE; //* Zone component type

    /// @brief Constructor for ZoneComponent
    /// @param owner The GameObject that owns this component.
    /// @param shape The shape of the zone (box or sphere).
    /// @param pos The position of the zone center.
    /// @param size The size of the zone (whd for box, r for sphere).
    /// @param oneShot Whether the zone is a one-shot zone (triggers only once per object).
    /// @note This should only be called by GameObject::AddComponent<T>()
    ZoneComponent(GameObject* owner,
                  ZoneShape   shape   = ZoneShape::BOX,
                  const float pos[3]  = (const float[]){ 0.0f, 0.0f, 0.0f },
                  const float size[3] = (const float[]){ 1.0f, 1.0f, 1.0f },
                  bool        oneShot = false);

    ZoneComponent(const ZoneComponent& other);
    ZoneComponent& operator=(const ZoneComponent& other);

    ~ZoneComponent();

    /// @brief Gets the component type of this component.
    /// @return The component type of this component.
    /// @threadsafety read-only
    /// @since v0.0.1
    Components GetComponentType() override;

    /// @brief Initializes the zone component with shape, position, and size.
    /// @param shape The shape of the zone (box or sphere).
    /// @param pos The position of the zone center.
    /// @param size The size of the zone (whd for box, r for sphere).
    /// @param oneShot Whether the zone is a one-shot zone (triggers only once per object).
    /// @note This should only be called when the component is added to a GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void Init(ZoneShape   shape   = ZoneShape::BOX,
              const float pos[3]  = (const float[]){ 0.0f, 0.0f, 0.0f },
              const float size[3] = (const float[]){ 1.0f, 1.0f, 1.0f },
              bool        oneShot = false);

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
    /// @param outPos Output array to store the position (size 3).
    /// @threadsafety read-only
    /// @since v0.0.1
    void GetPosition(float outPos[3]) const;

    /// @brief Set the position of the zone center.
    /// @param pos The new position of the zone center (size 3).
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetPosition(const float pos[3]);

    /// @brief Get the size of the zone.
    /// @param outSize Output array to store the size (size 3).
    /// @threadsafety read-only
    /// @since v0.0.1
    void GetSize(float outSize[3]) const;

    /// @brief Set the size of the zone.
    /// @param size The new size of the zone (size 3).
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetSize(const float size[3]);

    /// @brief Get the rotation of the zone (for box shape).
    /// @param outRot Output array to store the rotation (size 3).
    /// @threadsafety read-only
    /// @since v0.0.1
    void GetRotation(float outRot[3]) const;

    /// @brief Set the rotation of the zone (for box shape).
    /// @param rot The new rotation of the zone (size 3).
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetRotation(const float rot[3]);

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
    void _AddTriggeredObject(GameObject* object);

    /// @brief Check if an object is in the triggered list (for one-shot zones).
    /// @param object The GameObject to check.
    /// @return True if the object is in the triggered list, false otherwise.
    /// @threadsafety read-only
    /// @since v0.0.1
    /// @internal
    bool _HasTriggeredObject(GameObject* object);

    /// @brief Remove an object from the triggered list (for one-shot zones).
    /// @param object The GameObject to remove.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _RemoveTriggeredObject(GameObject* object);

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
    bool IsInZone(const float point[3]) const;

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
