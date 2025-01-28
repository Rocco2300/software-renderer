#include "texture.hpp"

static int index(int x, int y, size_t width) { return y * width + x; }

namespace sfr::texture {

texture_data create(size_t width, size_t height, type textureType) {
    texture_data ret;
    ret.type   = textureType;
    ret.width  = width;
    ret.height = height;

    if (textureType == Color) {
        auto* data = new color[width * height];
        for (int i = 0; i < width * height; i++) {
            data[i] = color{};
        }
        ret.data = data;
    } else {
        auto* data = new vec3[width * height];
        for (int i = 0; i < width * height; i++) {
            data[i] = vec3{1.f};
        }
        ret.data = data;
    }
    return ret;
}

void destroy(texture_data& tex) {
    if (tex.type == Color) {
        auto* data = static_cast<color*>(tex.data);
        delete[] data;

        tex.data = nullptr;
    } else {
        auto* data = static_cast<vec3*>(tex.data);
        delete[] data;

        tex.data = nullptr;
    }
}

void clear(texture_data& tex, const vec3& col) {
    assert(tex.type == Depth);

    auto* data = static_cast<vec3*>(tex.data);
    for (int i = 0; i < tex.width * tex.height; i++) {
        data[i] = col;
    }
}

void clear(texture_data& tex, const color& col) {
    assert(tex.type == Color);

    auto* data = static_cast<color*>(tex.data);
    for (int i = 0; i < tex.width * tex.height; i++) {
        data[i] = col;
    }
}

float getDepth(texture_data& tex, int x, int y) {
    assert(tex.type == Depth);

    auto idx   = index(x, y, tex.width);
    auto* data = static_cast<vec3*>(tex.data);
    return data[idx].r;
}

color* getPixel(texture_data& tex, int x, int y) {
    assert(tex.type == Color);

    auto idx   = index(x, y, tex.width);
    auto* data = static_cast<color*>(tex.data);
    return &data[idx];
}

void setPixel(texture_data& tex, int x, int y, const vec3& col) {
    assert(tex.type == Depth);

    auto idx   = index(x, y, tex.width);
    auto* data = static_cast<vec3*>(tex.data);
    data[idx]  = col;
}

void setPixel(texture_data& tex, int x, int y, const color& col) {
    assert(tex.type == Color);

    auto idx   = index(x, y, tex.width);
    auto* data = static_cast<color*>(tex.data);
    data[idx]  = col;
}

};// namespace sfr::texture
