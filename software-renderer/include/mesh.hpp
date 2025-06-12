#pragma once

#include "types.hpp"
#include "math/vec.hpp"
#include "math/mat.hpp"

#include <string>
#include <vector>

namespace sfr::mesh {

struct mesh_data {
    std::vector<u32> indices;
    std::vector<vec3> vertices;
    std::vector<vec3> normals;

    mat4 model;
};

mesh_data loadFromFile(const std::string& path);

};