#pragma once
#include "Components.h"
#include "RigidbodyComponent.h"
#include "SDL3/SDL_video.h"
#include "SyngineGraphics.h"
#include "SyngineGameobject.h"

#include "SDL3/SDL_events.h"

class PlayerComponent : public SynComponent {
  public:
    static constexpr SynComponents componentType = SYN_COMPONENT_PLAYER;

    PlayerComponent(GameObject* owner);

    SynComponents getComponentType() override;
    void          Initialize(Camera*                      camera,
                             SDL_Window*                  win,
                             Syngine::RigidbodyComponent* RigidbodyComponent);
    void          HandleInput(const SDL_Event& event,
                              bool             simulate,
                              float            mouseSens,
                              float            maxPitchAndle);
    void          Update(const bool* keystate,
                         float       moveSpeed,
                         float       sprintMult,
                         float       crouchSpeed);

    float currentYaw = 0.0f;
    float currentPitch = 0.0f;
    float eyeHeight = 1.8f; // Default eye height for player camera

  private:
    GameObject*         m_owner     = nullptr;
    TransformComponent* m_transform = nullptr;
    Camera*             m_camera    = nullptr;
    SDL_Window*         m_window    = nullptr;
    Syngine::RigidbodyComponent* m_RigidbodyComponent = nullptr;
    float halfHeight = 0.0f; // Half height for collision detection
    bool  isGrounded = false;
    float lastGroundNormal = 0.0f;
    float m_targetEyeHeight = 1.8f;
    float m_targetFov       = 70.0f;
    float m_targetMoveSpeed = 2.0f;
    float m_moveSpeed       = 2.0f;

    void CheckGrounded();
};