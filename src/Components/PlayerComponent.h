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
#include "bx/math.h"

namespace Syngine {

enum class PlayerState {
    IDLE      = 0,
    WALKING   = 1,
    SPRINTING = 2,
    CROUCHING = 3,
    FALLING   = 4,
    SLIDING   = 5,
    CLIMBING  = 6,
    SWIMMING  = 7,
    DEAD      = 8,
    SLEEPING  = 9,
    SITTING   = 10,
    INVALID   = 11,
};

class PlayerComponent : public Syngine::Component {
  public:
    static constexpr Syngine::Components componentType = SYN_COMPONENT_PLAYER;

    PlayerComponent(GameObject* owner);
    ~PlayerComponent();

    // Gets the component type
    Syngine::Components getComponentType() override;

    // Initializes the player component with camera, window, and physics manager.
    // @param camera The camera component for the player.
    // @param win The SDL window for rendering.
    // @param physicsManager The physics manager for the player.
    void Init(Syngine::CameraComponent* camera,
              SDL_Window*               win,
              Syngine::Phys*            physicsManager);

    // Handles input events for the player component.
    // @param event The SDL Events.
    void HandleInput(const SDL_Event& event);

    // Updates the player. Mostly handles movement, physics, and updating the state.
    // @param keystate The current state of the keyboard.
    // @param simulate Whether to simulate physics or not.
    void Update(const bool* keystate, bool simulate);
    // Updates position and camera after physics simulation.
    void PostPhysicsUpdate();
    // Gets the current player state.
    // @return The current player state.
    PlayerState GetPlayerState() const;

   
    float maxPitchAngle = 89.0f; // Max vertical angle for the camera pitch (in degrees).
    float currentPitch  = 0.0f; // Current pitch of the camera (in degrees).
    float currentYaw    = 0.0f; // Current yaw of the camera (in degrees).
    
    float sprintMult    = 2.0f; // Multiplier for sprinting speed.
    float crouchSpeed   = 0.5f; // Speed when crouching.
    float moveSpeed     = 3.0f; // Default movement speed of the player.
    float mouseSens     = 0.002f; // Mouse sensitivity for camera movement.

    bool enableMovement  = true; // Whether player movement is enabled (on by default).
    bool enableSliding   = true; // Whether player sliding is enabled (on by default).
    bool enableJumping   = true; // Whether player jumping is enabled (on by default).
    bool enableSprinting = true; // Whether player sprinting is enabled (on by default).
    bool enableCrouching = true; // Whether player crouching is enabled (on by default).

  private:
    GameObject*               m_owner     = nullptr;
    TransformComponent*       m_transform = nullptr;
    Syngine::CameraComponent* m_camera    = nullptr;
    SDL_Window*               m_window    = nullptr;

    float m_targetEyeHeight = 1.3f;  // offset from transform pos (center of player) (default is 1.3m) (this is init value, "real" value in in impl/Update)
    float m_targetFov       = 70.0f;
    float m_targetMoveSpeed = 2.0f;
    float m_moveSpeed       = 2.0f;
    float m_eyeHeight       = 1.3f;

    PlayerState m_playerState = PlayerState::IDLE;
    PlayerState m_prevPlayerState = PlayerState::IDLE;
    bool        m_simulate    = false;

    bx::Vec3 m_moveDirection = { 0.0f,
                                 0.0f,
                                 0.0f }; // Direction of movement in world space
    JPH::Vec3 m_newVelocity   = { 0.0f,
                                 0.0f,
                                 0.0f }; // New velocity to be applied to the character

    Syngine::Phys* m_physicsManager = nullptr;
    JPH::Ref<JPH::Character> m_character;
};

} // namespace Syngine