// ╒═════════════════════════════ Math.h ═╕
// │ Syngine                              │
// │ Created 2026-07-07                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

// This file includes the rest of the math library headers for convenience.
// It also defines some common math types and constants.

#include "Syngine/Math/Vector2.hpp"
#include "Syngine/Math/Vector3.hpp"
#include "Syngine/Math/Vector4.hpp"
#include "Syngine/Math/Matrix3x3.hpp"
#include "Syngine/Math/Matrix4x4.hpp"
#include "Syngine/Math/Quaternion.hpp"
#include "Syngine/Math/Ray.hpp"

namespace Syngine::Math {

/* clang-format off */
inline constexpr double PI = 3.14159265358979323846; // Pi constant
inline constexpr double DEG2RAD(double degrees) { return degrees * (PI / 180.0); } // Degrees to radians
inline constexpr double RAD2DEG(double radians) { return radians * (180.0 / PI); } // Radians to degrees
inline constexpr double EPSILON = 1e-6; // Small value for floating-point comparisons

inline const Vector3 AXIS_Z() { return Vector3(0.0f, 0.0f, 1.0f); }
inline const Vector3 AXIS_Y() { return Vector3(0.0f, 1.0f, 0.0f); }
inline const Vector3 AXIS_X() { return Vector3(1.0f, 0.0f, 0.0f); }

inline const float Clampf(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

inline Vector3 Unproject(const Vector2&   screenPoint,
                            const Matrix4x4& proj,
                            const Matrix4x4& view,
                            float            viewportWidth,
                            float            viewportHeight,
                            float            depth) {
    DirectX::XMVECTOR screen =
        DirectX::XMVectorSet(screenPoint.x(), screenPoint.y(), depth, 1.0f);

    DirectX::XMFLOAT3 screenStorage;
    DirectX::XMStoreFloat3(&screenStorage, screen);
    DirectX::XMFLOAT4X4 projStorage = proj.m_getStorage();
    DirectX::XMFLOAT4X4 viewStorage = view.m_getStorage();

    DirectX::XMVECTOR v     = DirectX::XMLoadFloat3(&screenStorage);
    DirectX::XMMATRIX mProj = DirectX::XMLoadFloat4x4(&projStorage);
    DirectX::XMMATRIX mView = DirectX::XMLoadFloat4x4(&viewStorage);
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

    DirectX::XMVECTOR result = DirectX::XMVector3Unproject(v,
                                                            0.0f,
                                                            0.0f,
                                                            viewportWidth,
                                                            viewportHeight,
                                                            0.0f,
                                                            1.0f,
                                                            mProj,
                                                            mView,
                                                            world);

    DirectX::XMFLOAT3 resultStorage;
    DirectX::XMStoreFloat3(&resultStorage, result);
    return Vector3(resultStorage);
}

/* clang-format on */

} // namespace Syngine::Math
