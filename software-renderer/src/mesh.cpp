#include "mesh.hpp"

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>

#include <iostream>

namespace sfr::mesh {

mesh_data loadFromFile(const std::string& path) {
    mesh_data data{};

    auto* mesh = fast_obj_read(path.c_str());
    if (!mesh) {
        std::cerr << "Failed to load OBJ file: " << path << '\n';
        return data;
    }

    data.indices.resize(mesh->index_count);
    data.vertices.resize(mesh->position_count);
    data.normals.resize(mesh->normal_count);
    for (int i = 0; i < mesh->index_count; i++) {
        data.indices[i] = mesh->indices[i].p;
    }

    for (int i = 0; i < mesh->position_count; i++) {
        auto* vertex = reinterpret_cast<vec3*>(&mesh->positions[i * 3]);
        data.vertices[i] = *vertex;
    }

    for (int i = 0; i < mesh->normal_count; i++) {
        auto* normal = reinterpret_cast<vec3*>(&mesh->normals[i * 3]);
        data.normals[i] = *normal;
    }

    return data;
}

};