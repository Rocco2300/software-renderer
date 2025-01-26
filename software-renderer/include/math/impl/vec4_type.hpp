#pragma once

#include "vec_type.hpp"

template <typename T>
struct vec<T, 4> {
    typedef T type;
    static const size_t size = 4;

    union {
        storage<T, 4> data{};
        struct {
            union {T x, r, u; };
            union {T y, g, v; };
            union {T z, b, s; };
            union {T w, a, t; };
        };
    };

    vec() = default;
    vec(const T& value);
    vec(const T& x, const T& y, const T& z, const T& w);
    vec(const __m128& reg);
    vec(const vec<T, 2>& other, T z = 0, T w = 0);
    vec(const vec<T, 3>& other, T w = 0);
    vec(const vec<T, 4>& other);

    vec<T, 4>& operator=(const vec<T, 4>& other);

    operator T*();
    operator __m128() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    vec<T, 4>& operator+=(const vec<T, 4>& v);
    vec<T, 4>& operator-=(const vec<T, 4>& v);
    vec<T, 4>& operator*=(float s);
    vec<T, 4>& operator/=(float s);

    template <typename V>
    friend bool operator==(const vec<V, 4>& u, const vec<V, 4>& v);
    template <typename V>
    friend bool operator!=(const vec<V, 4>& u, const vec<V, 4>& v);

    template <typename V>
    friend vec<V, 4> operator+(const vec<V, 4>& u, const vec<V, 4>& v);
    template <typename V>
    friend vec<V, 4> operator-(const vec<V, 4>& u, const vec<V, 4>& v);
    template <typename V>
    friend vec<V, 4> operator*(const float& s, const vec<V, 4>& v);
    template <typename V>
    friend vec<V, 4> operator*(const vec<V, 4>& v, const float& s);
    template <typename V>
    friend vec<V, 4> operator/(const vec<V, 4>& v, const float& s);
};

#include "vec4_type.inl"
