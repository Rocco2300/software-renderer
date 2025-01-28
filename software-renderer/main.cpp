// clang-format off
#include "mesh.hpp"
#include "renderer.hpp"
#include "window.hpp"

constexpr int WindowWidth  = 1280;
constexpr int WindowHeight = WindowWidth * (9.0f / 16.f);
// clang-format on

int main() {
    auto window = sfr::window::init(WindowWidth, WindowHeight);

    auto mesh = sfr::mesh::loadFromFile("C:/Users/grigo/Repos/software-renderer/monkey.obj");

    auto renderer = sfr::renderer::init(WindowWidth, WindowHeight);

    while (!sfr::window::shouldClose(window)) {
        sfr::renderer::begin(renderer);
        sfr::renderer::render(renderer, mesh);
        sfr::renderer::end(renderer);

        sfr::window::display(window);
    }

    sfr::window::destroy(window);
    return 0;
}
