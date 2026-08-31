// ╒═════════════ RigidbodyComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-05-22                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Core/Core.h>
#include <Syngine/Core/Logger.h>
#include <Syngine/GameObjects/Component.h>
#include <Syngine/GameObjects/Components/RigidbodyComponent.h>
#include <Syngine/GameObjects/ComponentRegistry.h>
#include <Syngine/GameObjects/Components/MeshComponent.h>
#include <Syngine/GameObjects/Components/TransformComponent.h>
#include <Syngine/GameObjects/GameObject.h>
#include <Syngine/Math/Quaternion.hpp>
#include <Syngine/Math/Vector3.hpp>
#include <Syngine/Physics/PhysicsManager.h>
#include <Syngine/Utils/Serializer.h>

#include <Jolt/Math/Quat.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/EActivation.h>
#include <Jolt/Math/MathTypes.h>

#include <sol/sol.hpp>

#include <cmath>
#include <memory>
#include <vector>

namespace Syngine {
RigidbodyComponent::RigidbodyComponent(GameObject*                  owner,
                                       Syngine::RigidbodyParameters params)
    : IComponent(owner) {
    if (!Core::IsPhysicsEnabled()) return;
    this->Init(params);
}

RigidbodyComponent::RigidbodyComponent(const RigidbodyComponent& other)
    : IComponent(other.m_owner) {
    if (!Core::IsPhysicsEnabled()) return;

    this->m_physicsManager  = other.m_physicsManager;
    this->m_transform       = other.m_transform;
    this->m_bodyID          = other.m_bodyID;
    this->m_mass            = other.m_mass;
    this->m_friction        = other.m_friction;
    this->m_restitution     = other.m_restitution;
    this->m_shape           = other.m_shape;
    this->m_shapeParameters = other.m_shapeParameters;
}

RigidbodyComponent&
RigidbodyComponent::operator=(const RigidbodyComponent& other) {
    if (!Core::IsPhysicsEnabled()) return *this;

    if (this != &other) {
        this->m_owner           = other.m_owner;
        this->m_physicsManager  = other.m_physicsManager;
        this->m_transform       = other.m_transform;
        this->m_bodyID          = other.m_bodyID;
        this->m_mass            = other.m_mass;
        this->m_friction        = other.m_friction;
        this->m_restitution     = other.m_restitution;
        this->m_shape           = other.m_shape;
        this->m_shapeParameters = other.m_shapeParameters;
    }
    return *this;
}

RigidbodyComponent::~RigidbodyComponent() { Destroy(); }

Syngine::ComponentTypeID RigidbodyComponent::GetComponentType() {
    return SYN_COMPONENT_RIGIDBODY;
}

Serializer::DataNode RigidbodyComponent::Serialize() const {
    Serializer::DataNode node;
    node / "type" =
        static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_RIGIDBODY);
    node / "mass"            = m_mass;
    node / "friction"        = m_friction;
    node / "restitution"     = m_restitution;
    node / "shape"           = static_cast<int>(m_shape);
    node / "shapeParameters" = m_shapeParameters;
    return node;
}

JPH::BodyID    RigidbodyComponent::_GetBodyID() const { return m_bodyID; }
Syngine::Phys* RigidbodyComponent::_GetPhysicsManager() const {
    return m_physicsManager;
}
Math::Vector3 RigidbodyComponent::GetShapeParameters() const {
    return m_shapeParameters;
}
float RigidbodyComponent::GetMass() const { return m_mass; }
float RigidbodyComponent::GetFriction() const { return m_friction; }
float RigidbodyComponent::GetRestitution() const { return m_restitution; }

