// ╒═══════════════ TransformComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-05-20                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"

namespace Syngine {

/// @brief Component to handle position, rotation, and scale of a GameObject.
/// @section TransformComponent
/// @since v0.0.1
class TransformComponent : public Syngine::Component {
        // local TRS
        float m_position[3]; //* Position of the transform
        float m_rotation[4]; //* Rotation of the transform (Quaternion)
        float m_scale[3];    //* Scale of the transform

        TransformComponent* m_parent =
            nullptr; //* Parent transform component (if any)
        std::vector<TransformComponent*>
            m_children; //* Child transform components

        // Cached matrices and world TRS
        mutable float m_localMtx[16];
        mutable float m_worldMtx[16];
        mutable float m_worldPosition[3];
        mutable float m_worldRotation[4];

        mutable bool m_dirtyLocal = true;
        mutable bool m_dirtyWorld = true;

        // dirty helpers
        void _MarkLocalDirty();
        void _MarkWorldDirty();

        void _UpdateLocalMatrix() const;
        void _UpdateWorldMatrix() const;

        static void
        _QuatFromEulerDeg(float x, float y, float z, float* outQuat);
        static void _QuatNormalize(float* quat);
        static void
        _QuatMultiply(const float* q1, const float* q2, float* outQuat);
    public:
        static constexpr Syngine::Components componentType = SYN_COMPONENT_TRANSFORM; //* Transform component type

        /// @brief Constructor for the TransformComponent class
        /// @param owner Pointer to the GameObject that owns this component
        /// @note This should only be called by GameObject::AddComponent<T>()
        /// @since v0.0.1
        TransformComponent(GameObject* owner);

        /// @brief Initializes the transform component with default values.
        /// @param position Initial position of the GameObject
        /// @param rotation Initial rotation of the GameObject (as a quaternion)
        /// @param scale Initial scale of the GameObject
        /// @note This is an entirely optional step, as the transform component
        /// can and is initialized directly in the constructor.
        /// @threadsafety not-safe
        /// @since v0.0.1
        void Init(std::vector<float> position = {0.0f, 0.0f, 0.0f},
                  std::vector<float> rotation = {0.0f, 0.0f, 0.0f, 1.0f},
                  std::vector<float> scale = {1.0f, 1.0f, 1.0f});

        /// @brief Update the transform component.
        /// @note There is no specific update logic for the transform component.
        /// @threadsafety not-safe
        /// @since v0.0.1
        void Update() {};

        /// @brief Get the type of this component
        /// @return The component type as an enum value
        /// @threadsafety read-only
        /// @since v0.0.1
        Syngine::Components GetComponentType() override;

        /// @brief Get the GLOBAL rotation of the transform as XYZ Euler angles (in radians)
        /// @param x X component of the rotation
        /// @param y Y component of the rotation
        /// @param z Z component of the rotation
        /// @threadsafety read-only
        /// @since v0.0.1
        void GetRotationEuler(float& x, float& y, float& z) const;

        /// @brief Get the GLOBAL rotation of the transform as a quaternion
        /// @return Array of 4 floats representing the quaternion (x, y, z, w)
        /// @threadsafety read-only
        /// @since v0.0.1
        void GetRotationQuaternion(float& x, float& y, float& z, float& w) const;

        /// @brief Get a GLOBAL model matrix for the transform
        /// @param result Array to fill with the model matrix (16 floats)
        /// @threadsafety read-only
        /// @since v0.0.1
        void GetModelMatrix(float* result);

        /// @brief Get a LOCAL model matrix for the transform
        /// @param result Array to fill with the local model matrix (16 floats)
        /// @threadsafety read-only
        /// @since v0.0.1
        void GetLocalMatrix(float* result);

        /// @brief Get the GLOBAL position of the transform
        /// @return Array of 3 floats representing the position (x, y, z)
        /// @threadsafety read-only
        /// @since v0.0.1
        float* GetPosition();

        /// @brief Get the GLOBAL scale of the transform
        /// @return Array of 3 floats representing the scale (x, y, z)
        /// @threadsafety read-only
        /// @since v0.0.1
        float* GetScale();

        /// @brief Get the LOCAL position of the transform
        /// @return Array of 3 floats representing the local position (x, y, z)
        /// @threadsafety read-only
        /// @since v0.0.1
        float* GetLocalPosition();

        /// @brief Get the LOCAL rotation of the transform as a quaternion
        /// @return Array of 4 floats representing the local rotation quaternion (x, y, z, w)
        /// @threadsafety read-only
        /// @since v0.0.1
        float* GetLocalRotation();

        /// @brief Get the LOCAL scale of the transform
        /// @return Array of 3 floats representing the local scale (x, y, z)
        /// @threadsafety read-only
        /// @since v0.0.1
        float* GetLocalScale();

        /// @brief Set the LOCAL position of the transform
        /// @param x X component of the position
        /// @param y Y component of the position
        /// @param z Z component of the position
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetPosition(float x,
                         float y,
                         float z);

        /// @brief Set the LOCAL rotation of the transform as XYZ Euler angles (in degrees)
        /// @param x X component of the rotation
        /// @param y Y component of the rotation
        /// @param z Z component of the rotation
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetRotationEuler(float x, float y, float z);

        /// @brief Set the LOCAL rotation of the transform as a quaternion
        /// @param x X component of the quaternion
        /// @param y Y component of the quaternion
        /// @param z Z component of the quaternion
        /// @param w W component of the quaternion
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetRotationQuat(float x, float y, float z, float w);

        /// @brief Set the LOCAL scale of the transform
        /// @param x X component of the scale
        /// @param y Y component of the scale
        /// @param z Z component of the scale
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetScale(float x, float y, float z);

        /// @brief Set the GLOBAL position of the transform
        /// @param x X component of the position
        /// @param y Y component of the position
        /// @param z Z component of the position
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetWorldPosition(float x, float y, float z);

        /// @brief Set the GLOBAL rotation of the transform as a quaternion
        /// @param x X component of the quaternion
        /// @param y Y component of the quaternion
        /// @param z Z component of the quaternion
        /// @param w W component of the quaternion
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetWorldRotationQuat(float x, float y, float z, float w);

        /// @brief Set the GLOBAL rotation of the transform as XYZ Euler angles (in radians)
        /// @param x X component of the rotation
        /// @param y Y component of the rotation
        /// @param z Z component of the rotation
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetWorldRotationEuler(float x, float y, float z);

        /// @brief Set the GLOBAL scale of the transform
        /// @param x X component of the scale
        /// @param y Y component of the scale
        /// @param z Z component of the scale
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetWorldScale(float x, float y, float z);

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
};

} // namespace Syngine