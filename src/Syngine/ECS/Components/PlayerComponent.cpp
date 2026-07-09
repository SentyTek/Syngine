// ╒════════════════ PlayerComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
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
namespace {
float CapsuleCenterOffset(float height) {
    return height * 0.5f;
}
} // namespace

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
    this->m_eyeHeight       = other.m_eyeHeight;
    this->m_crouchEyeHeight = other.m_crouchEyeHeight;
    this->m_moveDirection = other.m_moveDirection;
    this->m_realMoveSpeed   = other.m_realMoveSpeed;

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
    this->maxPitchAngle   = other.maxPitchAngle;
    this->normalFov       = other.normalFov;
    this->sprintFov       = other.sprintFov;
    this->crouchFov       = other.crouchFov;
    this->slideFov        = other.slideFov;
    this->slideDecay      = other.slideDecay;
    this->slideSpeedMult  = other.slideSpeedMult;

    // Re-initialize the character
    if (this->m_transform) {
        JPH::Ref<JPH::CharacterSettings> settings = new JPH::CharacterSettings();
        settings->mMaxSlopeAngle                  = bx::kPi / 4.0f; // 45 degrees
        settings->mLayer = Syngine::Layers::MOVING; // Set to the moving layer
        settings->mShape =
            new JPH::CapsuleShape(standHeight / 2, playerRadius); // Half height and radius
        settings->mFriction = 0.45f;
        settings->mMass     = 80.0f;

        Vector3 pos = m_transform->GetPosition();
        JPH::RVec3 initialPosition(pos.x(), pos.y() + CapsuleCenterOffset(standHeight), pos.z());
        this->m_character =
            new JPH::Character(settings,
                               initialPosition,
                               JPH::Quat::sIdentity(),
                               0,
                               &this->m_physicsManager->_GetPhysicsSystem());
        this->m_character->AddToPhysicsSystem(JPH::EActivation::Activate);
        this->m_camera->SetPosition(Math::Vector3(pos.x(), pos.y() + m_eyeHeight, pos.z()));
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
    node["maxPitchAngle"]   = maxPitchAngle;
    node["normalFov"]       = normalFov;
    node["sprintFov"]       = sprintFov;
    node["crouchFov"]       = crouchFov;
    node["slideFov"]        = slideFov;
    node["slideDecay"]      = slideDecay;
    node["slideSpeedMult"]  = slideSpeedMult;
    return node;
}

