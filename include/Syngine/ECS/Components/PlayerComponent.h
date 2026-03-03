// ╒══════════════════ PlayerComponent.h ═╕
// │ Syngine                              │
// │ Created 2025-05-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Graphics/Renderer.h"
#include "Syngine/Physics/Physics.h"

#include "SDL3/SDL_video.h"
#include "SDL3/SDL_events.h"
#include "bx/math.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Character/Character.h"

namespace Syngine {
// Forward declaration of TransformComponent
class TransformComponent;

/// @brief Enum representing the different states a player can be in. Each state
/// corresponds to a specific action or condition of the player
/// @section PlayerComponent
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

/// @brief PlayerComponent is responsible for handling player input, movement,
/// and state. It manages the player's position, rotation, and interactions with
/// the game world.
/// @section PlayerComponent
class PlayerComponent : public Syngine::Component {
  public:
    static constexpr Syngine::Components componentType = SYN_COMPONENT_PLAYER; //* Player component type

    /// @brief Constructor for PlayerComponent
    /// @param owner The GameObject that owns this component.
    /// @param camera The camera component to use for the player.
    /// @param win The SDL window for the player.
    /// @note This should only be called by GameObject::AddComponent<T>()
    /// @since v0.0.1
    /// @internal
    PlayerComponent(GameObject* owner, Syngine::CameraComponent* camera);

    // No assignment operator because the copy is bad enough
    PlayerComponent(const PlayerComponent& other);
    
    ~PlayerComponent();

    /// @brief Gets the component type
    /// @return The component type of this component.
    /// @threadsafety read-only
    /// @since v0.0.1
    Syngine::Components GetComponentType() override;

    /// @brief Clone the PlayerComponent
    /// @return A unique pointer to the cloned PlayerComponent
    std::unique_ptr<Component> Clone() const override {
        return std::make_unique<PlayerComponent>(*this);
    }

    /// @brief Serializes the PlayerComponent to a data node
    /// @return A pointer to the serialized data node representing the
    /// PlayerComponent's state
    Serializer::DataNode Serialize() const override;

    /// @brief Initializes the player component with camera, window, and physics
    /// manager.
    /// @param camera The camera component to use for the player.
    /// @param win The SDL window for the player.
    /// @param physicsManager The physics manager to use for the player.
    /// @note This should only be called when the component is added to a GameObject
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void Init(Syngine::CameraComponent* camera);

    /// @brief Handles input events for the player component.
    /// @param event The SDL Events.
    /// @note This is called every time an input event occurs, such as key presses or mouse movements.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _HandleInput(const SDL_Event& event);

    /// @brief Updates the player. Mostly handles movement, physics, and
    /// updating the state.
    /// @param keystate The current state of the keyboard.
    /// @param simulate Whether to simulate physics or not.
    /// @param deltaTime The time since the last update.
    /// @note This is called every frame to update the player's position,
    /// rotation, and state.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void Update(const bool* keystate, float deltaTime);

    /// @brief Updates position and camera after physics simulation.
    /// @note This is called after the physics simulation to update the player's
    /// position and camera based on the physics simulation results.
    /// @threadsafety not-safe
    /// @since v0.0.1
    /// @internal
    void _PostPhysicsUpdate();

    /// @brief Gets the current player state.
    /// @return The current player state.
    /// @threadsafety read-only
    /// @since v0.0.1
    PlayerState GetPlayerState() const;

    /// @brief Gets the player's world space rotation.
    /// @return pointer to an array of two floats, representing yaw and pitch
    /// respectively.
    /// @threadsafety read-only
    /// @since v0.0.1
    float* GetRotation() const;

    /// @brief Sets the player's world space rotation.
    /// @param yaw The yaw rotation in radians.
    /// @param pitch The pitch rotation in radians.
    /// @threadsafety not-safe
    /// @since v0.0.1
    void SetRotation(float yaw, float pitch);
   
    float maxPitchAngle = 89.0f; //* Max vertical angle for the camera pitch (in degrees).
    
    float sprintMult    = 2.0f; //* Multiplier for sprinting speed.
    float crouchSpeed   = 0.5f; //* Speed when crouching.
    float moveSpeed     = 1.5f; //* Default movement speed of the player.
    float mouseSens     = 0.002f; //* Mouse sensitivity for camera movement.
    float standHeight   = 0.5f;   //* Height of the player when standing.
    float crouchHeight  = 0.2f;   //* Height of the player when crouching.
    float playerRadius  = 0.35f;  //* Radius of the player collider.

    bool enableMovement  = true; //* Whether player movement is enabled (on by default).
    bool enableSliding   = true; //* Whether player sliding is enabled (on by default).
    bool enableJumping   = true; //* Whether player jumping is enabled (on by default).
    bool enableSprinting = true; //* Whether player sprinting is enabled (on by default).
    bool enableCrouching = true; //* Whether player crouching is enabled (on by default).

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
    bool        m_simulate        = true;
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