#pragma once

#include "types.hpp"
#include "math/vec.hpp"

using color = vec<u8, 3>;

namespace sfr::texture {

struct texture_data {
    size_t width;
    size_t height;
    color* data;
};

texture_data create(size_t width, size_t height);
void destroy(texture_data* tex);

void clear(texture_data* tex, const color& col);
color* getPixel(texture_data* tex, int x, int y);
void setPixel(texture_data* tex, int x, int y, const color& col);

};