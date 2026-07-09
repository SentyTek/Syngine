// ╒═════════════ RigidbodyComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Core.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/ECS/Component.h"
#include "Syngine/ECS/Components/RigidbodyComponent.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/ECS/Components/MeshComponent.h"
#include "Syngine/ECS/Components/TransformComponent.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/Math/Quaternion.hpp"
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Physics/Physics.h"
#include "Syngine/Utils/Serializer.h"

#include "Jolt/Math/Quat.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/EActivation.h"
#include "Jolt/Math/MathTypes.h"
#include "Jolt/Physics/Body/MotionProperties.h"

#include "bx/math.h"
#include <sol/sol.hpp>

#include <memory>
#include <vector>

namespace Syngine {
RigidbodyComponent::RigidbodyComponent(GameObject*                  owner,
                                       Syngine::RigidbodyParameters params) {
    if (!Core::IsPhysicsEnabled()) return;

    this->m_owner = owner;
    this->Init(params);
}

RigidbodyComponent::RigidbodyComponent(const RigidbodyComponent& other) {
    if (!Core::IsPhysicsEnabled()) return;

    this->m_owner = other.m_owner;
    this->physicsManager = other.physicsManager;
    this->transform = other.transform;
    this->bodyID = other.bodyID;
    this->mass = other.mass;
    this->friction = other.friction;
    this->restitution = other.restitution;
    this->shape = other.shape;
    this->shapeParameters = other.shapeParameters;
}

RigidbodyComponent&
RigidbodyComponent::operator=(const RigidbodyComponent& other) {
    if (!Core::IsPhysicsEnabled()) return *this;

    if (this != &other) {
        this->m_owner = other.m_owner;
        this->physicsManager = other.physicsManager;
        this->transform = other.transform;
        this->bodyID = other.bodyID;
        this->mass = other.mass;
        this->friction = other.friction;
        this->restitution = other.restitution;
        this->shape = other.shape;
        this->shapeParameters = other.shapeParameters;
    }
    return *this;
}

RigidbodyComponent::~RigidbodyComponent() {
    Destroy();
}

Syngine::ComponentTypeID RigidbodyComponent::GetComponentType() {
    return SYN_COMPONENT_RIGIDBODY;
}

Serializer::DataNode RigidbodyComponent::Serialize() const {
    Serializer::DataNode node;
    node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_RIGIDBODY);
    node / "mass" = mass;
    node / "friction" = friction;
    node / "restitution" = restitution;
    node / "shape"       = static_cast<int>(shape);
    node / "shapeParameters" = shapeParameters;
    return node;
}

JPH::BodyID RigidbodyComponent::_GetBodyID() const { return bodyID; }
Syngine::Phys* RigidbodyComponent::_GetPhysicsManager() const { return physicsManager; }
Math::Vector3 RigidbodyComponent::GetShapeParameters() const { return shapeParameters; }
float RigidbodyComponent::GetMass() const { return mass; }
float RigidbodyComponent::GetFriction() const { return friction; }
float RigidbodyComponent::GetRestitution() const { return restitution; }

