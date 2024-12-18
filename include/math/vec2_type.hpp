#pragma once

#include "vec_type.hpp"

template <typename T>
struct vec<T, 2> {
    union {
        storage<T, 2> data{};
        struct {
            union {T x, r; };
            union {T y, g; };
        };
    };

    vec() = default;
    vec(const T& value);
    vec(const T& x, const T& y);
    vec(const __m128& reg);

    operator T*();
    operator __m128() const;

    //T* data();
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    //vec2 operator+(vec2 v);
    //vec2 operator-(vec2 v);
    //vec2 operator*(float s);
    //vec2 operator/(float s);

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

using vec2 = vec<float, 2>;

#include "vec2_type.inl"