// Initialize the RigidbodyComponent with the given parameters.
void RigidbodyComponent::Init(Syngine::RigidbodyParameters params) {
    m_pendingParams = params;
    m_initPending   = false;

    if (m_initComplete && !m_bodyID.IsInvalid()) {
        return;
    }

    this->m_physicsManager = Syngine::Core::_GetContext()->physicsManager.get();
    this->m_transform      = this->m_owner->GetComponent<TransformComponent>();
    this->m_mass           = params.mass;
    this->m_friction       = params.friction;
    this->m_restitution    = params.restitution;
    this->m_shape          = params.shape;
    this->m_shapeParameters = params.shapeParameters;
    if (!this->m_transform) {
        m_initPending = true;
        Syngine::Logger::Warn("RigidbodyComponent deferred initialization: "
                              "waiting for TransformComponent");
        return;
    }

    if (!m_physicsManager || !m_transform) {
        m_initPending = true;
        return;
    }
    m_currentParams = params;

    JPH::RVec3 curPos = m_transform->GetPosition().toJoltRVec3();
    JPH::Quat  transformRot =
        m_transform->GetWorldRotationQuaternion().toJoltQuat();

    switch (m_shape) {
    case PhysicsShapes::SPHERE: {
        if (m_shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::Init: No radius "
                                   "provided for sphere shape.");
            return;
        }
        float radius = m_shapeParameters.x();
        m_bodyID     = m_physicsManager->_CreateSphere(
            curPos, radius, params.motionType, params.layer, m_mass);
        break;
    }
    case PhysicsShapes::BOX: {
        if (m_shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::Init: No half extents "
                                   "provided for box shape.");
            return;
        }
        JPH::Vec3 shapeParametersVec(m_shapeParameters.x(),
                                     m_shapeParameters.y(),
                                     m_shapeParameters.z());
        m_bodyID = m_physicsManager->_CreateBox(curPos,
                                                transformRot,
                                                shapeParametersVec,
                                                params.motionType,
                                                params.layer,
                                                m_mass);
        break;
    }
    case PhysicsShapes::MESH: {
        if (!m_owner) {
            Syngine::Logger::Error("RigidbodyComponent::Init: No owner game "
                                   "object provided for mesh shape.");
            return;
        }
        MeshComponent* meshComp = m_owner->GetComponent<MeshComponent>();
        if (!meshComp) {
            Syngine::Logger::Error("RigidbodyComponent::Init: No mesh "
                                   "component or mesh loaded for mesh shape.");
            return;
        }

        JPH::Vec3 scale(1.0f, 1.0f, 1.0f);
        if (!m_shapeParameters.isZero()) {
            scale = JPH::Vec3(m_shapeParameters.x(),
                              m_shapeParameters.y(),
                              m_shapeParameters.z());
        }

        m_bodyID = m_physicsManager->_CreateMeshBody(curPos,
                                                     transformRot,
                                                     meshComp->modelData,
                                                     params.motionType,
                                                     params.layer,
                                                     scale,
                                                     m_mass);
        break;
    }
    case PhysicsShapes::CAPSULE: {
        if (m_shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::Init: No radius and "
                                   "half height provided for capsule shape.");
            return;
        }
        float radius     = m_shapeParameters.x();
        float halfHeight = m_shapeParameters.y();
        m_bodyID         = m_physicsManager->_CreateCapsule(curPos,
                                                            radius,
                                                            halfHeight,
                                                            params.motionType,
                                                            params.layer,
                                                            m_mass);
        break;
    }
    case PhysicsShapes::CYLINDER: {
        if (m_shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::Init: Not enough "
                                   "parameters for cylinder shape.");
            return;
        }
        float radius     = m_shapeParameters.y();
        float halfHeight = m_shapeParameters.x();
        m_bodyID         = m_physicsManager->_CreateCylinder(curPos,
                                                             transformRot,
                                                             radius,
                                                             halfHeight,
                                                             params.motionType,
                                                             params.layer,
                                                             m_mass);
        break;
    }
    case PhysicsShapes::COMPOUND: {
        if (params.compoundParts.empty()) {
            Syngine::Logger::Error("RigidbodyComponent::Init: No parts "
                                   "provided for compound shape.");
            return;
        }
        m_bodyID = m_physicsManager->_CreateCompound(curPos,
                                                     transformRot,
                                                     params.compoundParts,
                                                     params.motionType,
                                                     params.layer,
                                                     m_mass);
        break;
    }
    default: return;
    }

    if (!m_bodyID.IsInvalid()) {
        JPH::BodyInterface& bodyInterface =
            m_physicsManager->_GetBodyInterface();
        // Set the body properties (mass set during body creation)
        if (m_friction > 0) bodyInterface.SetFriction(m_bodyID, m_friction);
        if (m_restitution > 0)
            bodyInterface.SetRestitution(m_bodyID, m_restitution);
        m_initComplete = true;
        m_initPending  = false;
    } else {
        m_initPending = true;
    }
}

void RigidbodyComponent::RetryInitIfPending() {
    if (!m_initPending || m_initComplete) {
        return;
    }

    Syngine::Logger::LogF(
        LogLevel::INFO,
        true,
        "RigidbodyComponent: Retrying initialization for GameObject '%s'",
        m_owner->name.c_str());
    Init(m_pendingParams);
}

