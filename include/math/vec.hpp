#pragma once

namespace vec {

struct vec2 {
    float x;
    float y;

    //vec2 operator+(vec2 v);
    //vec2 operator-(vec2 v);
    //vec2 operator*(float s);
    //vec2 operator/(float s);

    friend vec2 operator+(vec2 u, vec2 v);
    friend vec2 operator-(vec2 u, vec2 v);
    friend vec2 operator*(float s, vec2 v);
    friend vec2 operator*(vec2 v, float s);
    friend vec2 operator/(vec2 v, float s);
};

float length(vec2 v);
float dot(vec2 u, vec2 v);

vec2 normalize(vec2 v);

};// namespace vec

using vec::vec2;