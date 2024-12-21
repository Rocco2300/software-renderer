#pragma once

#include "mat.hpp"
#include "vec.hpp"

inline mat4 translate(const vec3& translate) {
    auto ret  = mat4(1.f);
    ret[3][0] = translate.x;
    ret[3][1] = translate.y;
    ret[3][2] = translate.z;
    return ret;
}

inline mat4 scale(const vec3& scale) {
    auto ret  = mat4(1.f);
    ret[0][0] = scale.x;
    ret[1][1] = scale.y;
    ret[2][2] = scale.z;
    return ret;
}

inline mat4 rotateOX(float angle) {
    auto rad  = angle * (M_PI / 180);
    auto ret  = mat4(1.f);
    ret[1][1] = cos(rad);
    ret[1][2] = sin(rad);
    ret[2][1] = -sin(rad);
    ret[2][2] = cos(rad);
    return ret;
}

inline mat4 rotateOY(float angle) {
    auto rad  = angle * (M_PI / 180);
    auto ret  = mat4(1.f);
    ret[0][0] = cos(rad);
    ret[0][2] = -sin(rad);
    ret[2][0] = sin(rad);
    ret[2][2] = cos(rad);
    return ret;
}

inline mat4 rotateOZ(float angle) {
    auto rad  = angle * (M_PI / 180);
    auto ret  = mat4(1.f);
    ret[0][0] = cos(rad);
    ret[0][1] = sin(rad);
    ret[1][0] = -sin(rad);
    ret[1][1] = cos(rad);
    return ret;
}

inline mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
    auto mat  = mat4(1.f);
    mat[0][0] = 1.f / (std::tan(fovy / 2.f) * aspect);
    mat[1][1] = 1.f / std::tan(fovy / 2.f);
    mat[2][2] = (-1.f * (zFar + zNear)) / (zFar - zNear);
    mat[3][2] = (-2.f * zFar * zNear) / (zFar - zNear);
    mat[2][3] = -1.f;

    return mat;
}

inline mat4 View(const vec3& position, const vec3& forward, const vec3& right, const vec3& up) {
    auto mat  = mat4(1.f);
    mat[0][0] = right.x;
    mat[1][0] = right.y;
    mat[2][0] = right.z;

    mat[0][1] = up.x;
    mat[1][1] = up.y;
    mat[2][1] = up.z;

    mat[0][2] = forward.x;
    mat[1][2] = forward.y;
    mat[2][2] = forward.z;

    return mat * translate(-1 * position);
}

struct logic_space {
    float x, y, width, height;
};

struct viewport_space {
    float x, y, width, height;
};

inline mat4 viewport(const logic_space& logicSpace, const viewport_space& viewportSpace) {
    auto originTranslate = translate({-logicSpace.x, -logicSpace.y, 0.0f});
    auto viewTranslate   = translate({viewportSpace.x, viewportSpace.y, 0.0f});
    auto viewScale =
            scale({viewportSpace.width / logicSpace.width,
                   viewportSpace.height / logicSpace.height,
                   1.0f});
    return viewTranslate * viewScale * originTranslate;
}