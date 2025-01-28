#include "window.hpp"

#include "math/vec.hpp"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

constexpr float vertices[] = {-1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f};

static std::string loadShaderSource(const std::string& fileName) {
    std::string path = R"(C:\Users\grigo\Repos\software-renderer\software-renderer\shader\)";
    path += fileName;

    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Cannot open file " << path << '\n';
        return "";
    }

    std::stringstream fileData;
    fileData << in.rdbuf();

    return fileData.str();
}

static u32 compileShader(i32 shaderType, const std::string& source) {
    auto shader            = glCreateShader(shaderType);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    int compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation failed:\n" << log << "\n";
    }

    return shader;
}

static u32 createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource   = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);
    if (vertexSource.empty() || fragmentSource.empty()) {
        return 0;
    }

    auto vertexShader   = compileShader(GL_VERTEX_SHADER, vertexSource);
    auto fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking failed:\n" << log << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

static u32 createTexture(const void* data, int width, int height) {
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

static u32 createPBO(int width, int height) {
    u32 pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 3, nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    return pbo;
}

static void updateTexture(
        unsigned int texture,
        unsigned int pbo,
        int width,
        int height,
        const void* data
) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    void* buffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (buffer) {
        std::memcpy(buffer, data, width * height * 3);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

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

    if (gl3wInit()) {
        std::cerr << "Failed to initialize GL3W\n";
        exit(-1);
    }

    window.program = createShaderProgram("screen.vert", "screen.frag");

    glGenBuffers(1, &window.VBO);
    glGenVertexArrays(1, &window.VAO);

    glBindVertexArray(window.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, window.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    window.colorBuf = texture::create(width, height);
    window.depthBuf = texture::create(width, height, sfr::texture::Depth);
    window.pbo      = createPBO(width, height);
    window.texture  = createTexture(window.colorBuf.data, width, height);

    return window;
}

void destroy(window_data& window) {
    glDeleteVertexArrays(1, &window.VAO);
    glDeleteBuffers(1, &window.VBO);
    glDeleteProgram(window.program);
    glDeleteTextures(1, &window.texture);
    glfwTerminate();
}

void clear(window_data& window, const color& col) {
    texture::clear(window.colorBuf, col);
    texture::clear(window.depthBuf, vec3(1.f));
}

void setPixel(window_data& window, int x, int y, const color& col) {
    assert(x >= 0 && x < window.width);
    assert(y >= 0 && y < window.height);

    texture::setPixel(window.colorBuf, x, y, col);
}

void setDepth(window_data& window, int x, int y, const float& depth) {
    assert(x >= 0 && x < window.width);
    assert(y >= 0 && y < window.height);

    texture::setPixel(window.depthBuf, x, y, vec3(depth));
}

float getDepth(window_data& window, int x, int y) {
    assert(x >= 0 && x < window.width);
    assert(y >= 0 && y < window.height);

    return texture::getDepth(window.depthBuf, x, y);
}

void blitPixels(window_data& window) {
    updateTexture(window.texture, window.pbo, window.width, window.height, window.colorBuf.data);
}

void display(window_data& window) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(window.program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, window.texture);

    glBindVertexArray(window.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window.glfwWindow);
    glfwPollEvents();
}

bool shouldClose(window_data& window) { return glfwWindowShouldClose(window.glfwWindow); }
};// namespace sfr::window