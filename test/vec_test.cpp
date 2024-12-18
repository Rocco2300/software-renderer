#include <catch2/catch_test_macros.hpp>

#include "math/vec_type.hpp"
#include "math/vec2_type.hpp"

const float Epsilon = 0.001f;

TEST_CASE("vec equality", "[math]") {
    vec2 a(2.3f, 5.2f);
    vec2 b(5.1f, 0.5f);
    REQUIRE_FALSE(a == b);

    vec2 v1(1.0f, 0.5f);
    vec2 v2(1.0f, 0.5f);
    REQUIRE(v1 == v2);
    REQUIRE(v1 == v1);
    REQUIRE(v2 == v2);
}

TEST_CASE("vec inequality", "[math]") {
    vec2 a(2.3f, 5.2f);
    vec2 b(5.1f, 0.5f);
    REQUIRE(a != b);

    vec2 v1(1.0f, 0.5f);
    vec2 v2(1.2f, 0.7f);
    REQUIRE(v1 != v2);
}

TEST_CASE("vec addition", "[math]") {
    vec2 a(2.3f, 5.2f);
    vec2 b(5.1f, 0.5f);
    vec2 c(1.0f, 0.25f);
    vec3 d(1.0f, 1.5f, 3.2f);
    vec3 e(0.1f, 2.6f, 8.2f);

    vec2 res1 = a + b;
    REQUIRE(res1[0] - 7.4f < Epsilon);
    REQUIRE(res1[1] - 5.7f < Epsilon);

    vec2 res2 = a + c;
    REQUIRE(res2[0] - 3.3f < Epsilon);
    REQUIRE(res2[1] - 5.45f < Epsilon);

    vec2 res3 = b + c;
    REQUIRE(res3[0] - 6.1f < Epsilon);
    REQUIRE(res3[1] - 0.75f < Epsilon);

    vec3 res4 = d + e;
    REQUIRE(res4[0] - 1.1f < Epsilon);
    REQUIRE(res4[1] - 4.1f < Epsilon);
    REQUIRE(res4[2] - 11.4f < Epsilon);
}

TEST_CASE("vec subtract", "[math]") {
    vec2 a(2.3f, 5.2f);
    vec2 b(1.2f, 0.5f);
    vec3 c(2.2f, 1.0f, 0.7f);
    vec3 d(1.9f, 0.7f, 3.2f);

    auto res1 = a - b;
    auto res2 = b - a;
    REQUIRE(res1 != res2);
    REQUIRE(res1 == vec2(1.1f, 4.7f));
    REQUIRE(res2 == vec2(-1.1f, -4.7f));

    auto res3 = c - d;
    auto res4 = d - c;
    REQUIRE(res3 != res4);
    REQUIRE(res3 == vec3(0.3f, 0.3f, -2.5f));
    REQUIRE(res4 == vec3(-0.3f, -0.3f, 2.5f));
}

TEST_CASE("vec multiplication with scalar", "[math]") {
    float s = 2.f;
    vec2 a(2.1f, 5.6f);
    vec3 b(1.7f, 8.4f, 3.3f);

    auto res1 = s * a;
    auto res2 = a * s;
    REQUIRE(res1 == res2);
    REQUIRE(res1 == vec2(4.2f, 11.2f));

    auto res3 = s * b;
    auto res4 = b * s;
    REQUIRE(res3 == res4);
    REQUIRE(res3 == vec3(3.4f, 16.8f, 6.6f));
}

TEST_CASE("vec division with scalar", "[math]") {
    float s = 2.f;
    vec3 a(2.f, 3.3f, 5.2f);

    auto res = a / s;
    REQUIRE(res == vec3(1.f, 1.65f, 2.6f));
}

TEST_CASE("vec dot product", "[math]") {
    vec2 a(1.f, 0.5f);
    vec2 b(1.2f, 2.2f);
    vec3 c(1.f, 2.f, 3.f);
    vec3 d(0.5f, 7.5f, 8.9f);

    auto res1 = dot(a, b);
    auto res2 = dot(c, d);
    REQUIRE(res1 - 2.3f < Epsilon);
    REQUIRE(res2 - 42.2f < Epsilon);
}

TEST_CASE("vec length", "[math]") {
    vec2 a(1.2f, 0.5f);
    vec3 b(1.1f, 0.7f, 2.1f);

    auto res1 = length(a);
    auto res2 = length(b);
    REQUIRE(res1 - 1.3f < Epsilon);
    REQUIRE(res2 - 2.471f < Epsilon);
}

TEST_CASE("vec normalization", "[math]") {
    vec2 a(1.0f, 1.0f);
    vec3 b(1.1f, 0.7f, 2.1f);

    auto res1 = normalize(a);
    auto res2 = normalize(b);
    auto baseline1 = vec2(0.707f, 0.707f);
    auto baseline2 = vec3(0.445f, 0.283f, 0.849f);
    REQUIRE(res1 == baseline1);
    REQUIRE(res2 == baseline2);
}