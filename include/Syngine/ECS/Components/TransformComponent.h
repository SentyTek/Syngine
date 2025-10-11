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
/*
 * @brief Syngine Transform Component. The TransformComponent is used to
 * represent the position, rotation, and scale of a GameObject in the game world.
 * @section TransformComponent
 * @since v0.0.1
 */
class TransformComponent : public Syngine::Component {
        float m_position[3]; //* Position of the transform
        float m_rotation[4]; //* Rotation of the transform (Quaternion)
        float m_scale[3];    //* Scale of the transform

        TransformComponent* m_parent = nullptr; //* Parent transform, if any
        std::vector<TransformComponent*> m_children; //* Child transforms

        void _AddChild(TransformComponent* child);
        void _RemoveChild(TransformComponent* child);
        void _CalcModelMatrix(float* result);

      public:
        static constexpr Syngine::Components componentType =
            SYN_COMPONENT_TRANSFORM; //* Transform component type

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

        /// @brief Get the rotation of the transform as Euler angles (in radians)
        /// @param x X component of the rotation
        /// @param y Y component of the rotation
        /// @param z Z component of the rotation
        /// @threadsafety read-only
        /// @since v0.0.1
        void GetRotationEuler(float& x, float& y, float& z) const;

        /// @brief Get the rotation of the transform as a quaternion
        /// @return Array of 4 floats representing the quaternion (x, y, z, w)
        /// @threadsafety read-only
        /// @since v0.0.1
        float* GetRotationQuat() const;

        /// @brief Get the global rotation of the transform as a quaternion
        /// @param outQuat Array of 4 floats to store the quaternion (x, y, z, w)
        /// @threadsafety read-only
        /// @since v0.0.1
        void GetGlobalRotation(float* outQuat);

        /// @brief Set the position of the transform
        /// @param x X component of the position
        /// @param y Y component of the position
        /// @param z Z component of the position
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetPosition(float x,
                         float y,
                         float z);

        /// @brief Set the rotation of the transform as Euler angles (in radians)
        /// @param x X component of the rotation
        /// @param y Y component of the rotation
        /// @param z Z component of the rotation
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetRotation(float x, float y, float z);

        /// @brief Set the rotation of the transform as a quaternion
        /// @param x X component of the quaternion
        /// @param y Y component of the quaternion
        /// @param z Z component of the quaternion
        /// @param w W component of the quaternion
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetRotation(float x, float y, float z, float w);

        /// @brief Set the scale of the transform
        /// @param x X component of the scale
        /// @param y Y component of the scale
        /// @param z Z component of the scale
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetScale(float x, float y, float z);

        /// @brief Get a model matrix for the transform
        /// @param result Array to fill with the model matrix (16 floats)
        /// @threadsafety read-only
        /// @since v0.0.1
        void GetModelMatrix(float* result);

        /// @brief Get the position of the transform
        /// @return Array of 3 floats representing the position (x, y, z)
        /// @param global If true, returns the global position (including parent
        /// transforms). If false, returns the local position.
        /// @threadsafety read-only
        /// @since v0.0.1
        float* GetPosition(bool global = false);

        /// @brief Set the parent transform of this transform
        /// @param parent Pointer to the parent TransformComponent
        /// @threadsafety not-safe
        /// @since v0.0.1
        void SetParent(TransformComponent* parent);

        /// @brief Get the parent transform of this transform
        /// @return Pointer to the parent TransformComponent, or nullptr if none
        /// @threadsafety read-only
        /// @since v0.0.1
        TransformComponent* GetParent() const;

        /// @brief Is this transform a root (no parent)?
        /// @return true if this transform has no parent, false otherwise
        /// @threadsafety read-only
        /// @since v0.0.1
        bool IsRoot() const;

        /// @brief Get the number of child transforms
        /// @return Number of child transforms
        /// @threadsafety read-only
        /// @since v0.0.1
        size_t GetChildCount() const;
};

} // namespace Syngine