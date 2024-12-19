#pragma once

#include "storage.hpp"

template <typename T, size_t Size>
struct vec;

template <typename T, size_t Size>
class mat {
private:
    //T data[4][4]{};
    storage<T, Size> data[Size];

public:
    mat() = default;
    mat(const T& diag);

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

#include "mat_type.inl"