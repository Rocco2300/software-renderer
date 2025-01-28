#pragma once

#include "types.hpp"
#include "texture.hpp"

class GLFWwindow;

namespace sfr::window {
struct window_data {
    GLFWwindow* glfwWindow;

    int width;
    int height;
};

window_data init(int width, int height);
void destroy(window_data& window);

void display(window_data& window);
bool shouldClose(window_data& window);

};// namespace sfr::window
