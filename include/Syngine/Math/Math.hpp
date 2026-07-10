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

namespace Syngine::Math {

inline constexpr double PI = 3.14159265358979323846; // Pi constant
inline constexpr double DEG2RAD(double degrees) { return degrees * (PI / 180.0); } // Degrees to radians
inline constexpr double RAD2DEG(double radians) { return radians * (180.0 / PI); } // Radians to degrees
inline constexpr double EPSILON = 1e-6; // Small value for floating-point comparisons

inline const Vector3 AXIS_Z() { return Vector3(0.0f, 0.0f, 1.0f); }
inline const Vector3 AXIS_Y() { return Vector3(0.0f, 1.0f, 0.0f); }
inline const Vector3 AXIS_X() { return Vector3(1.0f, 0.0f, 0.0f); }

} // namespace Syngine::Math
