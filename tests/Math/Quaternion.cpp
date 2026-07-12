// ╒═════════════════════ Quaternion.cpp ═╕
// │ Syngine                              │
// │ Created 2026-07-11                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../defines.h"
#include "Syngine/Math/Math.hpp"
#include <Syngine/Syngine.h>

using namespace Syngine;
using namespace Syngine::Math;
using namespace Catch::Matchers;

// Tests for the Quaternion class in the Syngine Math library

static SVec3 eulerAngles = SVec3(45.0f, 45.0f, 0.0f).toRads();;

TEST_CASE("Quaternion operations", "[Math]") {
    // multiplcation
    SQuat q1(eulerAngles);
    SQuat q2(eulerAngles);

    SQuat q3 = q1 * q2; // Quaternion multiplication
    REQUIRE_THAT(q3.x(), WithinAbs(0.6036f, FLOAT_MARGIN));
    REQUIRE_THAT(q3.y(), WithinAbs(0.6036f, FLOAT_MARGIN));
    REQUIRE_THAT(q3.z(), WithinAbs(-0.25f, FLOAT_MARGIN));
    REQUIRE_THAT(q3.w(), WithinAbs(0.4571, FLOAT_MARGIN));

    // conjugation
    SQuat q4 = q1.conjugated();
    REQUIRE_THAT(q4.x(), WithinAbs(-q1.x(), FLOAT_MARGIN));
    REQUIRE_THAT(q4.y(), WithinAbs(-q1.y(), FLOAT_MARGIN));
    REQUIRE_THAT(q4.z(), WithinAbs(-q1.z(), FLOAT_MARGIN));
    REQUIRE_THAT(q4.w(), WithinAbs(q1.w(), FLOAT_MARGIN));

    // default constructor and identity quaternion
    SQuat q5;
    REQUIRE_THAT(q5.x(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(q5.y(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(q5.z(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(q5.w(), WithinAbs(1.0f, FLOAT_MARGIN));

    // vec * identity should return the same vector
    SVec3 t(1.0f, 2.0f, 3.0f);
    SVec3 r = q5 * t;
    REQUIRE_THAT(r.x(), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(r.y(), WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(r.z(), WithinAbs(3.0f, FLOAT_MARGIN));

    // Rotate +X by +90 deg around +Z -> +Y
    SQuat qz90(SVec3(0.0f, 0.0f, 1.0f), DEG2RAD(90.0f));
    SVec3 xAxis(1.0f, 0.0f, 0.0f);
    SVec3 out = qz90 * xAxis;
    REQUIRE_THAT(out.x(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(out.y(), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(out.z(), WithinAbs(0.0f, FLOAT_MARGIN));

    // multiplying a normalized quat by its conjugate should yield the identity quaternion
    SQuat q8 = q1.normalized() * q1.normalized().conjugated();
    REQUIRE_THAT(q8.x(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(q8.y(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(q8.z(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(q8.w(), WithinAbs(1.0f, FLOAT_MARGIN));
}
