#pragma once
#include "CameraComponent.h"
#include "Components.h"
#include "SDL3/SDL_video.h"
#include "SyngineGraphics.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"
#include "Components/TransformComponent.h"

#include "SDL3/SDL_events.h"

#include "Jolt/Physics/Character/Character.h"

namespace Syngine {
class PlayerComponent : public Syngine::Component {
  public:
    static constexpr Syngine::Components componentType = SYN_COMPONENT_PLAYER;

    PlayerComponent(GameObject* owner);
    ~PlayerComponent();

    Syngine::Components getComponentType() override;
    void                Init(Syngine::CameraComponent* camera,
                             SDL_Window*               win,
                             Syngine::Phys*            physicsManager);
    void                HandleInput(const SDL_Event& event,
                                    bool             simulate,
                                    float            mouseSens,
                                    float            maxPitchAndle);
    void                Update(const bool* keystate,
                               float       moveSpeed,
                               float       sprintMult,
                               float       crouchSpeed);
    void                PostPhysicsUpdate();
    

    float currentYaw = 0.0f;
    float currentPitch = 0.0f;
    float eyeHeight = 1.8f; // Default eye height for player camera

  private:
    GameObject*         m_owner     = nullptr;
    TransformComponent* m_transform = nullptr;
    Syngine::CameraComponent* m_camera    = nullptr;
    SDL_Window*               m_window    = nullptr;
    bool  isGrounded = false;
    float m_targetEyeHeight = 1.8f;
    float m_targetFov       = 70.0f;
    float m_targetMoveSpeed = 2.0f;
    float m_moveSpeed       = 2.0f;

    Syngine::Phys* m_physicsManager = nullptr;
    JPH::Ref<JPH::Character> m_character;
};

} // namespace Syngine