// clang-format off
#include "mesh.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include "glfw/glfw3.h"

#include <iostream>

constexpr int WindowWidth  = 1280;
constexpr int WindowHeight = WindowWidth * (9.0f / 16.f);
// clang-format on

void updateCamera(GLFWwindow* window, sfr::camera::camera_data& camera, double deltaTime) {
    auto status = glfwGetKey(window, GLFW_KEY_UP);
    if (status == GLFW_PRESS) {
        sfr::camera::rotateOX(camera, 45.f * deltaTime);
    }
    status = glfwGetKey(window, GLFW_KEY_DOWN);
    if (status == GLFW_PRESS) {
        sfr::camera::rotateOX(camera, -45.f * deltaTime);
    }
    status = glfwGetKey(window, GLFW_KEY_RIGHT);
    if (status == GLFW_PRESS) {
        sfr::camera::rotateOY(camera, -45.f * deltaTime);
    }
    status = glfwGetKey(window, GLFW_KEY_LEFT);
    if (status == GLFW_PRESS) {
        sfr::camera::rotateOY(camera, 45.f * deltaTime);
    }

    status = glfwGetKey(window, GLFW_KEY_W);
    if (status == GLFW_PRESS) {
        auto forward = sfr::camera::getForward(camera);
        auto position = sfr::camera::getPosition(camera);
        auto newPosition = position + forward * -5.f * deltaTime;

        sfr::camera::setPosition(camera, newPosition);
    }
    status = glfwGetKey(window, GLFW_KEY_S);
    if (status == GLFW_PRESS) {
        auto forward = sfr::camera::getForward(camera);
        auto position = sfr::camera::getPosition(camera);
        auto newPosition = position + forward * 5.f * deltaTime;

        sfr::camera::setPosition(camera, newPosition);
    }
    status = glfwGetKey(window, GLFW_KEY_A);
    if (status == GLFW_PRESS) {
        auto right = sfr::camera::getRight(camera);
        auto position = sfr::camera::getPosition(camera);
        auto newPosition = position + right * -5.f * deltaTime;

        sfr::camera::setPosition(camera, newPosition);
    }
    status = glfwGetKey(window, GLFW_KEY_D);
    if (status == GLFW_PRESS) {
        auto right = sfr::camera::getRight(camera);
        auto position = sfr::camera::getPosition(camera);
        auto newPosition = position + right * 5.f * deltaTime;

        sfr::camera::setPosition(camera, newPosition);
    }
}

int main() {
    auto window = sfr::window::init(WindowWidth, WindowHeight);

    auto mesh = sfr::mesh::loadFromFile("C:/Users/grigo/Repos/software-renderer/monkey.obj");
    std::cout << "Mesh index count: " << mesh.indices.size() << '\n';
    std::cout << "Mesh vertex count: " << mesh.vertices.size() << '\n';
    std::cout << "Mesh normal count: " << mesh.normals.size() << '\n';

    auto camera = sfr::camera::init(vec3(0, 0, 4), 60.f);
    auto renderer = sfr::renderer::init(WindowWidth, WindowHeight);

    double deltaTime;
    auto currentFrame = glfwGetTime();
    auto lastFrame    = currentFrame;
    while (!sfr::window::shouldClose(window)) {
        currentFrame = glfwGetTime();
        deltaTime    = currentFrame - lastFrame;
        lastFrame    = currentFrame;

        updateCamera(window.glfwWindow, camera, deltaTime);

        sfr::renderer::begin(renderer);
        sfr::renderer::render(renderer, camera, mesh);
        sfr::renderer::end(renderer);

        sfr::window::display(window);
    }

    sfr::window::destroy(window);
    return 0;
}
