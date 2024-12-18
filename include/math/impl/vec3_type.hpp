#pragma once

#include "vec_type.hpp"

template <typename T>
struct vec<T, 3> {
    union {
        storage<T, 3> data{};
        struct {
            union {T x, r, u; };
            union {T y, g, v; };
            union {T z, b, s; };
        };
    };

    vec() = default;
    vec(const T& value);
    vec(const T& x, const T& y, const T& z);
    vec(const __m128& reg);
    vec(const vec<T, 3>& other);

    vec<T, 3>& operator=(const vec<T, 3>& other);

    operator T*();
    operator __m128() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    vec<T, 3>& operator+=(const vec<T, 3>& v);
    vec<T, 3>& operator-=(const vec<T, 3>& v);
    vec<T, 3>& operator*=(float s);
    vec<T, 3>& operator/=(float s);

    template <typename V>
    friend bool operator==(const vec<V, 3>& u, const vec<V, 3>& v);
    template <typename V>
    friend bool operator!=(const vec<V, 3>& u, const vec<V, 3>& v);

    template <typename V>
    friend vec<V, 3> operator+(const vec<V, 3>& u, const vec<V, 3>& v);
    template <typename V>
    friend vec<V, 3> operator-(const vec<V, 3>& u, const vec<V, 3>& v);
    template <typename V>
    friend vec<V, 3> operator*(const float& s, const vec<V, 3>& v);
    template <typename V>
    friend vec<V, 3> operator*(const vec<V, 3>& v, const float& s);
    template <typename V>
    friend vec<V, 3> operator/(const vec<V, 3>& v, const float& s);
};

#include "vec3_type.inl"