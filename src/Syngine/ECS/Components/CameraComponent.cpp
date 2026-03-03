// ╒════════════════ CameraComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-06-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/CameraComponent.h"
#include "Syngine/ECS/GameObject.h"

#include "Syngine/Utils/Profiler.h"
#include "Syngine/Utils/Serializer.h"
#include "bx/math.h"
#include "bgfx/bgfx.h"

#include <cmath>

namespace Syngine {
CameraComponent::CameraComponent(GameObject* owner) {
    this->m_owner = owner;
    this->camera = {};
    this->camera.eye[0] = 0.0f;
    this->camera.eye[1] = 0.0f;
    this->camera.eye[2] = 0.0f;
    this->camera.target[0] = 0.0f;
    this->camera.target[1] = 0.0f;
    this->camera.target[2] = 0.0f;
    this->camera.up[0]     = 0.0f;
    this->camera.up[1]     = 1.0f;
    this->camera.up[2]     = 0.0f;

    if (this->m_owner) {
        this->m_owner->gizmo = "camera_render"; // Set gizmo type for this component
    }
}

CameraComponent::CameraComponent(const CameraComponent& other) {
    this->m_owner = other.m_owner;
    this->camera = other.camera;

    if (this->m_owner) {
        this->m_owner->gizmo = "camera_render"; // Set gizmo type for this component
    }
}

CameraComponent& CameraComponent::operator=(const CameraComponent& other) {
    if (this != &other) {
        this->m_owner = other.m_owner;
        this->camera = other.camera;

        if (this->m_owner) {
            this->m_owner->gizmo = "camera_render"; // Set gizmo type for this component
        }
    }
    return *this;
}

CameraComponent::~CameraComponent() {
    // No specific cleanup needed for now
}

Serializer::DataNode CameraComponent::Serialize() const {
    Serializer::DataNode node;
    node / "type" = static_cast<int>(SYN_COMPONENT_CAMERA);
    node / "eye" = std::vector<float>{ camera.eye[0], camera.eye[1], camera.eye[2] };
    node / "target" = std::vector<float>{ camera.target[0], camera.target[1], camera.target[2] };
    node / "up" = std::vector<float>{ camera.up[0], camera.up[1], camera.up[2] };
    node / "fov" = camera.fov;
    node / "nearPlane" = camera.nearPlane;
    node / "farPlane" = camera.farPlane;
    node / "yaw" = camera.yaw;
    node / "pitch" = camera.pitch;
    return node;
}

void CameraComponent::Update(int viewId, int width, int height) {
    // update view and projection matrices
    Syngine::Camera& cam = this->camera;
    float* eye = this->camera.eye;
    bx::Vec3 eyeVec = { eye[0], eye[1], eye[2] };
    float aspect = float(width) / float(height);

    bx::Vec3 forward = {
        cosf(cam.pitch) * sinf(cam.yaw),
        sinf(cam.pitch),
        cosf(cam.pitch) * cosf(cam.yaw)
    };
    bx::Vec3 right = {
        sinf(cam.yaw - bx::kPiHalf),
        0.0f,
        cosf(cam.yaw - bx::kPiHalf)
    };

    bx::Vec3 targetVec = bx::add(eyeVec, forward);
    bx::Vec3 upVec = bx::cross(right, forward);

    bx::mtxLookAt(cam.view, eyeVec, targetVec, upVec);

    bx::mtxLookAt(cam.view, eyeVec, targetVec, upVec);
    bx::mtxProj(cam.proj, cam.fov, aspect, cam.nearPlane, cam.farPlane, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(viewId, cam.view, cam.proj);
}

Components CameraComponent::GetComponentType() {
    return SYN_COMPONENT_CAMERA;
}

void CameraComponent::SetPosition(float x, float y, float z) {
    this->camera.eye[0] = x;
    this->camera.eye[1] = y;
    this->camera.eye[2] = z;
}

const float* CameraComponent::GetPosition() const {
    return this->camera.eye;
}

void CameraComponent::SetFOV(float fov) {
    if (fov < 1.0f || fov > 179.0f) {
        return;
    }
    this->camera.fov = fov;
}

float CameraComponent::GetFOV() const { return this->camera.fov; }

void CameraComponent::SetFarPlane(float farPlane) {
    if (farPlane < 0.0f && farPlane > this->camera.nearPlane && farPlane > 50000.0f) {
        return;
    }
    this->camera.farPlane = farPlane;
}

float CameraComponent::GetFarPlane() const { return this->camera.farPlane; }

void CameraComponent::SetAngles(float yaw, float pitch) {
    this->camera.yaw = yaw;
    this->camera.pitch = pitch;
}

void CameraComponent::GetAngles(float& yaw, float& pitch) const {
    yaw = this->camera.yaw;
    pitch = this->camera.pitch;
}

Camera Syngine::CameraComponent::GetCamera() const { return this->camera; }

void CameraComponent::_normalizePlane(CameraComponent::Plane& plane) {
    const float len = bx::sqrt(plane.normal[0] * plane.normal[0] +
                               plane.normal[1] * plane.normal[1] +
                               plane.normal[2] * plane.normal[2]);
    const float invLen = 1.0f / len;
    plane.normal[0] *= invLen;
    plane.normal[1] *= invLen;
    plane.normal[2] *= invLen;
    plane.distance  *= invLen;
}

CameraComponent::Frustum CameraComponent::_extractFrustum() {
    Frustum frustum;

    float* vp = new float[16];
    bx::mtxMul(vp, this->camera.view, this->camera.proj);

    frustum.left.normal[0] = vp[3] + vp[0];
    frustum.left.normal[1] = vp[7] + vp[4];
    frustum.left.normal[2] = vp[11] + vp[8];
    frustum.left.distance  = vp[15] + vp[12];

    frustum.right.normal[0] = vp[3] - vp[0];
    frustum.right.normal[1] = vp[7] - vp[4];
    frustum.right.normal[2] = vp[11] - vp[8];
    frustum.right.distance  = vp[15] - vp[12];

    frustum.bottom.normal[0] = vp[3] + vp[1];
    frustum.bottom.normal[1] = vp[7] + vp[5];
    frustum.bottom.normal[2] = vp[11] + vp[9];
    frustum.bottom.distance  = vp[15] + vp[13];

    frustum.top.normal[0] = vp[3] - vp[1];
    frustum.top.normal[1] = vp[7] - vp[5];
    frustum.top.normal[2] = vp[11] - vp[9];
    frustum.top.distance  = vp[15] - vp[13];

    frustum.n.normal[0] = vp[3] + vp[2];
    frustum.n.normal[1] = vp[7] + vp[6];
    frustum.n.normal[2] = vp[11] + vp[10];
    frustum.n.distance  = vp[15] + vp[14];

    frustum.f.normal[0] = vp[3] - vp[2];
    frustum.f.normal[1] = vp[7] - vp[6];
    frustum.f.normal[2] = vp[11] - vp[10];
    frustum.f.distance  = vp[15] - vp[14];

    _normalizePlane(frustum.left);
    _normalizePlane(frustum.right);
    _normalizePlane(frustum.top);
    _normalizePlane(frustum.bottom);
    _normalizePlane(frustum.n);
    _normalizePlane(frustum.f);
    delete[] vp;

    return frustum;
}

bool CameraComponent::_aabbInsidePlane(const Plane& plane, const bx::Vec3& min, const bx::Vec3& max) {
    bx::Vec3 v = { (plane.normal[0] >= 0.0f) ? max.x : min.x,
                   (plane.normal[1] >= 0.0f) ? max.y : min.y,
                   (plane.normal[2] >= 0.0f) ? max.z : min.z };
    
    const float distance = bx::dot(bx::Vec3(plane.normal[0], plane.normal[1], plane.normal[2]), v) + plane.distance;

    return distance >= 0;
}

bool CameraComponent::_aabbInsideFrustum(const Frustum&  frustum,
                                         const bx::Vec3& min,
                                         const bx::Vec3& max) {
    SYN_PROFILE_FUNCTION();
    return _aabbInsidePlane(frustum.left, min, max) &&
           _aabbInsidePlane(frustum.right, min, max) &&
           _aabbInsidePlane(frustum.top, min, max) &&
           _aabbInsidePlane(frustum.bottom, min, max) &&
           _aabbInsidePlane(frustum.n, min, max) &&
           _aabbInsidePlane(frustum.f, min, max);
}

} // namespace Syngine
