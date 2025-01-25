#pragma once

#include "vec_type.hpp"

template <>
struct vec<float, 3> {
    union {
        storage<float, 3> data{};
        struct {
            union {float x, r, u; };
            union {float y, g, v; };
            union {float z, b, s; };
        };
    };

    vec() = default;
    vec(const float& value);
    vec(const float& x, const float& y, const float& z);
    vec(const __m128& reg);

    vec(const vec<float, 3>& other);
    template <typename T>
    vec(const vec<T, 4>& other);

    vec<float, 3>& operator=(const vec<float, 3>& other);

    operator float*();
    operator __m128() const;

    float& operator[](size_t index);
    const float& operator[](size_t index) const;

    vec<float, 3>& operator+=(const vec<float, 3>& v);
    vec<float, 3>& operator-=(const vec<float, 3>& v);
    vec<float, 3>& operator*=(float s);
    vec<float, 3>& operator/=(float s);

    friend bool operator==(const vec<float, 3>& u, const vec<float, 3>& v);
    friend bool operator!=(const vec<float, 3>& u, const vec<float, 3>& v);

    friend vec<float, 3> operator+(const vec<float, 3>& u, const vec<float, 3>& v);
    friend vec<float, 3> operator-(const vec<float, 3>& u, const vec<float, 3>& v);
    friend vec<float, 3> operator*(const float& s, const vec<float, 3>& v);
    friend vec<float, 3> operator*(const vec<float, 3>& v, const float& s);
    friend vec<float, 3> operator/(const vec<float, 3>& v, const float& s);
};

#include "vec3_type.inl"
