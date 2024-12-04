#pragma once

#include "vec.hpp"

template <typename T, size_t Size>
class mat {
private:
    T data[4][4]{};

public:
    mat() = default;
    mat(const T& diag);
    mat(const T (&data)[4][4]);

    T* operator[](size_t index);
    const T* operator[](size_t index) const;

    template <typename V, size_t VSize>
    friend mat<V, VSize> operator+(const mat<V, VSize>& a, const mat<V, VSize>& b);
    template <typename V, size_t VSize>
    friend mat<V, VSize> operator-(const mat<V, VSize>& a, const mat<V, VSize>& b);
    template <typename V, size_t VSize>
    friend mat<V, VSize> operator*(const mat<V, VSize>& a, const mat<V, VSize>& b);
    template <typename V, size_t VSize>
    friend vec<V, VSize> operator*(const mat<V, VSize>& m, const vec<V, VSize>& v);
};

#include "mat.inl"