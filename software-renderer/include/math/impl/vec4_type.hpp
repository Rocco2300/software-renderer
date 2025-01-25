#pragma once

#include "vec_type.hpp"

template <>
struct vec<float, 4> {
    union {
        storage<float, 4> data{};
        struct {
            union {float x, r, u; };
            union {float y, g, v; };
            union {float z, b, s; };
            union {float w, a, t; };
        };
    };

    vec() = default;
    vec(const float& value);
    vec(const float& x, const float& y, const float& z, const float& w);
    vec(const __m128& reg);
    vec(const vec<float, 4>& other);

    template <typename T>
    vec(const vec<T, 2>& other, float z = 0, float w = 0);
    template <typename T>
    vec(const vec<T, 3>& other, float w = 0);

    vec<float, 4>& operator=(const vec<float, 4>& other);

    operator float*();
    operator __m128() const;

    float& operator[](size_t index);
    const float& operator[](size_t index) const;

    vec<float, 4>& operator+=(const vec<float, 4>& v);
    vec<float, 4>& operator-=(const vec<float, 4>& v);
    vec<float, 4>& operator*=(float s);
    vec<float, 4>& operator/=(float s);

    friend bool operator==(const vec<float, 4>& u, const vec<float, 4>& v);
    friend bool operator!=(const vec<float, 4>& u, const vec<float, 4>& v);

    friend vec<float, 4> operator+(const vec<float, 4>& u, const vec<float, 4>& v);
    friend vec<float, 4> operator-(const vec<float, 4>& u, const vec<float, 4>& v);
    friend vec<float, 4> operator*(const float& s, const vec<float, 4>& v);
    friend vec<float, 4> operator*(const vec<float, 4>& v, const float& s);
    friend vec<float, 4> operator/(const vec<float, 4>& v, const float& s);
};

#include "vec4_type.inl"
