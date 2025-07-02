#include "PlayerComponent.h"
#include "CameraComponent.h"
#include "Components.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "SDL3/SDL_events.h"
#include "SyngineGraphics.h"
#include "TransformComponent.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"

#include "Jolt/Math/Vec3.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Character/Character.h"
#include "Jolt/Physics/EActivation.h"
#include "Jolt/Core/TempAllocator.h"

#include "bx/math.h"

#include <cmath>

namespace Syngine {
PlayerComponent::PlayerComponent(GameObject* owner) {
    this->m_owner = owner;
    m_transform = owner->GetComponent<TransformComponent>();
}

PlayerComponent::~PlayerComponent() {
    if (m_character) {
        m_character->RemoveFromPhysicsSystem();
        m_character = nullptr;
    }
}

Syngine::Components PlayerComponent::getComponentType() {
    return SYN_COMPONENT_PLAYER;
}

void PlayerComponent::Init(Syngine::CameraComponent*    camera,
                           SDL_Window*                  win,
                           Syngine::Phys* physicsManager) {
    m_camera = camera;
    m_window = win;

    if (!m_owner || !m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "PlayerComponent is missing a required component");
        return;
    }

    JPH::Ref<JPH::CharacterSettings> settings = new JPH::CharacterSettings();
    settings->mMaxSlopeAngle                  = bx::kPi / 4.0f; // 45 degrees
    settings->mLayer = Syngine::Layers::MOVING; // Set to the moving layer
    settings->mShape =
        new JPH::CapsuleShape(1.0f, 0.5f); // Half height and radius
    settings->mFriction = 0.45f;
    settings->mMass     = 80.0f;

    JPH::RVec3 initialPosition(m_transform->position[0],
                               m_transform->position[1],
                               m_transform->position[2]);

    m_character = new JPH::Character(settings,
                                     initialPosition,
                                     JPH::Quat::sIdentity(),
                                     0,
                                     &physicsManager->GetPhysicsSystem());
    m_character->AddToPhysicsSystem(JPH::EActivation::Activate);

    m_camera->SetPosition(m_transform->position[0],
                          m_transform->position[1],
                          m_transform->position[2]);

    m_physicsManager = physicsManager;
}

void PlayerComponent::HandleInput(const SDL_Event& event) {
    if (!m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
        return;
    }

    if (event.type == SDL_EVENT_MOUSE_MOTION && m_simulate) {
        float dx = event.motion.xrel * mouseSens;
        float dy = event.motion.yrel * mouseSens;

        currentYaw += dx;
        currentPitch -= dy;

        // Clamp pitch to avoid gimbal lock
        currentPitch = bx::clamp(currentPitch, -maxPitchAngle, maxPitchAngle);

        // Lock mouse position to the center of the window
        int w, h;
        SDL_GetWindowSize(m_window, &w, &h);
        float x = w / 2.0f, y = h / 2.0f;
        SDL_WarpMouseInWindow(m_window, x, y);
    } else if (event.type == SDL_EVENT_KEY_UP) {
        if (event.key.key == SDL_SCANCODE_LCTRL && m_playerState == PlayerState::SLIDING) {
            m_playerState = PlayerState::IDLE; // Stop sliding when releasing crouch
        }
    }
}

