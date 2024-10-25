#include "vec.hpp"

#include <cmath>

namespace vec {

vec2 operator+(vec2 u, vec2 v) {
    vec2 res{};
    res.x = u.x + v.x;
    res.y = u.y + v.y;
    return res;
}

vec2 operator-(vec2 u, vec2 v) {
    vec2 res{};
    res.x = u.x - v.x;
    res.y = u.y - v.y;
    return res;
}

vec2 operator*(float s, vec2 v) {
    vec2 res{};
    res.x = s * v.x;
    res.y = s * v.y;
    return res;
}

vec2 operator*(vec2 v, float s) {
    vec2 res{};
    res.x = s * v.x;
    res.y = s * v.y;
    return res;
}

vec2 operator/(vec2 v, float s) {
    vec2 res{};
    res.x = v.x / s;
    res.y = v.y / s;
    return res;
}

float length(vec2 v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float dot(vec2 u, vec2 v) {
    return (u.x * v.x + u.y * v.y);
}

vec2 normalize(vec2 v) {
    auto len = length(v);
    return v / len;
}

};// namespace vec