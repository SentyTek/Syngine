#include "PlayerComponent.h"
#include "Components.h"
#include "Jolt/Math/Vec3.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"
#include "SyngineGraphics.h"
#include "TransformComponent.h"
#include "RigidbodyComponent.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"
#include "bx/math.h"
#include <cmath>

PlayerComponent::PlayerComponent(GameObject* owner) {
    this->m_owner = owner;
    m_transform = owner->GetComponent<TransformComponent>();
}

SynComponents PlayerComponent::getComponentType() {
    return SYN_COMPONENT_PLAYER;
}

void PlayerComponent::Initialize(Camera*                    camera,
                                 SDL_Window*                win,
                                 Syngine::RigidbodyComponent* RigidbodyComponent) {
    m_camera = camera;
    m_window = win;
    m_RigidbodyComponent = RigidbodyComponent;

    if (!m_owner || !m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "PlayerComponent is missing a required component");
        return;
    }

    // Initialize player component
    currentYaw   = m_camera->yaw;
    currentPitch = m_camera->pitch;

    m_camera->eye[0] = m_transform->position[0];
    m_camera->eye[1] = m_transform->position[1];
    m_camera->eye[2] = m_transform->position[2];
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

void PlayerComponent::Update(float        deltaTime,
                             const bool* keystate,
                             float        moveSpeed,
                             float        sprintMult,
                             float        crouchSpeed) {
    if (!m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
        return;
    }

    float currentMoveSpeed = moveSpeed;
    if (keystate[SDL_SCANCODE_LSHIFT]) currentMoveSpeed *= sprintMult;
    if (keystate[SDL_SCANCODE_LCTRL]) currentMoveSpeed *= crouchSpeed;

    bx::Vec3 moveDirection = { 0.0f, 0.0f, 0.0f };
    bx::Vec3 forwardView   = {
        cosf(currentPitch) * sinf(currentYaw),
        sinf(currentPitch), // Would be worth flattening for XZ regular movement
        cosf(currentPitch) * cosf(currentYaw)
    };
    bx::Vec3 rightView = { sinf(currentYaw - bx::kPiHalf),
                           0.0f,
                           cosf(currentYaw - bx::kPiHalf) };

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

    if (m_RigidbodyComponent && !m_RigidbodyComponent->GetBodyID().IsInvalid()) {
        JPH::BodyInterface& bodyInterface =
            m_RigidbodyComponent->GetPhysicsManager()->GetBodyInterface();
        JPH::BodyID bodyID = m_RigidbodyComponent->GetBodyID();

        JPH::Vec3 currentVel = bodyInterface.GetLinearVelocity(bodyID);
        JPH::Vec3 desiredVel(
            0, currentVel.GetY(), 0); // Keep Y velocity for jumping/falling

        if (bx::length(moveDirection) > 0.0001f) {
            moveDirection = bx::normalize(moveDirection);
            desiredVel.SetX(moveDirection.x * currentMoveSpeed);
            desiredVel.SetZ(moveDirection.z * currentMoveSpeed);
        } else {
            // If no movement keys are pressed, we can zero out the XZ velocity
            desiredVel.SetX(0.0f);
            desiredVel.SetZ(0.0f);
        }

        // Basic jump: apply an upward velocity if jump is pressed (ideally) if
        // grounded Bitch basic ground check: y vel == 0 (ish)
        if (keystate[SDL_SCANCODE_SPACE]) {
            const float jumpForce = 5.0f;

            //World's worst ground detection Gotta update this to contact persist listener
            if (currentVel.GetY() < 1.0f && currentVel.GetY() > -1.0f) { 
                desiredVel.SetY(jumpForce);
            }
        }

        bodyInterface.SetLinearVelocity(bodyID, desiredVel);

        // Prevent capsule from falling over
        // Jolt's Character class handles this better
        bodyInterface.SetAngularVelocity(bodyID, JPH::Vec3::sZero());
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent has no physics object!");
    }

    // Update camera position and orientation
    m_camera->eye[0] = m_transform->position[0];
    m_camera->eye[1] = m_transform->position[1] + eyeHeight; // Fortunately, our eyes aren't at our feet
    m_camera->eye[2] = m_transform->position[2];
    m_camera->yaw    = currentYaw;
    m_camera->pitch  = currentPitch;
}