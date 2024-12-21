// clang-format off
#include "window.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "math/transform.hpp"

#include <vector>

constexpr int WindowWidth  = 1280;
constexpr int WindowHeight = WindowWidth * (9.0f / 16.f);

const std::vector<vec2> triangleVertices = {
    {6.0f, 3.0f},
    {10.f, 3.0f},
    {8.f, 7.f},
    {12.f, 7.f},
    {4.f, 7.f}
};

const std::vector<int> triangleIndices = {
    0, 1, 2,
    1, 2, 3,
    0, 2, 4
};

const std::vector<u8> triangleColors = {
    255, 0, 0,
    0, 255, 0,
    0, 0, 255
};
// clang-format on

float triangleArea(const vec2& v1, const vec2& v2, const vec2& v3) {
    auto mat = mat3(v1, v2, v3);
    return 0.5f * std::abs(det(mat));
}

void raster(
        sfr::window::window_data& window,
        const std::vector<vec2>& vertices,
        const std::vector<int>& indices
) {
    for (int i = 0; i < indices.size(); i += 3) {
        auto& v1    = vertices[indices[i + 0]];
        auto& v2    = vertices[indices[i + 1]];
        auto& v3    = vertices[indices[i + 2]];
        auto area   = triangleArea(v1, v2, v3);
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

                if (std::abs(area - (subArea1 + subArea2 + subArea3)) < 0.001f) {
                    sfr::window::setPixel(
                            window,
                            x,
                            y,
                            triangleColors[i + 0],
                            triangleColors[i + 1],
                            triangleColors[i + 2]
                    );
                }
            }
        }
    }
}

std::vector<vec2> viewportTransform(
        const logic_space& logicSpace,
        const viewport_space& viewportSpace,
        const std::vector<vec2>& vertices
) {
    auto viewportTransform = viewport(logicSpace, viewportSpace);

    std::vector<vec2> ret(vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        auto v = vec4(vertices[i]);
        ret[i] = vec2(viewportTransform * v);
    }

    return ret;
}

int main() {
    auto window = sfr::window::init(WindowWidth, WindowHeight);

    logic_space logicSpace{0, 0, 16, 9};
    viewport_space viewportSpace{0, 0, 1280, 720};

    auto newVerts = viewportTransform(logicSpace, viewportSpace, triangleVertices);

    raster(window, newVerts, triangleIndices);
    sfr::window::blitPixels(window);

    while (!sfr::window::shouldClose(window)) {
        sfr::window::blitPixels(window);
        sfr::window::display(window);
    }

    sfr::window::destroy(window);
    return 0;
}
