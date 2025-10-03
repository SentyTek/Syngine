// ╒════════════════ CameraComponent.cpp ═╕
// │ Syngine                              │
// │ Created 2025-06-10                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/ECS/Components/CameraComponent.h"
#include "Syngine/ECS/GameObject.h"

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

Camera Syngine::CameraComponent::GetCamera() const {
    return this->camera;
}

} // namespace Syngine