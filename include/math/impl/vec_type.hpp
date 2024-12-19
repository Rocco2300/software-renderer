#pragma once

#include "storage.hpp"

template <typename T, size_t Size>
struct vec {
    storage<T, Size> data{};

    vec() = default;
    vec(const T& value);
    vec(const T& x, const T& y);
    vec(const T& x, const T& y, const T& z);
    vec(const T& x, const T& y, const T& z, const T& w);
    vec(const __m128& reg);
    vec(const vec<T, Size>& other);

    vec<T, Size>& operator=(const vec<T, Size>& other);

    operator T*();
    operator __m128() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    vec<T, Size>& operator+=(const vec<T, Size>& v);
    vec<T, Size>& operator-=(const vec<T, Size>& v);
    vec<T, Size>& operator*=(float s);
    vec<T, Size>& operator/=(float s);

    template <typename V, size_t VSize>
    friend bool operator==(const vec<V, VSize>& u, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend bool operator!=(const vec<V, VSize>& u, const vec<V, VSize>& v);

    template <typename V, size_t VSize>
    friend vec<V, VSize> operator+(const vec<V, VSize>& u, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator-(const vec<V, VSize>& u, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator*(const float& s, const vec<V, VSize>& v);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator*(const vec<V, VSize>& v, const float& s);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator/(const vec<V, VSize>& v, const float& s);
};

template <typename T, size_t Size>
float length(const vec<T, Size>& v);

template <typename T, size_t Size>
float dot(const vec<T, Size>& u, const vec<T, Size>& v);

template <typename T, size_t Size>
vec<T, Size> normalize(const vec<T, Size>& v);

template <typename T>
struct vec<T, 2>;

template <typename T>
struct vec<T, 3>;

template <typename T>
struct vec<T, 4>;

#include "vec_type.inl"