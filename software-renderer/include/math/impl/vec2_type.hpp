#pragma once

#include "vec_type.hpp"

template <typename T>
struct vec<T, 2> {
    typedef T type;
    static const size_t size = 2;

    union {
        storage<T, 2> data{};
        struct {
            union {T x, r, u; };
            union {T y, g, v; };
        };
    };

    vec() = default;
    vec(const T& value);
    vec(const T& x, const T& y);
    vec(const __m128& reg);
    vec(const vec<T, 2>& other);
    vec(const vec<T, 3>& other);
    vec(const vec<T, 4>& other);

    vec<T, 2>& operator=(const vec<T, 2>& other);

    operator T*();
    operator __m128() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    vec<T, 2>& operator+=(const vec<T, 2>& v);
    vec<T, 2>& operator-=(const vec<T, 2>& v);
    vec<T, 2>& operator*=(float s);
    vec<T, 2>& operator/=(float s);

    template <typename V>
    friend bool operator==(const vec<V, 2>& u, const vec<V, 2>& v);
    template <typename V>
    friend bool operator!=(const vec<V, 2>& u, const vec<V, 2>& v);

    template <typename V>
    friend vec<V, 2> operator+(const vec<V, 2>& u, const vec<V, 2>& v);
    template <typename V>
    friend vec<V, 2> operator-(const vec<V, 2>& u, const vec<V, 2>& v);
    template <typename V>
    friend vec<V, 2> operator*(const float& s, const vec<V, 2>& v);
    template <typename V>
    friend vec<V, 2> operator*(const vec<V, 2>& v, const float& s);
    template <typename V>
    friend vec<V, 2> operator/(const vec<V, 2>& v, const float& s);
};

#include "vec2_type.inl"