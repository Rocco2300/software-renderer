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
mat4 getTransform(const camera_data& camera);

vec3 getUp(const camera_data& camera);
vec3 getRight(const camera_data& camera);
vec3 getForward(const camera_data& camera);

vec3 getPosition(const camera_data& camera);
void setPosition(camera_data& camera, const vec3& position);

void rotateOX(camera_data& camera, float angle);
void rotateOY(camera_data& camera, float angle);
void rotateOZ(camera_data& camera, float angle);

};