// ╒══════════════════ CameraComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-06-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "bx/math.h"

namespace Syngine {

/// @brief Struct to hold camera data
/// @section CameraComponent
/// @since v0.0.1
struct Camera {
    float eye[3] = {0.0f, 0.0f, -5.0f}; //* Camera position in world space
    float target[3] = {0.0f, 0.0f, 0.0f}; //* Camera target position
    float up[3] = {0.0f, 1.0f, 0.0f}; //* Camera up vector
    float fov = 70.0f; //* Field of view in degrees
    float nearPlane = 0.1f; //* Near clipping plane
    float farPlane = 100.0f; //* Far clipping plane
    float view[16]; //* View matrix
    float proj[16]; //* Projection matrix

    float yaw = 0.0f; //* Yaw angle in radians
    float pitch = 0.0f; //* Pitch angle in radians
    float roll = 0.0f; //* Roll angle in radians
};

/// @brief CameraComponent class for managing camera functionality in a game
/// object
/// @section CameraComponent
/// @since v0.0.1
class CameraComponent : public Syngine::Component {
  public:
    static constexpr Syngine::ComponentTypeID componentType = Syngine::SYN_COMPONENT_CAMERA; //* Camera component type

    /// @brief Constructor for the CameraComponent class
    /// @param owner Pointer to the GameObject that owns this component
    /// @note This should only be called by GameObject::AddComponent<T>()
    /// @since v0.0.1
    /// @internal
    CameraComponent(GameObject* owner);

    CameraComponent(const CameraComponent& other);
    CameraComponent& operator=(const CameraComponent& other);
    
    ~CameraComponent();

    /// @brief Get the type of this component
    /// @return The component type as an enum value
    /// @threadsafety read-only
    /// @since v0.0.1
    Syngine::ComponentTypeID GetComponentType() override;

    /// @brief Clone the CameraComponent
    /// @return A unique pointer to the cloned CameraComponent

    std::unique_ptr<Component> Clone() const override {
        return std::make_unique<CameraComponent>(*this);
    }

    /// @brief Serializes the CameraComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// CameraComponent's state
    Serializer::DataNode Serialize() const override;

    /// @brief Initialize the camera component
    /// @note This should only be called when the component is added to a GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    void Init() override {} // No specific initialization needed

    /// @brief Update the camera component
    /// @param viewId ID of the view to update
    /// @param width Width of the viewport
    /// @param height Height of the viewport
    /// @note This is called every frame to update the camera's view and
    /// projection matrices
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void Update(int viewId, int width, int height);

    /// @brief Set the camera position
    /// @param x X coordinate of the camera position
    /// @param y Y coordinate of the camera position
    /// @param z Z coordinate of the camera position
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetPosition(float x, float y, float z);

    /// @brief Get the camera position
    /// @return Array of 3 floats representing the camera position (x, y, z)
    /// @threadsafety read-only
    /// @since v0.0.1
    const float* GetPosition() const;

    /// @brief Set the camera FOV
    /// @param fov Field of view in degrees
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetFOV(float fov);

    /// @brief Get the camera FOV
    /// @return Field of view in degrees
    /// @threadsafety read-only
    /// @since v0.0.1
    float GetFOV() const;

    /// @brief Set the camera far clipping plane
    /// @param farPlane Far clipping plane distance
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetFarPlane(float farPlane);

    /// @brief Get the camera far clipping plane
    /// @return Far clipping plane distance
    /// @threadsafety read-only
    /// @since v0.0.1
    float GetFarPlane() const;

    /// @brief Set the camera near clipping plane
    /// @param nearPlane Near clipping plane distance
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetAngles(float yaw, float pitch);

    /// @brief Get the camera angles
    /// @param yaw Reference to store the yaw angle in radians
    /// @param pitch Reference to store the pitch angle in radians
    /// @threadsafety read-only
    /// @since v0.0.1
    void GetAngles(float& yaw, float& pitch) const;

    Syngine::Camera GetCamera() const;

  private:
    GameObject* m_owner; // Reference to the owner game object
    Camera      camera;  // Camera data

    /// @brief Structure representing a plane in 3D space
    /// @since v0.0.1
    /// @internal
    struct Plane {
        float normal[3] = { 0.f, 1.f, 0.f }; //* Normal vector of the plane
        float distance  = 0.f;               //* Distance from origin
    };

    /// @brief Structure representing a frustum for view culling
    /// @since v0.0.1
    /// @internal
    struct Frustum {
        Plane top;    //* Top plane
        Plane bottom; //* Bottom plane
        Plane left;   //* Left plane
        Plane right;  //* Right plane
        Plane n;   //* Near plane
        Plane f;    //* Far plane
    };

    static void _normalizePlane(CameraComponent::Plane& plane);

    Frustum _extractFrustum();

    bool _aabbInsidePlane(const Plane&    plane,
                          const bx::Vec3& min,
                          const bx::Vec3& max);
    bool _aabbInsideFrustum(const Frustum&  frustum,
                            const bx::Vec3& min,
                            const bx::Vec3& max);

    friend class RenderCore;
}; // class CameraComponent

} // namespace Syngine