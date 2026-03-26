// ╒════════════════ PlayerComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Core.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/Graphics/Windowing.h"
#include "Syngine/ECS/Components/PlayerComponent.h"
#include "Syngine/ECS/Components/CameraComponent.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Physics/Physics.h"
#include "Syngine/Graphics/Renderer.h"

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"

#include "Jolt/Math/Vec3.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Character/Character.h"
#include "Jolt/Physics/EActivation.h"
#include "Jolt/Physics/Body/BodyInterface.h"

#include "bx/math.h"

#include <cmath>
#include <string>

namespace Syngine {
PlayerComponent::PlayerComponent(GameObject*               owner,
                                 Syngine::CameraComponent* camera) {
    if (!Core::IsPhysicsEnabled()) return;
    
    this->m_owner = owner;
    this->Init(camera);
}

// I don't like this.
// I do not like it one bit.
// I do not like it, Sam-I-Am.
PlayerComponent::PlayerComponent(const PlayerComponent& other) {
    if (!Core::IsPhysicsEnabled()) return;
    
    this->m_owner = other.m_owner;
    this->m_camera = other.m_camera;
    this->m_window = other.m_window;
    this->m_physicsManager = other.m_physicsManager;
    this->m_transform = other.m_transform;
    this->m_character = nullptr; // Character should not be copied
    this->m_playerState = other.m_playerState;
    this->m_prevPlayerState = other.m_prevPlayerState;
    this->m_simulate = other.m_simulate;
    this->m_currentYaw = other.m_currentYaw;
    this->m_currentPitch = other.m_currentPitch;
    this->m_deltaTime = other.m_deltaTime;
    this->m_targetMoveSpeed = other.m_targetMoveSpeed;
    this->m_targetEyeHeight = other.m_targetEyeHeight;
    this->m_targetFov = other.m_targetFov;

    // Copy configuration parameters
    this->moveSpeed = other.moveSpeed;
    this->sprintMult = other.sprintMult;
    this->crouchSpeed = other.crouchSpeed;
    this->enableSprinting = other.enableSprinting;
    this->enableCrouching = other.enableCrouching;
    this->enableSliding = other.enableSliding;
    this->standHeight = other.standHeight;
    this->crouchHeight = other.crouchHeight;
    this->playerRadius = other.playerRadius;
    this->mouseSens = other.mouseSens;
    this->maxPitchAngle = other.maxPitchAngle;

    // Re-initialize the character
    if (this->m_transform) {
        JPH::Ref<JPH::CharacterSettings> settings = new JPH::CharacterSettings();
        settings->mMaxSlopeAngle                  = bx::kPi / 4.0f; // 45 degrees
        settings->mLayer = Syngine::Layers::MOVING; // Set to the moving layer
        settings->mShape =
            new JPH::CapsuleShape(standHeight, playerRadius); // Half (quarter?) height and radius
        settings->mFriction = 0.45f;
        settings->mMass     = 80.0f;

        float* pos = m_transform->GetPosition();
        JPH::RVec3 initialPosition(pos[0], pos[1], pos[2]);
        this->m_character =
            new JPH::Character(settings,
                               initialPosition,
                               JPH::Quat::sIdentity(),
                               0,
                               &this->m_physicsManager->_GetPhysicsSystem());
        this->m_character->AddToPhysicsSystem(JPH::EActivation::Activate);
        this->m_camera->SetPosition(pos[0], pos[1], pos[2]);
    }
}

Serializer::DataNode PlayerComponent::Serialize() const {
    Serializer::DataNode node;
    node["type"] = static_cast<int>(Syngine::SYN_COMPONENT_PLAYER);
    node["moveSpeed"] = moveSpeed;
    node["sprintMult"] = sprintMult;
    node["crouchSpeed"] = crouchSpeed;
    node["enableSprinting"] = enableSprinting;
    node["enableCrouching"] = enableCrouching;
    node["enableSliding"] = enableSliding;
    node["standHeight"] = standHeight;
    node["crouchHeight"] = crouchHeight;
    node["playerRadius"] = playerRadius;
    node["mouseSens"] = mouseSens;
    node["maxPitchAngle"] = maxPitchAngle;
    return node;
}

void PlayerComponent::Init(Syngine::CameraComponent* camera) {
    m_transform   = m_owner->GetComponent<TransformComponent>();

    m_camera = camera;
    m_window = Window::_GetSDLWindow();
    m_physicsManager = Syngine::Core::_GetApp()->physicsManager.get();

    if (!m_owner || !m_transform || !m_camera) {
        Syngine::Logger::Error(
            "PlayerComponent is missing a required component");
        return;
    }

    JPH::Ref<JPH::CharacterSettings> settings = new JPH::CharacterSettings();
    settings->mMaxSlopeAngle                  = bx::kPi / 4.0f; // 45 degrees
    settings->mLayer = Syngine::Layers::MOVING; // Set to the moving layer
    settings->mShape =
        new JPH::CapsuleShape(standHeight, playerRadius); // Half (quarter?) height and radius
    settings->mFriction = 0.45f;
    settings->mMass     = 80.0f;

    float* pos = m_transform->GetPosition();
    JPH::RVec3 initialPosition(pos[0], pos[1], pos[2]);

    m_character = new JPH::Character(settings,
                                     initialPosition,
                                     JPH::Quat::sIdentity(),
                                     0,
                                     &m_physicsManager->_GetPhysicsSystem());
    m_character->AddToPhysicsSystem(JPH::EActivation::Activate);

    m_camera->SetPosition(pos[0], pos[1], pos[2]);
}

PlayerComponent::~PlayerComponent() {
    if (m_character) {
        m_character->RemoveFromPhysicsSystem();
        m_character = nullptr;
    }
}

Syngine::ComponentTypeID PlayerComponent::GetComponentType() {
    return SYN_COMPONENT_PLAYER;
}

void PlayerComponent::_HandleInput(const SDL_Event& event) {
    if (!m_transform || !m_camera) {
        Syngine::Logger::Error("PlayerComponent is missing a required component");
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

void PlayerComponent::Update(float deltaTime) {
    if (!m_transform || !m_camera) {
        Syngine::Logger::Error("PlayerComponent is missing a required component");
        return;
    }
    
    // Fetch keyboard state internally
    const bool* keystate = SDL_GetKeyboardState(NULL);
    
    m_prevPlayerState = m_playerState;
    m_deltaTime = deltaTime;

    // Sliding persists between frames, so we don't reset it here
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
        m_targetEyeHeight = 0.5f;
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
            BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();
            bodyInterface.SetFriction(m_character->GetBodyID(), 0.25f);
            m_targetFov = 100.0f;
            //m_targetEyeHeight = 0.7f; // Lower eye height when sliding
            m_targetMoveSpeed -= 4.5f * m_deltaTime;
            if (m_targetMoveSpeed < 0.01f) {
                m_targetMoveSpeed = 0.0f;
            }
        } else {
            BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();
            bodyInterface.SetFriction(m_character->GetBodyID(), 0.45f);
            // Reset move direction
            m_moveDirection = { 0.0f, 0.0f, 0.0f };
        }

        // Shrink collider when crouching or sliding
        if (m_playerState == PlayerState::CROUCHING || m_playerState == PlayerState::SLIDING) {
            if (m_prevPlayerState != PlayerState::CROUCHING && m_prevPlayerState != PlayerState::SLIDING) {
                m_character->SetShape(new JPH::CapsuleShape(crouchHeight, playerRadius), 0.1f); // Half height and radius
            }
            m_targetEyeHeight = 0.2f; // Lower eye height when crouching or sliding
            
        } else { // Reset sizes only when state changes 
            if (m_prevPlayerState == PlayerState::CROUCHING || m_prevPlayerState == PlayerState::SLIDING) {
                // Adjust position so collider actually changes size
                float* pos = m_transform->GetPosition();
                pos[1] += (standHeight - crouchHeight);
                m_character->SetPosition(JPH::RVec3(pos[0], pos[1], pos[2]));

                // Reset to normal size
                m_character->SetShape(
                    new JPH::CapsuleShape(standHeight, playerRadius),
                    0.1f); // Reset to normal size
            }
            m_targetEyeHeight = 0.5f; // Normal eye height
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
                    desiredHorizontalVel.SetX(m_moveDirection.x * m_realMoveSpeed);
                    desiredHorizontalVel.SetZ(m_moveDirection.z * m_realMoveSpeed);
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
                    const float jumpForce = 4.0f;
                    m_newVelocity.SetY(jumpForce);
                }
                
            } else {
                Syngine::Logger::Error("PlayerComponent has no character object!");
            }
    }
        
