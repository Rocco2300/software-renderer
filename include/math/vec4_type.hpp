#pragma once

#include "vec_type.hpp"

template <typename T>
struct vec<T, 4> {
    union {
        storage<T, 4> data{};
        struct {
            union {T x, r; };
            union {T y, g; };
            union {T z, b; };
            union {T w, a; };
        };
    };

    vec() = default;
    vec(const T& value);
    vec(const T& x, const T& y, const T& z, const T& w);
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
