#include "renderer.hpp"

#include <GL/gl3w.h>

#include "camera.hpp"
#include "math/mat.hpp"
#include "math/transform.hpp"
#include "math/util.hpp"
#include "math/vec.hpp"
#include "mesh.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <string>

#pragma region OPENGL
constexpr float screen_vertices[] = {-1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f};

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

static float edge(const vec2& v1, const vec2& v2, const vec2& v3) {
    //return ((v3.x - v1.x) * (v2.y - v1.y) - (v3.y - v1.y) * (v2.x - v1.x)); // CCW
    return ((v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x));// CW
}

static void clipSpaceTransform(
        sfr::renderer::thread_pool* threadPool,
        const std::vector<vec3>& vertices,
        const mat4& transformation,
        std::vector<vec3>& output
) {
    for (int i = 0; i < vertices.size(); i++) {
        //threadPool->process([transformation, vertices, &output, i] {
        auto v = transformation * vec4(vertices[i], 1);
        v /= v.w;

        output[i] = vec3(v);
        //});
    }
}

static void viewportTransform(
        sfr::renderer::thread_pool* threadPool,
        const transform::logic_space& logicSpace,
        const transform::viewport_space& viewportSpace,
        const std::vector<vec3>& vertices,
        std::vector<vec3>& output
) {
    auto viewportTransform = viewport(logicSpace, viewportSpace);

    for (int i = 0; i < vertices.size(); i++) {
        //threadPool->process([vertices, &output, i, viewportTransform] {
        auto v    = vec4(vertices[i], 1);
        output[i] = vec3(viewportTransform * v);
        //});
    }
}

static u8 encodeNormal(float n) { return (u8) ((n * 0.5f + 0.5f) * 255.f); }

static bool numberOutOfUnitBounds(float x) { return x < -1.f || x > 1.f; }

static bool pointOutOfCubeBounds(vec3 v) {
    return numberOutOfUnitBounds(v.x) || numberOutOfUnitBounds(v.y) || numberOutOfUnitBounds(v.z);
}

static bool triangleOutOfCubeBounds(const vec3& v1, const vec3& v2, const vec3& v3) {
    return pointOutOfCubeBounds(v1) || pointOutOfCubeBounds(v2) || pointOutOfCubeBounds(v3);
}

enum volume_side { Near = 0, Far, Right, Left, Top, Bottom };

static float signedDistance(volume_side side, const vec3& vertex) {
    float dist{};
    switch (side) {
    case Near:
        dist = vertex.z + 1;
        break;
    case Far:
        dist = -vertex.z + 1;
        break;
    case Right:
        dist = -vertex.x + 1;
        break;
    case Left:
        dist = vertex.x + 1;
        break;
    case Top:
        dist = -vertex.y + 1;
        break;
    case Bottom:
        dist = vertex.y + 1;
        break;
    }
    return dist;
}

struct triangles {
    std::vector<u32> i;
    std::vector<vec3> v;
};

static const std::vector<vec3> clipping_origins =
        {{0, 0, 1}, {0, 0, -1}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}};

static const std::vector<vec3> clipping_normals =
        {{0, 0, -1}, {0, 0, 1}, {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}};

static vec3 intersect(volume_side side, vec3 a, vec3 b) {
    vec3 lineOrigin    = a;
    vec3 lineDirection = b - a;

    vec3 planeOrigin = clipping_origins[side];
    vec3 planeNormal = clipping_normals[side];

    float t = dot((planeOrigin - lineOrigin), planeNormal) / dot(lineDirection, planeOrigin);
    return a + (b - a) * t;
}

static vec3 intersection(volume_side side, vec3 a, vec3 b) {
    float t;
    switch (side) {
    case Near:
        t = (-1 - a.z) / (b.z - a.z);
        break;
    case Far:
        t = (1 - a.z) / (b.z - a.z);
        break;
    case Right:
        t = (1 - a.x) / (b.x - a.x);
        break;
    case Left:
        t = (-1 - a.x) / (b.x - a.x);
        break;
    case Top:
        t = (1 - a.y) / (b.y - a.y);
        break;
    case Bottom:
        t = (-1 - a.y) / (b.y - a.y);
        break;
    }

    return a + (b - a) * t;
}

