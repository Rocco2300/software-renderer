#include "window.hpp"

#include <GLFW/glfw3.h>

#include <cassert>
#include <iostream>

namespace sfr::window {

window_data init(int width, int height) {
    window_data window;
    window.width  = width;
    window.height = height;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        exit(-1);
    }

    GLFWwindow* glfwWindow = glfwCreateWindow(width, height, "Software Renderer", nullptr, nullptr);
    if (!glfwWindow) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(-1);
    }

    window.glfwWindow = glfwWindow;
    glfwMakeContextCurrent(glfwWindow);

    return window;
}

void destroy(window_data& window) {
    glfwTerminate();
}

void display(window_data& window) {
    glfwSwapBuffers(window.glfwWindow);
    glfwPollEvents();
}

bool shouldClose(window_data& window) { return glfwWindowShouldClose(window.glfwWindow); }
};// namespace sfr::window