void PlayerComponent::Init(Syngine::CameraComponent* camera) {
    m_transform   = m_owner->GetComponent<TransformComponent>();

    m_camera = camera;
    m_window = Window::_GetSDLWindow();
    m_physicsManager = Syngine::Core::_GetContext()->physicsManager.get();

    if (!m_owner || !m_transform || !m_camera) {
        Syngine::Logger::Error(
            "PlayerComponent is missing a required component");
        return;
    }

    float halfHeight =
        (standHeight / 2.0f) -
        playerRadius; // Capsule half height minus radius to get the real half
                      // height, since Jolt does not include the spherical caps
                      // in the half height parameter

    JPH::Ref<JPH::CharacterSettings> settings = new JPH::CharacterSettings();
    settings->mMaxSlopeAngle                  = bx::kPi / 4.0f; // 45 degrees
    settings->mLayer = Syngine::Layers::MOVING; // Set to the moving layer
    settings->mShape =
        new JPH::CapsuleShape(halfHeight, playerRadius); // Half height and radius
    settings->mFriction = 0.45f;
    settings->mMass     = 80.0f;

    Vector3 pos = m_transform->GetPosition();
    JPH::RVec3 initialPosition(pos.x(), pos.y() + CapsuleCenterOffset(standHeight), pos.z());

    m_character = new JPH::Character(settings,
                                     initialPosition,
                                     JPH::Quat::sIdentity(),
                                     0,
                                     &m_physicsManager->_GetPhysicsSystem());
    m_character->AddToPhysicsSystem(JPH::EActivation::Activate);

    m_camera->SetPosition(Math::Vector3(pos.x(), pos.y() + m_eyeHeight, pos.z()));
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
            if (event.key.key == SDLK_LCTRL) {
                if (m_playerState == PlayerState::SLIDING) {
                    m_playerState = PlayerState::WALKING; // Stop sliding when releasing crouch
                } else if (m_playerState == PlayerState::CROUCHING) {
                    m_playerState = PlayerState::IDLE; // Stand up immediately on crouch release
                }
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

    m_deltaTime = deltaTime;

    // Sliding persists between frames, so we don't reset it here
    if (m_playerState != PlayerState::SLIDING) {
        m_playerState = PlayerState::IDLE;
    }

    // Local variables to allow for smooth transitions without affecting the actual target values immediately
    float l_targetEyeHeight = m_targetEyeHeight;
    float l_targetFov       = normalFov;
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
        if (enableSprinting && keystate[SDL_SCANCODE_LSHIFT] && m_playerState != PlayerState::SLIDING) {
            m_targetMoveSpeed *= sprintMult;
            l_targetFov = sprintFov;
            m_playerState = PlayerState::SPRINTING;
        }
        if (enableCrouching && keystate[SDL_SCANCODE_LCTRL] && m_playerState != PlayerState::SLIDING) {
            m_targetMoveSpeed *= crouchSpeed;
            //m_targetEyeHeight = 0.7f;
            l_targetFov       = crouchFov;
            m_playerState = PlayerState::CROUCHING;
        }

        // If both sprint and crouch are pressed, we slide
        if (enableSliding && keystate[SDL_SCANCODE_LSHIFT] && keystate[SDL_SCANCODE_LCTRL] && isGrounded && m_prevPlayerState == PlayerState::SPRINTING) {
            m_playerState = PlayerState::SLIDING;
            m_targetMoveSpeed *= sprintMult * slideSpeedMult; // Increase speed while sliding
            //m_targetEyeHeight = 0.7f;
            l_targetFov       = slideFov;
        }

        // Low friction when sliding
        if (m_playerState == PlayerState::SLIDING) {
            BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();
            bodyInterface.SetFriction(m_character->GetBodyID(), 0.25f);
            l_targetFov = slideFov;
            //m_targetEyeHeight = 0.7f; // Lower eye height when sliding
            m_targetMoveSpeed -= slideDecay * m_deltaTime;
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
            if (m_prevPlayerState != PlayerState::CROUCHING &&
                m_prevPlayerState != PlayerState::SLIDING) {
                float halfHeight =
                    (crouchHeight / 2.0f) -
                    playerRadius; // New half height for crouching (Note: See
                                  // the earlier comment about how Jolt handles
                                  // capsule heights)
                m_character->SetShape(new JPH::CapsuleShape(halfHeight, playerRadius), 0.1f); // Half height and radius

                Vector3 feetPos = m_transform->GetPosition();
                m_character->SetPosition(JPH::RVec3(feetPos.x(), feetPos.y() + CapsuleCenterOffset(crouchHeight), feetPos.z()));
            }
            l_targetEyeHeight =
                m_crouchEyeHeight; // Lower eye height when crouching or sliding

            if (m_newVelocity.Length() < 0.5f &&
                m_playerState == PlayerState::SLIDING) {
                m_playerState = PlayerState::CROUCHING;
            }

        } else { // Reset sizes only when state changes
            if (m_prevPlayerState == PlayerState::CROUCHING || m_prevPlayerState == PlayerState::SLIDING) {

                Vector3 feetPos = m_transform->GetPosition();
                m_character->SetPosition(JPH::RVec3(feetPos.x(), feetPos.y() + CapsuleCenterOffset(standHeight), feetPos.z()));

                // Reset to normal size
                float halfHeight =
                    (standHeight / 2.0f) -
                    playerRadius; // New half height for standing (Note: See the
                                  // earlier comment about how Jolt handles
                                  // capsule heights)
                m_character->SetShape(
                    new JPH::CapsuleShape(halfHeight, playerRadius),
                    0.1f); // Reset to normal size
            }
            l_targetEyeHeight = m_targetEyeHeight; // Normal eye height
        }

        // For ground movement, typically we want to ignore the Y component
        // Y gets applied separately for jumping or falling
        /*bx::Vec3 forward = bx::normalize(
            bx::Vec3(sinf(m_currentYaw), 0.0f, cosf(m_currentYaw)));
        bx::Vec3 right =
            bx::normalize(bx::Vec3(sinf(m_currentYaw - bx::kPiHalf),
                                   0.0f,
                                   cosf(m_currentYaw - bx::kPiHalf)));*/

        Math::Vector3 forward =
            Math::Vector3(sinf(m_currentYaw), 0.0f, cosf(m_currentYaw))
                .normalized();
        Math::Vector3 right = Math::Vector3(sinf(m_currentYaw - bx::kPiHalf),
                                            0.0f,
                                            cosf(m_currentYaw - bx::kPiHalf))
                                  .normalized();

        if (enableMovement && m_playerState != PlayerState::SLIDING) {
            if (keystate[SDL_SCANCODE_W]) {
                m_moveDirection = m_moveDirection + forward;
            }
            if (keystate[SDL_SCANCODE_S]) {
                m_moveDirection = m_moveDirection - forward;
            }
            if (keystate[SDL_SCANCODE_A]) {
                m_moveDirection = m_moveDirection + right;
            }
            if (keystate[SDL_SCANCODE_D]) {
                m_moveDirection = m_moveDirection - right;
            }
        }

        if (m_character) {
            JPH::Vec3 desiredHorizontalVel(0, 0, 0);
            if (m_moveDirection.length() > 0.001f) {
                m_moveDirection = m_moveDirection.normalized();
                desiredHorizontalVel.SetX(m_moveDirection.x() * m_realMoveSpeed);
                desiredHorizontalVel.SetZ(m_moveDirection.z() * m_realMoveSpeed);
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
    m_prevPlayerState = m_playerState;

    // Update the character
    m_character->SetLinearVelocity(m_newVelocity);

    // Update some lerps
    // Higher is faster
    const float fovLerpSpeed = 15.0f;
    const float eyeHeightLerpSpeed = 8.0f;
    const float moveSpeedLerpSpeed = 10.0f;

    m_camera->SetFOV(bx::lerp(m_camera->GetFOV(), l_targetFov, 1.0f - bx::exp(-fovLerpSpeed * m_deltaTime)));
    m_eyeHeight = bx::lerp(m_eyeHeight, l_targetEyeHeight, 1.0f - bx::exp(-eyeHeightLerpSpeed * m_deltaTime));
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

    // Update transform from the character center, then convert back to feet.
    JPH::RVec3 charPos = m_character->GetPosition();
    const float colliderHeight =
        (m_playerState == PlayerState::CROUCHING || m_playerState == PlayerState::SLIDING)
            ? crouchHeight
            : standHeight;
    bx::Vec3 targetPos = {
        charPos.GetX(),
        charPos.GetY() - CapsuleCenterOffset(colliderHeight),
        charPos.GetZ()
    };

    bx::Vec3 currentPos = m_transform->GetPosition().toBxVec3();

    const float positionLerpSpeed = 12.0f;
    bx::Vec3 newPos = bx::lerp(currentPos, targetPos, 1.0f - bx::exp(-positionLerpSpeed * m_deltaTime));
    m_transform->SetWorldPosition(Vector3(newPos));

    // Update camera position and orientation
    m_camera->SetPosition(Math::Vector3(newPos.x, newPos.y + m_eyeHeight, newPos.z));
    m_camera->SetAngles(m_currentYaw, m_currentPitch);
}

PlayerState PlayerComponent::GetPlayerState() const {
    if (!m_character) {
        Syngine::Logger::Error("PlayerComponent has no character object!");
        return PlayerState::INVALID;
    }

    return m_playerState;
}

Math::Vector2 PlayerComponent::GetRotation() const {
    return Math::Vector2(m_currentYaw, m_currentPitch);
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
            } else if (key == "normalFov") {
                node[key] = std::stof(value);
            } else if (key == "sprintFov") {
                node[key] = std::stof(value);
            } else if (key == "crouchFov") {
                node[key] = std::stof(value);
            } else if (key == "slideFov") {
                node[key] = std::stof(value);
            } else if (key == "slideDecay") {
                node[key] = std::stof(value);
            } else if (key == "slideSpeedMult") {
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
        playerComp->maxPitchAngle   = data["maxPitchAngle"].As<float>(89.0f);
        playerComp->normalFov       = data["normalFov"].As<float>(70.0f);
        playerComp->sprintFov       = data["sprintFov"].As<float>(90.0f);
        playerComp->crouchFov       = data["crouchFov"].As<float>(60.0f);
        playerComp->slideFov        = data["slideFov"].As<float>(100.0f);
        playerComp->slideDecay      = data["slideDecay"].As<float>(4.5f);
        playerComp->slideSpeedMult  = data["slideSpeedMult"].As<float>(1.45f);
        return playerComp;
    }
);

} // namespace Syngine
