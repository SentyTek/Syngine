// ╒════════════════ CameraComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-06-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include "Syngine/GameObjects/Components/CameraComponent.h"
#include "Syngine/GameObjects/ComponentRegistry.h"
#include "Syngine/GameObjects/Components/TransformComponent.h"
#include "Syngine/GameObjects/GameObject.h"

#include "Syngine/Math/Math.hpp"
#include "Syngine/Utils/Serializer.h"
#include "bx/math.h"
#include "bgfx/bgfx.h"

#include <algorithm>
#include <cmath>

namespace Syngine {
CameraComponent::CameraComponent(GameObject* owner)
    : Syngine::IComponent(owner) {
    this->camera           = {};
    this->camera.eye       = Math::Vector3();
    this->camera.target    = Math::Vector3();
    this->camera.up        = Math::Vector3(0.0f, 1.0f, 0.0f);
    this->camera.fov       = 70.0f;
    this->camera.nearPlane = 0.1f;
    this->camera.farPlane  = 100.0f;
    this->camera.view      = Math::Matrix4x4();
    this->camera.proj      = Math::Matrix4x4();
    this->camera.yaw       = 0.0f;
    this->camera.pitch     = 0.0f;
    this->camera.roll      = 0.0f;

    this->camera.screenFocalLength = this->_setScreenFocalLength();

    if (this->m_owner) {
        this->m_owner->gizmo =
            "camera_render"; // Set gizmo type for this component
    }
}

CameraComponent::CameraComponent(const CameraComponent& other)
    : Syngine::IComponent(other.m_owner) {
    this->camera = other.camera;

    if (this->m_owner) {
        this->m_owner->gizmo =
            "camera_render"; // Set gizmo type for this component
    }
}

CameraComponent& CameraComponent::operator=(const CameraComponent& other) {
    if (this != &other) {
        IComponent::m_owner = other.m_owner;
        this->camera        = other.camera;

        if (this->m_owner) {
            this->m_owner->gizmo =
                "camera_render"; // Set gizmo type for this component
        }
    }
    return *this;
}

CameraComponent::~CameraComponent() {
    // No specific cleanup needed for now
}

Serializer::DataNode CameraComponent::Serialize() const {
    Serializer::DataNode node;
    node / "type" = static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_CAMERA);
    node / "eye"  = std::vector<float>(camera.eye);
    node / "fov"  = camera.fov;
    node / "farPlane" = camera.farPlane;
    node / "yaw"      = camera.yaw;
    node / "pitch"    = camera.pitch;
    return node;
}

void CameraComponent::Update(int width, int height) {
    // update view and projection matrices
    if (this->syncToTransform && this->m_owner) {
        TransformComponent* transform =
            this->m_owner->GetComponent<TransformComponent>();
        if (transform) {
            this->camera.eye     = transform->GetWorldPosition();
            Math::Vector3 angles = transform->GetWorldRotationEuler();
            this->camera.yaw     = angles.x();
            this->camera.pitch   = angles.y();
        }
    }
    Syngine::Camera& cam        = this->camera;
    Math::Vec3       eyeVec     = this->camera.eye.toBxVec3();
    const int        safeWidth  = std::max(width, 1);
    const int        safeHeight = std::max(height, 1);
    float            aspect     = float(safeWidth) / float(safeHeight);

    Math::Vec3 forward(cosf(cam.pitch) * sinf(cam.yaw),
                       sinf(cam.pitch),
                       cosf(cam.pitch) * cosf(cam.yaw));
    forward = forward.normalized();

    Math::Vec3 right = Math::Vec3(
        sinf(cam.yaw - bx::kPiHalf), 0.0f, cosf(cam.yaw - bx::kPiHalf));
    right = right.normalized();

    Math::Vec3 targetVec = eyeVec + forward;
    Math::Vec3 upVec     = right.cross(forward);

    bx::mtxLookAt(cam.view.data(),
                  eyeVec.toBxVec3(),
                  targetVec.toBxVec3(),
                  upVec.toBxVec3());
    bx::mtxProj(cam.proj.data(),
                cam.fov,
                aspect,
                cam.nearPlane,
                cam.farPlane,
                bgfx::getCaps()->homogeneousDepth);
}

Syngine::ComponentTypeID CameraComponent::GetComponentType() {
    return SYN_COMPONENT_CAMERA;
}

void CameraComponent::SetPosition(Math::Vector3 position) {
    this->camera.eye = position;
}

Math::Vector3 CameraComponent::GetPosition() const { return this->camera.eye; }

void CameraComponent::SetFOV(float fov) {
    if (fov < 1.0f || fov > 179.0f) {
        return;
    }
    this->camera.fov               = fov;
    this->camera.screenFocalLength = this->_setScreenFocalLength();
}

float CameraComponent::GetFOV() const { return this->camera.fov; }

void CameraComponent::SetFarPlane(float farPlane) {
    if (farPlane < 0.0f || farPlane < this->camera.nearPlane ||
        farPlane > 50000.0f) {
        return;
    }
    this->camera.farPlane = farPlane;
}

void CameraComponent::SetNearPlane(float nearPlane) {
    if (nearPlane < 0.0f || nearPlane > this->camera.farPlane ||
        nearPlane > 50000.0f) {
        return;
    }
    this->camera.nearPlane = nearPlane;
}

float CameraComponent::GetNearPlane() const { return this->camera.nearPlane; }

