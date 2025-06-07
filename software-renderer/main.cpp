// clang-format off
#include "mesh.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <iostream>

constexpr int WindowWidth  = 1280;
constexpr int WindowHeight = WindowWidth * (9.0f / 16.f);
// clang-format on

int main() {
    auto window = sfr::window::init(WindowWidth, WindowHeight);

    auto mesh = sfr::mesh::loadFromFile("C:/Users/grigo/Repos/software-renderer/monkey.obj");
    std::cout << "Mesh index count: " << mesh.indices.size() << '\n';
    std::cout << "Mesh vertex count: " << mesh.vertices.size() << '\n';
    std::cout << "Mesh normal count: " << mesh.normals.size() << '\n';

    auto camera = sfr::camera::init(vec3(0, 0, 4), 60.f);
    sfr::camera::rotateOY(camera, 45.f);
    sfr::camera::rotateOX(camera, 25.f);

    auto renderer = sfr::renderer::init(WindowWidth, WindowHeight);

    while (!sfr::window::shouldClose(window)) {
        sfr::renderer::begin(renderer);
        sfr::renderer::render(renderer, camera, mesh);
        sfr::renderer::end(renderer);

        sfr::window::display(window);
    }

    sfr::window::destroy(window);
    return 0;
}
