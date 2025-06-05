#include "PhysComponent.h"
#include "Jolt/Physics/EActivation.h"
#include "MeshComponent.h"
#include "TransformComponent.h"

#include "SynginePhys.h"
#include "SyngineGameobject.h"

#include "Jolt/Math/MathTypes.h"
#include "Jolt/Physics/Body/MotionType.h"

namespace Syngine {
PhysicsComponent::PhysicsComponent(GameObject* owner)
    : bodyID(BodyID()), shape(PhysicsShapes::BOX), mass(1.0f), friction(0.5f) { this->m_owner = owner; }

PhysicsComponent::~PhysicsComponent() {
    Destroy();
}

SynComponents PhysicsComponent::getComponentType() {
    return SYN_COMPONENT_PHYSICS;
}

void PhysicsComponent::Init(
    TransformComponent* transform,
    Syngine::SynginePhys* physicsManager,
    PhysicsShapes shape,
    float mass,
    float friction,
    JPH::EMotionType motionType,
    JPH::ObjectLayer layer,
    const std::vector<float>& shapeParameters
) {
    this->physicsManager = physicsManager;
    this->transform = transform;
    this->friction = friction;
    this->shape = shape;
    this->mass = mass;

    if (!physicsManager || !transform) {
        return;
    }

    float* position = transform->position;
    float* rotation = transform->rotation;
    JPH::Quat rotationQuat = JPH::Quat::sEulerAngles(JPH::Vec3(rotation[0], rotation[1], rotation[2]));
    JPH::RVec3 posVec(position[0], position[1], position[2]);

    switch (shape) {
        case PhysicsShapes::SPHERE: {
            if (shapeParameters.empty()) { SDL_Log("PhysicsComponent::Init: No radius provided for sphere shape."); return; }
            float radius = 1.0f; //how on earth do I make this a parameter?
            bodyID = physicsManager->CreateSphere(posVec, radius, motionType, layer);
            break;
        }
        case PhysicsShapes::BOX: {
            if (shapeParameters.size() < 3) { SDL_Log("PhysicsComponent::Init: Not enough parameters for box shape."); return; }
            float halfSize = 1.0f;
            bodyID = physicsManager->CreateBox(posVec, rotationQuat, {halfSize, halfSize, halfSize}, motionType, layer);
            break;
        }
        case PhysicsShapes::MESH: {
            if(!m_owner) {
                SDL_Log("PhysicsComponent::Init: No owner game object provided for mesh shape.");
                return;
            }
            MeshComponent* meshComp = m_owner->GetComponent<MeshComponent>();
            if (!meshComp ) {
                SDL_Log("PhysicsComponent::Init: No mesh component or mesh loaded for mesh shape.");
                return;
            }
            bodyID = physicsManager->CreateMeshBody(posVec, rotationQuat, meshComp->meshData, motionType, layer);
            break;
        }
        default:
            return;
    }
}

void PhysicsComponent::Update(bool simulate) {
    if (!physicsManager || !transform || bodyID.IsInvalid()) {
        return;
    }
    if (!physicsManager || !transform || bodyID.IsInvalid()) return;

    BodyInterface& bodyInterface = physicsManager->GetBodyInterface();

    if(simulate) {
        // Sync the TransformComponent with the physics body's position and rotation
        RVec3 position = bodyInterface.GetPosition(bodyID);
        Quat rotation = bodyInterface.GetRotation(bodyID);

        transform->SetPosition(position.GetX(), position.GetY(), position.GetZ());
        transform->SetRotation(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());
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

void PhysicsComponent::Destroy() {
    if (physicsManager && !bodyID.IsInvalid()) {
        BodyInterface& bodyInterface = physicsManager->GetBodyInterface();
        bodyInterface.RemoveBody(bodyID);
        bodyInterface.DestroyBody(bodyID);
        bodyID = BodyID(); // Reset the body ID to an invalid state
    }
}
} // namespace Syngine