#pragma once

#include "types.hpp"
#include "math/vec.hpp"

using color = vec<u8, 3>;

namespace sfr::texture {

enum type {
    Color = 0,
    Depth
};

struct texture_data {
    type type;

    size_t width;
    size_t height;
    void* data;
};

texture_data create(size_t width, size_t height, type textureType = Color);
void destroy(texture_data& tex);

void clear(texture_data& tex, const vec3& col);
void clear(texture_data& tex, const color& col);

float getDepth(texture_data& tex, int x, int y);
color* getPixel(texture_data& tex, int x, int y);
void setPixel(texture_data& tex, int x, int y, const vec3& col);
void setPixel(texture_data& tex, int x, int y, const color& col);

};