    // Update the character
    m_character->SetLinearVelocity(m_newVelocity);

    // Update some lerps
    // Higher is faster
    const float fovLerpSpeed = 15.0f;
    const float eyeHeightLerpSpeed = 8.0f;
    const float moveSpeedLerpSpeed = 10.0f;

    m_camera->SetFOV(bx::lerp(m_camera->GetFOV(), m_targetFov, 1.0f - bx::exp(-fovLerpSpeed * m_deltaTime)));
    m_eyeHeight = bx::lerp(m_eyeHeight, m_targetEyeHeight, 1.0f - bx::exp(-eyeHeightLerpSpeed * m_deltaTime));
    m_realMoveSpeed = bx::lerp(m_realMoveSpeed, m_targetMoveSpeed, 1.0f - bx::exp(-moveSpeedLerpSpeed * m_deltaTime));
}

void PlayerComponent::PostPhysicsUpdate() {
    if (!m_transform || !m_camera) {
        Syngine::Logger::Error("PlayerComponent is missing a required component");
        return;
    }

    if (m_character) {
        const float maxSeparationDistance = 0.1f; // Distance to consider "grounded"
        m_character->PostSimulation(maxSeparationDistance);
    }

    // Update transform
    float* playerPos = m_transform->GetPosition();
    JPH::RVec3 charPos    = m_character->GetPosition();
    bx::Vec3   targetPos  = { charPos.GetX(), charPos.GetY(), charPos.GetZ() };

    float* pos = m_transform->GetPosition();
    bx::Vec3   currentPos = { pos[0], pos[1], pos[2] };

    const float positionLerpSpeed = 12.0f;
    bx::Vec3   newPos     = bx::lerp(currentPos, targetPos, 1.0f - bx::exp(-positionLerpSpeed * m_deltaTime));
    m_transform->SetPosition(newPos.x, newPos.y, newPos.z);

    // Update camera position and orientation
    m_camera->SetPosition(newPos.x, newPos.y + m_eyeHeight, newPos.z);
    m_camera->SetAngles(m_currentYaw, m_currentPitch);
}