void RigidbodyComponent::SyncBodyToTransform() {
    if (m_initPending && !m_initComplete) {
        RetryInitIfPending();
    }

    if (!m_physicsManager || !m_transform || m_bodyID.IsInvalid()) {
        return;
    }

    BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();

    JPH::Vec3 pos = m_transform->GetWorldPosition().toJoltVec3();
    JPH::Quat rot = m_transform->GetWorldRotationQuaternion().toJoltQuat();
    bodyInterface.SetPositionAndRotation(
        m_bodyID, pos, rot, EActivation::Activate);
}

void RigidbodyComponent::Update(float deltaTime) {
    if (m_initPending && !m_initComplete) {
        RetryInitIfPending();
    }

    if (!m_physicsManager || !m_transform || m_bodyID.IsInvalid()) {
        return;
    }

    BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();
    bool           simulate      = Syngine::Core::Get()->GetSimulationState();

    if (simulate) {
        // Smoothly lerp the TransformComponent towards the physics body's
        // position and rotation over time
        static const float lerpAlpha = 0.2f;
        // When physics drives the transform
        JPH::Vec3 physicsPos = bodyInterface.GetPosition(m_bodyID);
        JPH::Quat physicsRot = bodyInterface.GetRotation(m_bodyID);

        JPH::Vec3 currentPos = m_transform->GetPosition().toJoltVec3();
        JPH::Quat currentRot =
            m_transform->GetWorldRotationQuaternion().toJoltQuat();

        // Lerp pos and slerp rot
        JPH::Vec3 lerpedPos =
            currentPos + (physicsPos - currentPos) * lerpAlpha;
        JPH::Quat lerpedRot = currentRot.SLERP(physicsRot, lerpAlpha);

        m_transform->SetWorldPosition(Vector3(lerpedPos));
        m_transform->SetWorldRotationQuat(Quaternion(lerpedRot));

    } else {
        JPH::Vec3 pos = m_transform->GetWorldPosition().toJoltVec3();
        JPH::Quat rot = m_transform->GetWorldRotationQuaternion().toJoltQuat();
        Math::Vector3 scale = m_transform->GetWorldScale();

        bodyInterface.SetPositionAndRotation(
            m_bodyID, pos, rot, EActivation::Activate);

        Math::Vector3 scaledParams(m_shapeParameters.x() * std::abs(scale.x()),
                                   m_shapeParameters.y() * std::abs(scale.y()),
                                   m_shapeParameters.z() * std::abs(scale.z()));

        JPH::Shape* shape = nullptr;

        switch (this->m_shape) {
        case PhysicsShapes::SPHERE: {
            if (scaledParams.x() <= 0.0f) {
                Syngine::Logger::Error(
                    "RigidbodyComponent::SyncBodyToTransform: "
                    "Invalid radius for sphere shape.");
                return;
            }
            float radius = scaledParams.x();
            shape        = new JPH::SphereShape(radius);
            break;
        }
        case PhysicsShapes::BOX: {
            if (scaledParams.x() <= 0.0f || scaledParams.y() <= 0.0f ||
                scaledParams.z() <= 0.0f) {
                Syngine::Logger::Error(
                    "RigidbodyComponent::SyncBodyToTransform: "
                    "Invalid extents for box shape.");
                return;
            }
            JPH::Vec3 extents(
                scaledParams.x(), scaledParams.y(), scaledParams.z());
            shape = new JPH::BoxShape(extents);
            break;
        }
        case PhysicsShapes::CAPSULE: {
            if (scaledParams.x() <= 0.0f || scaledParams.y() <= 0.0f) {
                Syngine::Logger::Error("RigidbodyComponent::"
                                       "SyncBodyToTransform: Invalid radius or "
                                       "half height for capsule shape.");
                return;
            }
            float radius             = scaledParams.x();
            float halfHeightCylinder = scaledParams.y();
            shape = new JPH::CapsuleShape(halfHeightCylinder, radius);
            break;
        }
        case PhysicsShapes::CYLINDER: {
            if (scaledParams.x() <= 0.0f || scaledParams.y() <= 0.0f) {
                Syngine::Logger::Error("RigidbodyComponent::"
                                       "SyncBodyToTransform: Invalid radius or "
                                       "half height for cylinder shape.");
                return;
            }
            float radius     = scaledParams.x();
            float halfHeight = scaledParams.y();
            shape            = new JPH::CylinderShape(halfHeight, radius);
            break;
        }
        default: break;
        }

        if (shape) {
            bodyInterface.SetShape(
                m_bodyID, shape, true, JPH::EActivation::Activate);
        }
    }
}

void RigidbodyComponent::Destroy() {
    if (m_physicsManager && !m_bodyID.IsInvalid()) {
        BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();
        bodyInterface.RemoveBody(m_bodyID);
        bodyInterface.DestroyBody(m_bodyID);
        m_bodyID = BodyID(); // Reset the body ID to an invalid state
    }
}