// Initialize the RigidbodyComponent with the given parameters.
void RigidbodyComponent::Init(Syngine::RigidbodyParameters params) {
    pendingParams = params;
    initPending = false;

    if (initComplete && !bodyID.IsInvalid()) {
        return;
    }

    this->physicsManager = Syngine::Core::_GetContext()->physicsManager.get();
    this->transform      = this->m_owner->GetComponent<TransformComponent>();
    this->mass           = params.mass;
    this->friction       = params.friction;
    this->restitution    = params.restitution;
    this->shape          = params.shape;
    this->shapeParameters = params.shapeParameters;
    if (!this->transform) {
        initPending = true;
        Syngine::Logger::Warn("RigidbodyComponent deferred initialization: waiting for TransformComponent");
        return;
    }

    if (!physicsManager || !transform) {
        initPending = true;
        return;
    }

    JPH::RVec3 curPos = transform->GetPosition().toJoltRVec3();
    JPH::Quat transformRot = transform->GetRotationQuaternion().toJoltQuat();

    JPH::Quat physicsRot = transformRot.Conjugated();

    switch (shape) {
        case PhysicsShapes::SPHERE: {
            if (shapeParameters.isZero()) {
                Syngine::Logger::Error("RigidbodyComponent::Init: No radius "
                                       "provided for sphere shape.");
                return;
            }
            float radius = shapeParameters.x();
            bodyID = physicsManager->_CreateSphere(curPos, radius, params.motionType, params.layer, mass);
            break;
        }
        case PhysicsShapes::BOX: {
            if (shapeParameters.isZero()) {
                Syngine::Logger::Error("RigidbodyComponent::Init: No half extents provided for box shape.");
                return;
            }
            JPH::Vec3 shapeParametersVec(shapeParameters.x(), shapeParameters.y(), shapeParameters.z());
            bodyID         = physicsManager->_CreateBox(curPos,
                                               physicsRot,
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

            JPH::Vec3 scale(1.0f, 1.0f, 1.0f);
            if (!shapeParameters.isZero()) {
                scale = JPH::Vec3(shapeParameters.x(), shapeParameters.y(), shapeParameters.z());
            }

            bodyID = physicsManager->_CreateMeshBody(curPos, physicsRot, meshComp->meshData, params.motionType, params.layer, scale, mass);
            break;
        }
        case PhysicsShapes::CAPSULE: {
            if (shapeParameters.isZero()) { Syngine::Logger::Error("RigidbodyComponent::Init: No radius and half height provided for capsule shape."); return; }
            float radius = shapeParameters.x();
            float halfHeight = shapeParameters.y();
            bodyID = physicsManager->_CreateCapsule(curPos, radius, halfHeight, params.motionType, params.layer, mass);
            break;
        }
        case PhysicsShapes::CYLINDER: {
            if (shapeParameters.isZero()) { Syngine::Logger::Error("RigidbodyComponent::Init: Not enough parameters for cylinder shape."); return; }
            float radius = shapeParameters.y();
            float halfHeight = shapeParameters.x();
            bodyID = physicsManager->_CreateCylinder(curPos, physicsRot, radius, halfHeight, params.motionType, params.layer, mass);
            break;
        }
        case PhysicsShapes::COMPOUND: {
            if (params.compoundParts.empty()) {
                Syngine::Logger::Error("RigidbodyComponent::Init: No parts provided for compound shape.");
                return;
            }
            bodyID = physicsManager->_CreateCompound(curPos, physicsRot, params.compoundParts, params.motionType, params.layer, mass);
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
        initComplete = true;
        initPending = false;
    } else {
        initPending = true;
    }
}

void RigidbodyComponent::RetryInitIfPending() {
    if (!initPending || initComplete) {
        return;
    }

    Syngine::Logger::LogF(
        LogLevel::INFO,
        true,
        "RigidbodyComponent: Retrying initialization for GameObject '%s'",
        m_owner->name.c_str());
    Init(pendingParams);
}

void RigidbodyComponent::SyncBodyToTransform() {
    if (initPending && !initComplete) {
        RetryInitIfPending();
    }

    if (!physicsManager || !transform || bodyID.IsInvalid()) {
        return;
    }

    BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();

    JPH::Vec3 pos = transform->GetPosition().toJoltVec3();
    JPH::Quat rot = transform->GetRotationQuaternion().toJoltQuat();
    rot = rot.Conjugated();
    bodyInterface.SetPositionAndRotation(bodyID, pos, rot, EActivation::Activate);
}

void RigidbodyComponent::Update(float deltaTime) {
    if (initPending && !initComplete) {
        RetryInitIfPending();
    }

    if (!physicsManager || !transform || bodyID.IsInvalid()) {
        return;
    }

    BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();
    bool simulate = Syngine::Core::Get()->GetSimulationState();

    if (simulate) {
        // Smoothly lerp the TransformComponent towards the physics body's
        // position and rotation over time
        static const float lerpAlpha = 0.2f;
        // When physics drives the transform
        JPH::Vec3 physicsPos = bodyInterface.GetPosition(bodyID);
        JPH::Quat physicsRot = bodyInterface.GetRotation(bodyID);

        JPH::Vec3 currentPos = transform->GetPosition().toJoltVec3();
        JPH::Quat currentRot = transform->GetRotationQuaternion().toJoltQuat();

        // Lerp pos and slerp rot
        JPH::Vec3 lerpedPos = currentPos + (physicsPos - currentPos) * lerpAlpha;

        JPH::Quat targetRot = physicsRot.Conjugated();
        JPH::Quat lerpedRot = currentRot.SLERP(targetRot, lerpAlpha);

        transform->SetWorldPosition(Vector3(lerpedPos));
        transform->SetWorldRotationQuat(Quaternion(lerpedRot));

    } else {
        JPH::Vec3 pos = transform->GetPosition().toJoltVec3();
        JPH::Quat rot = transform->GetRotationQuaternion().toJoltQuat();
        rot = rot.Conjugated();

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

void RigidbodyComponent::UpdateShapeParameters(const Math::Vector3 newShapeParameters) {
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
        if (this->shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: No radius provided for sphere shape.");
            return;
        }
        float radius = this->shapeParameters.x();
        shape = new JPH::SphereShape(radius);
        break;
    }
    case PhysicsShapes::BOX: {
        if (this->shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: No extents provided for box shape.");
            return;
        }
        JPH::Vec3 extents(this->shapeParameters.x(),
                          this->shapeParameters.y(),
                          this->shapeParameters.z());
        shape = new JPH::BoxShape(extents);
        break;
    }
    case PhysicsShapes::CAPSULE: {
            if (this->shapeParameters.isZero()) {
                Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: No radius and half height provided for capsule shape.");
                return;
            }
            float radius = this->shapeParameters.x();
            float halfHeightCylinder = this->shapeParameters.y();
            shape = new JPH::CapsuleShape(halfHeightCylinder, radius);
            break;
        }
    case PhysicsShapes::CYLINDER: {
        if (this->shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: No radius and half height provided for cylinder shape.");
            return;
        }
        float radius = this->shapeParameters.x();
        float halfHeight = this->shapeParameters.y();
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

//----- Forces and whatnot
// Important to note again that ACCELERATION and VELOCITY_CHANGE modes are
// identical to FORCE and IMPULSE respectively, at least if user did not set
// mass manually during rb creation. Jolt doesn't really support these modes so
// it is what it is.
void RigidbodyComponent::AddForce(const Math::Vector3 force, ForceMode mode) {
    if (bodyID.IsInvalid() || !physicsManager) return;
    JPH::BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();

    switch (mode) {
    case ForceMode::FORCE:
        bodyInterface.AddForce(bodyID, force.toJoltRVec3());
        break;
    case ForceMode::ACCELERATION:
        bodyInterface.AddForce(bodyID, force.toJoltRVec3() * (mass == 0 ? 1.0f : mass));
        break;
    case ForceMode::IMPULSE:
        bodyInterface.AddImpulse(bodyID, force.toJoltRVec3());
        break;
    case ForceMode::VELOCITY_CHANGE:
        bodyInterface.AddImpulse(bodyID, force.toJoltRVec3() * (mass == 0 ? 1.0f : mass));
        break;
    }
}

void RigidbodyComponent::AddForceAtPosition(const Math::Vector3 force, const Math::Vector3 position, ForceMode mode) {
    if (bodyID.IsInvalid() || !physicsManager) return;
    JPH::BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();

    JPH::RVec3 pos(position.x(), position.y(), position.z());
    switch (mode) {
    case ForceMode::FORCE:
        bodyInterface.AddForce(bodyID, force.toJoltVec3(), pos);
        break;
    case ForceMode::ACCELERATION:
        bodyInterface.AddForce(bodyID, force.toJoltVec3() * mass, pos);
        break;
    case ForceMode::IMPULSE:
        bodyInterface.AddImpulse(bodyID, force.toJoltVec3(), pos);
        break;
    case ForceMode::VELOCITY_CHANGE:
        bodyInterface.AddImpulse(bodyID, force.toJoltVec3() * mass, pos);
        break;
    }
}

void RigidbodyComponent::AddTorque(const Math::Vector3 torque, ForceMode mode) {
    if (bodyID.IsInvalid() || !physicsManager) return;
    JPH::BodyInterface& bodyInterface = physicsManager->_GetBodyInterface();

    switch (mode) {
    case ForceMode::FORCE:
        bodyInterface.AddTorque(bodyID, torque.toJoltVec3());
        break;
    case ForceMode::ACCELERATION:
        bodyInterface.AddTorque(bodyID, torque.toJoltVec3() * mass);
        break;
    case ForceMode::IMPULSE:
        bodyInterface.AddAngularImpulse(bodyID, torque.toJoltVec3());
        break;
    case ForceMode::VELOCITY_CHANGE:
        bodyInterface.AddAngularImpulse(bodyID, torque.toJoltVec3() * mass);
        break;
    }
}

static Syngine::ComponentRegistrar s_rigidbodyRegistrar(
    Syngine::SYN_COMPONENT_RIGIDBODY,

    // ParseXml
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_RIGIDBODY);
        for (const auto& attr : elem->attributes()) {
            std::string key = attr->tag().cstr();
            std::string value = attr->data().cstr();

            if (key == "mass")
                node["mass"] = std::stof(value);
            else if (key == "friction")
                node["friction"] = std::stof(value);
            else if (key == "restitution")
                node["restitution"] = std::stof(value);
            else if (key == "shape")
                node["shape"] = static_cast<int>(std::stoi(value));
            else if (key == "shapeParameters") {
                scl::string v = attr->data();
                node["shapeParameters"] = Serializer::_ParseFloatArray(v);
            }
        }
        return node;
    },

    // Instantiate
    [](GameObject* owner, const Serializer::DataNode& data)
        -> std::unique_ptr<Syngine::Component> {
        RigidbodyParameters params;
        params.mass = data["mass"].As(1.0f);
        params.friction = data["friction"].As(0.5f);
        params.restitution = data["restitution"].As(0.0f);
        params.shape = static_cast<PhysicsShapes>(data["shape"].As(0));
        params.shapeParameters = data["shapeParameters"].As<std::vector<float>>({});
        return std::make_unique<RigidbodyComponent>(owner, params);
    },

    // Lua bindings
    [](sol::state& lua) {
        lua.new_usertype<RigidbodyComponent>(
            "RigidbodyComponent",
            // Methods
            "AddForce",
            [](RigidbodyComponent& self, sol::variadic_args args) {
                Math::Vector3 force(0.0f, 0.0f, 0.0f);
                ForceMode mode = ForceMode::FORCE;
                int i = 0;
                for (auto arg : args) {
                    if (arg.is<float>() && i < 3) {
                        force.set(i++, arg.as<float>());
                    } else if (arg.is<std::string>()) {
                        std::string modeStr = arg.as<std::string>();
                        if (modeStr == "IMPULSE") mode = ForceMode::IMPULSE;
                    }
                }
                self.AddForce(force, mode);
            },
            "AddForceAtPosition",
            [](RigidbodyComponent& self, sol::variadic_args args) {
                Math::Vector3 force, position;
                ForceMode mode = ForceMode::FORCE;
                int i = 0;
                for (auto arg : args) {
                    if (arg.is<float>() && i < 3) {
                        force.set(i++, arg.as<float>());
                    } else if (arg.is<float>() && i >= 3 && i < 6) {
                        position.set(i - 3, arg.as<float>());
                        i++;
                    } else if (arg.is<std::string>()) {
                        std::string modeStr = arg.as<std::string>();
                        if (modeStr == "IMPULSE") mode = ForceMode::IMPULSE;
                    }
                }
                self.AddForceAtPosition(force, position, mode);
            },
            "AddTorque",
            [](RigidbodyComponent& self, sol::variadic_args args) {
                Math::Vector3 torque;
                ForceMode mode = ForceMode::FORCE;
                int i = 0;
                for (auto arg : args) {
                    if (arg.is<float>() && i < 3) {
                        torque.set(i++, arg.as<float>());
                    } else if (arg.is<std::string>()) {
                        std::string modeStr = arg.as<std::string>();
                        if (modeStr == "IMPULSE") mode = ForceMode::IMPULSE;
                    }
                }
                self.AddTorque(torque, mode);
            },
            "UpdateShapeParameters",
            [](RigidbodyComponent& self, sol::variadic_args args) {
                Math::Vector3 newParams;
                int i = 0;
                for (auto arg : args) {
                    if (arg.is<float>()) {
                        newParams.set(i++, arg.as<float>());
                    }
                }
                self.UpdateShapeParameters(newParams);
             }
        );
    }
);

} // namespace Syngine
