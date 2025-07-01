#include "PlayerComponent.h"
#include "CameraComponent.h"
#include "Components.h"
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
    settings->mFriction = 0.2f;
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

void PlayerComponent::HandleInput(const SDL_Event& event,
                                  bool             simulate,
                                  float            mouseSens,
                                  float maxPitchAngle) {
    if (!m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
        return;
    }

    if (event.type == SDL_EVENT_MOUSE_MOTION && simulate) {
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
    }
}

void PlayerComponent::Update(const bool* keystate,
                             float        moveSpeed,
                             float        sprintMult,
                             float        crouchSpeed) {
    if (!m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
        return;
    }

    m_character->SetRotation(
        JPH::Quat::sRotation(JPH::Vec3::sAxisY(), currentYaw));
    isGrounded =
        m_character->GetGroundState() == JPH::Character::EGroundState::OnGround;

    // Calculate the target move speed, eye height, and FOV based on input
    m_targetMoveSpeed = moveSpeed;
    m_targetEyeHeight = 1.8f;
    m_targetFov       = 70.0f;
    if (keystate[SDL_SCANCODE_LSHIFT]) {
        m_targetMoveSpeed *= sprintMult;
        m_targetFov = 90.0f;
    }
    if (keystate[SDL_SCANCODE_LCTRL]) {
        m_targetMoveSpeed *= crouchSpeed;
        m_targetEyeHeight = 0.7f;
        m_targetFov = 60.0f;
    }

    m_camera->SetFOV(bx::lerp(m_camera->GetFOV(), m_targetFov, 0.1f));
    eyeHeight = bx::lerp(eyeHeight, m_targetEyeHeight, 0.1f);
    m_moveSpeed = bx::lerp(m_moveSpeed, m_targetMoveSpeed, 0.1f);

    bx::Vec3 moveDirection = { 0.0f, 0.0f, 0.0f };

    // For ground movement, typically we want to ignore the Y component
    // Y gets applied separately for jumping or falling
    bx::Vec3 forward =
        bx::normalize(bx::Vec3(sinf(currentYaw), 0.0f, cosf(currentYaw)));
    bx::Vec3 right = bx::normalize(bx::Vec3(
        sinf(currentYaw - bx::kPiHalf), 0.0f, cosf(currentYaw - bx::kPiHalf)));

    if (keystate[SDL_SCANCODE_W]) {
        moveDirection = bx::add(moveDirection, forward);
    }
    if (keystate[SDL_SCANCODE_S]) {
        moveDirection = bx::sub(moveDirection, forward);
    }
    if (keystate[SDL_SCANCODE_A]) {
        moveDirection = bx::add(moveDirection, right);
    }
    if (keystate[SDL_SCANCODE_D]) {
        moveDirection = bx::sub(moveDirection, right);
    }

    if (m_character) {
        JPH::Vec3 desiredHorizontalVel(0, 0, 0);
        if (bx::length(moveDirection) > 0.001f) {
            moveDirection = bx::normalize(moveDirection);
            desiredHorizontalVel.SetX(moveDirection.x * m_moveSpeed);
            desiredHorizontalVel.SetZ(moveDirection.z * m_moveSpeed);
        }

        // Get current velocity and preserve the vertical component (for gravity)
        JPH::Vec3 currentVel = m_character->GetLinearVelocity();
        JPH::Vec3 newVelocity = JPH::Vec3(desiredHorizontalVel.GetX(), currentVel.GetY(), desiredHorizontalVel.GetZ());

        // JUMP! *van halen guitar solo*
        if (keystate[SDL_SCANCODE_SPACE] && isGrounded) {
            const float jumpForce = 7.0f;
            newVelocity.SetY(jumpForce);
            isGrounded = false;
        }

        // Update the character
        m_character->SetLinearVelocity(newVelocity);
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent has no character object!");
    }
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
    JPH::RVec3 charPos = m_character->GetPosition();
    m_transform->position[0] = charPos.GetX();
    m_transform->position[1] = charPos.GetY();
    m_transform->position[2] = charPos.GetZ();

    // Update camera position and orientation
    m_camera->SetPosition(m_transform->position[0],
                          m_transform->position[1] + eyeHeight,
                          m_transform->position[2]);
    m_camera->SetAngles(currentYaw, currentPitch);
}

} // namespace Syngine