static void clipTriangles(
        const std::vector<u32>& indices,
        std::vector<vec3>& vertices,
        std::vector<u32>& outputIndices
) {
    // clang-format off
    auto allPositive = [](float d0, float d1, float d2) { return d0 >= 0 && d1 >= 0 && d2 >= 0; };
    auto allNegative = [](float d0, float d1, float d2) { return d0 < 0 && d1 < 0 && d2 < 0; };
    auto onePositive = [](float d0, float d1, float d2) {
        return (d0 >= 0 && d1 < 0 && d2 < 0) ||
               (d0 < 0 && d1 >= 0 && d2 < 0) ||
               (d0 < 0 && d1 < 0 && d2 >= 0);
    };
    // clang-format on
    //std::copy(vertices.begin(), vertices.end(), std::back_inserter(outputVertices));

    std::vector<u32> inputBuffer;
    std::vector<u32> outputBuffer;
    inputBuffer = indices;
    for (int side = 0; side < 6; side++) {
        for (int i = 0; i < inputBuffer.size(); i += 3) {
            auto i1 = inputBuffer[i + 0];
            auto i2 = inputBuffer[i + 1];
            auto i3 = inputBuffer[i + 2];
            auto v1 = vertices[i1];
            auto v2 = vertices[i2];
            auto v3 = vertices[i3];
            auto d1 = signedDistance(static_cast<volume_side>(side), v1);
            auto d2 = signedDistance(static_cast<volume_side>(side), v2);
            auto d3 = signedDistance(static_cast<volume_side>(side), v3);

            //if (triangleOutOfCubeBounds(v1, v2, v3)) {
            //    continue;
            //}

            if (allPositive(d1, d2, d3)) {
                outputBuffer.push_back(i1);
                outputBuffer.push_back(i2);
                outputBuffer.push_back(i3);
            } else if (allNegative(d1, d2, d3)) {
                continue;
            } else if (onePositive(d1, d2, d3)) {
                if (d1 >= 0) {
                    auto v1i = intersection(static_cast<volume_side>(side), v1, v2);
                    auto v2i = intersection(static_cast<volume_side>(side), v3, v1);

                    auto v1iIndex = vertices.size();
                    auto v2iIndex = v1iIndex + 1;
                    vertices.push_back(v1i);
                    vertices.push_back(v2i);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(v2iIndex);
                    outputBuffer.push_back(i1);
                } else if (d2 >= 0) {
                    auto v1i = intersection(static_cast<volume_side>(side), v2, v3);
                    auto v2i = intersection(static_cast<volume_side>(side), v1, v2);

                    auto v1iIndex = vertices.size();
                    auto v2iIndex = v1iIndex + 1;
                    vertices.push_back(v1i);
                    vertices.push_back(v2i);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(v2iIndex);
                    outputBuffer.push_back(i2);
                } else if (d3 >= 0) {
                    auto v1i = intersection(static_cast<volume_side>(side), v3, v1);
                    auto v2i = intersection(static_cast<volume_side>(side), v2, v3);

                    auto v1iIndex = vertices.size();
                    auto v2iIndex = v1iIndex + 1;
                    vertices.push_back(v1i);
                    vertices.push_back(v2i);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(v2iIndex);
                    outputBuffer.push_back(i3);
                }
            } else {
                if (d1 <= 0) {
                    auto v1i = intersection(static_cast<volume_side>(side), v1, v2);
                    auto v2i = intersection(static_cast<volume_side>(side), v3, v1);

                    auto v1iIndex = vertices.size();
                    auto v2iIndex = v1iIndex + 1;
                    vertices.push_back(v1i);
                    vertices.push_back(v2i);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(i2);
                    outputBuffer.push_back(i3);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(i3);
                    outputBuffer.push_back(v2iIndex);
                } else if (d2 <= 0) {
                    auto v1i = intersection(static_cast<volume_side>(side), v2, v3);
                    auto v2i = intersection(static_cast<volume_side>(side), v1, v2);

                    auto v1iIndex = vertices.size();
                    auto v2iIndex = v1iIndex + 1;
                    vertices.push_back(v1i);
                    vertices.push_back(v2i);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(i3);
                    outputBuffer.push_back(i1);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(i1);
                    outputBuffer.push_back(v2iIndex);
                } else if (d3 <= 0) {
                    auto v1i = intersection(static_cast<volume_side>(side), v3, v1);
                    auto v2i = intersection(static_cast<volume_side>(side), v2, v3);

                    auto v1iIndex = vertices.size();
                    auto v2iIndex = v1iIndex + 1;
                    vertices.push_back(v1i);
                    vertices.push_back(v2i);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(i1);
                    outputBuffer.push_back(i2);

                    outputBuffer.push_back(v1iIndex);
                    outputBuffer.push_back(i2);
                    outputBuffer.push_back(v2iIndex);
                }
            }
        }

        std::swap(inputBuffer, outputBuffer);
        outputBuffer.clear();
    }

    outputIndices = std::move(inputBuffer);
}

