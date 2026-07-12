// ╒═══════════════════════ Matrices.cpp ═╕
// │ Syngine                              │
// │ Created 2026-07-11                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../defines.h"
#include "Syngine/Math/Quaternion.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include <Syngine/Syngine.h>

using namespace Syngine;
using namespace Syngine::Math;
using namespace Catch::Matchers;

// Tests matrix math operations

// Only half the require_that are here because i am lazy
TEST_CASE("Matrix addition", "[Math]") {
    Mat4 m4a(
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );
    Mat4 m4b(
        16.0f, 15.0f, 14.0f, 13.0f,
        12.0f, 11.0f, 10.0f, 9.0f,
        8.0f, 7.0f, 6.0f, 5.0f,
        4.0f, 3.0f, 2.0f, 1.0f
    );
    Mat4 m4r = m4a + m4b;
    REQUIRE_THAT(m4r.m(0, 0), WithinAbs(17.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(0, 1), WithinAbs(17.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(0, 2), WithinAbs(17.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(0, 3), WithinAbs(17.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(1, 0), WithinAbs(17.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(1, 1), WithinAbs(17.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(1, 2), WithinAbs(17.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(1, 3), WithinAbs(17.0f, FLOAT_MARGIN));

    Mat3 m3a(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    Mat3 m3b(9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f);
    Mat3 m3r = m3a + m3b;
    REQUIRE_THAT(m3r.m(0, 0), WithinAbs(10.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m3r.m(0, 1), WithinAbs(10.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m3r.m(0, 2), WithinAbs(10.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m3r.m(1, 0), WithinAbs(10.0f, FLOAT_MARGIN));
}

TEST_CASE("Matrix multiplication", "[Math]") {
    Mat4 m4a(
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    );
    Mat4 m4b(
        16.0f, 15.0f, 14.0f, 13.0f,
        12.0f, 11.0f, 10.0f, 9.0f,
        8.0f, 7.0f, 6.0f, 5.0f,
        4.0f, 3.0f, 2.0f, 1.0f
    );
    Mat4 m4r = m4a * m4b;
    REQUIRE_THAT(m4r.m(0, 0), WithinAbs(80.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(0, 1), WithinAbs(70.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(0, 2), WithinAbs(60.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4r.m(0, 3), WithinAbs(50.0f, FLOAT_MARGIN));

    // Vec * matrix

    SVec4 v4(1.0f, 2.0f, 3.0f, 4.0f);
    SVec4 v4r = v4 * m4a;
    REQUIRE_THAT(v4r.x(), WithinAbs(90.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.y(), WithinAbs(100.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.z(), WithinAbs(110.0f, FLOAT_MARGIN));
    REQUIRE_THAT(v4r.w(), WithinAbs(120.0f, FLOAT_MARGIN));
}

TEST_CASE("Model Matrix creation", "[Math]") {
    SVec3      scale(5.0f);
    Quaternion rot(SVec3(45, 45, 0).toRads());
    SVec3      translate(1.0f, 2.0f, 3.0f);
    Mat4       m4a(scale, rot, translate);
    /*
    output matrix should be (row major (dont question it))
    3.5355, 0.0000, -3.5355, 0.0000
    2.5000, 3.5355,  2.5000, 0.0000
    2.5000, -3.5355, 2.5000, 0.0000
    1.0000, 2.0000,  3.0000, 1.0000
    */
    REQUIRE_THAT(m4a.m(0, 0), WithinAbs(3.5355f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(0, 1), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(0, 2), WithinAbs(-3.5355f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(0, 3), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(1, 0), WithinAbs(2.5000f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(1, 1), WithinAbs(3.5355f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(1, 2), WithinAbs(2.5000f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(1, 3), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(2, 0), WithinAbs(2.5000f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(2, 1), WithinAbs(-3.5355f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(2, 2), WithinAbs(2.5000f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(2, 3), WithinAbs(0.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(3, 0), WithinAbs(1.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(3, 1), WithinAbs(2.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(3, 2), WithinAbs(3.0f, FLOAT_MARGIN));
    REQUIRE_THAT(m4a.m(3, 3), WithinAbs(1.0f, FLOAT_MARGIN));
}

TEST_CASE("Matrix Conversion", "[Math]") {
    SVec3      scale(5.0f);
    Quaternion rot(SVec3(45, 45, 0).toRads());
    SVec3      translate(1.0f, 2.0f, 3.0f);
    Mat4       m4a(scale, rot, translate);
    const float* elements = m4a.data();
    REQUIRE_THAT(elements[0], WithinAbs(3.5355f, FLOAT_MARGIN)); // probably all we need to check
}
