#pragma once

#include "vec_type.hpp"

template <>
struct vec<float, 2> {
    union {
        storage<float, 2> data{};
        struct {
            union {float x, r, u; };
            union {float y, g, v; };
        };
    };

    vec() = default;
    vec(const float& value);
    vec(const float& x, const float& y);
    vec(const __m128& reg);

    vec(const vec<float, 2>& other);
    template <typename T>
    vec(const vec<T, 3>& other);
    template <typename T>
    vec(const vec<T, 4>& other);

    vec<float, 2>& operator=(const vec<float, 2>& other);

    operator float*();
    operator __m128() const;

    float& operator[](size_t index);
    const float& operator[](size_t index) const;

    vec<float, 2>& operator+=(const vec<float, 2>& v);
    vec<float, 2>& operator-=(const vec<float, 2>& v);
    vec<float, 2>& operator*=(float s);
    vec<float, 2>& operator/=(float s);

    friend bool operator==(const vec<float, 2>& u, const vec<float, 2>& v);
    friend bool operator!=(const vec<float, 2>& u, const vec<float, 2>& v);

    friend vec<float, 2> operator+(const vec<float, 2>& u, const vec<float, 2>& v);
    friend vec<float, 2> operator-(const vec<float, 2>& u, const vec<float, 2>& v);
    friend vec<float, 2> operator*(const float& s, const vec<float, 2>& v);
    friend vec<float, 2> operator*(const vec<float, 2>& v, const float& s);
    friend vec<float, 2> operator/(const vec<float, 2>& v, const float& s);
};

#include "vec2_type.inl"