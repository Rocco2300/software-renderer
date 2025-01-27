// clang-format off
#include "window.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "math/transform.hpp"

#include <fast_obj.h>

#include <vector>

constexpr int WindowWidth  = 1280;
constexpr int WindowHeight = WindowWidth * (9.0f / 16.f);

const std::vector<vec3> triangleVertices = {
    {-0.5, -0.5, 0},
    {0.5, -0.5, 0},
    {0.5, 0.5, 0},
    {-0.5, 0.5, 0}
};

const std::vector<int> triangleIndices = {
    0, 1, 2,
    2, 3, 0
};

const std::vector<color> triangleColors = {
    {255, 0, 0},
    {0, 255, 0},
    {0, 0, 255}
};
// clang-format on

float triangleArea(const vec2& v1, const vec2& v2, const vec2& v3) {
    auto mat = mat3(v1, v2, v3);
    return 0.5f * std::abs(det(mat));
}

void raster(
        sfr::window::window_data& window,
        const std::vector<vec3>& vertices,
        const std::vector<int>& indices
) {
    for (int i = 0; i < indices.size(); i += 3) {
        auto& color = triangleColors[i / 3];
        auto& v1    = vertices[indices[i + 0]];
        auto& v2    = vertices[indices[i + 1]];
        auto& v3    = vertices[indices[i + 2]];
        auto area   = triangleArea(vec2(v1), vec2(v2), vec2(v3));
        auto left   = std::min(std::min(v1.x, v2.x), v3.x);
        auto bottom = std::min(std::min(v1.y, v2.y), v3.y);
        auto top    = std::max(std::max(v1.y, v2.y), v3.y);
        auto right  = std::max(std::max(v1.x, v2.x), v3.x);
        for (int y = bottom; y <= top; y++) {
            for (int x = left; x <= right; x++) {
                auto p        = vec2(x, y);
                auto subArea1 = triangleArea(p, v2, v3);
                auto subArea2 = triangleArea(v1, p, v3);
                auto subArea3 = triangleArea(v1, v2, p);

                auto inBounds = (x >= 0 && x < WindowWidth && y >= 0 && y < WindowHeight);
                if (std::abs(area - (subArea1 + subArea2 + subArea3)) < 12.f && inBounds) {
                    sfr::window::setPixel(
                            window,
                            x,
                            y,
                            color
                    );
                }
            }
        }
    }
}

std::vector<vec3> viewportTransform(
        const logic_space& logicSpace,
        const viewport_space& viewportSpace,
        const std::vector<vec3>& vertices
) {
    auto viewportTransform = viewport(logicSpace, viewportSpace);

    std::vector<vec3> ret(vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        auto v = vec4(vertices[i].x, vertices[i].y, 1, 1);
        ret[i] = vec3(viewportTransform * v);
    }

    return ret;
}

std::vector<vec3> clipSpaceTransform(
        const std::vector<vec3>& vertices,
        const mat4& transformation
) {
    std::vector<vec3> ret(vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        auto v = transformation * vec4(vertices[i], 1);
        v /= v.w;

        ret[i] = vec3(v);
    }
    return ret;
}

int main() {
    auto window = sfr::window::init(WindowWidth, WindowHeight);

    logic_space logicSpace{-1, -1, 2, 2};
    viewport_space viewportSpace{0, 0, 1280, 720};

    mat4 transformation = mat4(1.f);
    transformation *= perspective(60.f * (M_PI / 180.f), 16.0f / 9.0f, 0.1f, 100.f);
    transformation *= view(vec3(0, 0, 2), vec3(0, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0));
    //transformation *= translate({1.0f, 0.0f, 0.0f});

    auto clipspaceVerts = clipSpaceTransform(triangleVertices, transformation);
    // clip out of bounds triangles
    auto viewportVerts = viewportTransform(logicSpace, viewportSpace, clipspaceVerts);

    raster(window, viewportVerts, triangleIndices);
    sfr::window::blitPixels(window);

    while (!sfr::window::shouldClose(window)) {
        sfr::window::blitPixels(window);
        sfr::window::display(window);
    }

    sfr::window::destroy(window);
    return 0;
}
