#pragma once

#include "impl/vec_type.hpp"

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

using mat2 = mat<float, 2>;
using mat3 = mat<float, 3>;
using mat4 = mat<float, 4>;

#include "mat.inl"