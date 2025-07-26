#pragma once
#include "Components.h"

namespace Syngine {
struct Camera {
    float eye[3] = {0.0f, 0.0f, -5.0f};
    float target[3] = {0.0f, 0.0f, 0.0f};
    float up[3] = {0.0f, 1.0f, 0.0f};
    float fov = 70.0f; //deg
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float view[16];
    float proj[16];

    float yaw = 0.0f;
    float pitch = 0.0f;
};

class CameraComponent : public Syngine::Component {
  public:
    static constexpr Syngine::Components componentType = Syngine::SYN_COMPONENT_CAMERA;

    CameraComponent(GameObject* owner);
    ~CameraComponent();
    Syngine::Components getComponentType() override;

    void Update(int viewId, int width, int height);

    void SetPosition(float x, float y, float z);
    const float* GetPosition() const;
    void   SetFOV(float fov);
    float  GetFOV() const;
    void   SetFarPlane(float farPlane);
    float  GetFarPlane() const;

    void SetAngles(float yaw, float pitch);
    void GetAngles(float& yaw, float& pitch) const;

    Syngine::Camera GetCamera() const;

  private:
    GameObject* m_owner; // Reference to the owner game object
    Camera      camera;  // Camera data
}; // class CameraComponent

} // namespace Syngine