float CameraComponent::GetFarPlane() const { return this->camera.farPlane; }

void CameraComponent::SetAngles(float yaw, float pitch) {
    this->camera.yaw   = yaw;
    this->camera.pitch = pitch;
}

Math::Vector2 CameraComponent::GetAngles() const {
    return Math::Vector2(this->camera.yaw, this->camera.pitch);
}

Camera Syngine::CameraComponent::GetCamera() const { return this->camera; }

void CameraComponent::_normalizePlane(CameraComponent::Plane& plane) {
    const float len = plane.normal.length();
    if (len == 0.0f) {
        return;
    }
    plane.normal = plane.normal / len;
    plane.distance /= len;
}

CameraComponent::Frustum CameraComponent::_extractFrustum() {
    Frustum frustum;

    // The math layer uses row-major matrices with row vectors, so the clip
    // matrix is view * proj and the frustum planes come from rows.
    Math::Matrix4x4 vp = this->camera.view * this->camera.proj;

    const auto extractPlane = [&vp](int axis, float sign) {
        Plane plane;
        plane.normal   = Math::Vec3(vp.m(0, 3) + sign * vp.m(0, axis),
                                    vp.m(1, 3) + sign * vp.m(1, axis),
                                    vp.m(2, 3) + sign * vp.m(2, axis));
        plane.distance = vp.m(3, 3) + sign * vp.m(3, axis);
        return plane;
    };

    frustum.left   = extractPlane(0, 1.0f);
    frustum.right  = extractPlane(0, -1.0f);
    frustum.bottom = extractPlane(1, 1.0f);
    frustum.top    = extractPlane(1, -1.0f);
    frustum.n      = extractPlane(2, 1.0f);
    frustum.f      = extractPlane(2, -1.0f);

    _normalizePlane(frustum.left);
    _normalizePlane(frustum.right);
    _normalizePlane(frustum.top);
    _normalizePlane(frustum.bottom);
    _normalizePlane(frustum.n);
    _normalizePlane(frustum.f);

    return frustum;
}

bool CameraComponent::_aabbInsidePlane(const Plane&         plane,
                                       const Math::Vector3& min,
                                       const Math::Vector3& max) {
    const Math::Vector3 positiveVertex(
        (plane.normal.x() >= 0.0f) ? max.x() : min.x(),
        (plane.normal.y() >= 0.0f) ? max.y() : min.y(),
        (plane.normal.z() >= 0.0f) ? max.z() : min.z());

    const float distance = plane.normal.dot(positiveVertex) + plane.distance;

    return distance >= 0;
}

bool CameraComponent::_aabbInsideFrustum(const Frustum&       frustum,
                                         const Math::Vector3& min,
                                         const Math::Vector3& max) {
    return _aabbInsidePlane(frustum.left, min, max) &&
           _aabbInsidePlane(frustum.right, min, max) &&
           _aabbInsidePlane(frustum.top, min, max) &&
           _aabbInsidePlane(frustum.bottom, min, max) &&
           _aabbInsidePlane(frustum.n, min, max) &&
           _aabbInsidePlane(frustum.f, min, max);
}

static Syngine::ComponentRegistrar s_cameraRegistrar(
    Syngine::SYN_COMPONENT_CAMERA,

    // ParseXml
    [](const scl::xml::XmlElem* elem) -> Serializer::DataNode {
        Serializer::DataNode node;
        node / "type" =
            static_cast<Syngine::ComponentTypeID>(SYN_COMPONENT_CAMERA);
        for (const auto& attr : elem->attributes()) {
            scl::string key   = attr->tag();
            std::string value = attr->data().cstr();
            if (key == "eye") {
                scl::string eyeValue(value);
                node / "eye" = Serializer::_ParseFloatArray(eyeValue);
            } else if (key == "fov") {
                node / "fov" = std::stof(value);
            } else if (key == "farPlane") {
                node / "farPlane" = std::stof(value);
            } else if (key == "yaw") {
                node / "yaw" = std::stof(value);
            } else if (key == "pitch") {
                node / "pitch" = std::stof(value);
            } else if (key == "isMainCamera") {
                node / "isMainCamera" = (value == "true");
            } else if (key == "syncToTransform") {
                node / "syncToTransform" = (value == "true");
            }
        }
        return node;
    },

    // Instantiate
    [](Syngine::GameObject* owner, const Serializer::DataNode& data)
        -> std::unique_ptr<Syngine::IComponent> {
        auto comp = std::make_unique<CameraComponent>(owner);
        if (data.Has("eye")) {
            const std::vector<float>& eyeArr =
                data["eye"].As<std::vector<float>>();
            comp->SetPosition(Math::Vector3{ eyeArr });
        }
        if (data.Has("fov")) {
            comp->SetFOV(data["fov"].As<float>());
        }
        if (data.Has("farPlane")) {
            comp->SetFarPlane(data["farPlane"].As<float>());
        }
        if (data.Has("yaw") && data.Has("pitch")) {
            comp->SetAngles(data["yaw"].As<float>(), data["pitch"].As<float>());
        }
        if (data.Has("isMainCamera")) {
            comp->isMainCamera = data["isMainCamera"].As<bool>();
        }
        if (data.Has("syncToTransform")) {
            comp->syncToTransform = data["syncToTransform"].As<bool>();
        }
        return comp;
    });

} // namespace Syngine
