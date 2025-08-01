#pragma once
#include "Components.h"
#include "SDL3/SDL_video.h"
#include "SyngineGraphics.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"

#include "SDL3/SDL_events.h"
#include "bx/math.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Character/Character.h"

namespace Syngine {
// Forward declaration of TransformComponent
class TransformComponent;

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

    PlayerComponent(GameObject*               owner,
                    Syngine::CameraComponent* camera,
                    SDL_Window*               win);
    ~PlayerComponent();

    // Gets the component type
    Syngine::Components getComponentType() override;

    // Initializes the player component with camera, window, and physics
    // manager.
    // @param camera The camera component to use for the player.
    // @param win The SDL window for the player.
    // @param physicsManager The physics manager to use for the player.
    void Init(Syngine::CameraComponent* camera,
              SDL_Window*               win);

    // Handles input events for the player component.
    // @param event The SDL Events.
    void HandleInput(const SDL_Event& event);

    // Updates the player. Mostly handles movement, physics, and updating the
    // state.
    // @param keystate The current state of the keyboard.
    // @param simulate Whether to simulate physics or not.
    // @param deltaTime The time since the last update.
    void Update(const bool* keystate, bool simulate, float deltaTime);
    // Updates position and camera after physics simulation.
    void PostPhysicsUpdate();
    // Gets the current player state.
    // @return The current player state.
    PlayerState GetPlayerState() const;

    float* GetRotation() const;
    void SetRotation(float yaw, float pitch);
   
    float maxPitchAngle = 89.0f; // Max vertical angle for the camera pitch (in degrees).
    
    float sprintMult    = 2.0f; // Multiplier for sprinting speed.
    float crouchSpeed   = 0.5f; // Speed when crouching.
    float moveSpeed     = 1.5f; // Default movement speed of the player.
    float mouseSens     = 0.002f; // Mouse sensitivity for camera movement.
    float standHeight   = 0.5f;   // Height of the player when standing.
    float crouchHeight  = 0.2f;   // Height of the player when crouching.
    float playerRadius  = 0.35f;  // Radius of the player collider.

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

    float m_targetEyeHeight = 0.5f;  // offset from transform pos (center of player) (default is 0.5m) (this is init value, "real" value in in impl/Update)
    float m_targetFov       = 70.0f;
    float m_targetMoveSpeed = 1.0f;
    float m_eyeHeight       = 0.5f;
    float m_realMoveSpeed   = 1.0f;

    float m_currentPitch  = 0.0f;
    float m_currentYaw    = 0.0f;

    PlayerState m_playerState = PlayerState::IDLE;
    PlayerState m_prevPlayerState = PlayerState::IDLE;
    bool        m_simulate        = false;
    float       m_deltaTime       = 0.0f;

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