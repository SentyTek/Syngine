#include "PlayerComponent.h"
#include "CameraComponent.h"
#include "Components.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
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

    if (m_simulate) {
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            float dx = event.motion.xrel * mouseSens;
            float dy = event.motion.yrel * mouseSens;

            m_currentYaw += dx;
            m_currentPitch -= dy;

            // Clamp pitch to avoid gimbal lock
            m_currentPitch = bx::clamp(m_currentPitch, -bx::toRad(maxPitchAngle), bx::toRad(maxPitchAngle));

            // Lock mouse position to the center of the window
            int w, h;
            SDL_GetWindowSize(m_window, &w, &h);
            float x = w / 2.0f, y = h / 2.0f;
            SDL_WarpMouseInWindow(m_window, x, y);
        } else if (event.type == SDL_EVENT_KEY_UP) {
            if (event.key.key == SDLK_LCTRL && m_playerState == PlayerState::SLIDING) {
                m_playerState = PlayerState::IDLE; // Stop sliding when releasing crouch
            }
        }
    }
}

void PlayerComponent::Update(const bool* keystate, bool simulate) {
    if (!m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
        return;
    }
    m_prevPlayerState = m_playerState;

    m_simulate = simulate;
    if (m_playerState != PlayerState::SLIDING) {
        m_playerState = PlayerState::IDLE;
    }

    if (m_simulate) {
        bool isGrounded = m_character->GetGroundState() ==
                          JPH::Character::EGroundState::OnGround;

        if (!isGrounded && m_playerState != PlayerState::SLIDING) {
            m_playerState = PlayerState::FALLING;
        }

        // Calculate the target move speed, eye height, and FOV based on input
        if (m_playerState != PlayerState::SLIDING) {
            m_targetMoveSpeed = moveSpeed;
        }
        m_targetEyeHeight = 1.3f;
        m_targetFov       = 70.0f;   
        if (enableSprinting && keystate[SDL_SCANCODE_LSHIFT] && m_playerState != PlayerState::SLIDING) {
            m_targetMoveSpeed *= sprintMult;
            m_targetFov = 90.0f;
            m_playerState = PlayerState::SPRINTING;
        }
        if (enableCrouching && keystate[SDL_SCANCODE_LCTRL] && m_playerState != PlayerState::SLIDING) {
            m_targetMoveSpeed *= crouchSpeed;
            //m_targetEyeHeight = 0.7f;
            m_targetFov       = 60.0f;
            m_playerState = PlayerState::CROUCHING;
        }

        // If both sprint and crouch are pressed, we slide
        if (enableSliding && keystate[SDL_SCANCODE_LSHIFT] && keystate[SDL_SCANCODE_LCTRL] && isGrounded && m_prevPlayerState == PlayerState::SPRINTING) {
            m_playerState = PlayerState::SLIDING;
            m_targetMoveSpeed *= sprintMult * 1.45f; // Increase speed while sliding
            //m_targetEyeHeight = 0.7f;
            m_targetFov       = 100.0f;
        }
        
        // Low friction when sliding
        if (m_playerState == PlayerState::SLIDING) {
            BodyInterface& bodyInterface = m_physicsManager->GetBodyInterface();
            bodyInterface.SetFriction(m_character->GetBodyID(), 0.25f);
            m_targetFov = 100.0f;
            //m_targetEyeHeight = 0.7f; // Lower eye height when sliding
            m_targetMoveSpeed -= 0.025f;
            if (m_targetMoveSpeed < 0.01f) {
                m_targetMoveSpeed = 0.0f;
            }
        } else {
            BodyInterface& bodyInterface = m_physicsManager->GetBodyInterface();
            bodyInterface.SetFriction(m_character->GetBodyID(), 0.45f);
            // Reset move direction
            m_moveDirection = { 0.0f, 0.0f, 0.0f };
        }

        // Shrink collider when crouching or sliding
        if (m_playerState == PlayerState::CROUCHING || m_playerState == PlayerState::SLIDING) {
            if (m_prevPlayerState != PlayerState::CROUCHING && m_prevPlayerState != PlayerState::SLIDING) {
                m_character->SetShape(new JPH::CapsuleShape(0.5f, 0.5f), 0.1f); // Half height and radius
            }
            m_targetEyeHeight = 0.7f; // Lower eye height when crouching or sliding
        } else { // Reset sizes only when state changes
            if (m_prevPlayerState == PlayerState::CROUCHING || m_prevPlayerState == PlayerState::SLIDING) {
                // Adjust position to avoid sinking into the ground
                m_transform->position[1] += 0.5f;
                m_character->SetPosition(JPH::RVec3(m_transform->position[0],
                                                    m_transform->position[1],
                                                    m_transform->position[2]));

                // Reset to normal size
                m_character->SetShape(new JPH::CapsuleShape(1.0f, 0.5f),
                                      0.1f); // Reset to normal size
            }
            m_targetEyeHeight = 1.3f; // Normal eye height
        }
        
        // For ground movement, typically we want to ignore the Y component
        // Y gets applied separately for jumping or falling
        bx::Vec3 forward = bx::normalize(
            bx::Vec3(sinf(m_currentYaw), 0.0f, cosf(m_currentYaw)));
        bx::Vec3 right =
            bx::normalize(bx::Vec3(sinf(m_currentYaw - bx::kPiHalf),
                                   0.0f,
                                   cosf(m_currentYaw - bx::kPiHalf)));

            if (enableMovement && m_playerState != PlayerState::SLIDING) {
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
                    if (m_playerState != PlayerState::SLIDING && m_playerState != PlayerState::CROUCHING && m_playerState != PlayerState::SPRINTING) {
                        // If not sliding or crouching, we are walking
                        m_playerState = PlayerState::WALKING;
                    }
                }
                
                // Get current velocity and preserve the vertical component (for gravity)
                JPH::Vec3 currentVel = m_character->GetLinearVelocity();
                m_newVelocity = JPH::Vec3(desiredHorizontalVel.GetX(),
                currentVel.GetY(),
                desiredHorizontalVel.GetZ());
                
                // JUMP! *van halen guitar solo*
                if (enableJumping && keystate[SDL_SCANCODE_SPACE] && isGrounded && m_playerState != PlayerState::SLIDING) {
                    const float jumpForce = 5.0f;
                    m_newVelocity.SetY(jumpForce);
                }
                
            } else {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent has no character object!");
            }
        }
        
        // Update the character
        m_character->SetRotation(
            JPH::Quat::sRotation(JPH::Vec3::sAxisY(), m_currentYaw));

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
    bx::Vec3   newPos     = bx::lerp(currentPos, targetPos, 0.2f);
    m_transform->position[0] = newPos.x;
    m_transform->position[1] = newPos.y;
    m_transform->position[2] = newPos.z;

    // Update camera position and orientation
    m_camera->SetPosition(m_transform->position[0],
                          m_transform->position[1] + m_eyeHeight,
                          m_transform->position[2]);
    m_camera->SetAngles(m_currentYaw, m_currentPitch);
}

PlayerState PlayerComponent::GetPlayerState() const {
    if (!m_character) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent has no character object!");
        return PlayerState::INVALID;
    }

    return m_playerState;
}

float* PlayerComponent::GetRotation() const {
    static float rotation[2];
    rotation[0] = m_currentYaw;
    rotation[1] = m_currentPitch;
    return rotation;
}

void PlayerComponent::SetRotation(float yaw, float pitch) {
    if (pitch < -maxPitchAngle || pitch > maxPitchAngle) {
        return;
    }
    m_currentYaw = yaw;
    m_currentPitch = pitch;
}

} // namespace Syngine