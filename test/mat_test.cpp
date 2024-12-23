#include <catch2/catch_test_macros.hpp>

#include "math/mat.hpp"
#include "math/vec.hpp"

TEST_CASE("mat3 addition", "[math]") {
    mat3 a(1.f);
    mat3 b(2.f);

    auto res = a + b;
    REQUIRE(std::abs(res[0][0] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[1][1] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[2][2] - 3.0f) < 0.001f);
}

TEST_CASE("mat3 subtraction", "[math]") {
    mat3 a(5.f);
    mat3 b(2.f);

    auto res = a - b;
    REQUIRE(std::abs(res[0][0] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[1][1] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[2][2] - 3.0f) < 0.001f);
}

TEST_CASE("mat3 multiplication", "[math]") {
    mat3 a(1.f);
    mat3 b(2.f);

    auto res1 = a * b;
    REQUIRE(std::abs(res1[0][0] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res1[1][1] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res1[2][2] - 2.0f) < 0.001f);

    mat3 c(1.f);
    mat3 d(2.f);
    mat3 e(1.f);

    d[2][0] = 1.f;
    d[2][1] = 1.f;
    e[2][0] = 3.f;
    e[2][1] = 2.f;

    auto res2 = c * d * e;
    REQUIRE(std::abs(res2[0][0] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res2[1][1] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res2[2][2] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res2[2][0] - 7.0f) < 0.001f);
    REQUIRE(std::abs(res2[2][1] - 5.0f) < 0.001f);
}

TEST_CASE("vec3-mat3 multiplication", "[math]") {
    mat3 t(1.f);
    vec3 v(1.f);

    t[2][0] = 2.0f;
    t[2][1] = 1.0f;
    auto res = t * v;

    vec3 baseline(3.0f, 2.0f, 1.0f);
    REQUIRE(res == baseline);
}

TEST_CASE("mat4 addition", "[math]") {
    mat4 a(1.f);
    mat4 b(2.f);

    auto res = a + b;
    REQUIRE(std::abs(res[0][0] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[1][1] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[2][2] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[3][3] - 3.0f) < 0.001f);
}

TEST_CASE("mat4 subtraction", "[math]") {
    mat4 a(5.f);
    mat4 b(2.f);

    auto res = a - b;
    REQUIRE(std::abs(res[0][0] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[1][1] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[2][2] - 3.0f) < 0.001f);
    REQUIRE(std::abs(res[3][3] - 3.0f) < 0.001f);
}

TEST_CASE("mat4 multiplication", "[math]") {
    mat4 a(1.f);
    mat4 b(2.f);

    auto res1 = a * b;
    REQUIRE(std::abs(res1[0][0] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res1[1][1] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res1[2][2] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res1[3][3] - 2.0f) < 0.001f);

    mat4 c(1.f);
    mat4 d(2.f);
    mat4 e(1.f);

    d[3][0] = 1.f;
    d[3][1] = 1.f;
    d[3][2] = 1.f;
    e[3][0] = 3.f;
    e[3][1] = 2.f;
    e[3][2] = 4.f;

    auto res2 = c * d * e;
    REQUIRE(std::abs(res2[0][0] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res2[1][1] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res2[2][2] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res2[3][3] - 2.0f) < 0.001f);
    REQUIRE(std::abs(res2[3][0] - 7.0f) < 0.001f);
    REQUIRE(std::abs(res2[3][1] - 5.0f) < 0.001f);
    REQUIRE(std::abs(res2[3][2] - 9.0f) < 0.001f);

    mat4 f(1.f);
    mat4 g(1.f);

    f[3][0] = 1.f;
    f[3][1] = 1.f;
    f[3][2] = 1.f;
    g[3][0] = 1.f;
    g[3][1] = 2.f;
    g[3][2] = 3.f;
    auto res3 = f * g;
    REQUIRE(std::abs(res3[3][0] - 2.f) < 0.001f);
    REQUIRE(std::abs(res3[3][1] - 3.f) < 0.001f);
    REQUIRE(std::abs(res3[3][2] - 4.f) < 0.001f);
}

TEST_CASE("vec4-mat4 multiplication", "[math]") {
    mat4 t(1.f);
    vec4 v(1.f);

    t[3][0] = 2.0f;
    t[3][1] = 1.0f;
    t[3][2] = 3.0f;
    auto res = t * v;

    vec4 baseline(3.0f, 2.0f, 4.0f, 1.0f);
    REQUIRE(res == baseline);
}
