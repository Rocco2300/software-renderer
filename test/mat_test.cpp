#include <catch2/catch_test_macros.hpp>

#include "math/mat.hpp"
#include "math/vec.hpp"

TEST_CASE("2d vec translation", "[math]") {
    mat3 t(1.f);
    vec3 v(1.f);

    t[2][0] = 2.0f;
    t[2][1] = 1.0f;
    auto res = t * v;

    vec3 baseline(3.0f, 2.0f, 1.0f);
    REQUIRE(res == baseline);
}