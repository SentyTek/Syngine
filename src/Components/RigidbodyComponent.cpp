// ╒═════════════ RigidbodyComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "RigidbodyComponent.h"
#include "Components.h"
#include "SyngineCore.h"
#include "SyngineLogger.h"
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
RigidbodyComponent::RigidbodyComponent(GameObject* owner, Syngine::RigidbodyParameters params) {
    this->m_owner = owner;
    this->Init(params);
}

RigidbodyComponent::~RigidbodyComponent() {
    Destroy();
}

Components RigidbodyComponent::GetComponentType() {
    return SYN_COMPONENT_RIGIDBODY;
}

JPH::BodyID RigidbodyComponent::_GetBodyID() const { return bodyID; }
Syngine::Phys* RigidbodyComponent::_GetPhysicsManager() const { return physicsManager; }
std::vector<float> RigidbodyComponent::GetShapeParameters() const { return shapeParameters; }
float RigidbodyComponent::GetMass() const { return mass; }
float RigidbodyComponent::GetFriction() const { return friction; }
float RigidbodyComponent::GetRestitution() const { return restitution; }

// Initialize the RigidbodyComponent with the given parameters.
void RigidbodyComponent::Init(Syngine::RigidbodyParameters params) {
    this->physicsManager = Syngine::Core::_GetApp()->physicsManager;
    this->transform      = this->m_owner->GetComponent<TransformComponent>();
    this->mass           = params.mass;
    this->friction       = params.friction;
    this->restitution    = params.restitution;
    this->shape          = params.shape;
    this->shapeParameters = params.shapeParameters;

    if (!physicsManager || !transform) {
        return;
    }

    float* position = transform->position;
    float* rotation = transform->rotation;
    JPH::Quat rotationQuat = JPH::Quat::sEulerAngles(JPH::Vec3(rotation[0], rotation[1], rotation[2]));
    JPH::RVec3 posVec(position[0], position[1], position[2]);

    switch (shape) {
        case PhysicsShapes::SPHERE: {
            if (shapeParameters.empty()) { Syngine::Logger::Error("RigidbodyComponent::Init: No radius provided for sphere shape."); return; }
            float radius = shapeParameters[0]/2;
            bodyID = physicsManager->_CreateSphere(posVec, radius, params.motionType, params.layer, mass);
            break;
        }
        case PhysicsShapes::BOX: {
            if (shapeParameters.size() < 3) {
                Syngine::Logger::Error("RigidbodyComponent::Init: Not enough parameters for box shape.");
                return;
            }
            JPH::Vec3 shapeParametersVec(shapeParameters[0]/2, shapeParameters[1]/2, shapeParameters[2]/2);
            bodyID         = physicsManager->_CreateBox(posVec,
                                               rotationQuat,
                                               shapeParametersVec,
                                               params.motionType,
                                               params.layer,
                                               mass);
            break;
        }
        case PhysicsShapes::MESH: {
            if(!m_owner) {
                Syngine::Logger::Error("RigidbodyComponent::Init: No owner game object provided for mesh shape.");
                return;
            }
            MeshComponent* meshComp = m_owner->GetComponent<MeshComponent>();
            if (!meshComp ) {
                Syngine::Logger::Error("RigidbodyComponent::Init: No mesh component or mesh loaded for mesh shape.");
                return;
            }

            JPH::Vec3 scale(0.5f, 0.5f, 0.5f);
            if (!shapeParameters.empty()) {
                scale = JPH::Vec3(shapeParameters[0]/2, shapeParameters[1]/2, shapeParameters[2]/2);
            }

            bodyID = physicsManager->_CreateMeshBody(posVec, rotationQuat, meshComp->meshData, params.motionType, params.layer, scale);
            break;
        }
        case PhysicsShapes::CAPSULE: {
            if (shapeParameters.size() < 2) { Syngine::Logger::Error("RigidbodyComponent::Init: Not enough parameters for capsule shape."); return; }
            float radius = shapeParameters[0];
            float halfHeight = shapeParameters[1];
            bodyID = physicsManager->_CreateCapsule(posVec, radius, halfHeight, params.motionType, params.layer, mass);
            break;
        }
        case PhysicsShapes::CYLINDER: {
            if (shapeParameters.size() < 2) { Syngine::Logger::Error("RigidbodyComponent::Init: Not enough parameters for cylinder shape."); return; }
            float radius = shapeParameters[1];
            float halfHeight = shapeParameters[0];
            bodyID = physicsManager->_CreateCylinder(posVec, rotationQuat, radius, halfHeight, params.motionType, params.layer, mass);
            break;
        }
        default:
            return;
    }

    if (!bodyID.IsInvalid()) {
        JPH::BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();
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

    BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();

    if(simulate) {
        // Smoothly lerp the TransformComponent towards the physics body's
        // position and rotation over time
        static const float lerpAlpha = 1.0f / 5.0f;

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
        BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();
        bodyInterface.RemoveBody(bodyID);
        bodyInterface.DestroyBody(bodyID);
        bodyID = BodyID(); // Reset the body ID to an invalid state
    }
}

void RigidbodyComponent::UpdateShapeParameters(const std::vector<float>& newShapeParameters) {
    if (bodyID.IsInvalid() || !physicsManager) {
        Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: Invalid body ID or physics manager.");
        return;
    }

    JPH::BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();
    JPH::Shape*         shape         = nullptr;

    // Store new parameters. For MESH shape, interpreted as scalars
    // For other shapes, they are dimensions (radius, extents, etc.)
    this->shapeParameters = newShapeParameters;

    switch (this->shape) {
    case PhysicsShapes::SPHERE: {
        if (this->shapeParameters.empty()) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: No radius provided for sphere shape.");
            return;
        }
        float radius = this->shapeParameters[0];
        shape = new JPH::SphereShape(radius);
        break;
    }
    case PhysicsShapes::BOX: {
        if (this->shapeParameters.size() < 3) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: Not enough parameters for box shape.");
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
                Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: Not enough parameters for capsule shape (expected 2: radius, halfHeightOfCylinder).");
                return;
            }
            float radius = this->shapeParameters[0];
            float halfHeightCylinder = this->shapeParameters[1];
            shape = new JPH::CapsuleShape(halfHeightCylinder, radius);
            break;
        }
    case PhysicsShapes::CYLINDER: {
        if (this->shapeParameters.size() < 2) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: Not enough parameters for cylinder shape (expected 2: radius, halfHeight).");
            return;
        }
        float radius = this->shapeParameters[0];
        float halfHeight = this->shapeParameters[1];
        shape = new JPH::CylinderShape(halfHeight, radius);
        break;
    }
    default:
        Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: Unsupported shape type for update.");
        return;
    }

    if (shape) {
        bodyInterface.SetShape(bodyID, shape, true, JPH::EActivation::Activate);
    } else if (this->shape != PhysicsShapes::MESH) {
        Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: Failed to create shape for the specified type.");
        return;
    }
}

