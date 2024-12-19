#include "mat_type.hpp"

#include <smmintrin.h>
#include <iostream>

template <typename T, size_t Size>
mat<T, Size>::mat(const T& diag) {
    for (int i = 0; i < Size; i++) {
        data[i][i] = diag;
    }
}

template <typename T, size_t Size>
storage<T, Size>& mat<T, Size>::operator[](size_t index) {
    assert(index >= 0 && index < Size);
    return data[index];
}

template <typename T, size_t Size>
const storage<T, Size>& mat<T, Size>::operator[](size_t index) const {
    assert(index >= 0 && index < Size);
    return data[index];
}

template <typename T, size_t Size>
mat<T, Size>& mat<T, Size>::operator*=(const mat<T, Size>& other) {
    *this = *this * other;
    return *this;
}

template <typename T, size_t Size>
mat<T, Size> operator+(const mat<T, Size>& a, const mat<T, Size>& b) {
    mat<T, Size> result;
    for (int i = 0; i < Size; i++) {
        result[i] = _mm_add_ps(a[i], b[i]);
    }
    return result;
}

template <typename T, size_t Size>
mat<T, Size> operator-(const mat<T, Size>& a, const mat<T, Size>& b) {
    mat<T, Size> result;
    for (int i = 0; i < Size; i++) {
        result[i] = _mm_sub_ps(a[i], b[i]);
    }
    return result;
}

template <typename T, size_t Size>
mat<T, Size> operator*(const mat<T, Size>& a, const mat<T, Size>& b) {
    mat<T, Size> result;

    __m128 row[4];
    for (int i = 0; i < 4; i++) {
        if (i >= Size) {
            row[i] = _mm_set1_ps(0);
            continue;
        }
        row[i] = a[i];
    }

    for (int i = 0; i < 4; i++) {
        __m128 brod[4];
        if (i >= Size) {
            brod[i] = _mm_set1_ps(0);
        }
        else {
            brod[i] = _mm_set1_ps(b[0][i]);
        }

        auto currRow = _mm_add_ps(
                _mm_add_ps(_mm_mul_ps(brod[0], row[0]), _mm_mul_ps(brod[1], row[1])),
                _mm_add_ps(_mm_mul_ps(brod[2], row[2]), _mm_mul_ps(brod[3], row[3]))
        );

        if (i < Size) {
            result[i] = currRow;
        }
    }
    return result;
}

template <typename T, size_t Size>
vec<T, Size> operator*(const mat<T, Size>& m, const vec<T, Size>& v) {
    vec<T, Size> result;

    __m128 row[4];
    for (int i = 0; i < 4; i++) {
        if (i >= Size) {
            row[i] = _mm_set1_ps(0);
            continue;
        }
        row[i] = m[i];
    }

    __m128 brod[4];
    for (int i = 0; i < 4; i++) {
        if (i >= Size) {
            brod[i] = _mm_set1_ps(0);
            continue;
        }

        brod[i] = _mm_set1_ps(v[i]);
    }

    auto regRes = _mm_add_ps(
            _mm_add_ps(_mm_mul_ps(brod[0], row[0]), _mm_mul_ps(brod[1], row[1])),
            _mm_add_ps(_mm_mul_ps(brod[2], row[2]), _mm_mul_ps(brod[3], row[3]))
    );

    float buffer[4];
    _mm_store_ps(buffer, regRes);
    for (int i = 0; i < Size; i++) {
        result[i] = buffer[i];
    }

    return result;
}
