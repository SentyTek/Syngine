#pragma once
#include "Components.h"
#include "SDL3/SDL_video.h"
#include "SyngineGraphics.h"
#include "SyngineGameobject.h"
#include "SynComponents.h"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "bx/math.h"

class PlayerComponent : public SynComponent {
  public:
    static constexpr SynComponents componentType = SYN_COMPONENT_PLAYER;

    PlayerComponent(GameObject* owner);

    SynComponents getComponentType() override;
    void          Initialize(Camera* camera, SDL_Window* win);
    void          HandleInput(const SDL_Event& event,
                              bool             simulate,
                              float            mouseSens,
                              float            maxPitchAndle);
    void          Update(float        deltaTime,
                         const bool* keystate,
                         float        moveSpeed,
                         float        sprintMult,
                         float        crouchSpeed);

    float currentYaw = 0.0f;
    float currentPitch = 0.0f;

  private:
    GameObject*         m_owner     = nullptr;
    TransformComponent* m_transform = nullptr;
    Camera*             m_camera    = nullptr;
    SDL_Window*         m_window    = nullptr;
};