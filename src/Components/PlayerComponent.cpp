#include "PlayerComponent.h"
#include "Components.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_video.h"
#include "SyngineGraphics.h"
#include "TransformComponent.h"
#include "SyngineGameobject.h"
#include <cmath>

PlayerComponent::PlayerComponent(GameObject* owner) {
    this->m_owner = owner;
    m_transform = owner->GetComponent<TransformComponent>();
}

SynComponents PlayerComponent::getComponentType() {
    return SYN_COMPONENT_PLAYER;
}

void PlayerComponent::Initialize(Camera* camera, SDL_Window* win) {
    m_camera = camera;
    m_window = win;
    if (!m_owner || !m_transform || !m_camera) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent is missing a required component");
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
    if (keystate[SDL_SCANCODE_LSHIFT]) {
        currentMoveSpeed *= sprintMult; // Apply sprint multiplier
    } else if (keystate[SDL_SCANCODE_LCTRL]) {
        currentMoveSpeed *= crouchSpeed; // Apply normal speed
    }

    bx::Vec3 moveDirection = { 0.0f, 0.0f, 0.0f };
    bx::Vec3 forward       = { cosf(currentPitch) * sinf(currentYaw),
                               sinf(currentPitch),
                               cosf(currentPitch) * cosf(currentYaw) };
    bx::Vec3 right         = { sinf(currentYaw - bx::kPiHalf),
                               0.0f,
                               cosf(currentYaw - bx::kPiHalf) };

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

    if (bx::length(moveDirection) > 0.0001f) { // Check for non-zero length
        moveDirection = bx::normalize(moveDirection);
        bx::Vec3 velocity =
            bx::mul(moveDirection, currentMoveSpeed * deltaTime);
        m_transform->position[0] += velocity.x;
        m_transform->position[1] += velocity.y;
        m_transform->position[2] += velocity.z;
    }

    // Update camera position and orientation
    m_camera->eye[0] = m_transform->position[0];
    m_camera->eye[1] = m_transform->position[1];
    m_camera->eye[2] = m_transform->position[2];
    m_camera->yaw    = currentYaw;
    m_camera->pitch  = currentPitch;
}