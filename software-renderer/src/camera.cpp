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

mat4 transform(const camera_data& camera) {
    mat4 transformation = mat4(1.f);
    transformation *= perspective(deg2Rad(camera.fov), 16.0f / 9.0f, 0.1f, 100.f);
    transformation *= view(camera.position, camera.forward, camera.right, camera.up);
    return transformation;
}

}