void RigidbodyComponent::UpdateShapeParameters(
    const Math::Vector3 newShapeParameters) {
    if (m_bodyID.IsInvalid() || !m_physicsManager) {
        Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: "
                               "Invalid body ID or physics manager.");
        return;
    }

    JPH::BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();
    JPH::Shape*         shape         = nullptr;

    // Store new parameters. For MESH shape, interpreted as scalars
    // For other shapes, they are dimensions (radius, extents, etc.)
    this->m_shapeParameters         = newShapeParameters;
    m_currentParams.shapeParameters = m_shapeParameters;

    switch (this->m_shape) {
    case PhysicsShapes::SPHERE: {
        if (this->m_shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: "
                                   "No radius provided for sphere shape.");
            return;
        }
        float radius = this->m_shapeParameters.x();
        shape        = new JPH::SphereShape(radius);
        break;
    }
    case PhysicsShapes::BOX: {
        if (this->m_shapeParameters.isZero()) {
            Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: "
                                   "No extents provided for box shape.");
            return;
        }
        JPH::Vec3 extents(this->m_shapeParameters.x(),
                          this->m_shapeParameters.y(),
                          this->m_shapeParameters.z());
        shape = new JPH::BoxShape(extents);
        break;
    }
    case PhysicsShapes::CAPSULE: {
        if (this->m_shapeParameters.isZero()) {
            Syngine::Logger::Error(
                "RigidbodyComponent::UpdateShapeParameters: No radius and half "
                "height provided for capsule shape.");
            return;
        }
        float radius             = this->m_shapeParameters.x();
        float halfHeightCylinder = this->m_shapeParameters.y();
        shape = new JPH::CapsuleShape(halfHeightCylinder, radius);
        break;
    }
    case PhysicsShapes::CYLINDER: {
        if (this->m_shapeParameters.isZero()) {
            Syngine::Logger::Error(
                "RigidbodyComponent::UpdateShapeParameters: No radius and half "
                "height provided for cylinder shape.");
            return;
        }
        float radius     = this->m_shapeParameters.x();
        float halfHeight = this->m_shapeParameters.y();
        shape            = new JPH::CylinderShape(halfHeight, radius);
        break;
    }
    default:
        Syngine::Logger::Error("RigidbodyComponent::UpdateShapeParameters: "
                               "Unsupported shape type for update.");
        return;
    }

    if (shape) {
        bodyInterface.SetShape(
            m_bodyID, shape, true, JPH::EActivation::Activate);
    } else if (this->m_shape != PhysicsShapes::MESH) {
        Syngine::Logger::Error(
            "RigidbodyComponent::UpdateShapeParameters: Failed to create shape "
            "for the specified type.");
        return;
    }
}

void RigidbodyComponent::SetCurrentParameters(
    const RigidbodyParameters& newParams) {
    m_currentParams = newParams;
    if (!m_physicsManager) return;
    this->Destroy();
    this->Init(m_currentParams);
}

void RigidbodyComponent::SetFriction(float newFriction) {
    if (newFriction < 0.0f) {
        Syngine::Logger::Warn(
            "RigidbodyComponent::SetFriction: Friction cannot be negative.");
        return;
    }
    if (newFriction > 1.0f) {
        Syngine::Logger::Warn("RigidbodyComponent::SetFriction: Friction "
                              "cannot be greater than 1.0.");
        return;
    }
    if (m_bodyID.IsInvalid() || !m_physicsManager) {
        Syngine::Logger::Error("RigidbodyComponent::SetFriction: Invalid body "
                               "ID or physics manager.");
        return;
    }

    m_friction               = newFriction;
    m_currentParams.friction = m_friction;
    m_physicsManager->_GetBodyInterface().SetFriction(m_bodyID, m_friction);
}

void RigidbodyComponent::SetRestitution(float newRestitution) {
    if (newRestitution < 0.0f) {
        Syngine::Logger::Warn("RigidbodyComponent::SetRestitution: Restitution "
                              "cannot be negative.");
        return;
    }
    if (newRestitution > 1.0f) {
        Syngine::Logger::Warn("RigidbodyComponent::SetRestitution: Restitution "
                              "cannot be greater than 1.0.");
        return;
    }
    if (m_bodyID.IsInvalid() || !m_physicsManager) {
        Syngine::Logger::Error("RigidbodyComponent::SetRestitution: Invalid "
                               "body ID or physics manager.");
        return;
    }

    m_restitution               = newRestitution;
    m_currentParams.restitution = m_restitution;
    m_physicsManager->_GetBodyInterface().SetRestitution(m_bodyID,
                                                         m_restitution);
}

