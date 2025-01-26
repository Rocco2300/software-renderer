#include "texture.hpp"

static int index(int x, int y, size_t width) {
    return y * width + x;
}

namespace sfr::texture {

texture_data create(size_t width, size_t height) {
    texture_data ret;
    ret.width = width;
    ret.height = height;
    ret.data = new color[width * height];
    for (int i = 0; i < width * height; i++) {
        ret.data[i] = color{};
    }
    return ret;
}

void destroy(texture_data* tex) {
    delete[] tex->data;
}

void clear(texture_data* tex, const color& col) {
    for (int i = 0; i < tex->width * tex->height; i++) {
        tex->data[i] = col;
    }
}

color* getPixel(texture_data* tex, int x, int y) {
    auto idx = index(x, y, tex->width);
    return &tex->data[idx];
}

void setPixel(texture_data* tex, int x, int y, const color& col) {
    auto idx = index(x, y, tex->width);
    tex->data[idx] = col;
}


};