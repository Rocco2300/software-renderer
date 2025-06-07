#pragma once

#include "math/vec.hpp"
#include "math/mat.hpp"

namespace sfr::camera {

struct camera_data {
    vec3 position;
    vec3 up;
    vec3 forward;
    vec3 right;

    float fov;
};

camera_data init(const vec3& position, float fov);
mat4 transform(const camera_data& camera);

};