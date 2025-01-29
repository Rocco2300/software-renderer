#include "renderer.hpp"

#include <GL/gl3w.h>

#include "math/mat.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"
#include "mesh.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#pragma region OPENGL
constexpr float vertices[] = {-1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f};

static std::string loadShaderSource(const std::string& fileName);
static u32 compileShader(i32 shaderType, const std::string& source);
static u32 createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
static u32 createTexture(const void* data, int width, int height);
static u32 createPBO(int width, int height);
static void updateTexture(
        unsigned int texture,
        unsigned int pbo,
        int width,
        int height,
        const void* data
);
#pragma endregion OPENGL

const std::vector<color> triangleColors = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};

static float triangleArea(const vec2& v1, const vec2& v2, const vec2& v3) {
    auto mat = mat3(v1, v2, v3);
    return 0.5f * std::abs(det(mat));
}

static void clipSpaceTransform(
        const std::vector<vec3>& vertices,
        const mat4& transformation,
        std::vector<vec3>& output
) {
    for (int i = 0; i < vertices.size(); i++) {
        auto v = transformation * vec4(vertices[i], 1);
        v /= v.w;

        output[i] = vec3(v);
    }
}

static void viewportTransform(
        const logic_space& logicSpace,
        const viewport_space& viewportSpace,
        const std::vector<vec3>& vertices,
        std::vector<vec3>& output
) {
    auto viewportTransform = viewport(logicSpace, viewportSpace);

    for (int i = 0; i < vertices.size(); i++) {
        auto v    = vec4(vertices[i], 1);
        output[i] = vec3(viewportTransform * v);
    }
}

namespace sfr::renderer {

static void raster(
        render_data& renderer,
        const std::vector<vec3>& vertices,
        const std::vector<u32>& indices
);

render_data init(int width, int height) {
    render_data renderer;

    if (gl3wInit()) {
        std::cerr << "Failed to initialize GL3W\n";
        exit(-1);
    }

    renderer.program = createShaderProgram("screen.vert", "screen.frag");

    glGenBuffers(1, &renderer.VBO);
    glGenVertexArrays(1, &renderer.VAO);

    glBindVertexArray(renderer.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    renderer.width    = width;
    renderer.height   = height;
    renderer.colorBuf = texture::create(width, height);
    renderer.depthBuf = texture::create(width, height, sfr::texture::Depth);
    renderer.pbo      = createPBO(width, height);
    renderer.texture  = createTexture(renderer.colorBuf.data, width, height);

    return renderer;
}

void destroy(render_data& renderer) {
    glDeleteVertexArrays(1, &renderer.VAO);
    glDeleteBuffers(1, &renderer.VBO);
    glDeleteProgram(renderer.program);
    glDeleteTextures(1, &renderer.texture);
}

void begin(render_data& renderer) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);
}

void render(render_data& renderer, const mesh::mesh_data& mesh) {
    logic_space logicSpace{-1, -1, 2, 2};
    viewport_space viewportSpace{0, 0, 1280, 720};

    mat4 transformation = mat4(1.f);
    transformation *= perspective(60.f * (M_PI / 180.f), 16.0f / 9.0f, 0.1f, 100.f);
    transformation *= view(vec3(0, 0, 4), vec3(0, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0));

    std::vector<vec3> transformedVertices(mesh.vertices.size());

    clipSpaceTransform(mesh.vertices, transformation, transformedVertices);
    // clip out of bounds triangles
    viewportTransform(logicSpace, viewportSpace, transformedVertices, transformedVertices);

    raster(renderer, transformedVertices, mesh.indices);
}

void end(render_data& renderer) {
    updateTexture(
            renderer.texture,
            renderer.pbo,
            renderer.width,
            renderer.height,
            renderer.colorBuf.data
    );

    glUseProgram(renderer.program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer.texture);

    glBindVertexArray(renderer.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void raster(
        render_data& renderer,
        const std::vector<vec3>& vertices,
        const std::vector<u32>& indices
) {
    auto width  = renderer.width;
    auto height = renderer.height;
    for (int i = 0; i < indices.size(); i += 3) {
        auto& color = triangleColors[(i / 3) % 3];
        auto& v1    = vertices[indices[i + 0]];
        auto& v2    = vertices[indices[i + 1]];
        auto& v3    = vertices[indices[i + 2]];
        auto area   = triangleArea(vec2(v1), vec2(v2), vec2(v3));
        auto left   = std::min(std::min(v1.x, v2.x), v3.x);
        auto bottom = std::min(std::min(v1.y, v2.y), v3.y);
        auto top    = std::max(std::max(v1.y, v2.y), v3.y);
        auto right  = std::max(std::max(v1.x, v2.x), v3.x);
        vec2 p{};
        for (p.y = bottom; p.y <= top; p.y++) {
            for (p.x = left; p.x <= right; p.x++) {
                auto subArea1 = triangleArea(p, v2, v3);
                auto subArea2 = triangleArea(v1, p, v3);
                auto subArea3 = triangleArea(v1, v2, p);

                auto u     = subArea1 / area;
                auto v     = subArea2 / area;
                auto w     = subArea3 / area;
                auto depth = u * v1.z + v * v2.z + w * v3.z;

                const float eps     = 12.f;
                auto above          = sfr::texture::getDepth(renderer.depthBuf, p.x, p.y) >= depth;
                auto inBounds       = (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height);
                auto insideTriangle = std::abs(area - (subArea1 + subArea2 + subArea3)) < eps;
                if (insideTriangle && inBounds && above) {
                    sfr::texture::setPixel(renderer.colorBuf, p.x, p.y, color);
                    sfr::texture::setPixel(renderer.depthBuf, p.x, p.y, vec3(depth));
                }
            }
        }
    }
}

};// namespace sfr::renderer

#pragma region OPENGL
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
#pragma endregion OPENGL