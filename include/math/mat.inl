#include "math/mat.hpp"

#include <smmintrin.h>
#include <iostream>

template <typename T, size_t Size>
mat<T, Size>::mat(const T& diag) {
    data[0][0] = diag;
    data[1][1] = diag;
    data[2][2] = diag;
    data[3][3] = diag;
}

template <typename T, size_t Size>
mat<T, Size>::mat(const T (&data)[4][4]) {
    this->data = data;
    for (int i = 0; i < 4; i++) {
        auto row = _mm_load_ps(data[i]);
        _mm_store_ps(this->data[i]);
    }
}

template <typename T, size_t Size>
T* mat<T, Size>::operator[](size_t index) {
    return data[index];
}

template <typename T, size_t Size>
const T* mat<T, Size>::operator[](size_t index) const {
    return data[index];
}

template <typename T, size_t Size>
mat<T, Size> operator+(const mat<T, Size>& a, const mat<T, Size>& b) {
    mat<T, Size> result;
    for (int i = 0; i < 4; i++) {
        auto aRow = _mm_load_ps(a[i]);
        auto bRow = _mm_load_ps(b[i]);

        _mm_store_ps(result[i], _mm_add_ps(aRow, bRow));
    }
    return result;
}

template <typename T, size_t Size>
mat<T, Size> operator-(const mat<T, Size>& a, const mat<T, Size>& b) {
    mat<T, Size> result;
    for (int i = 0; i < 4; i++) {
        auto aRow = _mm_load_ps(a[i]);
        auto bRow = _mm_load_ps(b[i]);

        _mm_store_ps(result[i], _mm_sub_ps(aRow, bRow));
    }
    return result;
}

template <typename T, size_t Size>
mat<T, Size> operator*(const mat<T, Size>& a, const mat<T, Size>& b) {
    mat<T, Size> result;
    auto row1 = _mm_load_ps(a[0]);
    auto row2 = _mm_load_ps(a[1]);
    auto row3 = _mm_load_ps(a[2]);
    auto row4 = _mm_load_ps(a[3]);
    for (int i = 0; i < 4; i++) {
        auto brod1 = _mm_set1_ps(b[0][i]);
        auto brod2 = _mm_set1_ps(b[1][i]);
        auto brod3 = _mm_set1_ps(b[2][i]);
        auto brod4 = _mm_set1_ps(b[3][i]);

        auto row = _mm_add_ps(
                _mm_add_ps(_mm_mul_ps(brod1, row1), _mm_mul_ps(brod2, row2)),
                _mm_add_ps(_mm_mul_ps(brod3, row3), _mm_mul_ps(brod4, row4))
        );
        _mm_store_ps(result[i], row);
    }
    return result;
}

template <typename T, size_t Size>
vec<T, Size> operator*(const mat<T, Size>& m, const vec<T, Size>& v) {
    vec<T, Size> result;
    auto row1 = _mm_load_ps(m[0]);
    auto row2 = _mm_load_ps(m[1]);
    auto row3 = _mm_load_ps(m[2]);
    auto row4 = _mm_load_ps(m[3]);

    auto brod1 = _mm_set1_ps(v[0]);
    auto brod2 = _mm_set1_ps(v[1]);
    auto brod3 = _mm_set1_ps(v[2]);
    auto brod4 = _mm_set1_ps(v[3]);

    auto row = _mm_add_ps(
            _mm_add_ps(_mm_mul_ps(brod1, row1), _mm_mul_ps(brod2, row2)),
            _mm_add_ps(_mm_mul_ps(brod3, row3), _mm_mul_ps(brod4, row4))
    );
    _mm_store_ps(result.data, row);

    return result;
}
