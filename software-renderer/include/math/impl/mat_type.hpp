#pragma once

#include "storage.hpp"

template <typename T, size_t Size>
struct vec;

template <typename T, size_t Size>
struct mat {
    //T data[4][4]{};
    storage<T, Size> data[Size];

    mat() = default;
    mat(const T& diag);
    mat(const vec<T, 2>& v1, const vec<T, 2>& v2, const vec<T, 2>& v3);

    storage<T, Size>& operator[](size_t index);
    const storage<T, Size>& operator[](size_t index) const;

    mat<T, Size>& operator*=(const mat<T, Size>& other);

    template <typename V, size_t VSize>
    friend mat<V, VSize> operator+(const mat<V, VSize>& a, const mat<V, VSize>& b);
    template <typename V, size_t VSize>
    friend mat<V, VSize> operator-(const mat<V, VSize>& a, const mat<V, VSize>& b);
    template <typename V, size_t VSize>
    friend mat<V, VSize> operator*(const mat<V, VSize>& a, const mat<V, VSize>& b);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator*(const mat<V, VSize>& m, const vec<V, VSize>& v);
};

template <typename T>
T det(mat<T, 3> m);

#include "mat_type.inl"