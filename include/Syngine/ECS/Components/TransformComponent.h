// ╒═══════════════ TransformComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Math/Quaternion.hpp"
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Utils/Serializer.h"
#include "Syngine/Math/Math.hpp"

namespace Syngine {
using namespace Syngine::Math;

/// @brief Component to handle position, rotation, and scale of a GameObject.
/// @section TransformComponent
/// @since v0.0.1
class TransformComponent : public Syngine::Component {
    // local TRS
    Vector3 m_position; //* Position of the transform
    Quaternion m_rotation; //* Rotation of the transform (Quaternion)
    Vector3 m_scale;       //* Scale of the transform

    TransformComponent* m_parent =
        nullptr; //* Parent transform component (if any)
    std::vector<TransformComponent*> m_children; //* Child transform components

    // Cached matrices and world TRS
    mutable Mat4 m_localMtx;
    mutable Mat4 m_worldMtx;
    mutable Vector3 m_worldPosition;
    mutable Quaternion m_worldRotation;

    mutable bool m_dirtyLocal = true;
    mutable bool m_dirtyWorld = true;

    uint64_t m_version = 0;

    // dirty helpers
    void _MarkLocalDirty();
    void _MarkWorldDirty();

    void _UpdateLocalMatrix() const;
    void _UpdateWorldMatrix() const;
  public:
    static constexpr Syngine::ComponentTypeID componentType =
        SYN_COMPONENT_TRANSFORM; //* Transform component type

    /// @brief Constructor for the TransformComponent class
    /// @param owner Pointer to the GameObject that owns this component
    /// @note This should only be called by GameObject::AddComponent<T>()
    /// @since v0.0.1
    TransformComponent(GameObject* owner);

    TransformComponent(const TransformComponent& other);
    TransformComponent& operator=(const TransformComponent& other);

    /// @brief Clone the TransformComponent
    /// @return A unique pointer to the cloned TransformComponent
    std::unique_ptr<Component> Clone() const override {
        return std::make_unique<TransformComponent>(*this);
    }

    /// @brief Serializes the TransformComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// TransformComponent's state
    Serializer::DataNode Serialize() const override;

    /// @brief Initializes the transform component with default values.
    /// @param position Initial position of the GameObject
    /// @param rotation Initial rotation of the GameObject (as a quaternion)
    /// @param scale Initial scale of the GameObject
    /// @note This is an entirely optional step, as the transform component
    /// can and is initialized directly in the constructor.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Init(Vector3    position = Vector3(),
              Quaternion rotation = Quaternion(),
              Vector3    scale    = Vector3(1.0f));

    /// @brief Update the transform component.
    /// @note There is no specific update logic for the transform component.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Update() {};

    /// @brief Get the type of this component
    /// @return The component type as an enum value
    /// @threadsafety read-only
    /// @since v0.0.1
    Syngine::ComponentTypeID GetComponentType() override {
        return TransformComponent::componentType;
    };

    /// @brief Get the GLOBAL rotation of the transform as XYZ Euler angles (in degrees)
    /// @return Vector3 representing the rotation (x, y, z)
    /// @threadsafety read-only
    /// @since v0.0.1
    Vector3 GetRotationEuler() const;

    /// @brief Get the GLOBAL rotation of the transform as a quaternion
    /// @return Quaternion representing the rotation (x, y, z, w)
    /// @threadsafety read-only
    /// @since v0.0.1
    Quaternion GetRotationQuaternion() const;

    /// @brief Get a GLOBAL model matrix for the transform
    /// @return 4x4 matrix filled with the global model matrix
    /// @note This matrix is computed from the GLOBAL position, rotation, and scale
    /// @threadsafety read-only
    /// @since v0.0.1
    Mat4 GetModelMatrix() const;

    /// @brief Get a LOCAL model matrix for the transform
    /// @return 4x4 matrix filled with the local model matrix
    /// @note This matrix is computed from the LOCAL position, rotation, and scale
    /// @threadsafety read-only
    /// @since v0.0.1
    Mat4 GetLocalMatrix() const;

    /// @brief Get the GLOBAL position of the transform
    /// @return Vector3 representing the position (x, y, z)
    /// @note GLOBAL position
    /// @threadsafety read-only
    /// @since v0.0.1
    Vector3 GetPosition() const;

    /// @brief Get the GLOBAL scale of the transform
    /// @return Vector3 representing the scale (x, y, z)
    /// @note GLOBAL scale
    /// @threadsafety read-only
    /// @since v0.0.1
    Vector3 GetScale() const;

    /// @brief Get the LOCAL position of the transform
    /// @return Vector3 representing the position (x, y, z)
    /// @note LOCAL position
    /// @threadsafety read-only
    /// @since v0.0.1
    Vector3 GetLocalPosition() const;

    /// @brief Get the LOCAL rotation of the transform as a quaternion
    /// @return Quaternion representing the rotation (x, y, z, w)
    /// @note LOCAL rotation
    /// @threadsafety read-only
    /// @since v0.0.1
    Quaternion GetLocalRotation() const;

    /// @brief Get the LOCAL scale of the transform
    /// @return Vector3 representing the local scale (x, y, z)
    /// @threadsafety read-only
    /// @since v0.0.1
    Vector3 GetLocalScale() const;

    /// @brief Set the LOCAL position of the transform
    /// @param position New local position as a Vector3
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetLocalPosition(Vector3 position);

    /// @brief Set the LOCAL rotation of the transform as XYZ Euler angles (in
    /// degrees)
    /// @param rotation New local rotation as XYZ Euler angles (in degrees)
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetLocalRotationEuler(Vector3 rotation);

    /// @brief Set the LOCAL rotation of the transform as a quaternion
    /// @param rotation New local rotation as a quaternion
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetLocalRotationQuat(Quaternion rotation);

    /// @brief Set the LOCAL scale of the transform
    /// @param scale New local scale as a Vector3
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetLocalScale(Vector3 scale);

    /// @brief Set the GLOBAL position of the transform
    /// @param position New global position as a Vector3
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetWorldPosition(Vector3 position);

    /// @brief Set the GLOBAL rotation of the transform as a quaternion
    /// @param rotation New global rotation as a quaternion
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetWorldRotationQuat(Quaternion rotation);

    /// @brief Set the GLOBAL rotation of the transform as XYZ Euler angles (in
    /// radians)
    /// @param rotation New global rotation as XYZ Euler angles (in radians)
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetWorldRotationEuler(Vector3 rotation);

    /// @brief Set the GLOBAL scale of the transform
    /// @param scale New global scale as a Vector3
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetWorldScale(Vector3 scale);

    /// @brief Set the parent transform of this transform
    /// @param parent Pointer to the parent TransformComponent
    /// @note This will automatically update the child list of the parent
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetParent(TransformComponent* parent);

    /// @brief Get the parent transform of this transform
    /// @return Pointer to the parent TransformComponent (or nullptr if none)
    /// @threadsafety read-only
    /// @since v0.0.1
    TransformComponent* GetParent() const;

    /// @brief Get the child transforms of this transform
    /// @return Vector of pointers to the child TransformComponents
    /// @threadsafety read-only
    /// @since v0.0.1
    std::vector<TransformComponent*>& GetChildren() const;

    /// @brief Get the version number of this transform
    /// @return Version number that increments on each change
    uint64_t GetVersion() const { return m_version; }
};

} // namespace Syngine
