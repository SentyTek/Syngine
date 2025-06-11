#pragma once
#include "Components.h"

namespace Syngine {
struct Camera {
    float eye[3] = {0.0f, 0.0f, -5.0f};
    float target[3] = {0.0f, 0.0f, 0.0f};
    float up[3] = {0.0f, 1.0f, 0.0f};
    float fov = 70.0f; //deg
    float near = 0.1f;
    float far = 100.0f;
    float view[16];
    float proj[16];

    float yaw = 0.0f;
    float pitch = 0.0f;
};

class CameraComponent : public SynComponent {
  public:
    static constexpr SynComponents componentType = SYN_COMPONENT_CAMERA;

    CameraComponent(GameObject* owner);
    ~CameraComponent();
    SynComponents getComponentType() override;

    void Update(int viewId, int width, int height);

    void SetPosition(float x, float y, float z);
    const float* GetPosition() const;
    void   SetFOV(float fov);
    float  GetFOV() const;
    void   SetFarPlane(float farPlane);
    float  GetFarPlane() const;

  private:
    GameObject* m_owner; // Reference to the owner game object
    Camera      camera;  // Camera data
}; // class CameraComponent

} // namespace Syngine