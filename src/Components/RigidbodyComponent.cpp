#include "RigidbodyComponent.h"
#include "Components.h"
#include "Jolt/Math/Quat.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/EActivation.h"
#include "MeshComponent.h"
#include "TransformComponent.h"

#include "SynginePhys.h"
#include "SyngineGameobject.h"

#include "Jolt/Math/MathTypes.h"
#include "Jolt/Physics/Body/MotionType.h"
#include <vector>

namespace Syngine {
RigidbodyComponent::RigidbodyComponent(GameObject* owner)
    : bodyID(BodyID()), shape(PhysicsShapes::BOX), mass(1.0f), friction(0.5f) { this->m_owner = owner; }

RigidbodyComponent::~RigidbodyComponent() {
    Destroy();
}

Components RigidbodyComponent::getComponentType() {
    return SYN_COMPONENT_RIGIDBODY;
}

JPH::BodyID RigidbodyComponent::GetBodyID() const { return bodyID; }
Syngine::Phys* RigidbodyComponent::GetPhysicsManager() const { return physicsManager; }
std::vector<float> RigidbodyComponent::GetShapeParameters() const { return shapeParameters; }
float RigidbodyComponent::GetMass() const { return mass; }
float RigidbodyComponent::GetFriction() const { return friction; }
float RigidbodyComponent::GetRestitution() const { return restitution; }

void RigidbodyComponent::Init(TransformComponent*       transform,
                              Syngine::Phys*            physicsManager,
                              PhysicsShapes             shape,
                              const std::vector<float>& parameters,
                              JPH::EMotionType          motionType,
                              JPH::ObjectLayer          layer,
                              const std::vector<float>& shapeParameters) {
    if (parameters.size() < 3) {
        SDL_Log("RigidbodyComponent::Init: Not enough parameters provided.");
        return;
    }
    this->physicsManager = physicsManager;
    this->transform      = transform;
    this->friction       = parameters[0];
    this->mass           = parameters[1];
    this->restitution    = parameters[2];
    this->shape          = shape;
    this->shapeParameters = shapeParameters;

    if (!physicsManager || !transform) {
        return;
    }

    float* position = transform->position;
    float* rotation = transform->rotation;
    JPH::Quat rotationQuat = JPH::Quat::sEulerAngles(JPH::Vec3(rotation[0], rotation[1], rotation[2]));
    JPH::RVec3 posVec(position[0], position[1], position[2]);

    switch (shape) {
        case PhysicsShapes::SPHERE: {
            if (shapeParameters.empty()) { SDL_Log("RigidbodyComponent::Init: No radius provided for sphere shape."); return; }
            float radius = shapeParameters[0];
            bodyID = physicsManager->CreateSphere(posVec, radius, motionType, layer, mass);
            break;
        }
        case PhysicsShapes::BOX: {
            if (shapeParameters.size() < 3) {
                SDL_Log("RigidbodyComponent::Init: Not enough parameters for "
                        "box shape.");
                return;
            }
            JPH::Vec3 shapeParametersVec(shapeParameters[0], shapeParameters[1], shapeParameters[2]);
            bodyID         = physicsManager->CreateBox(posVec,
                                               rotationQuat,
                                               shapeParametersVec,
                                               motionType,
                                               layer,
                                               mass);
            break;
        }
        case PhysicsShapes::MESH: {
            if(!m_owner) {
                SDL_Log("RigidbodyComponent::Init: No owner game object provided for mesh shape.");
                return;
            }
            MeshComponent* meshComp = m_owner->GetComponent<MeshComponent>();
            if (!meshComp ) {
                SDL_Log("RigidbodyComponent::Init: No mesh component or mesh loaded for mesh shape.");
                return;
            }
            bodyID = physicsManager->CreateMeshBody(posVec, rotationQuat, meshComp->meshData, motionType, layer);
            break;
        }
        case PhysicsShapes::CAPSULE: {
            if (shapeParameters.size() < 2) { SDL_Log("RigidbodyComponent::Init: Not enough parameters for capsule shape."); return; }
            float radius = shapeParameters[0];
            float halfHeight = shapeParameters[1];
            bodyID = physicsManager->CreateCapsule(posVec, radius, halfHeight, motionType, layer, mass);
            break;
        }
        case PhysicsShapes::CYLINDER: {
            if (shapeParameters.size() < 2) { SDL_Log("RigidbodyComponent::Init: Not enough parameters for cylinder shape."); return; }
            float radius = shapeParameters[1];
            float halfHeight = shapeParameters[0];
            bodyID = physicsManager->CreateCylinder(posVec, rotationQuat, radius, halfHeight, motionType, layer, mass);
            break;
        }
        default:
            return;
    }

    if (!bodyID.IsInvalid()) {
        JPH::BodyInterface& bodyInterface = physicsManager->GetBodyInterface();
        // Set the body properties (mass set during body creation)
        if (friction > 0) bodyInterface.SetFriction(bodyID, friction);
        if (restitution > 0) bodyInterface.SetRestitution(bodyID, restitution);
    }
}

void RigidbodyComponent::Update(bool simulate) {
    if (!physicsManager || !transform || bodyID.IsInvalid()) {
        return;
    }
    if (!physicsManager || !transform || bodyID.IsInvalid()) return;

    BodyInterface& bodyInterface = physicsManager->GetBodyInterface();

    if(simulate) {
        // Smoothly lerp the TransformComponent towards the physics body's
        // position and rotation over time
        static const float lerpAlpha = 1.0f / 3.0f;

        RVec3 physicsPos = bodyInterface.GetPosition(bodyID);
        Quat  physicsRot = bodyInterface.GetRotation(bodyID);

        Vec3 currentPos(transform->position[0],
                        transform->position[1],
                        transform->position[2]);
        Quat currentRot(transform->rotation[0],
                        transform->rotation[1],
                        transform->rotation[2],
                        transform->rotation[3]);

        // Lerp pos and slerp rot
        Vec3 lerpedPos =
            currentPos +
            (Vec3(physicsPos.GetX(), physicsPos.GetY(), physicsPos.GetZ()) -
             currentPos) *
                lerpAlpha;
        Quat lerpedRot = currentRot.SLERP(physicsRot, lerpAlpha);

        transform->SetPosition(
            lerpedPos.GetX(), lerpedPos.GetY(), lerpedPos.GetZ());
        transform->SetRotation(lerpedRot.GetX(), lerpedRot.GetY(),
                               lerpedRot.GetZ(), lerpedRot.GetW());
    } else {
        Vec3 pos(transform->position[0], 
                  transform->position[1], 
                  transform->position[2]);
        Quat rot(transform->rotation[0],
                  transform->rotation[1],
                  transform->rotation[2],
                  transform->rotation[3]);
        
        bodyInterface.SetPositionAndRotation(bodyID, pos, rot, EActivation::Activate);
    }
}

void RigidbodyComponent::Destroy() {
    if (physicsManager && !bodyID.IsInvalid()) {
        BodyInterface& bodyInterface = physicsManager->GetBodyInterface();
        bodyInterface.RemoveBody(bodyID);
        bodyInterface.DestroyBody(bodyID);
        bodyID = BodyID(); // Reset the body ID to an invalid state
    }
}

void RigidbodyComponent::UpdateShapeParameters(const std::vector<float>& newShapeParameters) {
    if (bodyID.IsInvalid() || !physicsManager) {
        SDL_Log("RigidbodyComponent::UpdateShapeParameters: Invalid body ID or physics manager.");
        return;
    }

    JPH::BodyInterface& bodyInterface = physicsManager->GetBodyInterface();
    JPH::Shape*         shape         = nullptr;

    // Store new parameters. For MESH shape, interpreted as scalars
    // For other shapes, they are dimensions (radius, extents, etc.)
    this->shapeParameters = newShapeParameters;

    switch (this->shape) {
    case PhysicsShapes::SPHERE: {
        if (this->shapeParameters.empty()) {
            SDL_Log("RigidbodyComponent::UpdateShapeParameters: No radius provided for sphere shape.");
            return;
        }
        float radius = this->shapeParameters[0];
        shape = new JPH::SphereShape(radius);
        break;
    }
    case PhysicsShapes::BOX: {
        if (this->shapeParameters.size() < 3) {
            SDL_Log("RigidbodyComponent::UpdateShapeParameters: Not enough parameters for box shape.");
            return;
        }
        JPH::Vec3 extents(this->shapeParameters[0],
                          this->shapeParameters[1],
                          this->shapeParameters[2]);
        shape = new JPH::BoxShape(extents);
        break;
    }
    case PhysicsShapes::CAPSULE: {
            if (this->shapeParameters.size() < 2) {
                SDL_Log("RigidbodyComponent::UpdateShapeParameters: Not enough parameters for capsule shape (expected 2: radius, halfHeightOfCylinder).");
                return;
            }
            float radius = this->shapeParameters[0];
            float halfHeightCylinder = this->shapeParameters[1];
            shape = new JPH::CapsuleShape(halfHeightCylinder, radius);
            break;
        }
    case PhysicsShapes::CYLINDER: {
        if (this->shapeParameters.size() < 2) {
            SDL_Log("RigidbodyComponent::UpdateShapeParameters: Not enough parameters for cylinder shape (expected 2: radius, halfHeight).");
            return;
        }
        float radius = this->shapeParameters[0];
        float halfHeight = this->shapeParameters[1];
        shape = new JPH::CylinderShape(halfHeight, radius);
        break;
    }
    default:
        SDL_Log("RigidbodyComponent::UpdateShapeParameters: Unsupported shape type for update.");
        return;
    }

    if (shape) {
        bodyInterface.SetShape(bodyID, shape, true, JPH::EActivation::Activate);
    } else if (this->shape != PhysicsShapes::MESH) {
        SDL_Log("RigidbodyComponent::UpdateShapeParameters: Failed to create shape for the specified type.");
        return;
    }
}

void RigidbodyComponent::SetFriction(float newFriction) {
    if (newFriction < 0.0f) {
        SDL_Log("RigidbodyComponent::SetFriction: Friction cannot be negative.");
        return;
    }
    if (newFriction > 1.0f) {
        SDL_Log("RigidbodyComponent::SetFriction: Friction cannot be greater than 1.0.");
        return;
    }
    if (bodyID.IsInvalid() || !physicsManager) {
        SDL_Log("RigidbodyComponent::SetFriction: Invalid body ID or physics manager.");
        return;
    }

    friction = newFriction;
    physicsManager->GetBodyInterface().SetFriction(bodyID, friction);
}

void RigidbodyComponent::SetRestitution(float newRestitution) {
    if (newRestitution < 0.0f) {
        SDL_Log("RigidbodyComponent::SetRestitution: Restitution cannot be negative.");
        return;
    }
    if (newRestitution > 1.0f) {
        SDL_Log("RigidbodyComponent::SetRestitution: Restitution cannot be greater than 1.0.");
        return;
    }
    if (bodyID.IsInvalid() || !physicsManager) {
        SDL_Log("RigidbodyComponent::SetRestitution: Invalid body ID or physics manager.");
        return;
    }

    restitution = newRestitution;
    physicsManager->GetBodyInterface().SetRestitution(bodyID, restitution);
}

} // namespace Syngine