void PlayerComponent::Update(const bool* keystate, bool simulate) {
    if (!m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
        return;
    }

    m_simulate = simulate;
    m_playerState = PlayerState::IDLE;

    if (m_simulate) {
        bool isGrounded = m_character->GetGroundState() ==
                          JPH::Character::EGroundState::OnGround;

        if (!isGrounded) {
            m_playerState = PlayerState::FALLING;
        }
        

        // Calculate the target move speed, eye height, and FOV based on input
        m_targetMoveSpeed = moveSpeed;
        m_targetEyeHeight = 1.3f;
        m_targetFov       = 70.0f;
        if (keystate[SDL_SCANCODE_LSHIFT] && isGrounded) {
            m_targetMoveSpeed *= sprintMult;
            m_targetFov = 90.0f;
            m_playerState = PlayerState::SPRINTING;
        }
        if (keystate[SDL_SCANCODE_LCTRL] && isGrounded) {
            if (m_playerState != PlayerState::SLIDING) {
                m_targetMoveSpeed *= crouchSpeed;
                m_targetEyeHeight = 0.7f;
                m_targetFov       = 60.0f;
            }
        }
        if (keystate[SDL_SCANCODE_LSHIFT] && keystate[SDL_SCANCODE_LCTRL] && isGrounded) {
            // If both sprint and crouch are pressed, we slide
            m_playerState = PlayerState::SLIDING;
            m_targetMoveSpeed *= sprintMult * 1.5f; // Increase speed while sliding
            m_targetEyeHeight = 0.7f; // Lower eye height while sliding
            m_targetFov       = 100.0f; // Wider FOV while sliding
        }

        // Low friction when sliding
        if (m_playerState == PlayerState::SLIDING) {
            BodyInterface& bodyInterface = m_physicsManager->GetBodyInterface();
            bodyInterface.SetFriction(m_character->GetBodyID(), 0.05f);
            m_targetFov = 100.0f;
            m_targetMoveSpeed *= (sprintMult * 1.5f);
        } else {
            BodyInterface& bodyInterface = m_physicsManager->GetBodyInterface();
            bodyInterface.SetFriction(m_character->GetBodyID(), 0.45f);
            // Reset move direction
            m_moveDirection = { 0.0f, 0.0f, 0.0f };
        }


        // For ground movement, typically we want to ignore the Y component
        // Y gets applied separately for jumping or falling
        bx::Vec3 forward =
            bx::normalize(bx::Vec3(sinf(currentYaw), 0.0f, cosf(currentYaw)));
        bx::Vec3 right = bx::normalize(bx::Vec3(
            sinf(currentYaw - bx::kPiHalf), 0.0f, cosf(currentYaw - bx::kPiHalf)));

        if (m_playerState != PlayerState::SLIDING) {
            if (keystate[SDL_SCANCODE_W]) {
                m_moveDirection = bx::add(m_moveDirection, forward);
            }
            if (keystate[SDL_SCANCODE_S]) {
                m_moveDirection = bx::sub(m_moveDirection, forward);
            }
            if (keystate[SDL_SCANCODE_A]) {
                m_moveDirection = bx::add(m_moveDirection, right);
            }
            if (keystate[SDL_SCANCODE_D]) {
                m_moveDirection = bx::sub(m_moveDirection, right);
            }
        }

        if (m_character) {
            JPH::Vec3 desiredHorizontalVel(0, 0, 0);
            if (bx::length(m_moveDirection) > 0.001f) {
                m_moveDirection = bx::normalize(m_moveDirection);
                desiredHorizontalVel.SetX(m_moveDirection.x * m_moveSpeed);
                desiredHorizontalVel.SetZ(m_moveDirection.z * m_moveSpeed);
                m_playerState = PlayerState::WALKING;
            }

            // Get current velocity and preserve the vertical component (for gravity)
            JPH::Vec3 currentVel = m_character->GetLinearVelocity();
            m_newVelocity = JPH::Vec3(desiredHorizontalVel.GetX(),
                                    currentVel.GetY(),
                                    desiredHorizontalVel.GetZ());

            
            // JUMP! *van halen guitar solo*
            if (keystate[SDL_SCANCODE_SPACE] && isGrounded) {
                const float jumpForce = 5.0f;
                m_newVelocity.SetY(jumpForce);
                m_playerState = PlayerState::JUMPING;
            }

        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent has no character object!");
        }
    }

    // Update the character
    m_character->SetRotation(
        JPH::Quat::sRotation(JPH::Vec3::sAxisY(), currentYaw));

    m_character->SetLinearVelocity(m_newVelocity);

    // Update some lerps
    m_camera->SetFOV(bx::lerp(m_camera->GetFOV(), m_targetFov, 0.1f));
    m_eyeHeight = bx::lerp(m_eyeHeight, m_targetEyeHeight, 0.1f);
    m_moveSpeed = bx::lerp(m_moveSpeed, m_targetMoveSpeed, 0.1f);
}

void PlayerComponent::PostPhysicsUpdate() {
    if (!m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
        return;
    }

    if (m_character) {
        const float maxSeparationDistance = 0.1f; // Distance to consider "grounded"
        m_character->PostSimulation(maxSeparationDistance);
    }

    // Update transform
    JPH::RVec3 charPos    = m_character->GetPosition();
    bx::Vec3   targetPos  = { charPos.GetX(), charPos.GetY(), charPos.GetZ() };
    bx::Vec3   currentPos = { m_transform->position[0],
                              m_transform->position[1],
                              m_transform->position[2] };
    bx::Vec3   newPos     = bx::lerp(currentPos, targetPos, 0.1f);
    m_transform->position[0] = newPos.x;
    m_transform->position[1] = newPos.y;
    m_transform->position[2] = newPos.z;

    // Update camera position and orientation
    m_camera->SetPosition(m_transform->position[0],
                          m_transform->position[1] + m_eyeHeight,
                          m_transform->position[2]);
    m_camera->SetAngles(currentYaw, currentPitch);
}

PlayerState PlayerComponent::GetPlayerState() const {
    if (!m_character) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent has no character object!");
        return PlayerState::INVALID;
    }

    return m_playerState;
}

} // namespace Syngine