void RigidbodyComponent::SetFriction(float newFriction) {
    if (newFriction < 0.0f) {
        Syngine::Logger::Warn("RigidbodyComponent::SetFriction: Friction cannot be negative.");
        return;
    }
    if (newFriction > 1.0f) {
        Syngine::Logger::Warn("RigidbodyComponent::SetFriction: Friction cannot be greater than 1.0.");
        return;
    }
    if (bodyID.IsInvalid() || !physicsManager) {
        Syngine::Logger::Error("RigidbodyComponent::SetFriction: Invalid body ID or physics manager.");
        return;
    }

    friction = newFriction;
    physicsManager->_GetBodyInterface().SetFriction(bodyID, friction);
}

void RigidbodyComponent::SetRestitution(float newRestitution) {
    if (newRestitution < 0.0f) {
        Syngine::Logger::Warn("RigidbodyComponent::SetRestitution: Restitution cannot be negative.");
        return;
    }
    if (newRestitution > 1.0f) {
        Syngine::Logger::Warn("RigidbodyComponent::SetRestitution: Restitution cannot be greater than 1.0.");
        return;
    }
    if (bodyID.IsInvalid() || !physicsManager) {
        Syngine::Logger::Error("RigidbodyComponent::SetRestitution: Invalid body ID or physics manager.");
        return;
    }

    restitution = newRestitution;
    physicsManager->_GetBodyInterface().SetRestitution(bodyID, restitution);
}

} // namespace Syngine