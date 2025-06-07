#include "camera.hpp"

#include "math/util.hpp"
#include "math/transform.hpp"

namespace sfr::camera {

camera_data init(const vec3& position, float fov) {
    camera_data ret;
    ret.position = position;
    ret.up = vec3(0, 1, 0);
    ret.forward = vec3(0, 0, 1);
    ret.right = vec3(1, 0, 0);
    ret.fov = fov;

    return ret;
}

mat4 getTransform(const camera_data& camera) {
    mat4 transformation = mat4(1.f);
    transformation *= transform::perspective(deg2Rad(camera.fov), 16.0f / 9.0f, 0.1f, 100.f);
    transformation *= transform::view(camera.position, camera.forward, camera.right, camera.up);
    return transformation;
}

void rotateOX(camera_data& camera, float angle) {
    auto rotation = transform::rotate(camera.right, angle);

    auto newUp = rotation * vec4(camera.up);
    auto newForward = rotation * vec4(camera.forward);

    camera.up = vec3(newUp);
    camera.forward = vec3(newForward);
}

void rotateOY(camera_data& camera, float angle) {
    auto rotation = transform::rotate(camera.up, angle);

    auto newRight = rotation * vec4(camera.right);
    auto newForward = rotation * vec4(camera.forward);

    camera.right = vec3(newRight);
    camera.forward = vec3(newForward);
}

void rotateOZ(camera_data& camera, float angle) {
    auto rotation = transform::rotate(camera.forward, angle);

    auto newUp = rotation * vec4(camera.up);
    auto newRight = rotation * vec4(camera.right);

    camera.up = vec3(newUp);
    camera.right = vec3(newRight);
}

}