//----- Forces and whatnot
// Important to note again that ACCELERATION and VELOCITY_CHANGE modes are
// identical to FORCE and IMPULSE respectively, at least if user did not set
// mass manually during rb creation. Jolt doesn't really support these modes so
// it is what it is.
void RigidbodyComponent::AddForce(const Math::Vector3 force, ForceMode mode) {
    if (m_bodyID.IsInvalid() || !m_physicsManager) return;
    JPH::BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();

    switch (mode) {
    case ForceMode::FORCE:
        bodyInterface.AddForce(m_bodyID, force.toJoltRVec3());
        break;
    case ForceMode::ACCELERATION:
        bodyInterface.AddForce(
            m_bodyID, force.toJoltRVec3() * (m_mass == 0 ? 1.0f : m_mass));
        break;
    case ForceMode::IMPULSE:
        bodyInterface.AddImpulse(m_bodyID, force.toJoltRVec3());
        break;
    case ForceMode::VELOCITY_CHANGE:
        bodyInterface.AddImpulse(
            m_bodyID, force.toJoltRVec3() * (m_mass == 0 ? 1.0f : m_mass));
        break;
    }
}

void RigidbodyComponent::AddForceAtPosition(const Math::Vector3 force,
                                            const Math::Vector3 position,
                                            ForceMode           mode) {
    if (m_bodyID.IsInvalid() || !m_physicsManager) return;
    JPH::BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();

    JPH::RVec3 pos(position.x(), position.y(), position.z());
    switch (mode) {
    case ForceMode::FORCE:
        bodyInterface.AddForce(m_bodyID, force.toJoltVec3(), pos);
        break;
    case ForceMode::ACCELERATION:
        bodyInterface.AddForce(m_bodyID, force.toJoltVec3() * m_mass, pos);
        break;
    case ForceMode::IMPULSE:
        bodyInterface.AddImpulse(m_bodyID, force.toJoltVec3(), pos);
        break;
    case ForceMode::VELOCITY_CHANGE:
        bodyInterface.AddImpulse(m_bodyID, force.toJoltVec3() * m_mass, pos);
        break;
    }
}

void RigidbodyComponent::AddTorque(const Math::Vector3 torque, ForceMode mode) {
    if (m_bodyID.IsInvalid() || !m_physicsManager) return;
    JPH::BodyInterface& bodyInterface = m_physicsManager->_GetBodyInterface();

    switch (mode) {
    case ForceMode::FORCE:
        bodyInterface.AddTorque(m_bodyID, torque.toJoltVec3());
        break;
    case ForceMode::ACCELERATION:
        bodyInterface.AddTorque(m_bodyID, torque.toJoltVec3() * m_mass);
        break;
    case ForceMode::IMPULSE:
        bodyInterface.AddAngularImpulse(m_bodyID, torque.toJoltVec3());
        break;
    case ForceMode::VELOCITY_CHANGE:
        bodyInterface.AddAngularImpulse(m_bodyID, torque.toJoltVec3() * m_mass);
        break;
    }
}

static Syngine::ComponentRegistrar s_rigidbodyRegistrar(
    Syngine::SYN_COMPONENT_RIGIDBODY,

    // ParseXml
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" =
            static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_RIGIDBODY);
        for (const auto& attr : elem->attributes()) {
            std::string key   = attr->tag().cstr();
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
                scl::string v           = attr->data();
                node["shapeParameters"] = Serializer::_ParseFloatArray(v);
            }
        }
        return node;
    },

    // Instantiate
    [](GameObject* owner, const Serializer::DataNode& data)
        -> std::unique_ptr<Syngine::IComponent> {
        RigidbodyParameters params;
        params.mass        = data["mass"].As(1.0f);
        params.friction    = data["friction"].As(0.5f);
        params.restitution = data["restitution"].As(0.0f);
        params.shape       = static_cast<PhysicsShapes>(data["shape"].As(0));
        params.shapeParameters =
            data["shapeParameters"].As<std::vector<float>>({});
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
                ForceMode     mode = ForceMode::FORCE;
                int           i    = 0;
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
                ForceMode     mode = ForceMode::FORCE;
                int           i    = 0;
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
                ForceMode     mode = ForceMode::FORCE;
                int           i    = 0;
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
                int           i = 0;
                for (auto arg : args) {
                    if (arg.is<float>()) {
                        newParams.set(i++, arg.as<float>());
                    }
                }
                self.UpdateShapeParameters(newParams);
            });
    });

} // namespace Syngine
