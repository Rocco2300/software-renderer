#pragma once

#include "types.hpp"
#include "texture.hpp"

class GLFWwindow;

namespace sfr::window {
struct window_data {
    u32 pbo;
    u32 texture;
    u32 program;
    u32 VAO, VBO;
    GLFWwindow* glfwWindow;

    texture::texture_data tex;
    int width;
    int height;
};

window_data init(int width, int height);
void destroy(window_data& window);

void clear(window_data& window, const color& col);
void setPixel(window_data& window, int x, int y, const color& col);
void blitPixels(window_data& window);

void display(window_data& window);
bool shouldClose(window_data& window);

};// namespace sfr::window
