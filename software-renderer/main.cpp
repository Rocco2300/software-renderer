// clang-format off
#include "window.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "math/transform.hpp"
#include "mesh.hpp"

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
        const std::vector<u32>& indices
) {
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

                auto u = subArea1 / area;
                auto v = subArea2 / area;
                auto w = subArea3 / area;
                auto depth = u * v1.z + v * v2.z + w * v3.z;

                auto above = sfr::window::getDepth(window, p.x, p.y) >= depth;
                auto inBounds = (p.x >= 0 && p.x < WindowWidth && p.y >= 0 && p.y < WindowHeight);
                if (std::abs(area - (subArea1 + subArea2 + subArea3)) < 12.f && inBounds && above) {
                    sfr::window::setPixel(
                            window,
                            p.x,
                            p.y,
                            color
                    );

                    sfr::window::setDepth(window, p.x, p.y, depth);
                }
            }
        }
    }
}

void viewportTransform(
        const logic_space& logicSpace,
        const viewport_space& viewportSpace,
        const std::vector<vec3>& vertices,
        std::vector<vec3>& output
) {
    auto viewportTransform = viewport(logicSpace, viewportSpace);

    for (int i = 0; i < vertices.size(); i++) {
        auto v = vec4(vertices[i], 1);
        output[i] = vec3(viewportTransform * v);
    }
}

void clipSpaceTransform(
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

int main() {
    auto window = sfr::window::init(WindowWidth, WindowHeight);

    auto mesh = sfr::mesh::loadFromFile("C:/Users/grigo/Repos/software-renderer/monkey.obj");

    logic_space logicSpace{-1, -1, 2, 2};
    viewport_space viewportSpace{0, 0, 1280, 720};

    mat4 transformation = mat4(1.f);
    transformation *= perspective(60.f * (M_PI / 180.f), 16.0f / 9.0f, 0.1f, 100.f);
    transformation *= view(vec3(0, 0, 4), vec3(0, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0));
    //transformation *= translate({1.0f, 0.0f, 0.0f});

    //auto clipspaceVerts = clipSpaceTransform(mesh.vertices, transformation);
    //// clip out of bounds triangles
    //auto viewportVerts = viewportTransform(logicSpace, viewportSpace, clipspaceVerts);

    //raster(window, viewportVerts, mesh.indices);
    //sfr::window::blitPixels(window);

    std::vector<vec3> clipspaceVerts(mesh.vertices.size());
    std::vector<vec3> viewportVerts(mesh.vertices.size());
    while (!sfr::window::shouldClose(window)) {
        clipSpaceTransform(mesh.vertices, transformation, clipspaceVerts);
        // clip out of bounds triangles
        viewportTransform(logicSpace, viewportSpace, clipspaceVerts, viewportVerts);

        raster(window, viewportVerts, mesh.indices);
        sfr::window::blitPixels(window);
        sfr::window::display(window);
    }

    sfr::window::destroy(window);
    return 0;
}
