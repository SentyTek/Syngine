#include "Components/PhysComponent.h"
#include "Jolt/Math/MathTypes.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "SynginePhys.h"
#include "TransformComponent.h"

namespace Syngine {
PhysicsComponent::PhysicsComponent()
    : bodyID(BodyID()), shape(PhysicsShapes::BOX), mass(1.0f), friction(0.5f) {}

PhysicsComponent::~PhysicsComponent() {
    Destroy();
}

SynComponents PhysicsComponent::getComponentType() {
    return SYN_COMPONENT_PHYSICS;
}

void PhysicsComponent::Init(TransformComponent* transform, Syngine::SynginePhys* physicsManager, PhysicsShapes shape, float mass, float friction) {
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
    QuatArg rotationQuat(rotation[0], rotation[1], rotation[2], 1.0f); // Assuming rotation is in degrees

    switch (shape) {
        case PhysicsShapes::SPHERE: {
            float radius = 1.0f; //how on earth do I make this a parameter?
            bodyID = physicsManager->CreateSphere({position[0], position[1], position[2]}, radius, EMotionType::Dynamic, Layers::MOVING);
            break;
        }
        case PhysicsShapes::BOX: {
            float halfSize = 1.0f;
            bodyID = physicsManager->CreateBox({position[0], position[1], position[2]}, rotationQuat, {halfSize, halfSize, halfSize}, EMotionType::Dynamic, Layers::MOVING);
            break;
        }
        default:
            return;
    }
}

void PhysicsComponent::Update() {
    if (!physicsManager || !transform || bodyID.IsInvalid()) {
        return;
    }

    if (!physicsManager || !transform || bodyID.IsInvalid()) return;

    // Sync the TransformComponent with the physics body's position and rotation
    BodyInterface& bodyInterface = physicsManager->GetBodyInterface();
    RVec3 position = bodyInterface.GetPosition(bodyID);

    transform->SetPosition((float)position.GetX(), (float)position.GetY(), (float)position.GetZ());
}

void PhysicsComponent::Destroy() {
    if (physicsManager && !bodyID.IsInvalid()) {
        physicsManager->GetBodyInterface().DestroyBody(bodyID);
        bodyID = BodyID(); // Reset the body ID
    }
}
} // namespace Syngine