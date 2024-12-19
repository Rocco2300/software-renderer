#pragma once

#include "mat.hpp"
#include "vec.hpp"

mat4 translate(const vec3& translate) {
    auto ret = mat4(1.f);
    ret[3][0] = translate.x;
    ret[3][1] = translate.y;
    ret[3][2] = translate.z;
    return ret;
}

mat4 scale(const vec3& scale) {
    auto ret = mat4(1.f);
    ret[0][0] = scale.x;
    ret[1][1] = scale.y;
    ret[2][2] = scale.z;
    return ret;
}

mat4 rotateOX(float angle) {
    auto rad = angle * (M_PI / 180);
    auto ret = mat4(1.f);
    ret[1][1] = cos(rad);
    ret[1][2] = sin(rad);
    ret[2][1] = -sin(rad);
    ret[2][2] = cos(rad);
    return ret;
}

mat4 rotateOY(float angle) {
    auto rad = angle * (M_PI / 180);
    auto ret = mat4(1.f);
    ret[0][0] = cos(rad);
    ret[0][2] = -sin(rad);
    ret[2][0] = sin(rad);
    ret[2][2] = cos(rad);
    return ret;
}

mat4 rotateOZ(float angle) {
    auto rad = angle * (M_PI / 180);
    auto ret = mat4(1.f);
    ret[0][0] = cos(rad);
    ret[0][1] = sin(rad);
    ret[1][0] = -sin(rad);
    ret[1][1] = cos(rad);
    return ret;
}
