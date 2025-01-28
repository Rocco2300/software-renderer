#pragma once

#include "types.hpp"
#include "texture.hpp"

namespace sfr::mesh {
struct mesh_data;
}

namespace sfr::renderer {

struct render_data {
    u32 pbo;
    u32 texture;
    u32 program;
    u32 VAO, VBO;

    texture::texture_data colorBuf;
    texture::texture_data depthBuf;

    int width;
    int height;
};

render_data init(int width, int height);
void begin(render_data& renderer);
void render(render_data& renderer, const mesh::mesh_data& mesh);
void end(render_data& renderer);

};