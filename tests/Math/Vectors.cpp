// ╒════════════════════════ Vectors.cpp ═╕
// │ Syngine                              │
// │ Created 2026-07-11                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../defines.h"
#include <Syngine/Syngine.h>

using namespace Syngine;
using namespace Syngine::Math;
using namespace Catch::Matchers;

// Tests vector math operations

TEST_CASE("Vector addition", "[Math]") {
    SVec2 v2a(1.0f, 2.0f);
    SVec2 v2b(4.0f, 5.0f);
    SVec2 v2r = v2a + v2b;
    REQUIRE_THAT(v2r.x(), WithinAbs(5.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v2r.y(), WithinAbs(7.0f, FLOAT_MARGIN));

    SVec3 v3a(1.0f, 2.0f, 3.0f);
    SVec3 v3b(4.0f, 5.0f, 6.0f);
    SVec3 v3r = v3a + v3b;
    REQUIRE_THAT(v3r.x(), WithinAbs(5.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3r.y(), WithinAbs(7.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3r.z(), WithinAbs(9.0f, FLOAT_MARGIN));

    SVec4 v4a(1.0f, 2.0f, 3.0f, 4.0f);
    SVec4 v4b(5.0f, 6.0f, 7.0f, 8.0f);
    SVec4 v4r = v4a + v4b;
    REQUIRE_THAT(v4r.x(), WithinAbs(6.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.y(), WithinAbs(8.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.z(), WithinAbs(10.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.w(), WithinAbs(12.0f, FLOAT_MARGIN));
}

TEST_CASE("Vector subtraction", "[Math]") {
    SVec2 v2a(4.0f, 5.0f);
    SVec2 v2b(1.0f, 2.0f);
    SVec2 v2r = v2a - v2b;
    REQUIRE_THAT(v2r.x(), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v2r.y(), WithinAbs(3.0f, FLOAT_MARGIN));

    SVec3 v3a(4.0f, 5.0f, 6.0f);
    SVec3 v3b(1.0f, 2.0f, 3.0f);
    SVec3 v3r = v3a - v3b;
    REQUIRE_THAT(v3r.x(), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3r.y(), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3r.z(), WithinAbs(3.0f, FLOAT_MARGIN));

    SVec4 v4a(5.0f, 6.0f, 7.0f, 8.0f);
    SVec4 v4b(1.0f, 2.0f, 3.0f, 4.0f);
    SVec4 v4r = v4a - v4b;
    REQUIRE_THAT(v4r.x(), WithinAbs(4.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.y(), WithinAbs(4.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.z(), WithinAbs(4.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.w(), WithinAbs(4.0f, FLOAT_MARGIN));
}

TEST_CASE("Vector multiplication by scalar", "[Math]") {
    SVec2 v2a(1.0f, 2.0f);
    constexpr float scalar = 3.0f;
    SVec2 v2r = v2a * scalar;
    REQUIRE_THAT(v2r.x(), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v2r.y(), WithinAbs(6.0f, FLOAT_MARGIN));

    SVec3 v3a(1.0f, 2.0f, 3.0f);
    SVec3 v3r = v3a * scalar;
    REQUIRE_THAT(v3r.x(), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3r.y(), WithinAbs(6.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3r.z(), WithinAbs(9.0f, FLOAT_MARGIN));

    SVec4 v4a(1.0f, 2.0f, 3.0f, 4.0f);
    SVec4 v4r = v4a * scalar;
    REQUIRE_THAT(v4r.x(), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.y(), WithinAbs(6.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.z(), WithinAbs(9.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.w(), WithinAbs(12.0f, FLOAT_MARGIN));
}

TEST_CASE("Vector Conversions", "[Math]") {
    SVec2 v2(1.0f, 2.0f);
    SVec3 v3(v2, 0.0f);
    REQUIRE_THAT(v3.x(), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3.y(), WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v3.z(), WithinAbs(0.0f, FLOAT_MARGIN));

    SVec4 v4(v2, 0.0f, 0.0f);
    REQUIRE_THAT(v4.x(), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4.y(), WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4.z(), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4.w(), WithinAbs(0.0f, FLOAT_MARGIN));

    SVec2 v5(2.0f);
    std::vector<float> v5_components = v5;
    float* v5_arr = v5.data();
    REQUIRE_THAT(v5_components[0], WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v5_arr[0], WithinAbs(2.0f, FLOAT_MARGIN));

    SVec3 v6(1.0f);
    JPH::Vec3 jph_v6 = v6.toJoltVec3();
    bx::Vec3  bx_v6  = v6.toBxVec3();
    std::vector<float> v6_components = v6;
    float* v6_arr = v6.data();
    REQUIRE_THAT(jph_v6.GetX(), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(bx_v6.x, WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v6_components[0], WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v6_arr[0], WithinAbs(1.0f, FLOAT_MARGIN));

    SVec4 v7(3.0f);
    JPH::Vec4 jph_v7 = v7.toJoltVec4();
    std::vector<float> v7_components = v7;
    float* v7_arr = v7.data();
    REQUIRE_THAT(jph_v7.GetX(), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v7_components[0], WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v7_arr[0], WithinAbs(3.0f, FLOAT_MARGIN));
}

TEST_CASE("Vector math", "[Math]") {
    SVec2 v2a(5.0f, 7.0f);
    SVec2 v2b(2.0f, 3.0f);
    float dot = v2a.dot(v2b);
    float cross = v2a.cross(v2b);
    float length = v2a.length();
    float dist = v2a.distance(v2b);
    REQUIRE_THAT(dot, WithinAbs(31.0f, FLOAT_MARGIN));
    REQUIRE_THAT(cross, WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(length, WithinAbs(8.602325f, FLOAT_MARGIN));
    REQUIRE_THAT(dist, WithinAbs(5.0f, FLOAT_MARGIN));

    SVec3 v3a(1.0f, 2.0f, 3.0f);
    SVec3 v3b(4.0f, 5.0f, 6.0f);
    float dot3 = v3a.dot(v3b);
    SVec3 cross3 = v3a.cross(v3b);
    float length3 = v3a.length();
    float dist3 = v3a.distance(v3b);
    REQUIRE_THAT(dot3, WithinAbs(32.0f, FLOAT_MARGIN));
    REQUIRE_THAT(cross3.x(), WithinAbs(-3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(length3, WithinAbs(3.741657f, FLOAT_MARGIN));
    REQUIRE_THAT(dist3, WithinAbs(5.196152f, FLOAT_MARGIN));

    SVec4 v4a(1.0f, 2.0f, 3.0f, 4.0f);
    SVec4 v4b(5.0f, 6.0f, 7.0f, 8.0f);
    float dot4 = v4a.dot(v4b);
    SVec4 cross4 = v4a.cross(v4b);
    float length4 = v4a.length();
    float dist4 = v4a.distance(v4b);
    REQUIRE_THAT(dot4, WithinAbs(70.0f, FLOAT_MARGIN));
    REQUIRE_THAT(cross4.x(), WithinAbs(-4.0f, FLOAT_MARGIN));
    REQUIRE_THAT(length4, WithinAbs(5.477226f, FLOAT_MARGIN));
    REQUIRE_THAT(dist4, WithinAbs(8.0f, FLOAT_MARGIN));

    SVec3 v3c = SVec3(7.0f, 8.0f, 9.0f).normalized();
    REQUIRE_THAT(v3c.length(), WithinAbs(1.0f, FLOAT_MARGIN));
}
