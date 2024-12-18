#include "vec4_type.hpp"

#include <cmath>
#include <cassert>

template <typename T>
vec<T, 4>::vec(const T& value) {
    data[0] = value;
    data[1] = value;
    data[2] = value;
    data[3] = value;
}

template <typename T>
vec<T, 4>::vec(const T& x, const T& y, const T& z, const T& w) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

template <typename T>
vec<T, 4>::vec(const __m128& reg) {
    _mm_store_ps(data, reg);
}

template <typename T>
vec<T, 4>::operator T*() {
    return data;
}

template <typename T>
vec<T, 4>::operator __m128() const {
    return data;
}

template <typename T>
T& vec<T, 4>::operator[](size_t index) {
    return data[index];
}

// god bless https://stackoverflow.com/questions/6042399/how-to-compare-m128-types
template <typename T>
bool operator==(const vec<T, 4>& u, const vec<T, 4>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) == 0xF;
}

template <typename T>
bool operator!=(const vec<T, 4>& u, const vec<T, 4>& v) {
    auto eps = _mm_set1_ps(0.001f);
    auto abd = _mm_andnot_ps(_mm_set1_ps(-0.f), _mm_sub_ps(u, v));
    return _mm_movemask_ps(_mm_cmplt_ps(abd, eps)) != 0xF;
}

template <typename T>
const T& vec<T, 4>::operator[](size_t index) const {
    return data[index];
}

template <typename T>
vec<T, 4> operator+(const vec<T, 4>& u, const vec<T, 4>& v) {
    return _mm_add_ps(u, v);
}

template <typename T>
vec<T, 4> operator-(const vec<T, 4>& u, const vec<T, 4>& v) {
    return _mm_sub_ps(u, v);
}

template <typename T>
vec<T, 4> operator*(const float& s, const vec<T, 4>& v) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_mul_ps(v, scalar);
}

template <typename T>
vec<T, 4> operator*(const vec<T, 4>& v, const float& s) {
    return s * v;
}

template <typename T>
vec<T, 4> operator/(const vec<T, 4>& v, const float& s) {
    auto scalar = _mm_load1_ps(&s);
    return _mm_div_ps(v, scalar);
}