namespace sfr::renderer {

static void raster(
        render_data& renderer,
        const std::vector<u32>& indices,
        const std::vector<vec3>& vertices,
        const std::vector<vec3>& normals
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    renderer.width      = width;
    renderer.height     = height;
    renderer.colorBuf   = texture::create(width, height);
    renderer.depthBuf   = texture::create(width, height, sfr::texture::Depth);
    renderer.pbo        = createPBO(width, height);
    renderer.texture    = createTexture(renderer.colorBuf.data, width, height);
    renderer.threadPool = new thread_pool(32);

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

    sfr::texture::clear(renderer.colorBuf, color{});
    sfr::texture::clear(renderer.depthBuf, vec3(1.f));
}

void render(render_data& renderer, const camera::camera_data& camera, const mesh::mesh_data& mesh) {
    transform::logic_space logicSpace{-1, -1, 2, 2};
    transform::viewport_space viewportSpace{0, 0, 1280, 720};

    std::vector<vec3> transformedVertices(mesh.vertices.size());

    //std::vector<u32> clippedIndices;
    //std::vector<vec3> clippedVertices;

    clipSpaceTransform(
            renderer.threadPool,
            mesh.vertices,
            getTransform(camera),
            transformedVertices
    );

    std::vector<u32> newIndices;
    std::vector<vec3> newVertices;
    clipTriangles(mesh.indices, transformedVertices, newIndices);

    // TODO: make work
    //clipScene(mesh.indices, transformedVertices, clippedIndices, clippedVertices);
    viewportTransform(
            renderer.threadPool,
            logicSpace,
            viewportSpace,
            transformedVertices,
            transformedVertices
    );

    raster(renderer, newIndices, transformedVertices, mesh.normals);
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

struct triangle_data {
    vec3 vertices[3];
    vec3 normals[3];
    float area{};
};

static void drawPixel(render_data& renderer, triangle_data& triangle, int x, int y) {
    // clang-format off
    vec2 p{x + 0.5f, y + 0.5f};

    auto edge1 = edge(triangle.vertices[1], triangle.vertices[2], p);
    auto edge2 = edge(triangle.vertices[2], triangle.vertices[0], p);
    auto edge3 = edge(triangle.vertices[0], triangle.vertices[1], p);

    auto u     = edge1 / triangle.area;
    auto v     = edge2 / triangle.area;
    auto w     = edge3 / triangle.area;
    auto depth = u * triangle.vertices[0].z + v * triangle.vertices[1].z + w * triangle.vertices[2].z;

    auto shouldDraw = [&]() {
      // check if off-screen
      if (p.x < 0 || p.x >= renderer.width || p.y < 0 || p.y >= renderer.height) {
          return false;
      }
      // check if pixel would be obscured
      if (sfr::texture::getDepth(renderer.depthBuf, p.x, p.y) < depth) {
          return false;
      }
      // check if pixel is inside triangle
      return edge1 >= 0 && edge2 >= 0 && edge3 >= 0;
    };

    auto fragNormal = u * triangle.normals[0] + v * triangle.normals[1] + w * triangle.normals[2];
    // TODO: remove placeholder
    auto col =
            color(encodeNormal(fragNormal.x),
                  encodeNormal(fragNormal.y),
                  encodeNormal(fragNormal.z));
    if (shouldDraw()) {
        auto pixelX = static_cast<int>(std::floor(p.x));
        auto pixelY = static_cast<int>(std::floor(p.y));
        sfr::texture::setPixel(renderer.colorBuf, pixelX, pixelY, col);
        sfr::texture::setPixel(renderer.depthBuf, pixelX, pixelY, vec3(depth));
    }
    // clang-format on
}

static void rasterizeTriangle(render_data& renderer, triangle_data& triangle) {
    // clang-format off
    triangle.area = edge(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);

    auto left   = std::floor(std::min(std::min(triangle.vertices[0].x, triangle.vertices[1].x), triangle.vertices[2].x));
    auto bottom = std::floor(std::min(std::min(triangle.vertices[0].y, triangle.vertices[1].y), triangle.vertices[2].y));
    auto top    = std::floor(std::max(std::max(triangle.vertices[0].y, triangle.vertices[1].y), triangle.vertices[2].y));
    auto right  = std::floor(std::max(std::max(triangle.vertices[0].x, triangle.vertices[1].x), triangle.vertices[2].x));

    int lineCount = top - bottom;
    //renderer.threadPool->barrier(lineCount);
    //auto threadPool = sfr::renderer::thread_pool(lineCount);
    for (int y = bottom; y <= top; y++) {
        //renderer.threadPool->process([&] {
        //threadPool.process([&] {
          for (int x = left; x <= right; x++) {
              drawPixel(renderer, triangle, x, y);
          }
        //});
    }
    // clang-format on
}

static void raster(
        render_data& renderer,
        const std::vector<u32>& indices,
        const std::vector<vec3>& vertices,
        const std::vector<vec3>& normals
) {
    auto width  = renderer.width;
    auto height = renderer.height;

    triangle_data triangle;
    for (int i = 0; i < indices.size(); i += 3) {
        //auto& color = triangleColors[(i / 3) % 3];
        triangle.vertices[0] = vertices[indices[i + 0]];
        triangle.vertices[1] = vertices[indices[i + 1]];
        triangle.vertices[2] = vertices[indices[i + 2]];

        triangle.normals[0] = normals[indices[i + 0]];
        triangle.normals[1] = normals[indices[i + 1]];
        triangle.normals[2] = normals[indices[i + 2]];

        rasterizeTriangle(renderer, triangle);
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