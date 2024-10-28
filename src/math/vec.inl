#include "vec.hpp"

#include <cmath>

namespace vec {

template <typename T, size_t Size>
vec<T, Size>::vec(const __m128& reg) {
    _mm_store_ps(data, reg);
}

template <typename T, size_t Size>
vec<T, Size>::operator __m128() const {
    return _mm_load_ps(data);
}

template <typename T, size_t Size>
vec<T, Size>::vec(const T& x, const T& y) {
    static_assert(Size == 2, "Not a vector 2!");

    data[0] = x;
    data[1] = y;
}

template <typename T, size_t Size>
vec<T, Size>::vec(const T& x, const T& y, const T& z) {
    static_assert(Size == 3, "Not a vector 3!");

    data[0] = x;
    data[1] = y;
    data[2] = z;
}

template <typename T, size_t Size>
vec<T, Size>::vec(const T& x, const T& y, const T& z, const T& w) {
    static_assert(Size == 4, "Not a vector 4!");

    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

template <typename T, size_t Size>
vec<T, Size> operator+(const vec<T, Size>& u, const vec<T, Size>& v) {
    return _mm_add_ps(u, v);
}

template <typename T, size_t Size>
vec<T, Size> operator-(const vec<T, Size>& u, const vec<T, Size>& v) {
    return _mm_sub_ps(u, v);
}

template <typename T, size_t Size>
vec<T, Size> operator*(const float& s, const vec<T, Size>& v) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_mul_ps(v, scalar);
}

template <typename T, size_t Size>
vec<T, Size> operator*(const vec<T, Size>& v, const float& s) {
    return s * v;
}

template <typename T, size_t Size>
vec<T, Size> operator/(const vec<T, Size>& v, const float& s) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_div_ps(v, scalar);
}

template <typename T, size_t Size>
float length(const vec<T, Size>& v) {
    auto sq  = _mm_mul_ps(v, v);
    auto sum = _mm_hadd_ps(sq, sq);
    sum      = _mm_hadd_ps(sum, sum);

    return std::sqrt(_mm_cvtss_f32(sum));
}

template <typename T, size_t Size>
float dot(const vec<T, Size>& u, const vec<T, Size>& v) {
    auto result = _mm_dp_ps(u, v, 0x31);
    return _mm_cvtss_f32(result);
}

template <typename T, size_t Size>
vec<T, Size> normalize(const vec<T, Size>& v) {
    auto len = length(v);
    return v / len;
}

};// namespace vec