PlayerState PlayerComponent::GetPlayerState() const {
    if (!m_character) {
        Syngine::Logger::Error("PlayerComponent has no character object!");
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

static Syngine::ComponentRegistrar s_playerRegistrar(
    Syngine::SYN_COMPONENT_PLAYER,

    // ParseXml
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node["type"] = static_cast<int>(Syngine::SYN_COMPONENT_PLAYER);
        // Just collapse this loop please
        for (const auto& attr : elem->attributes()) {
            const std::string key = attr->tag().cstr();
            const std::string value = attr->data().cstr();

            if (key == "moveSpeed") {
                node[key] = std::stof(value);
            } else if (key == "sprintMult") {
                node[key] = std::stof(value);
            } else if (key == "crouchSpeed") {
                node[key] = std::stof(value);
            } else if (key == "enableSprinting") {
                node[key] = value == "true";
            } else if (key == "enableCrouching") {
                node[key] = value == "true";
            } else if (key == "enableSliding") {
                node[key] = value == "true";
            } else if (key == "standHeight") {
                node[key] = std::stof(value);
            } else if (key == "crouchHeight") {
                node[key] = std::stof(value);
            } else if (key == "playerRadius") {
                node[key] = std::stof(value);
            } else if (key == "mouseSens") {
                node[key] = std::stof(value);
            } else if (key == "maxPitchAngle") {
                node[key] = std::stof(value);
            }
        }
        return node;
    },

    // Instantiate
    [](GameObject* owner, const Serializer::DataNode& data) -> std::unique_ptr<Component> {
        auto playerComp = std::make_unique<PlayerComponent>(owner, nullptr);
        playerComp->moveSpeed = data["moveSpeed"].As<float>(5.0f);
        playerComp->sprintMult = data["sprintMult"].As<float>(1.5f);
        playerComp->crouchSpeed = data["crouchSpeed"].As<float>(0.5f);
        playerComp->enableSprinting = data["enableSprinting"].As<bool>(true);
        playerComp->enableCrouching = data["enableCrouching"].As<bool>(true);
        playerComp->enableSliding = data["enableSliding"].As<bool>(true);
        playerComp->standHeight = data["standHeight"].As<float>(1.8f);
        playerComp->crouchHeight = data["crouchHeight"].As<float>(1.0f);
        playerComp->playerRadius = data["playerRadius"].As<float>(0.3f);
        playerComp->mouseSens = data["mouseSens"].As<float>(0.1f);
        playerComp->maxPitchAngle = data["maxPitchAngle"].As<float>(89.0f);
        return playerComp;
    }
);

} // namespace Syngine