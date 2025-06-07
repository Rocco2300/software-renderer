#include "renderer.hpp"

#include <GL/gl3w.h>

#include "math/mat.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"
#include "math/util.hpp"
#include "mesh.hpp"
#include "camera.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
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

static u8 encodeNormal(float n) { return (u8) ((n * 0.5f + 0.5f) * 255.f); }

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

static triangles clipTriangle(volume_side side, const triangles& tri) {
    auto d0 = signedDistance(side, tri.v[0]);
    auto d1 = signedDistance(side, tri.v[1]);
    auto d2 = signedDistance(side, tri.v[2]);

    // clang-format off
    auto allPositive = [&]() { return d0 >= 0 && d1 >= 0 && d2 >= 0; };
    auto allNegative = [&]() { return d0 < 0 && d1 < 0 && d2 < 2; };
    auto onePositive = [&]() {
        return (d0 >= 0 && d1 < 0 && d2 < 0) ||
               (d0 < 0 && d1 >= 0 && d2 < 0) ||
               (d0 < 0 && d1 < 0 && d2 >= 0);
    };
    // clang-format on

    if (allPositive()) {
        return tri;
    } else if (allNegative()) {
        return triangles{};
    } else if (onePositive()) {
        if (d0 >= 0) {
            auto v1          = intersection(side, tri.v[0], tri.v[1]);
            auto v2          = intersection(side, tri.v[0], tri.v[2]);
            triangles newTri = tri;
            newTri.v[1]      = v1;
            newTri.v[2]      = v2;
            return newTri;
        } else if (d1 >= 0) {
            auto v0          = intersection(side, tri.v[1], tri.v[0]);
            auto v2          = intersection(side, tri.v[1], tri.v[2]);
            triangles newTri = tri;
            newTri.v[0]      = v0;
            newTri.v[2]      = v2;
            return newTri;
        } else {
            auto v0          = intersection(side, tri.v[2], tri.v[0]);
            auto v1          = intersection(side, tri.v[2], tri.v[1]);
            triangles newTri = tri;
            newTri.v[0]      = v0;
            newTri.v[1]      = v1;
            return newTri;
        }
    } else {
        if (d0 < 0) {
            auto v1  = intersection(side, tri.v[0], tri.v[1]);
            auto v2  = intersection(side, tri.v[0], tri.v[2]);
            auto idx = tri.i[0];

            triangles newTri;
            newTri.v.insert(newTri.v.end(), {tri.v[1], tri.v[2], v1, v2});
            newTri.i.insert(newTri.i.end(), {idx, idx + 1, idx + 3});
            newTri.i.insert(newTri.i.end(), {idx + 3, idx + 2, idx});
            return newTri;
        } else if (d1 < 0) {
            auto v0  = intersection(side, tri.v[1], tri.v[0]);
            auto v2  = intersection(side, tri.v[1], tri.v[2]);
            auto idx = tri.i[0];

            triangles newTri;
            newTri.v.insert(newTri.v.end(), {tri.v[0], tri.v[2], v0, v2});
            newTri.i.insert(newTri.i.end(), {idx, idx + 2, idx + 1});
            newTri.i.insert(newTri.i.end(), {idx + 2, idx + 3, idx + 1});
            return newTri;
        } else {
            auto v0  = intersection(side, tri.v[2], tri.v[0]);
            auto v1  = intersection(side, tri.v[2], tri.v[1]);
            auto idx = tri.i[0];

            triangles newTri;
            newTri.v.insert(newTri.v.end(), {tri.v[0], tri.v[1], v0, v1});
            newTri.i.insert(newTri.i.end(), {idx, idx + 1, idx + 3});
            newTri.i.insert(newTri.i.end(), {idx + 3, idx + 2, idx});
            return newTri;
        }
    }
}

static void clipTriangles(
        volume_side side,
        const std::vector<u32>& inputIndices,
        const std::vector<vec3>& inputVertices,
        std::vector<u32> outputIndices,
        std::vector<vec3>& outputVertices
) {
    outputIndices.clear();
    outputVertices.clear();

    for (int i = 0; i < inputIndices.size(); i += 3) {
        triangles tri;
        tri.v.resize(3);
        tri.i.resize(3);

        tri.i[0] = i + 0;
        tri.i[1] = i + 1;
        tri.i[2] = i + 2;

        tri.v[0] = inputVertices[inputIndices[i + 0]];
        tri.v[1] = inputVertices[inputIndices[i + 1]];
        tri.v[2] = inputVertices[inputIndices[i + 2]];

        auto newTris = clipTriangle(side, tri);
        outputIndices.insert(outputIndices.end(), newTris.i.begin(), newTris.i.end());
        outputVertices.insert(outputVertices.end(), newTris.v.begin(), newTris.v.end());
    }
}

static void clipScene(
        const std::vector<u32> inputIndices,
        const std::vector<vec3> inputVertices,
        std::vector<u32> outputIndices,
        std::vector<vec3>& outputVertices
) {
    auto indices  = inputIndices;
    auto vertices = inputVertices;
    indices.resize(indices.size() * 2);
    vertices.resize(vertices.size() * 2);
    outputIndices.resize(indices.size() * 2);
    outputVertices.resize(vertices.size() * 2);

    for (int i = 0; i < 1; i++) {
        auto side = static_cast<volume_side>(i);
        clipTriangles(side, indices, vertices, outputIndices, outputVertices);
    }
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

    auto camera = camera::init(vec3(0, 0, 4), 60.f);

    std::vector<vec3> transformedVertices(mesh.vertices.size());

    //std::vector<u32> clippedIndices;
    //std::vector<vec3> clippedVertices;

    clipSpaceTransform(mesh.vertices, transform(camera), transformedVertices);
    // TODO: make work
    //clipScene(mesh.indices, transformedVertices, clippedIndices, clippedVertices);
    viewportTransform(logicSpace, viewportSpace, transformedVertices, transformedVertices);

    raster(renderer, mesh.indices, transformedVertices, mesh.normals);
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
        const std::vector<u32>& indices,
        const std::vector<vec3>& vertices,
        const std::vector<vec3>& normals
) {
    auto width  = renderer.width;
    auto height = renderer.height;
    for (int i = 0; i < indices.size(); i += 3) {
        //auto& color = triangleColors[(i / 3) % 3];
        auto& v1    = vertices[indices[i + 0]];
        auto& v2    = vertices[indices[i + 1]];
        auto& v3    = vertices[indices[i + 2]];
        auto& v1N   = normals[indices[i + 0]];
        auto& v2N   = normals[indices[i + 1]];
        auto& v3N   = normals[indices[i + 2]];
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

                auto fragNormal = u * v1N + v * v2N + w * v3N;
                auto col =
                        color(encodeNormal(fragNormal.x),
                              encodeNormal(fragNormal.y),
                              encodeNormal(fragNormal.z));
                if (insideTriangle && inBounds && above) {
                    sfr::texture::setPixel(renderer.colorBuf, p.x, p.y, col);
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