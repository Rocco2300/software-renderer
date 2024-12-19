#include "vec_type.hpp"

#include <cmath>
#include <cassert>

template <typename T, size_t Size>
vec<T, Size>::vec(const T& value) {
    data[0] = value;
    data[1] = value;
    data[2] = value;
    data[3] = value;
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
vec<T, Size>::vec(const __m128& reg) {
    data = storage<T, Size>(reg);
}

template <typename T, size_t Size>
vec<T, Size>::vec(const vec<T, Size>& other) {
    this->data = other.data;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator=(const vec<T, Size>& other) {
    this->data = other.data;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>::operator T*() {
    return data;
}

template <typename T, size_t Size>
vec<T, Size>::operator __m128() const {
    return data;
}

template <typename T, size_t Size>
T& vec<T, Size>::operator[](size_t index) {
    return data[index];
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator+=(const vec<T, Size>& v) {
    *this = *this + v;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator-=(const vec<T, Size>& v) {
    *this = *this - v;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator*=(float s) {
    *this = *this * s;
    return *this;
}

template <typename T, size_t Size>
vec<T, Size>& vec<T, Size>::operator/=(float s) {
    *this = *this / s;
    return *this;
}

// god bless https://stackoverflow.com/questions/6042399/how-to-compare-m128-types
template <typename T, size_t Size>
bool operator==(const vec<T, Size>& u, const vec<T, Size>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) == 0xF;
}

template <typename T, size_t Size>
bool operator!=(const vec<T, Size>& u, const vec<T, Size>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) != 0xF;
}

template <typename T, size_t Size>
const T& vec<T, Size>::operator[](size_t index) const {
    return data[index];
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