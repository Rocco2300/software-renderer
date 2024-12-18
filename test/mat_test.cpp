#include <catch2/catch_test_macros.hpp>

#include "math/mat.hpp"
#include "math/vec.hpp"

TEST_CASE("mat addition", "[math]") {
    mat3 a(1.f);
    mat3 b(2.f);

    auto res = a + b;
    REQUIRE(res[0][0] - 3.0f < 0.001f);
    REQUIRE(res[1][1] - 3.0f < 0.001f);
    REQUIRE(res[2][2] - 3.0f < 0.001f);
}

TEST_CASE("mat subtraction", "[math]") {
    mat3 a(5.f);
    mat3 b(2.f);

    auto res = a - b;
    REQUIRE(res[0][0] - 3.0f < 0.001f);
    REQUIRE(res[1][1] - 3.0f < 0.001f);
    REQUIRE(res[2][2] - 3.0f < 0.001f);
}

TEST_CASE("mat multiplication", "[math]") {
    mat3 a(1.f);
    mat3 b(2.f);

    auto res1 = a * b;
    REQUIRE(res1[0][0] - 2.0f < 0.001f);
    REQUIRE(res1[1][1] - 2.0f < 0.001f);
    REQUIRE(res1[2][2] - 2.0f < 0.001f);

    mat3 c(1.f);
    mat3 d(2.f);
    mat3 e(1.f);

    e[2][0] = 3.f;
    e[2][1] = 2.f;

    auto res2 = c * d * e;
    REQUIRE(res2[0][0] - 2.0f < 0.001f);
    REQUIRE(res2[1][1] - 2.0f < 0.001f);
    REQUIRE(res2[2][2] - 2.0f < 0.001f);
    REQUIRE(res2[2][0] - 3.0f < 0.001f);
    REQUIRE(res2[2][1] - 2.0f < 0.001f);
}

TEST_CASE("vec-mat multiplication", "[math]") {
    mat3 t(1.f);
    vec3 v(1.f);

    t[2][0] = 2.0f;
    t[2][1] = 1.0f;
    auto res = t * v;

    vec3 baseline(3.0f, 2.0f, 1.0f);
    REQUIRE(res == baseline);
}
