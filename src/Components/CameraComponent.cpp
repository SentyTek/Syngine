#include "CameraComponent.h"
#include "bx/math.h"
#include "bgfx/bgfx.h"

Syngine::CameraComponent::CameraComponent(GameObject* owner) {
    this->m_owner = owner;
    this->camera = {};
    this->camera.eye[0] = 0.0f;
    this->camera.eye[1] = 0.0f;
    this->camera.eye[2] = -5.0f;
    this->camera.target[0] = 0.0f;
    this->camera.target[1] = 0.0f;
    this->camera.target[2] = 0.0f;
    this->camera.up[0] = 0.0f;
    this->camera.up[1] = 1.0f;
    this->camera.up[2] = 0.0f;
}

Syngine::CameraComponent::~CameraComponent() {
    // No specific cleanup needed for now
}

void Syngine::CameraComponent::Update(int viewId, int width, int height) {
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
    bx::mtxProj(cam.proj, cam.fov, aspect, cam.near, cam.far, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(viewId, cam.view, cam.proj);
}

SynComponents Syngine::CameraComponent::getComponentType() {
    return SYN_COMPONENT_CAMERA;
}

void Syngine::CameraComponent::SetPosition(float x, float y, float z) {
    this->camera.eye[0] = x;
    this->camera.eye[1] = y;
    this->camera.eye[2] = z;
}

const float* Syngine::CameraComponent::GetPosition() const {
    return this->camera.eye;
}

void Syngine::CameraComponent::SetFOV(float fov) {
    if (fov < 1.0f || fov > 179.0f) {
        return;
    }
    this->camera.fov = fov;
}

float Syngine::CameraComponent::GetFOV() const { return this->camera.fov; }

void Syngine::CameraComponent::SetFarPlane(float farPlane) {
    if (farPlane < 0.0f && farPlane > this->camera.near && farPlane > 50000.0f) {
        return;
    }
    this->camera.far = farPlane;
}

float Syngine::CameraComponent::GetFarPlane() const { return this->camera.far; }

void Syngine::CameraComponent::SetAngles(float yaw, float pitch) {
    this->camera.yaw = yaw;
    this->camera.pitch = pitch;
}

void Syngine::CameraComponent::GetAngles(float& yaw, float& pitch) const {
    yaw = this->camera.yaw;
    pitch = this->camera.pitch;
}

Syngine::Camera Syngine::CameraComponent::GetCamera() const {
    return this->camera;
}