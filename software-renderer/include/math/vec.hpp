#pragma once

#include "../types.hpp"

#include "impl/vec2_type.hpp"
#include "impl/vec3_type.hpp"
#include "impl/vec4_type.hpp"

using vec2 = vec<float, 2>;
using vec3 = vec<float, 3>;
using vec4 = vec<float, 4>;

using ivec2 = vec<i32, 2>;
using ivec3 = vec<i32, 3>;
using ivec4 = vec<i32, 4>;

using uvec2 = vec<u32, 2>;
using uvec3 = vec<u32, 3>;
using uvec4 = vec<u32, 4>;

using color = vec3;
using ucolor = vec<u8, 3>;