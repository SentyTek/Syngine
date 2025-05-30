#include "PlayerComponent.h"
#include "Components.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "SyngineGraphics.h"
#include "TransformComponent.h"
#include "RigidbodyComponent.h"
#include "SyngineGameobject.h"
#include "SynginePhys.h"

#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"

#include "Jolt/Math/Vec3.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Body/BodyLockInterface.h"
#include "Jolt/Physics/Collision/RayCast.h"
#include "Jolt/Physics/Collision/CastResult.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/CollisionCollectorImpl.h" 
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/Body/BodyFilter.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"

#include "bx/math.h"

#include <cmath>

PlayerComponent::PlayerComponent(GameObject* owner) {
    this->m_owner = owner;
    m_transform = owner->GetComponent<TransformComponent>();
}

SynComponents PlayerComponent::getComponentType() {
    return SYN_COMPONENT_PLAYER;
}

void PlayerComponent::CheckGrounded() {
    if (!m_RigidbodyComponent || m_RigidbodyComponent->GetBodyID().IsInvalid()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PlayerComponent has no physics object!");
        return;
    }

    Syngine::SynginePhys* physicsManager =
        m_RigidbodyComponent->GetPhysicsManager();
    JPH::PhysicsSystem& physicsSystem = physicsManager->GetPhysicsSystem();
    JPH::BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();
    JPH::BodyID         bodyID        = m_RigidbodyComponent->GetBodyID();
    const JPH::BodyLockInterfaceNoLock& bodyLockInt =
        physicsSystem.GetBodyLockInterfaceNoLock();
    JPH::RVec3 capsulePos = bodyInterface.GetCenterOfMassPosition(bodyID);

    float halfHeight = m_RigidbodyComponent->GetShapeParameters()[0];
    float radius     = m_RigidbodyComponent->GetShapeParameters()[1];

    JPH::RVec3 rayOrigin =
        capsulePos - JPH::Vec3(0.0f,
                  halfHeight,
                  0.0f); // Start ray sightly above the capsule bottom
    rayOrigin.SetY(rayOrigin.GetY() +
                   0.05f); // small offset to avoid starting ray inside object

    JPH::Vec3 rayDirection(0.0f, -1.0f, 0.0f); // Downward ray
    float rayLength = radius + 0.15f; // Length of the ray, slightly longer than the radius

    JPH::RRayCast ray{ rayOrigin, rayDirection * rayLength };
    JPH::RayCastSettings raysettings;

    // Collector
    JPH::ClosestHitCollisionCollector<JPH::CastRayCollector> collector;

    // Query (god Jolt is a pain)
    JPH::ObjectLayer playerObjectLayer = bodyInterface.GetObjectLayer(bodyID);
    const JPH::BroadPhaseLayerInterface& bpLayerInterface =
        physicsManager->GetBroadPhaseLayerInterface(); // Get from SynginePhys

    JPH::DefaultBroadPhaseLayerFilter broadphaseFilter(
        physicsManager->GetObjectVsBroadPhaseLayerFilter(), playerObjectLayer);
    JPH::DefaultObjectLayerFilter objectFilter(
        physicsManager->GetObjectLayerPairFilter(), playerObjectLayer);
    JPH::IgnoreSingleBodyFilter bodyFilter(bodyID);

    physicsSystem.GetNarrowPhaseQuery().CastRay(ray,
                                                raysettings,
                                                collector,
                                                broadphaseFilter,
                                                objectFilter,
                                                bodyFilter);
    
    if (collector.HadHit()) {
        JPH::BodyID hitBodyID = collector.mHit.mBodyID;

        // Lock the body that was hit
        JPH::BodyLockRead hitBodyLock(bodyLockInt, hitBodyID);
        if (hitBodyLock.Succeeded()) {
            const JPH::Body& actualHitBody = hitBodyLock.GetBody();

            // Calculate the world space hit position
            JPH::RVec3 hitPosition = rayOrigin + collector.mHit.mFraction * (rayDirection * rayLength);

            // Get the surface normal from the actual hit body
            JPH::Vec3 contactNormal = actualHitBody.GetWorldSpaceSurfaceNormal(collector.mHit.mSubShapeID2, hitPosition);

            if (contactNormal.GetY() >
                0.4f) { // Check if normal is mostly upward
                isGrounded = true;
                lastGroundNormal = contactNormal.GetY(); // Store the last ground normal
                return;
            }
        }
        isGrounded = false;
        return;
    }

    isGrounded = false;
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
    float m_targetEyeHeight = eyeHeight;
    float m_targetFov        = m_camera->fov;
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

    CheckGrounded();

    float currentMoveSpeed = moveSpeed;
    m_targetEyeHeight      = 1.8f;
    m_targetFov            = 70.0f;
    if (keystate[SDL_SCANCODE_LSHIFT]) {
        currentMoveSpeed *= sprintMult;
        m_targetFov = 90.0f;
    }
    if (keystate[SDL_SCANCODE_LCTRL]) {
        currentMoveSpeed *= crouchSpeed;
        m_targetEyeHeight = 0.7f;
        m_targetFov = 60.0f;
    }

    m_camera->fov = bx::lerp(m_camera->fov, m_targetFov, 0.1f);
    eyeHeight = bx::lerp(eyeHeight, m_targetEyeHeight, 0.1f);

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

        // Basic jump: apply an upward velocity if jump is pressed (ideally) if grounded
        if (keystate[SDL_SCANCODE_SPACE] && isGrounded) {
            const float jumpForce = 5.0f;
            desiredVel.SetY(jumpForce);
            isGrounded = false;
        }

        bodyInterface.SetLinearVelocity(bodyID, desiredVel);

        // Prevent capsule from falling over
        // Jolt's Character class handles this better
        bodyInterface.SetAngularVelocity(bodyID, JPH::Vec3::sZero());

        // Stop sliding on slopes
        // If the player is grounded and not intentionally moving, zero out velocity
        if (m_RigidbodyComponent && !m_RigidbodyComponent->GetBodyID().IsInvalid() && isGrounded) {
            JPH::Vec3 currentVelocity = bodyInterface.GetLinearVelocity(bodyID);
            float     horizontalSpeed = bx::length(bx::Vec3(desiredVel.GetX(), 0.0f, desiredVel.GetZ()));

            const float stopSlidingSpeed = 0.01f; // Speed below which we stop sliding

            if (bx::length(moveDirection) < 0.01f && horizontalSpeed > 1e-6f &&
                horizontalSpeed < stopSlidingSpeed &&
                lastGroundNormal > 0.6f) { // 0.6f is a threshold for "flat" ground
                bodyInterface.SetLinearVelocity(
                    bodyID, JPH::Vec3(0.0f, currentVelocity.GetY(), 0.0f));
            }
        }
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